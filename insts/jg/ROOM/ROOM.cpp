#include <stdio.h>
#include <stdlib.h>
#include <mixerr.h>
#include <Instrument.h>
#include "ROOM.h"
#include <rt.h>
#include <rtdefs.h>

extern "C" {
   #include <ugens.h>
   extern int resetval;
}

//#define DEBUG
#define AVERAGE_CHANS   -1           /* average input chans flag value */


ROOM::ROOM() : Instrument()
{
   in = new float[MAXBUF];
   echo = NULL;
}


ROOM::~ROOM()
{
   delete [] in;
   delete [] echo;
}


int ROOM::init(float p[], short n_args)
{
   int   i;
   float outskip, inskip, dur, ringdur;

   outskip = p[0];
   inskip = p[1];
   dur = p[2];
   amp = p[3];
   inchan = n_args > 4 ? (int)p[4] : AVERAGE_CHANS;

   if (NCHANS != 2) {
      fprintf(stderr, "ROOM requires stereo output.\n");
      exit(1);
   }
   rtsetinput(inskip, this);
   insamps = (int)(dur * SR);

   if (inchan >= inputchans) {
      fprintf(stderr, "You asked for channel %d of a %d-channel input file.\n",
                       inchan, inputchans);
      exit(1);
   }

   if (inputchans == 1)
      inchan = 0;

   nmax = get_room(ipoint, lamp, ramp);
   if (nmax == 0) {
      fprintf(stderr, "You need to call roomset before ROOM.\n");
      exit(1);
   }
   echo = new float[nmax];
   for (i = 0; i < nmax; i++)
      echo[i] = 0.0;
   jpoint = 0;

#ifdef DEBUG
   printf("maximum delay = %d samples.\n", nmax);
#endif

   ringdur = (float)nmax / SR;
   nsamps = rtsetoutput(outskip, dur + ringdur, this);

   amparray = floc(1);
   if (amparray) {
      int amplen = fsize(1);
      tableset(dur + ringdur, amplen, amptabs);
   }
   else
      printf("Setting phrase curve to all 1's\n");

   skip = (int)(SR / (float)resetval);

   return nsamps;
}


int ROOM::run()
{
   int   i, branch, rsamps;
   float aamp, insig;
   float out[2];

   rsamps = chunksamps * inputchans;

   rtgetin(in, this, rsamps);

   aamp = amp;                  /* in case amparray == NULL */

   branch = 0;
   for (i = 0; i < rsamps; i += inputchans) {
      if (--branch < 0) {
         if (amparray)
            aamp = tablei(cursamp, amparray, amptabs) * amp;
         branch = skip;
      }
      if (cursamp < insamps) {               /* still taking input from file */
         if (inchan == AVERAGE_CHANS) {
            insig = 0.0;
            for (int n = 0; n < inputchans; n++)
               insig += in[i + n];
            insig /= (float)inputchans;
         }
         else
            insig = in[i + inchan];
      }
      else                                   /* in ring-down phase */
         insig = 0.0;

      echo[jpoint++] = insig;
      if (jpoint >= nmax)
         jpoint -= nmax;

      out[0] = out[1] = 0.0;
      for (int j = 0; j < NTAPS; j++) {
         float e = echo[ipoint[j]];
         out[0] += e * lamp[j];
         out[1] += e * ramp[j];
         ipoint[j]++;
         if (ipoint[j] >= nmax)
            ipoint[j] -= nmax;
      }

      if (aamp != 1.0) {
         out[0] *= aamp;
         out[1] *= aamp;
      }

      rtaddout(out);
      cursamp++;
   }

   return i;
}


Instrument *makeROOM()
{
   ROOM *inst;

   inst = new ROOM();
   return inst;
}

void rtprofile()
{
   RT_INTRO("ROOM", makeROOM);
}

