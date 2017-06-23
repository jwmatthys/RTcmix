#include <Instrument.h>      // the base class for this instrument
#include <math.h> // for pow()

#define DEFAULT_CURVE      1
#define DEFAULT_NUMPTS    20
#define DEFAULT_BUFLEN 88200
#define MIN_BUFLEN   100
#define CLIP(a, lo, hi) ( (a)>(lo)?( (a)<(hi)?(a):(hi) ):(lo) )

class EXPDELAY : public Instrument {

public:
	EXPDELAY();
	virtual ~EXPDELAY();
	virtual int init(double *, int);
	virtual int configure();
	virtual int run();

private:
	double experp (double inval, double inlo, double inhi, double curve, double outlo, double outhi)
	{
		double lerp = (inval - inlo) / (inhi - inlo);
		double expval = pow (lerp, curve);
		return expval * (outhi - outlo) + outlo;
	}
	void doupdate();
	float *_delay_buf;
	float *_in;
	int _nargs, _inchan, _branch;
	float _amp, _pan;
	float _max_delay_time;
	int _delay_reps;
	float _delay_time;
	float _amp_curve, _decay_curve;
	int _buffer_pos;
};
