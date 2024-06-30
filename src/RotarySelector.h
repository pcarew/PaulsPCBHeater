
#ifndef RotarySelector_H
#define RotarySelector_H

#include "Arduino.h"
#include "Extensions.h"
#include "Button.h"

#define ROTDBAMOUNT 3					// 5ms for rotary debounce
#define SELDBAMOUNT 36					// 36ms for sector debounce

#define GetNextState(value) ((uint8_t)((uint8_t)value & (uint8_t)0x0F))	//((uint8_t)(value & 0x0F))

#define GetAction(value)((uint8_t)((uint8_t)value & (uint8_t)0xF0)>>4)
#define ANS(action,nextState)((uint8_t)((uint8_t)action<<(uint8_t)4|(uint8_t)nextState))		// Pack action and nextstate into single byte

typedef struct _RSE{
	enum Dir{			// Actions
	FW = 1,
	RV = 2,
	NC = 15				// No Change
	};

	enum State{
		FADBD = 0,
		FADBU = 1,
		FAUBD = 2,
		AUBU = 3,

		RAUBD = 4,
		RADBD = 5,
		RADBU = 6,

		NOCH = 7
	};

	enum Event{
		AU = 0,
		AD = 1,
		BU = 2,
		BD = 3,
		ER = 15			// Unrecognized event
	};
	uint8_t actionNextState;		// DirAction is in upper Nibble, NextState is in lower nibble

}RSE;		//RotaryStateEvent Action

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
	uint8_t state; // = (int8_t)RSE::State::AUBU;					// Initial assumed state
	uint8_t dir;   // = (int8_t)RSE::Dir::FW;							// Initial assumed direction

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
