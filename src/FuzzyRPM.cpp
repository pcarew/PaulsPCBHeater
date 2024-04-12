#include "Arduino.h"
#include "FuzzyLogic.h"
#include "FuzzyRPM.h"

// RPM Input fuzzy sets
#define TOOSLOW 0
#define SOMEWHATSLOW 1
#define ABOUTRIGHT 2
#define SOMEWHATFAST 3
#define TOOFAST 4

// PWM Output fuzzy sets
#define SPEEDUP 0
#define SPEEDUPSOME 1
#define NOCHANGE 2
#define SLOWDOWNSOME 3
#define SLOWDOWN 4

double errorPercentage(double actual,double desired) ;
double getRateOfChange(double currentValue, double *prevValue);

double prevErrPercent	 = 0.0;

int fuzzyModel;

InputFuzzySet minInSets[] = {
			// L   T   R
			{leftShoulder, {-150,-100,0}},		// Too Slow
			{rightShoulder, {0, 100, 150}},		// Too Fast
		};
InputFuzzySet inSets[] = {
			// L   T   R
			{leftShoulder, {-150,-100,-50}},		// Too Slow
			{triangle, {-100, -50, 0}},				// Somewhat Slow
			{triangle, { -20, 0, 20}},				// About Right
			{triangle, {0, 50, 100}},				// Somewhat Fast
			{rightShoulder, {50, 100, 150}},		// Too Fast
		};
OutputFuzzySet minOutSets[] = {
			{triangleWeight, {0,25,50}},		// Speed Up
			{triangleWeight, {-55, -25, 0}},	// Slow Down
		};
/*
InputFuzzySet errorFzSets[] = {
			// L   T   R
			{leftShoulder, {-150,-100,-50}},		// Too Slow
			{triangle, {-100, -50, 0}},				// Somewhat Slow
			{triangle, { -50, 0, 50}},				// About Right
			{triangle, {0, 50, 100}},				// Somewhat Fast
			{rightShoulder, {50, 100, 150}},		// Too Fast
		};
InputFuzzySet DerrorDTFzSets[] = {
			// L   T   R
			{leftShoulder, {-150,-100,-50}},		// Quickly Converging
			{triangle, {-100, -50, 0}},				// Slowly Converging
			{triangle, { -50, 0, 50}},				// No Convergence
			{triangle, {0, 50, 100}},				// Slowly Diverging
			{rightShoulder, {50, 100, 150}},		// Quickly Diverging
		};
		*/
InputFuzzySet *errorFzSets = inSets;				// Defined using same ranges
InputFuzzySet *DerrorDTFzSets = errorFzSets;		// Defined using same ranges

#define	NOERRORFUZZYSETS (sizeof(inSets)/sizeof(InputFuzzySet))			// Number of Error sets
#define	NODERRORDTFUZZYSETS (sizeof(inSets)/sizeof(InputFuzzySet))		// Number of dErrordt sets

OutputFuzzySet outSets[] = {
		/*
		// 20% Max 		+20% -> -20%
			{triangleWeight, {10,20,30}},		// Speed Up
			{triangleWeight, {0, 10, 20}},		// Speed Up some
			{triangleWeight, { -10, 0, 10}},	// No Change
			{triangleWeight, {-20, -10, 0}},	// Slow Down some
			{triangleWeight, {-30, -20, -10}},	// Slow Down
		};
		*/

		// 50%Max
			{triangleWeight, {25,50,75}},		// Speed Up
			{triangleWeight, {0, 25, 50}},		// Speed Up some
			{triangleWeight, { -25, 0, 25}},	// No Change
			{triangleWeight, {-50, -25, 0}},	// Slow Down some
			{triangleWeight, {-75, -50, -25}},	// Slow Down
		};

		/*
		// 100% Max
			{triangleWeight, {50,100,150}},		// Speed Up
			{triangleWeight, {0, 50, 100}},		// Speed Up some
			{triangleWeight, { -50, 0, 50}},	// No Change
			{triangleWeight, {-100, -50, 0}},	// Slow Down some
			{triangleWeight, {-150, -100, -50}},	// Slow Down
		};
		*/


OutputFuzzySet *rpmChange[NODERRORDTFUZZYSETS][NOERRORFUZZYSETS] = {
//				TOOSLOW 			SOMEWHATSLOW 		ABOUTRIGHT 				SOMEWHATFAST		TOOFAST
		{ // Quickly Converging
				&outSets[NOCHANGE], &outSets[SLOWDOWN], &outSets[SLOWDOWNSOME], &outSets[SPEEDUP], &outSets[NOCHANGE],
		},
		{ // Slowly Converging
				&outSets[SPEEDUP], &outSets[NOCHANGE], &outSets[NOCHANGE], &outSets[NOCHANGE], &outSets[SLOWDOWN],
		},
		{ // Zero Convergence
				&outSets[SPEEDUP], &outSets[SPEEDUPSOME], &outSets[NOCHANGE], &outSets[SLOWDOWNSOME], &outSets[SLOWDOWN],
		},
		{ // Slowly Diverging
				&outSets[SPEEDUP], &outSets[SPEEDUPSOME], &outSets[SLOWDOWNSOME], &outSets[SLOWDOWNSOME], &outSets[SLOWDOWN],
		},
		{ // Quickly Diverging
				&outSets[SPEEDUP], &outSets[SPEEDUP], &outSets[SLOWDOWNSOME], &outSets[SLOWDOWN], &outSets[SLOWDOWN],
		},
};

double getRpmChangePercent(double actualRpm, double desiredRpm){

	double weight					= 0.0;
	double station					= 0.0;

	double errPercent				= errorPercentage(actualRpm, desiredRpm);
	double dErrordt 				= getRateOfChange(errPercent, &prevErrPercent);
//	double dErrordt 				= getRateOfChange(actualRpm, &prevErrPercent);


	double totalWeight				= 0.0;
	double totalMoment				= 0.0;
	double change					= 0.0;

	switch(fuzzyModel){
	case MINIMALMODEL:
	{
		// Run the following rules
			//"If speed is Too slow then increase PWM to the speed controller to Speed up", // name
			//"If speed is Too fast then decrease PWM to the speed controller to Slow down"
		int ruleList[] = {0,4};
		//for(unsigned int rule=0;rule < sizeof(ruleList)/sizeof(int);rule++){
			//weight = runRuleSingular(errPercent, &inSets[ruleList[rule]],&outSets[ruleList[rule]],&station);
		for(unsigned int ruleNo=0;ruleNo < sizeof(minOutSets)/sizeof(OutputFuzzySet);ruleNo++){
			weight = runRuleSingular(errPercent, &minInSets[ruleNo],&minOutSets[ruleNo],&station);
			//Serial.print("\nRule#");Serial.print(rule);Serial.print(" Weight:");Serial.print(weight);Serial.print(" Stn:");Serial.println(station);

			totalWeight			+= weight;
			totalMoment			+= weight * station;
		}
		break;
	}
	case JUSTERRORMODEL:
		// Run the following rules
			//"If speed is Too slow then increase PWM to the speed controller to Speed up", // name
			//"If speed is Too fast then decrease PWM to the speed controller to Slow down"
			//"If speed is About right then Not much change needed in PWM to the speed controller"
			//"If speed is Somewhat Fast then Slow Down Some in PWM to the speed controller"
			//"If speed is Somewhat Slow then Speed Up Some in PWM to the speed controller"
		for(unsigned int ruleNo=0;ruleNo < sizeof(outSets)/sizeof(OutputFuzzySet);ruleNo++){
			weight = runRuleSingular(errPercent, &inSets[ruleNo],&outSets[ruleNo],&station);
			totalWeight			+= weight;
			totalMoment			+= weight * station;
		}
		break;
	case FULLMODEL: // Includes rate of change of error (with respect to time)  dErr/dt
		for(unsigned int DerrorDtFzId=0;DerrorDtFzId < NODERRORDTFUZZYSETS;DerrorDtFzId++){
			for(unsigned int errorFzId=0;errorFzId < NOERRORFUZZYSETS;errorFzId++){
				InputFuzzySet *DerrorDtFzSet	= &DerrorDTFzSets[DerrorDtFzId];
				InputFuzzySet *errorFzSet		= &errorFzSets[errorFzId];
				OutputFuzzySet *outFzSet		= rpmChange[DerrorDtFzId][errorFzId];

				weight = runRuleTwin(errPercent, errorFzSet, dErrordt, DerrorDtFzSet ,outFzSet, &station);
				totalWeight			+= weight;
				totalMoment			+= weight * station;
			}
		}
		break;
	}
	if(totalWeight!=0)
		change					= totalMoment/totalWeight;					// COG of change consequents
	else
		change = 0;
	//Serial.print("##InitChg:");Serial.print(change);Serial.print("TW:"),Serial.print(totalWeight);Serial.print("\n");

	return change;
}

double errorPercentage(double actual,double desired) {
	if(desired == 0.0) return 0.0;
	else return (actual/desired)*100.0 - 100.0;		// Turn measured speed into a +/- %
}

double getRateOfChange(double currentValue, double *prevValue){			// per period
	double rateOfChange = currentValue - *prevValue;
	/*
	double rateOfChange = 0.0;
	if(*prevValue !=0)
		rateOfChange = (currentValue - *prevValue)/(*prevValue);
		*/
	*prevValue = currentValue;
	return rateOfChange;
}

