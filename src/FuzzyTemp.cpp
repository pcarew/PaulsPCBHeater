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
#define FULLHEAT 0
#define MOREHEAT 1
#define SOMEHEAT 2
#define NOCHANGE 3
#define LESSHEAT 4
#define NOHEAT	 5


//int FuzzyTemp::fuzzyModel			= FULLMODEL;		// MINIMALMODEL JUSTERRORMODEL  FULLMODEL

/*
InputFuzzySet minInSets[] = {
			// L   T   R
			{leftShoulder, {-150,-100,0}},		// Too Low
			{rightShoulder, {0, 100, 150}},		// Too High
		};
	// Max 25%
OutputFuzzySet minOutSets[] = { // Max 25%
			{triangleWeight, {0,25,50}},		// FullHeat
			{triangleWeight, {-55, -25, 0}},	// NoHeat
		};

OutputFuzzySet minOutSets[] = {	// Max 100%
			{triangleWeight, {0,100,150}},		// FullHeat
			{triangleWeight, {-150, -100, 0}},	// NoHeat
		};
		*/

const PROGMEM InputFuzzySet inSets[] = {
			// L   T   R
			{leftShoulder, {-150,-20,-10}},		// Too Low			/ Quickly Converging (Improving fast)
			{triangle, {-20, -10, 0}},				// Somewhat Low		/ Slowly Converging	(Slow improvement)
			{triangle, { -2.5, 0, 2.5}},				// About Right		/ Zero Convergence	( Not moving)
			{triangle, {0, 10, 20}},				// Somewhat High	/ Slowly Diverging	( Getting worse)
			{rightShoulder, {10, 20, 150}},		// Too High			/ Quickly Diverging	(Quickly getting worse)
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

const PROGMEM OutputFuzzySet outSets[] = {
		/*
		// 20% Max 		+20% -> -20%
			{triangleWeight, {10,20,30}},		// FullHeat
			{triangleWeight, {0, 10, 20}},		// SomeHeat
			{triangleWeight, { -10, 0, 10}},	// No Change
			{triangleWeight, {-20, -10, 0}},	// LessHeat
			{triangleWeight, {-30, -20, -10}},	// NoHeat
		};
		*/

		/*
		// 50%Max
			{triangleWeight, {25,50,75}},		// FullHeat
			{triangleWeight, {0, 25, 50}},		// SomeHeat
			{triangleWeight, { -25, 0, 25}},	// No Change
			{triangleWeight, {-50, -25, 0}},	// LessHeat
			{triangleWeight, {-75, -50, -25}},	// NoHeat
		};

		*/
		// 100% Max
			{triangleWeight, {75,100,150}},		// FullHeat
			{triangleWeight, {50,75,100}},		// More Heat
			{triangleWeight, {0, 50, 100}},		// SomeHeat
			{triangleWeight, { -25, 0, 25}},	// No Change
			{triangleWeight, {-100, -50, 0}},	// LessHeat
			{triangleWeight, {-150, -100, -50}},	// NoHeat
		};

//InputFuzzySet *errorFzSets = inSets;				// Defined using same ranges
//InputFuzzySet *DerrorDTFzSets = errorFzSets;		// Defined using same ranges

#define	NUMERRORFUZZYSETS (sizeof(inSets)/sizeof(InputFuzzySet))			// Number of Error sets
#define	NUMDERRORDTFUZZYSETS (sizeof(inSets)/sizeof(InputFuzzySet))		// Number of dErrordt sets

const PROGMEM OutputFuzzySet valueChange[NUMDERRORDTFUZZYSETS][NUMERRORFUZZYSETS] = {
//				TOOLOW 			SOMEWHATLOW 		ABOUTRIGHT 				SOMEWHATHIGH		TOOHIGH
		{ // Quickly Converging
				outSets[SOMEHEAT], outSets[NOCHANGE], outSets[NOCHANGE], outSets[NOCHANGE], outSets[NOHEAT],
		},
		{ // Slowly Converging
				outSets[MOREHEAT], outSets[SOMEHEAT], outSets[NOCHANGE], outSets[NOCHANGE], outSets[NOHEAT],
		},
		{ // Zero Convergence
				outSets[FULLHEAT], outSets[MOREHEAT], outSets[NOCHANGE], outSets[LESSHEAT], outSets[NOHEAT],
		},
		{ // Slowly Diverging
				outSets[FULLHEAT], outSets[FULLHEAT], outSets[SOMEHEAT], outSets[LESSHEAT], outSets[NOHEAT],
		},
		{ // Quickly Diverging
				outSets[FULLHEAT], outSets[FULLHEAT], outSets[MOREHEAT], outSets[NOHEAT], outSets[NOHEAT],
		},
};

// This is the 'Entry' method
double FuzzyTemp::getPowerPercent(double actualValue, double desiredValue, double *prevErrPercent){

	double weight					= 0.0;
	double station					= 0.0;

	double errPercent				= errorPercentage(actualValue, desiredValue);
	double dErrordt 				= getRateOfChangePercentage(errPercent, prevErrPercent);


	double totalWeight				= 0.0;
	double totalMoment				= 0.0;
	double change					= 0.0;

//	Serial.print(F("Act:"));Serial.print(actualValue);
//	Serial.print(F(" Des:"));Serial.print(desiredValue);
//	Serial.print(F(" PrevErr:"));Serial.print(*prevErrPercent);
//	Serial.print(F(" errP:"));Serial.print(errPercent);Serial.print(F(" dErrdt:"));Serial.println(dErrordt);
	/*
	switch(fuzzyModel){
	case MINIMALMODEL:
	{
		// Run the following rules
			//"If Too low then increase PWM to the controller to FullHeat",
			//"If Too fast then decrease PWM to the controller to NoHeat"
		// ** As there is only 1 Output membership function on either side of zero, the consequent will be the 'staion' value of the appropriate side **

//		int ruleList[] = {0,4};
		//for(unsigned int rule=0;rule < sizeof(ruleList)/sizeof(int);rule++){
			//weight = runRuleSingular(errPercent, &inSets[ruleList[rule]],&outSets[ruleList[rule]],&station);
		for(unsigned int ruleNo=0;ruleNo < sizeof(minOutSets)/sizeof(OutputFuzzySet);ruleNo++){
			weight = runRuleSingular(errPercent, &minInSets[ruleNo],&minOutSets[ruleNo],&station);
//			Serial.print(F("Rule#"));Serial.print(ruleNo);Serial.print(F(" Weight:"));Serial.print(weight);Serial.print(F(" Stn:"));Serial.println(station);

			totalWeight			+= weight;
			totalMoment			+= weight * station;
//			Serial.print(F("MM TotW"));Serial.print(totalWeight);Serial.print(F(" TotM:"));Serial.println(totalMoment); delay(50);	//AJPC
		}
		break;
	}
	case JUSTERRORMODEL:
		// Run the following rules
			//"If Too slow then increase PWM to the controller to FullHeat", // name
			//"If Too fast then decrease PWM to the controller to NoHeat"
			//"If About right then Not much change needed in PWM to the controller"
			//"If Somewhat High then LessHeat in PWM to the controller"
			//"If Somewhat Low then SomeHeat in PWM to the controller"
//		for(unsigned int ruleNo=0;ruleNo < sizeof(outSets)/sizeof(OutputFuzzySet);ruleNo++){
		for(unsigned int ruleNo=0;ruleNo < sizeof(inSets)/sizeof(InputFuzzySet);ruleNo++){
			weight = runRuleSingular(errPercent, &inSets[ruleNo],&outSets[ruleNo],&station);
			totalWeight			+= weight;
			totalMoment			+= weight * station;
//			Serial.print(F("JE TotW"));Serial.print(totalWeight);Serial.print(F(" TotM:"));Serial.println(totalMoment);
		}
		break;
	case FULLMODEL: // Includes rate of change of error (with respect to time)  dErr/dt
		*/
		for(unsigned int DerrorDtFzId=0;DerrorDtFzId < NUMDERRORDTFUZZYSETS;DerrorDtFzId++){
			for(unsigned int errorFzId=0;errorFzId < NUMERRORFUZZYSETS;errorFzId++){
//				InputFuzzySet *DerrorDtFzSet	= &inSets[DerrorDtFzId]; //&DerrorDTFzSets[DerrorDtFzId];
//				InputFuzzySet *errorFzSet		= &inSets[errorFzId]; //&errorFzSets[errorFzId];

				InputFuzzySet DerrorDtFzSet;
				memcpy_P( &DerrorDtFzSet, &inSets[DerrorDtFzId], sizeof(InputFuzzySet) ); //&DerrorDTFzSets[DerrorDtFzId];
				InputFuzzySet errorFzSet; //		= &inSets[errorFzId]; //&errorFzSets[errorFzId];
				memcpy_P( &errorFzSet, &inSets[errorFzId], sizeof(InputFuzzySet) ); //&DerrorDTFzSets[DerrorDtFzId];

//				OutputFuzzySet *outFzSet		= valueChange[DerrorDtFzId][errorFzId];
				OutputFuzzySet outFzSet;
				memcpy_P(&outFzSet, &valueChange[DerrorDtFzId][errorFzId], sizeof(OutputFuzzySet));

				weight = runRuleTwin(errPercent, &errorFzSet, dErrordt, &DerrorDtFzSet ,&outFzSet, &station);
				totalWeight			+= weight;
				totalMoment			+= weight * station;

//			Serial.print(F(" DeId: "));Serial.println(DerrorDtFzId);
//			Serial.print(F(" errId "));Serial.println(errorFzId);
//			Serial.print(F(" TotW: "));Serial.print(totalWeight);Serial.print(F(" TotM: "));Serial.println(totalMoment); delay(50);	//AJPC
//			Serial.print(F("FM Wght: "));Serial.print(weight); Serial.print(F(" TotW: "));Serial.print(totalWeight);Serial.print(F(" TotM: "));Serial.println(totalMoment); delay(50);	//AJPC
			}
		}
//		break;
//	}
//	Serial.print(F("Resulting TW:")),Serial.println(totalWeight);
	if(totalWeight!=0)
		change					= totalMoment/totalWeight;					// COG of change consequents
	else
		change = 0;
//	Serial.print(F("##Change:"));Serial.print(change);Serial.print(F("TW:")),Serial.println(totalWeight);

	return change;
}

double FuzzyTemp::errorPercentage(double actual,double desired) {
//	Serial.print(F("Err% A:"));Serial.print(actual);Serial.print(F(" D:"));Serial.println(desired);
	if(desired == 0.0) return 0.0;
	else return (actual/desired)*100.0 - 100.0;		// Turn measured speed into a +/- %
}

double FuzzyTemp::getRateOfChangePercentage(double currentValue, double *prevValue){			// per period
	double rateOfChange = 0.0;
	if(*prevValue !=0)
		rateOfChange = ((currentValue - *prevValue)/(*prevValue))*100.0;
	*prevValue = currentValue;
	return rateOfChange;
}

