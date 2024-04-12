
#ifndef Button_H
#define Button_H
#include "Arduino.h"
#include "Extensions.h"

#define D0 0
#define D1 1
#define D2 2
#define D3 3
#define D4 4
#define D5 5
#define D6 6
#define D7 7
#define D8 8
#define D9 9
#define D10 10
#define D11 11
#define D12 12
#define D13 13

#define abstract

#define DBTIME ((unsigned long)80)				// In MicroSeconds
//#define DBTIME ((long)350000)

#define MAXBUTTONS 8

Interface ButtonAction{
private:
public:
	enum Level{
		BUTTONHIGH = 1,
		BUTTONLOW = 0
	};
	virtual ~ButtonAction(){}
	virtual void buttonAction(Level level, int param) =0 ;
};

abstract class Button {
private:
public:
	Button(ButtonAction *action, int param):Button(action,param,DBTIME){}
	Button(ButtonAction *action, int param, unsigned long dbAmount) {
		this->buttonAction = action;
		this->param = param;
		this->debounceAmount = dbAmount;
	}
	virtual ~Button(){}

	volatile unsigned long debounceTimeout = 0;
	unsigned long debounceAmount = 0;
	int param;
	ButtonAction *buttonAction;
};



class PolledPinButton : Button{
	private:
	public:
	PolledPinButton(int pinId, ButtonAction *action, int param) : PolledPinButton(pinId, action, param, DBTIME){}
	PolledPinButton(int pinId, ButtonAction *action, int param, unsigned long dbAmount) : Button(action, param, dbAmount){
		this->pinId = pinId;
	}
	static void buttonCheck(unsigned long timestamp);
	int pinId;
};

class IntPinButton : Button{
	private:
	public:
	IntPinButton(int pinId, ButtonAction *action, int param) : IntPinButton(pinId, action, param,DBTIME){}
	IntPinButton(int pinId, ButtonAction *action, int param, unsigned long dbAmount) : Button(action, param,dbAmount){
		this->pinId = pinId;
	}
	static void buttonCheck(unsigned long timestamp);
	int pinId;
};


// https://dronebotworkshop.com/interrupts/
// Pin Change Ports

/*
Interface PCINTHandler{
private:
public:
	virtual ~PCINTHandler(){}
	virtual void intCheck(unsigned portState, unsigned long timestamp) =0;
};
*/

//abstract class PCINTButton : public Button , Implements PCINTHandler{
abstract class PCINTButton : public Button{
	private:
	public:

	PCINTButton(int pinId, ButtonAction *action, int param,unsigned long dbAmount) : Button(action, param,dbAmount){
		this->pinId = pinId;
	}
	virtual void install() = 0;
	void intCheck(unsigned portState,unsigned long timestamp);
	int pinId = 0;
	int pcmskBit = 0;
	int lastPinState = 1;
};

class PortBButton : public PCINTButton{ // Port B		D8 -D13		PCINT0 - PCINT7		PCMSK0 bits 0-5
private:
public:
	static PCINTButton *portButtons[MAXBUTTONS];
	static int portButtonCount;
	static void buttonCheck(unsigned portState,unsigned long timestamp);
	PortBButton(int pinId, ButtonAction *action, int param): PortBButton(pinId,action,param,DBTIME){}
	PortBButton(int pinId, ButtonAction *action, int param, unsigned long dbamount): PCINTButton(pinId, action, param,dbamount) {
		this->install();
	}
	void install();
};

class PortCButton : public PCINTButton{ //Port C		A0 - A5		PCINT8 - PCINT14	PCMSK1 bits 0-5
private:
public:
	static PCINTButton *portButtons[MAXBUTTONS];
	static int portButtonCount;
	static void buttonCheck(unsigned portState,unsigned long timestamp);
	PortCButton(int pinId, ButtonAction *action, int param): PortCButton(pinId,action,param,DBTIME){}
	PortCButton(int pinId, ButtonAction *action, int param, unsigned long dbamount): PCINTButton(pinId, action, param,dbamount) {
		this->install();
	}
	void install();
};

class PortDButton : public PCINTButton{ // PortD		D0 - D7		PCINT16 - PCINT23	PCMSK2 bits 0-7
private:
public:
	static PCINTButton *portButtons[MAXBUTTONS];
	static int portButtonCount;
	static void buttonCheck(unsigned portState,unsigned long timestamp);
	PortDButton(int pinId, ButtonAction *action, int param): PortDButton(pinId,action,param,DBTIME){}
	PortDButton(int pinId, ButtonAction *action, int param, unsigned long dbamount): PCINTButton(pinId, action, param,dbamount) {
		this->install();
	}
	void install();
};

#endif
