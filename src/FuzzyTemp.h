#if !defined(FUZZYRPM)
#define FUZZYRPM

#define MINIMALMODEL 1
#define JUSTERRORMODEL 2
#define FULLMODEL 3

class FuzzyTemp {
private:
//	static int fuzzyModel;

public:
	FuzzyTemp();
	~FuzzyTemp();

	static double getPowerPercent(double actualValue, double desiredValue, double *prevErrPercent);

	static double errorPercentage(double actual,double desired);
	static double getRateOfChangePercentage(double currentValue, double *prevValue);			// per period

};

#endif
