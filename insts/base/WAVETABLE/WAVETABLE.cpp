/* WAVETABLE -- simple wavetable oscillator instrument
 
   p0 = start time
   p1 = duration
   p2 = amp *
   p3 = frequency (Hz or oct.pc **)
   p4 = pan (in percent-to-left form: 0-1) [optional; default is 0]
   p5 = reference to wavetable [optional; if missing, must use gen 2 ***]

   p2 (amplitude), p3 (freq) and p4 (pan) can receive dynamic updates
   from a table or real-time control source.

   * If an old-style gen table 1 is present, its values will be multiplied
   by the p2 amplitude multiplier, even if the latter is dynamic.

   ** oct.pc format generally will not work as you expect for p3 (osc freq)
   if the pfield changes dynamically.  Use Hz instead in that case.

   *** If p5 is missing, you must use an old-style gen table 2 for the
   oscillator waveform.

                                                rev for v4, JGG, 7/12/04
*/
#include <stdio.h>
#include <stdlib.h>
#include <ugens.h>
#include <Instrument.h>
#include <PField.h>
#include "WAVETABLE.h"
#include <rt.h>
#include <rtdefs.h>

#define AMP_GEN_SLOT     1
#define WAVET_GEN_SLOT   2


WAVETABLE::WAVETABLE() : Instrument()
{
	branch = 0;
}

WAVETABLE::~WAVETABLE()
{
	delete osc;
}

int WAVETABLE::init(double p[], int n_args)
{
	float outskip = p[0];
	float dur = p[1];

	nsamps = rtsetoutput(outskip, dur, this);
	if (outputChannels() > 2)
		return die("WAVETABLE", "Can't handle more than 2 output channels.");

	freqraw = p[3];
	float freq;
	if (freqraw < 15.0)
		freq = cpspch(freqraw);
	else
		freq = freqraw;

	wavetable = NULL;
	int tablelen = 0;
	if (n_args > 5) {      // handle table coming in as optional p5 TablePField
		const PField &field = getPField(5);
		tablelen = field.values();
		wavetable = (double *) field;
	}
	if (wavetable == NULL) {
		wavetable = floc(WAVET_GEN_SLOT);
		if (wavetable == NULL)
			return die("WAVETABLE", "Either use the wavetable pfield (p5) or make "
                    "an old-style gen function in slot %d.", WAVET_GEN_SLOT);
		tablelen = fsize(WAVET_GEN_SLOT);
	}

	osc = new Ooscili(SR, freq, wavetable, tablelen);

	amptable = floc(AMP_GEN_SLOT);
	if (amptable) {
		int alen = fsize(AMP_GEN_SLOT);
		tableset(SR, dur, alen, amptabs);
	}

	skip = (int) (SR / (float) resetval);

	return nSamps();
}

void WAVETABLE::doupdate()
{
	double p[5];
	update(p, 5);

	amp = p[2];
	if (amptable)
		amp *= table(currentFrame(), amptable, amptabs);

	if (p[3] != freqraw) {
		float freq;
		freqraw = p[3];
		if (freqraw < 15.0)
			freq = cpspch(freqraw);
		else
			freq = freqraw;
		osc->setfreq(freq);
	}

	spread = p[4];
}

int WAVETABLE::run()
{
	for (int i = 0; i < framesToRun(); i++) {
		if (--branch <= 0) {
			doupdate();
			branch = skip;
		}

		float out[2];

		out[0] = osc->next() * amp;

		if (outputChannels() == 2) {
			out[1] = (1.0 - spread) * out[0];
			out[0] *= spread;
		}

		rtaddout(out);
		increment();
	}
	return framesToRun();
}


Instrument *makeWAVETABLE()
{
	WAVETABLE *inst;
	inst = new WAVETABLE();
	inst->set_bus_config("WAVETABLE");
	return inst;
}

void rtprofile()
{
	RT_INTRO("WAVETABLE", makeWAVETABLE);
}

