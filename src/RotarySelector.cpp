
#include "Arduino.h"
#include "Button.h"
#include "RotarySelector.h"

//#define MAXQEV 20

char const *state_name[] =
{
		"ADBD",
		"ADBU",
		"AUBD",
		"AUBU",
		"NOCH"
};
char const *event_name[] =
{
		"AU",
		"AD",
		"BU",
		"BD",
		"ER" // Unrecognized event
};

//char rotQ[MAXQEV];
//char qPtr = 0;

RSE RotarySelector::rotaryFSM[4][4] = {
		//ADBD								ADBU								AUBD								AUBU
		{{RSE::Dir::FW,RSE::State::AUBD},	{RSE::Dir::RV,RSE::State::AUBU},	{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::NC,RSE::State::NOCH}}, //Event AU
		{{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::RV,RSE::State::ADBD},	{RSE::Dir::FW,RSE::State::ADBU}}, //Event AD
		{{RSE::Dir::RV,RSE::State::ADBU},	{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::FW,RSE::State::AUBU},	{RSE::Dir::NC,RSE::State::NOCH}}, //Event BU
		{{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::FW,RSE::State::ADBD},	{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::RV,RSE::State::AUBD}}  //Event BD
};

RotarySelector::RotarySelector(int portDpinA, int portDpinB, int portDselectorPin, RotaryAction *action, int param , int max){
		this->rotaryAction = action;
		this->rotaryParam = param;
		this->aPin = portDpinA;
		this->bPin = portDpinB;
		this->sPin= portDselectorPin;
		this->a = new PortDButton(portDpinA,this,A,ROTDBAMOUNT);
		this->b = new PortDButton(portDpinB,this,B,ROTDBAMOUNT);
		this->selector = new PortDButton(portDselectorPin,this,S,SELDBAMOUNT);
		this->counterMax = max;
}

void RotarySelector::notificationRegister(RotaryAction *requester){
	this->rotaryAction = requester;
}
void RotarySelector::deRegister(){
	this->rotaryAction = NULL;
}

void RotarySelector::buttonAction(ButtonAction::Level level, int pinSelector) {		// This is called whenever the rotary selector is rotated causing an interrupt
//	Serial.print("MBA:");Serial.println(pinSelector);
	switch(pinSelector){
	case S:
//		Serial.println("Selector");
		this->rotaryAction->rotaryAction(RotaryAction::SELECT,level,RSE::NC,this->rotaryParam);	// Notify client of this Rotary Selector *** Called within ISR ***
		return; // End of selector handling
	}
	this->tick();

	/*
		RSE::Event event = RSE::Event::ER;
		RSE::Dir newDir = RSE::Dir::NC;
		RSE::State nextState = RSE::State::NOCH;

		switch(pinSelector){
			case A:
				switch(level){
					case ButtonAction::Level::BUTTONHIGH:
						event = RSE::Event::AU; break;
					case ButtonAction::Level::BUTTONLOW:
						event = RSE::Event::AD; break;
				}
				break;
			case B:
				switch(level){
					case ButtonAction::Level::BUTTONHIGH:
						event = RSE::Event::BU; break;
					case ButtonAction::Level::BUTTONLOW:
						event = RSE::Event::BD; break;
				}
				break;
		}

//		rotQ[(int)qPtr++] = (char)event;
//		qPtr %= MAXQEV;

		if(event != RSE::Event::ER){
			newDir = RotarySelector::rotaryFSM[(int)event][(int)this->state].dir;
			nextState = RotarySelector::rotaryFSM[(int)event][(int)this->state].nextState;
//				Serial.print("SE:");Serial.print(this->state);Serial.print(event);Serial.println(nextState);

			if(nextState != RSE::State::NOCH)
				this->state = nextState;
			if(newDir != RSE::Dir::NC)
				this->dir = newDir;
			if(event == RSE::Event::AU || event == RSE::Event::AD){
				switch(this->dir){
					case RSE::Dir::FW: this->counter++; break;
					case RSE::Dir::RV: this->counter--; break;
					case RSE::Dir::NC:
//						Serial.println("EV NC");
						break;
				}
			}

			if(this->counter > this->counterMax) this->counter = this->counterMax;
			if(this->counter < 0) this->counter = 0;

			this->rotaryAction->rotaryAction(this->counter,this->dir, (((int)event)<<8) | (((int)this->state)));
*/
			this->rotaryAction->rotaryAction(RotaryAction::ROTATE,this->counter,this->dir,this->rotaryParam);		// Notify client of this Rotary Selector *** Called within ISR ***
//		}
		/*
		if(this->prevA != this->lastA ){
			if(this->lastA != this->lastB){
				this->counter++;
				this->dir = +1;
			}else{
				this->counter--;
				this->dir = -1;
			}
		}
		this->prevA = this->lastA;
		if(this->counter < 0) this->counter = 0;
		else if(this->counter > this->counterMax) this->counter = this->counterMax;

//		this->rotaryAction->rotaryAction(this->counter,this->dir,this->rotaryParam);
		this->rotaryAction->rotaryAction(this->counter,this->dir,this->lastA<<8|this->prevA);
		*/
		/*
		la = a;
		if(counter < 0) counter = 0;
		else if(counter > FRAMESIZE) counter = FRAMESIZE;

	}

	if(la !=a ){
		if(a != b){
			counter++;
			dir = +1;
		}else{
			counter--;
			dir = -1;
		}
	}
	la = a;
	if(counter < 0) counter = 0;
	*/
}

/*tick*/
void RotarySelector::tick(){
	static int aState;
	static int aLastState;
	 aState = digitalRead(this->aPin); // Reads the "current" state of the outputA
	   // If the previous and the current state of the outputA are different, that means a Pulse has occured
	   if (aState != aLastState){
	     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
	     if (digitalRead(this->bPin) != aState) {
	    	 this->dir = RSE::Dir::FW;
	       this->counter ++;
	     } else {
	    	 this->dir = RSE::Dir::RV;
	       this->counter --;
	     }
	   }
	   aLastState = aState; // Updates the previous state of the outputA with the current state

	   if(this->counter > this->counterMax) this->counter = this->counterMax;
	   if(this->counter < 0) this->counter = 0;

}
