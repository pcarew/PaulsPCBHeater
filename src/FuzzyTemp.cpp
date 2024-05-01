#include "FuzzyTemp.h"

#include "Arduino.h"
#include "FuzzyLogic.h"
#include "FuzzyTemp.h"

// Controlled value Input fuzzy sets
#define TOOLOW 0
#define SOMEWHATLOW 1
#define ABOUTRIGHT 2
#define SOMEWHATHIGH 3
#define TOOHIGH 4

// PWM Output fuzzy sets
#define INCREASE 0
#define INCREASEMORE 1
#define INCREASESOME 2
#define NOCHANGE 3
#define REDUCESOME 4
#define REDUCE 5

//double errorPercentage(double actual,double desired) ;
//double getRateOfChange(double currentValue, double *prevValue);

//double prevErrPercent	 = 0.0;
//int fuzzyModel;

double FuzzyTemp::prevErrPercent	= 0.0;
int FuzzyTemp::fuzzyModel			= JUSTERRORMODEL; //FULLMODEL;		// MINIMALMODEL JUSTERRORMODEL  FULLMODEL

InputFuzzySet minInSets[] = {
			// L   T   R
			{leftShoulder, {-150,-100,0}},		// Too Low
			{rightShoulder, {0, 100, 150}},		// Too High
		};
	/* Max 25%
OutputFuzzySet minOutSets[] = { // Max 25%
			{triangleWeight, {0,25,50}},		// Increase
			{triangleWeight, {-55, -25, 0}},	// Reduce
		};
		*/

OutputFuzzySet minOutSets[] = {	// Max 100%
			{triangleWeight, {0,100,150}},		// Increase
			{triangleWeight, {-150, -100, 0}},	// Reduce
		};

InputFuzzySet inSets[] = {
			// L   T   R
			{leftShoulder, {-150,-100,-50}},		// Too Low
			{triangle, {-100, -50, 0}},				// Somewhat Low
			{triangle, { -20, 0, 20}},				// About Right
			{triangle, {0, 50, 100}},				// Somewhat High
			{rightShoulder, {50, 100, 150}},		// Too High
		};
/*
InputFuzzySet errorFzSets[] = {
			// L   T   R
			{leftShoulder, {-150,-100,-50}},		// Too Low
			{triangle, {-100, -50, 0}},				// Somewhat Low
			{triangle, { -50, 0, 50}},				// About Right
			{triangle, {0, 50, 100}},				// Somewhat High
			{rightShoulder, {50, 100, 150}},		// Too High
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

OutputFuzzySet outSets[] = {
		/*
		// 20% Max 		+20% -> -20%
			{triangleWeight, {10,20,30}},		// Increase
			{triangleWeight, {0, 10, 20}},		// Increase some
			{triangleWeight, { -10, 0, 10}},	// No Change
			{triangleWeight, {-20, -10, 0}},	// Reduce some
			{triangleWeight, {-30, -20, -10}},	// Reduce
		};
		*/

		/*
		// 50%Max
			{triangleWeight, {25,50,75}},		// Increase
			{triangleWeight, {0, 25, 50}},		// Increase some
			{triangleWeight, { -25, 0, 25}},	// No Change
			{triangleWeight, {-50, -25, 0}},	// Reduce some
			{triangleWeight, {-75, -50, -25}},	// Reduce
		};

		*/
		// 100% Max
			{triangleWeight, {50,100,150}},		// Increase
			{triangleWeight, {50,75,100}},		// Increase more
			{triangleWeight, {0, 50, 100}},		// Increase some
			{triangleWeight, { -50, 0, 50}},	// No Change
			{triangleWeight, {-100, -50, 0}},	// Reduce some
			{triangleWeight, {-150, -100, -50}},	// Reduce
		};

InputFuzzySet *errorFzSets = inSets;				// Defined using same ranges
InputFuzzySet *DerrorDTFzSets = errorFzSets;		// Defined using same ranges

#define	NOERRORFUZZYSETS (sizeof(inSets)/sizeof(InputFuzzySet))			// Number of Error sets
#define	NODERRORDTFUZZYSETS (sizeof(inSets)/sizeof(InputFuzzySet))		// Number of dErrordt sets

OutputFuzzySet *valueChange[NODERRORDTFUZZYSETS][NOERRORFUZZYSETS] = {
//				TOOLOW 			SOMEWHATLOW 		ABOUTRIGHT 				SOMEWHATHIGH		TOOHIGH
		{ // Quickly Converging
				&outSets[NOCHANGE], &outSets[REDUCE], &outSets[REDUCESOME], &outSets[INCREASE], &outSets[NOCHANGE],
		},
		{ // Slowly Converging
				&outSets[INCREASE], &outSets[NOCHANGE], &outSets[NOCHANGE], &outSets[NOCHANGE], &outSets[REDUCE],
		},
		{ // Zero Convergence
//				&outSets[INCREASE], &outSets[INCREASESOME], &outSets[NOCHANGE], &outSets[REDUCESOME], &outSets[REDUCE],
				&outSets[INCREASE], &outSets[INCREASEMORE], &outSets[NOCHANGE], &outSets[REDUCESOME], &outSets[REDUCE],
		},
		{ // Slowly Diverging
//				&outSets[INCREASE], &outSets[INCREASESOME], &outSets[REDUCESOME], &outSets[REDUCESOME], &outSets[REDUCE],
				&outSets[INCREASE], &outSets[INCREASEMORE], &outSets[REDUCESOME], &outSets[REDUCESOME], &outSets[REDUCE],
		},
		{ // Quickly Diverging
				&outSets[INCREASE], &outSets[INCREASE], &outSets[REDUCESOME], &outSets[REDUCE], &outSets[REDUCE],
		},
};

double FuzzyTemp::getValueChangePercent(double actualValue, double desiredValue){

	double weight					= 0.0;
	double station					= 0.0;

	double errPercent				= errorPercentage(actualValue, desiredValue);
	double dErrordt 				= getRateOfChange(errPercent, &prevErrPercent);
//	double dErrordt 				= getRateOfChange(actualValue, &prevErrPercent);


	double totalWeight				= 0.0;
	double totalMoment				= 0.0;
	double change					= 0.0;

//	Serial.print(F("errP:"));Serial.print(errPercent);Serial.print(F(" dErrdt:"));Serial.println(dErrordt);
	switch(fuzzyModel){
	case MINIMALMODEL:
	{
		// Run the following rules
			//"If Too low then increase PWM to the controller to Increase",
			//"If Too fast then decrease PWM to the controller to Reduce"
		// ** As there is only 1 Output membership function on either side of zero, the consequent will be the 'staion' value of the appropriate side **

//		int ruleList[] = {0,4};
		//for(unsigned int rule=0;rule < sizeof(ruleList)/sizeof(int);rule++){
			//weight = runRuleSingular(errPercent, &inSets[ruleList[rule]],&outSets[ruleList[rule]],&station);
		for(unsigned int ruleNo=0;ruleNo < sizeof(minOutSets)/sizeof(OutputFuzzySet);ruleNo++){
			weight = runRuleSingular(errPercent, &minInSets[ruleNo],&minOutSets[ruleNo],&station);
//			Serial.print("Rule#");Serial.print(ruleNo);Serial.print(" Weight:");Serial.print(weight);Serial.print(" Stn:");Serial.println(station);

			totalWeight			+= weight;
			totalMoment			+= weight * station;
//			Serial.print("MM TotW");Serial.print(totalWeight);Serial.print(" TotM:");Serial.println(totalMoment); delay(50);	//AJPC
		}
		break;
	}
	case JUSTERRORMODEL:
		// Run the following rules
			//"If Too slow then increase PWM to the controller to Increase", // name
			//"If Too fast then decrease PWM to the controller to Reduce"
			//"If About right then Not much change needed in PWM to the controller"
			//"If Somewhat High then Reduce Some in PWM to the controller"
			//"If Somewhat Low then Increase Some in PWM to the controller"
//		for(unsigned int ruleNo=0;ruleNo < sizeof(outSets)/sizeof(OutputFuzzySet);ruleNo++){
		for(unsigned int ruleNo=0;ruleNo < sizeof(inSets)/sizeof(InputFuzzySet);ruleNo++){
			weight = runRuleSingular(errPercent, &inSets[ruleNo],&outSets[ruleNo],&station);
			totalWeight			+= weight;
			totalMoment			+= weight * station;
//			Serial.print("JE TotW");Serial.print(totalWeight);Serial.print(" TotM:");Serial.println(totalMoment);
		}
		break;
	case FULLMODEL: // Includes rate of change of error (with respect to time)  dErr/dt
		for(unsigned int DerrorDtFzId=0;DerrorDtFzId < NODERRORDTFUZZYSETS;DerrorDtFzId++){
			for(unsigned int errorFzId=0;errorFzId < NOERRORFUZZYSETS;errorFzId++){
				InputFuzzySet *DerrorDtFzSet	= &DerrorDTFzSets[DerrorDtFzId];
				InputFuzzySet *errorFzSet		= &errorFzSets[errorFzId];
				OutputFuzzySet *outFzSet		= valueChange[DerrorDtFzId][errorFzId];

				weight = runRuleTwin(errPercent, errorFzSet, dErrordt, DerrorDtFzSet ,outFzSet, &station);
				totalWeight			+= weight;
				totalMoment			+= weight * station;
//			Serial.print("FM TotW");Serial.print(totalWeight);Serial.print(" TotM:");Serial.println(totalMoment); delay(50);	//AJPC
			}
		}
		break;
	}
//	Serial.print("Resulting TW:"),Serial.println(totalWeight);
	if(totalWeight!=0)
		change					= totalMoment/totalWeight;					// COG of change consequents
	else
		change = 0;
//	Serial.print("##Change:");Serial.print(change);Serial.print("TW:"),Serial.println(totalWeight);

	return change;
}

double FuzzyTemp::errorPercentage(double actual,double desired) {
//	Serial.print(F("Err% A:"));Serial.print(actual);Serial.print(F(" D:"));Serial.println(desired);
	if(desired == 0.0) return 0.0;
	else return (actual/desired)*100.0 - 100.0;		// Turn measured speed into a +/- %
}

double FuzzyTemp::getRateOfChange(double currentValue, double *prevValue){			// per period
	double rateOfChange = currentValue - *prevValue;
	/*
	double rateOfChange = 0.0;
	if(*prevValue !=0)
		rateOfChange = (currentValue - *prevValue)/(*prevValue);
		*/
	*prevValue = currentValue;
	return rateOfChange;
}

