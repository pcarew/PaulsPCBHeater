
#include "TemperatureProbe.h"

#include "Arduino.h"

TemperatureProbe::TemperatureProbe (char type, int pin){
	this->type = type;
	this->pin = pin;
}

char TemperatureProbe::getType(){
	return this->type;
}
double TemperatureProbe::getTemperature() {
	return temperature;
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
