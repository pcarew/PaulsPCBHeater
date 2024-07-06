
#include "Arduino.h"
#include "TemperatureProbe.h"
#include "PaulsPCBHeater.h"

TemperatureProbe::TemperatureProbe (char type, int pin){
	this->type = type;
	this->pin = pin;
}

char TemperatureProbe::getType(){
	return this->type;
}
double TemperatureProbe::getTemperature() {
#ifdef PCBTEST
			return 55.0;
#else
	return temperature;
#endif
}

#define TEMPAVGLEN 2
void TemperatureProbe::readProbe() {
	double instTemp ;
	for(int i = 0; i< TEMPAVGLEN;i++){
			instTemp = (double)(((double)analogRead(this->pin)*1000.0)/1023.0);
			temperature = (temperature*(double)(TEMPAVGLEN-1)+instTemp)/(double)TEMPAVGLEN;
	}
}

void TemperatureProbe::update() {
	this->readProbe();
}
