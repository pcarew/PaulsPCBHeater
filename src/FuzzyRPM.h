#if !defined(FUZZYRPM)
#define FUZZYRPM

#define MINIMALMODEL 1
#define JUSTERRORMODEL 2
#define FULLMODEL 3

extern double getRpmChangePercent(double actualRpm, double desiredRpm);
extern double errorPercentage(double actual,double desired);

extern unsigned int pwm;
extern unsigned long rpm;
extern int fuzzyModel;

#endif
