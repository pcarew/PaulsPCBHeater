#ifndef TempDisplay_H
#define TempDisplay_H
#include <arduino.h>
#include "Button.h"
#include "Display.h"
#include "TempProbe.h"
#include "Menu.h"

#define AMBIANTSENSOR A0
#define BRDBOT_SENSOR A1
#define BRDTOP_SENSOR A2


class TempMonitoring: Implements MenuAction{
private:
public:

	static TempProbe ambient;
	static TempProbe brdBot;
	static TempProbe brdTop;


//	TempDisplay();
	void menuAction(int param) ;
//	void rotaryAction(const int type, int level, RSE::Dir direction, int param);		// type is ROTATE or SELECT
	void update();

//	~TempDisplay() {}
};

#endif
