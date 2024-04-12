
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

void TempProbe::readProbe() {
	double instTemp ;
	for(int i = 0; i< 50;i++){
			instTemp = (double)(((double)analogRead(this->pin)*1000.0)/1023.0);
			temperature = (temperature*49.0+instTemp)/50.0;
	}
}

void TempProbe::update() {
	this->readProbe();
}
