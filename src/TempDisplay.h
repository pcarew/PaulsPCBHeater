#include <arduino.h>
#include "Button.h"
#include "Display.h"
#include "TempProbe.h"
#include "Menu.h"

#define AMBIANTSENSOR A0
#define BRDBOT_SENSOR A1
#define BRDTOP_SENSOR A2

extern bool cancelled;
extern Display *myDisp;
extern volatile unsigned long time;
extern unsigned long nextDisplayTime;

class TempDisplay: Implements MenuAction{
private:
//	const char tmpFmt[16] = "%s: %d.%dC";
public:

	TempProbe *ambiant;
	TempProbe *brdBot;
	TempProbe *brdTop;


	TempDisplay();
	void menuAction(int param) ;
	void rotaryAction(const int type, int level, RSE::Dir direction, int param);		// type is ROTATE or SELECT
//	void displayTemps(int param) ;
	void update();

	~TempDisplay() {}
};
