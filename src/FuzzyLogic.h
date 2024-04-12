#if !defined(FUZZYLOGIC)
#define FUZZYLOGIC

typedef struct {
	double leftCorner;
	double topCorner;
	double rightCorner;
}FuzzySetData;

typedef	double (*FuzzifyFunction)(double crispValue, FuzzySetData *setData);
typedef double (*WeightedSetFunction)(double fuzzyValue, FuzzySetData *setData, double *station);

typedef struct {
	FuzzifyFunction  fuzzify;
	FuzzySetData setData;
}InputFuzzySet;

typedef struct {
	WeightedSetFunction calcWeight;
	FuzzySetData setData;
}OutputFuzzySet;

typedef struct {
	InputFuzzySet a;
	InputFuzzySet b;

}FuzzyRule;

extern double leftShoulder(double crispValue, FuzzySetData *setData);
extern double triangle(double crispValue, FuzzySetData *setData);
extern double rightShoulder(double crispValue, FuzzySetData *setData);

extern double triangleWeight(double fuzzyValue, FuzzySetData *setData, double *fuzzyStation);
extern double linearWeight(double fuzzyValue, FuzzySetData *setData, double *fuzzyStation);

double runRule(FuzzyRule rule);
double runRuleSingular(double input, InputFuzzySet *inSet,OutputFuzzySet *outSet,double *station);
double runRuleTwin(double inputA, InputFuzzySet *inSetA, double inputB, InputFuzzySet *inSetB ,OutputFuzzySet *outSet,double *station);
double getConsequentValue(double fuzzyValue,OutputFuzzySet *outSet,double *station);

#endif
