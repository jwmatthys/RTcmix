#define DELSIZE 100

class CLAR : public Instrument {
	int dl1[2],dl2[2],length1,length2;
	float del1[DELSIZE],del2[DELSIZE];
	float amp,namp,dampcoef,oldsig;
	float aamp,oamp;
	double *amparr, *oamparr;
	float amptabs[2], oamptabs[2];
	float d2gain, spread;
	int skip, branch;

public:
	CLAR();
	virtual int init(double*, int);
	virtual int run();
};
