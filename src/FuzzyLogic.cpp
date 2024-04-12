#include "Arduino.h"
#include "FuzzyLOGIC.h"


/*
 * Simplified Fuzzy Logic engine using only Triangle and Trapezoid (Shoulder) shapes
 *
 */

double predicate(double input, InputFuzzySet *inSet){
		FuzzifyFunction getFuzzyValue		= NULL;
		FuzzySetData *inSetData				= NULL;
		inSetData							= &inSet->setData;
		getFuzzyValue						= inSet->fuzzify;
		double fuzzyValue					= getFuzzyValue(input, inSetData );
	return fuzzyValue;
}
double runRuleSingular(double input, InputFuzzySet *inSet,OutputFuzzySet *outSet,double *station){											// if 'A' then 'Consequent'
		double fuzzyValue					= predicate(input, inSet);
		double consequent					= getConsequentValue(fuzzyValue,outSet, station);
		return consequent;
}
double runRuleTwin(double inputA, InputFuzzySet *inSetA, double inputB, InputFuzzySet *inSetB ,OutputFuzzySet *outSet,double *station){		// if 'A' and 'B' the 'Consequent'
		double fuzzyValueA					= predicate(inputA, inSetA);
//		Serial.print("FA:");Serial.print(fuzzyValueA);
		double fuzzyValueB					= predicate(inputB, inSetB);
//		Serial.print("FB:");Serial.print(fuzzyValueB);
		double fuzzyValue					= min(fuzzyValueA, fuzzyValueB);					// fvA & fvB
//		Serial.print("Fz:");Serial.print(fuzzyValue);
		double consequent					= getConsequentValue(fuzzyValue,outSet, station);
		return consequent;
}
double getConsequentValue(double fuzzyValue,OutputFuzzySet *outSet,double *station){
		WeightedSetFunction getWeightedValues	= NULL;
		FuzzySetData *outSetData				= NULL;
 		getWeightedValues						= outSet->calcWeight;
		outSetData								= &outSet->setData;
		double weight;

		weight									= getWeightedValues(fuzzyValue, outSetData, station);
		return weight;
}

double triangle(double crispValue, FuzzySetData *setData){
	double fuzzyValue = 0.0;
	if(crispValue >= setData->leftCorner && crispValue < setData->topCorner){
		fuzzyValue = (crispValue - setData->leftCorner) / (setData->topCorner - setData->leftCorner);
	}else if(crispValue >= setData->topCorner && crispValue <= setData->rightCorner) {
		fuzzyValue = (setData->rightCorner - crispValue) / (setData->rightCorner - setData->topCorner);
	}else{
		fuzzyValue = 0.0;
	}
	return fuzzyValue;
}

double rightShoulder(double crispValue, FuzzySetData *setData){
	double fuzzyValue = 0.0;
	if(crispValue >= setData->leftCorner && crispValue <= setData->topCorner){
			// Inside Triangle part
			// Calculated using the ratio of sides of similar triangles.
		fuzzyValue = (crispValue - setData->leftCorner) / (setData->topCorner - setData->leftCorner);
	}else if(crispValue > setData->topCorner){
			// Outside Fuzzy Subset
		fuzzyValue = 1.0;
	}else{
		fuzzyValue = 0.0;
	}
	//Serial.print("\nRShFV:");Serial.print("CV:"); Serial.print(crispValue);Serial.print("FV:"); Serial.print(fuzzyValue);Serial.print("\n");
	return fuzzyValue;
}

double leftShoulder(double crispValue, FuzzySetData *setData){
	double fuzzyValue = 0.0;
	if(crispValue >= setData->topCorner && crispValue <= setData->rightCorner){
			// Inside Triangle part
			// Calculated using the ratio of sides of similar triangles.
		fuzzyValue = (setData->rightCorner-crispValue) / (setData->rightCorner - setData->topCorner);
	}else if(crispValue < setData->topCorner){
			// Inside Shoulder
		fuzzyValue = 1.0;
	}else {
			// Outside Fuzzy Subset
		fuzzyValue = 0.0;
	}
	//Serial.print("\nLShFV:");Serial.print("CV:"); Serial.print(crispValue);Serial.print("FV:"); Serial.print(fuzzyValue);Serial.print("\n");
	return fuzzyValue;
}

/*
 * 1.0    T
 *       / \
 *      /   \
 * 0.0 L.....R
 *
 */
double triangleWeight(double fuzzyValue, FuzzySetData *setData, double *fuzzyStation){
	*fuzzyStation = setData->topCorner;
	//Serial.print("\nTriangleWt:");Serial.print("RC:"); Serial.print(setData->rightCorner);Serial.print("TC:"); Serial.print(setData->topCorner);Serial.print("FV:");Serial.print(fuzzyValue);
	//Serial.print("Wt:");Serial.print((setData->rightCorner - setData->topCorner) * fuzzyValue);Serial.print("\n");
	return (setData->rightCorner - setData->topCorner) * fuzzyValue;
}

/*
 * 1.0    T
 *       /
 *      /
 * 0.0 L...
 *
 */
double linearWeight(double fuzzyValue, FuzzySetData *setData, double *fuzzyStation){
	double result =  (setData->topCorner - setData->leftCorner) * fuzzyValue;
	*fuzzyStation =  result;
	return result;
}
