#ifndef TempDisplay_H
#define TempDisplay_H
#include <arduino.h>
#include "Button.h"
#include "Display.h"
#include "Menu.h"
#include "TemperatureProbe.h"

#define AMBIANTSENSOR A0
#define BRDBOT_SENSOR A1
#define BRDTOP_SENSOR A2


class TemperatureMonitoring: Implements MenuAction{
private:
public:

	static TemperatureProbe ambient;
	static TemperatureProbe brdBot;
	static TemperatureProbe brdTop;


	void menuAction(int param) ;
//	void rotaryAction(const int type, int level, RSE::Dir direction, int param);		// type is ROTATE or SELECT
	void update();

};

#endif
