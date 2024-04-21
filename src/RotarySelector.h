
#ifndef RotarySelector_H
#define RotarySelector_H

#include "Arduino.h"
#include "Extensions.h"
#include "Button.h"

#define ROTDBAMOUNT 15					// 5ms for rotary debounce
#define SELDBAMOUNT 36					// 36ms for sector debounce

typedef struct _RSE{
	enum Dir{
	FW = 1,
	RV = 2,
	NC = -1				// No Change
	};

	enum State{
		ADBD = 0,
		ADBU = 1,
		AUBD = 2,
		AUBU = 3,
		NOCH = -1
	};

	enum Event{
		AU = 0,
		AD = 1,
		BU = 2,
		BD = 3,
		ER = -1			// Unrecognized event
	};
	Dir dir;
	State nextState;
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

	RSE::State state = RSE::State::ADBD;						// Initial assumed state
	RSE::Dir dir = RSE::Dir::FW;								// Initial assumed direction

	static RSE rotaryFSM[4][4];

	private:
	public:

	RotarySelector(int portDpinA, int portDpinB, int portDselectorPin, RotaryAction *action, int param );
	~RotarySelector(){}
	void notificationRegister(RotaryAction *requester);
	void deRegister();

	void buttonAction(ButtonAction::Level level, int param);
	void tick();
};

// Pin Change Ports https://dronebotworkshop.com/interrupts/

#endif
