#include <ugens.h> // for die()
#include <rt.h>
#include <stdio.h> // for fprintf()
#include "HELMHOLTZ.h"

/* HELMHOLTZ - FFT-based Autocorrelation pitch tracker

   p[0]: outskip
   p[1]: inskip
   p[2]: duration
   p[3]: output amp
   p[4]: fidelity threshold 0-1, default 0.95
   p[5]: maximum frequency, default 1000
   p[6]: inchan, default 0
   p[7]: sensitivity (minimum RMS) 0-1, default 0.003
   p[8]: overlap (int), default 1
   p[9]: bias, default 0.2

   Joel Matthys <joel at matthys music dot com> 20 June 2013
*/

#define DEF_FIDELITY 0.95
#define DEF_MAXFREQ 1000.
#define DEF_OVERLAP 2
#define DEF_BIAS 0.2
#define DEF_MINRMS 0.003
#define SEEK 0.85
#define FILT_Q 3

#define DEBUG(x) // debug off
//#define DEBUG(x) x

extern double hh_pitch;

/* ------------------------------------------------------------- HELMHOLTZ -- */
HELMHOLTZ :: HELMHOLTZ() : Instrument()
{
  _branch = 0;
  _freq = 0;
  _fidelity = 0;
  _fidelitythreshold = DEF_FIDELITY;
  _maxfreq = DEF_MAXFREQ;
  _inchan = 0;
  _amp = 0;
  _helmholtz = new Helmholtz();  
  _hhbuf_in = new float [RTBUFSAMPS];
  _hhbuf_out = new float [RTBUFSAMPS];
  _filt = new Oequalizer(SR, OeqLowPass);
  DEBUG(fprintf(stdout,"Helmholtz initialized with buffer of %d\n",RTBUFSAMPS););
}


/* ------------------------------------------------------------ ~HELMHOLTZ -- */
HELMHOLTZ :: ~HELMHOLTZ()
{
  delete _helmholtz;
  delete _filt;
  delete[] _in;
  delete[] _hhbuf_in;
  delete[] _hhbuf_out;
  DEBUG(fprintf(stdout,"Helmholtz DESTROYED!\n"););
}


/* ------------------------------------------------------------------ init -- */
int HELMHOLTZ :: init(double p[], int n_args)
{
  const float outskip = p[0];
  const float inskip = p[1];
  const float dur = p[2];
  // additional arguments
  switch (n_args)
    {
    case 10:
      _helmholtz->setbias(p[9]);
    case 9:
      _helmholtz->setoverlap((int)p[8]);
    case 8:
      _helmholtz->setminRMS(p[7]);
    case 7:
      _inchan = (int) p[6];
    case 6:
	  _maxfreq = p[5];
    case 5:
      _fidelitythreshold = p[4];
    }

  if (rtsetoutput(outskip, dur, this) == -1)
    return DONT_SCHEDULE;

  if (rtsetinput(inskip, this) == -1)
    return DONT_SCHEDULE;

  if (outputChannels() > 2)
    return die("HELMHOLTZ", "No more than 2 channels are allowed with HELMHOLTZ.");

  if (_inchan >= inputChannels())
    return die("HELMHOLTZ", "You asked for channel %d of a %d-channel file.", 
	       _inchan, inputChannels());

  return nSamps();
}


/* ---------------------------------------------------- configure -- */
int HELMHOLTZ :: configure()
{
  _in = new float [RTBUFSAMPS * inputChannels()];
  _filt->setparams(_maxfreq,FILT_Q,1);
  return _in ? 0 : -1;
}


/* ----------------------------------------------------- doupdate -- */
void HELMHOLTZ :: doupdate()
{
	// The Instrument base class update() function fills the <p> array with
	// the current values of all pfields.  There is a way to limit the values
	// updated to certain pfields.  For more about this, read
	// src/rtcmix/Instrument.h.

	double p[10];
	update(p, 10);

	_amp = p[3];
}


/* ---------------------------------------------------- run -- */
int HELMHOLTZ :: run()
{
  const int inputchan = inputChannels();
  const int outputchan = outputChannels();
  const int samps = framesToRun() * inputchan;
  rtgetin(_in, this, samps);

  // deinterlace stereo channels
  for (int i=0; i<samps; i++)
    {
	  if ((i % inputchan)==_inchan)
	    _hhbuf_in[i/inputchan] = _filt->next(_in[i]);
    }

  //_hhbuf_in = _filt->next(_hhbuf_in);
  // Helmholtz finds the pitch
  _helmholtz->iosamples(_hhbuf_in, _hhbuf_out, RTBUFSAMPS);

  for (int i=0; i<samps; i+=inputchan)
	{
	  if (--_branch <= 0) {
		doupdate();
		_branch = getSkip();
	  }
	  float out[2];
	  out[0] = _in[i] * _amp;
	  if (outputchan>1)
		out[1] = _in[i+1] * _amp;
	  
      rtaddout(out);
      increment();
    }

  float freq = SR / (float)_helmholtz->getperiod();
  float fidelity = (float)_helmholtz->getfidelity();
  if (fidelity >= _fidelitythreshold &&
      freq <= _maxfreq)
    {
      _fidelity = fidelity;
      _freq = (double)freq;
      DEBUG(fprintf(stdout, "freq: %g, fidelity: %g\n", _freq, _fidelity););
	  hh_pitch = (double)_freq;
    }
  else
    {
      _fidelity = 0;
      _freq = 0;
    }
  return framesToRun();
}


/* --------------------------------------------------------- makeHELMHOLTZ -- */
Instrument *makeHELMHOLTZ()
{
   HELMHOLTZ *inst;

   inst = new HELMHOLTZ();
   inst->set_bus_config("HELMHOLTZ");

   return inst;
}

/* ------------------------------------------------------------- rtprofile -- */
void rtprofile()
{
   RT_INTRO("HELMHOLTZ", makeHELMHOLTZ);
}
