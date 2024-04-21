
#include "Arduino.h"
#include "TempProbe.h"

TempProbe::TempProbe (char type, int pin){
	this->type = type;
	this->pin = pin;
}

char TempProbe::getType(){
	return this->type;
}
double TempProbe::getTemperature() {
	return temperature;
}

#define TEMPAVGLEN 2
void TempProbe::readProbe() {
	double instTemp ;
	for(int i = 0; i< TEMPAVGLEN;i++){
			instTemp = (double)(((double)analogRead(this->pin)*1000.0)/1023.0);
			temperature = (temperature*(double)(TEMPAVGLEN-1)+instTemp)/(double)TEMPAVGLEN;
	}
}

void TempProbe::update() {
	this->readProbe();
}
