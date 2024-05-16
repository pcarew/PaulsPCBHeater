#if !defined(FUZZYRPM)
#define FUZZYRPM

#define MINIMALMODEL 1
#define JUSTERRORMODEL 2
#define FULLMODEL 3

class FuzzyTemp {
private:
	static double prevErrPercent;
	static int fuzzyModel;

public:
	FuzzyTemp();
	~FuzzyTemp();
	static double getValueChangePercent(double actualRpm, double desiredRpm);
	static double errorPercentage(double actual,double desired);
	static double getRateOfChange(double currentValue, double *prevValue);			// per period

};

#endif
