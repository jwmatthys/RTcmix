class REVERBIT : public Instrument {
   bool    dcblock, usefilt;
   int     insamps, skip, branch;
   int     deltabs[3];
   float   amp, reverbtime, rtchan_delaytime, reverbpct, cutoff;
   float   prev_in[2], prev_out[2];
   float   *in, *delarray, *rvbarray, amptabs[2];
   double  *amparray;
   double  tonedata[3];

   void updateRvb(double p[]);
public:
   REVERBIT();
   virtual ~REVERBIT();
   virtual int init(double p[], int n_args);
   virtual int configure();
   virtual int run();
};

