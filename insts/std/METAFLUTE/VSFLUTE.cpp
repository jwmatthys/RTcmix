#include <iostream.h>
#include <ugens.h>
#include <mixerr.h>
#include <Instrument.h>
#include "VSFLUTE.h"
#include <rt.h>
#include <rtdefs.h>

extern "C" {
	void mdelset(float*, int*, int);
	float mdliget(float*, float, int*);
}

VSFLUTE::VSFLUTE() : Instrument()
{
	// future setup here?
}

int VSFLUTE::init(float p[], int n_args)
{
// p0 = start; p1 = dur; p2 = noise amp; p3 = length1low; p4 = length1high
// p5 = length2low; p6 = length2high; p7 = amp multiplier; 
// p8 = vibrato freq 1 low; p9 = vibrato freq 1 high
// p10 = vibrato freq 2 low; p11 = vibrato freq 2 high
// p12 = stereo spread (0-1) <optional>
// function slot 1 is the noise amp envelope
// function slot 2 is the out amp envelope
// function slot 3 is the vibrato function for length 1
// function slot 4 is the vibrato function for length 2

	int imax;

	nsamps = rtsetoutput(p[0], p[1], this);

	dampcoef = .7;

	amparr = floc(1);
	if (amparr) {
		int len = fsize(1);
		tableset(p[1], len, amptabs);
	}
	else
		die("VSFLUTE", "You haven't made the noise amp envelope (table 1).");

	oamparr = floc(2);
	if (oamparr) {
		int len = fsize(2);
		tableset(p[1], len, oamptabs);
	}
	else
		die("VSFLUTE", "You haven't made the output amp envelope (table 2).");

	pcurve1 = floc(3);
	if (pcurve1 == NULL)
		die("VSFLUTE", "You haven't made the vibrato function for "
					"length 1 (table 3).");
	psize1 = fsize(3);
	si1lo = p[8] * psize1/SR;
	si1hi = p[9] * psize1/SR;

	pcurve2 = floc(4);
	if (pcurve2 == NULL)
		die("VSFLUTE", "You haven't made the vibrato function for "
					"length 2 (table 4).");
	psize2 = fsize(4);
	si2lo = p[10] * psize2/SR;
	si2hi = p[11] * psize2/SR;

	imax = DELSIZE;
	mdelset(del1,dl1,imax);
	mdelset(del2,dl2,imax);

//	srrand(0.1);

	l1base = p[3];
	l2base = p[5];
	l1span = p[4] - p[3];
	l2span = p[6] - p[5];

	oldsig = 0; /* for the filter */

	amp = p[7];
	namp = p[2];
	spread = p[12];
	skip = (int)(SR/(float)resetval);
	phs1 = 0.0;
	phs2 = 0.0;

	return(nsamps);
}

int VSFLUTE::run()
{
	int i;
	float out[2];
	float aamp=0.,oamp=0.;
	float sig,del1sig;
	float length1,length2;
	float si1=0.,si2=0.;
	int branch;

	Instrument::run();

	branch = 0;
	for (i = 0; i < chunksamps; i++) {
		if (--branch < 0) {
			aamp = tablei(cursamp, amparr, amptabs);
			oamp = tablei(cursamp, oamparr, oamptabs);
			si1 = ((rrand()+1.0/2.0) * (si1hi - si1lo)) + si1lo;
			si2 = ((rrand()+1.0/2.0) * (si2hi - si2lo)) + si2lo;
			branch = skip;
			}

		length1 = l1base + (l1span * (oscili(0.5, si1, pcurve1, psize1, &phs1) + 0.5));
		length2 = l2base + (l2span * (oscili(0.5, si2, pcurve2, psize2, &phs2) + 0.5));
		sig = (rrand() * namp * aamp) + aamp;
		del1sig = mdliget(del1,length1,dl1);
		sig = sig + (del1sig * -0.35);
		delput(sig,del2,dl2);

		sig = mdliget(del2,length2,dl2);
		sig = (sig * sig * sig) - sig;
		sig = (0.4 * sig) + (0.9 * del1sig);
		out[0] = sig * amp * oamp;
		sig = (dampcoef * sig) + ((1.0 - dampcoef) * oldsig);
		oldsig = sig;
		delput(sig,del1,dl1);

		if (outputchans == 2) {
			out[1] = (1.0 - spread) * out[0];
			out[0] *= spread;
			}

		rtaddout(out);
		cursamp++;
		}
	return i;
}

Instrument*
makeVSFLUTE()
{
	VSFLUTE *inst;

	inst = new VSFLUTE();
	inst->set_bus_config("VSFLUTE");

	return inst;
}
