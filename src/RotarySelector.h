
#ifndef RotarySelector_H
#define RotarySelector_H

#include "Arduino.h"
#include "Extensions.h"
#include "Button.h"

#define ROTDBAMOUNT 3					// 5ms for rotary debounce
#define SELDBAMOUNT 36					// 36ms for sector debounce

typedef struct _RSE{
	enum Dir{
	FW = 1,
	RV = 2,
	NC = -1				// No Change
	};

	enum State{
		FADBD = 0,
		FADBU = 1,
		FAUBD = 2,
		AUBU = 3,

		RAUBD = 4,
		RADBD = 5,
		RADBU = 6,

		NOCH = -1
	};

	enum Event{
		AU = 0,
		AD = 1,
		BU = 2,
		BD = 3,
		ER = -1			// Unrecognized event
	};
	/*
	Dir dir;
	State nextState;
	*/
	int8_t dir;				// byte version of 'DIR'
	int8_t nextState;		// byte version of 'State'
}RSE;		//RotaryStateEvent

Interface RotaryAction{
private:
public:
	static const int ROTATE = 3;
	static const int SELECT = 4;

	virtual ~RotaryAction(){}
	virtual void rotaryAction(const int type, int level, RSE::Dir direction, int param) =0 ;		// type is ROTATE or SELECT
};

class RotarySelector : Implements ButtonAction{
private:
public:
	static const int A = 1;
	static const int B = 2;
	static const int S = 3;

	PortDButton  *a;
	PortDButton  *b;
	PortDButton  *selector;
	int aPin;
	int bPin;
	int sPin;		// Selector Pin number
	int lastA = 0;
	int lastB = 0;
	int prevA = 0;
	int prevB = 0;
	RotaryAction *rotaryAction;
	int rotaryParam;

	/*
	RSE::State state = RSE::State::FADBD;						// Initial assumed state
	RSE::Dir dir = RSE::Dir::FW;								// Initial assumed direction
	*/
	int8_t state = (int8_t)RSE::State::FADBD;					// Initial assumed state
	int8_t dir = (int8_t)RSE::Dir::FW;							// Initial assumed direction

	static const RSE rotaryFSM[4][7];

	private:
	public:

	RotarySelector(int portDpinA, int portDpinB, int portDselectorPin, RotaryAction *action, int param );
	~RotarySelector(){}

	void handleEvent(ButtonAction::Level level, int pinSelector);
	void buttonAction(ButtonAction::Level level, int param);
	void tick();
};

// Pin Change Ports https://dronebotworkshop.com/interrupts/

#endif
