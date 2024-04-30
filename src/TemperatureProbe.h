#ifndef TempProbe_H
#define TempProbe_H
#include "Arduino.h"

class TemperatureProbe {
	private:
	char type;
	int pin;
	double temperature=0.0 ;

	public:
	TemperatureProbe (char type, int pin);
	char getType();
	double getTemperature();
	void readProbe() ;
	void update();

};
#endif
