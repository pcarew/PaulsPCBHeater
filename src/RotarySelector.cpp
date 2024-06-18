
#include "Arduino.h"
#include "Button.h"
#include "RotarySelector.h"

RotarySelector::RotarySelector(int portDpinA, int portDpinB, int portDselectorPin, RotaryAction *action, int param ){
		this->rotaryAction = action;
		this->rotaryParam = param;
		this->aPin = portDpinA;
		this->bPin = portDpinB;
		this->sPin= portDselectorPin;
		this->a = new PortDButton(portDpinA,this,RotarySelector::A,ROTDBAMOUNT);
		this->b = new PortDButton(portDpinB,this,RotarySelector::B,ROTDBAMOUNT);
		this->selector = new PortDButton(portDselectorPin,this,RotarySelector::S,SELDBAMOUNT);
		this->state = (int8_t)RSE::State::AUBU;
		this->dir = (int8_t)RSE::Dir::FW;							// Initial assumed direction
}

void RotarySelector::buttonAction(ButtonAction::Level level, int pinSelector) {		// This is called whenever the rotary selector is rotated causing an interrupt
	switch(pinSelector){
	case RotarySelector::S:
		this->rotaryAction->rotaryAction(RotaryAction::SELECT,level,RSE::NC,this->rotaryParam);	// Notify client of this Rotary Selector *** Called within ISR ***
		break; // End of selector handling
	case RotarySelector::A:
	case RotarySelector::B:
		this->handleEvent(level, pinSelector);
//		this->tick();
		if(this->dir != RSE::NC)
				this->rotaryAction->rotaryAction(RotaryAction::ROTATE,(int)NULL,(RSE::Dir)this->dir,this->rotaryParam);		// Notify client of this Rotary Selector *** Called within ISR ***
		break; // End of Rotator handling
		break;
	}
}

/*tick*/
/*
void RotarySelector::tick(){
			int aState;
	static	int aLastState;
	 aState = digitalRead(this->aPin); // Reads the "current" state of the outputA
	   // If the previous and the current state of the outputA are different, that means a Pulse has occured
	   if (aState != aLastState){
	     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
	     if (digitalRead(this->bPin) != aState) {
	    	 this->dir = RSE::Dir::FW;
	     } else {
	    	 this->dir = RSE::Dir::RV;
	     }
	   }else{
//		 Serial.println(F("No AChange"));
	   	 this->dir = RSE::Dir::NC;
	   }
	   aLastState = aState; // Updates the previous state of the outputA with the current state

}
*/

// FSM version

/*
const char *state_name[] =
{
		"FADBD",
		"FADBU",
		"FAUBD",
		"AUBU",
		"RAUBD",
		"RADBD",
		"RADBU",
		"NOCH"
};

const char *event_name[] = {
		"A",
		"a",
		"B",
		"b",
		"ER" // Unrecognized event
};
*/

const RSE RotarySelector::rotaryFSM[4][7] = {
		// Separate states for forward vs Reverse
		//FADBD									FADBU									FAUBD									AUBU									RRAUBD								RADBD									RADBU
		{{ANS(RSE::Dir::NC,RSE::State::FAUBD)},	{ANS(RSE::Dir::NC,RSE::State::AUBU)},	{ANS(RSE::Dir::NC,RSE::State::NOCH)},	{ANS(RSE::Dir::NC,RSE::State::NOCH)}, {ANS(RSE::Dir::NC,RSE::State::NOCH)}, {ANS(RSE::Dir::NC,RSE::State::RAUBD)}, {ANS(RSE::Dir::RV,RSE::State::AUBU)} },  //Event AU
		{{ANS(RSE::Dir::NC,RSE::State::NOCH)},	{ANS(RSE::Dir::NC,RSE::State::NOCH)},	{ANS(RSE::Dir::NC,RSE::State::FADBD)},	{ANS(RSE::Dir::NC,RSE::State::FADBU)},{ANS(RSE::Dir::NC,RSE::State::RADBD)}, {ANS(RSE::Dir::NC,RSE::State::NOCH)}, {ANS(RSE::Dir::NC,RSE::State::NOCH)} },  //Event AD
		{{ANS(RSE::Dir::NC,RSE::State::FADBU)},	{ANS(RSE::Dir::NC,RSE::State::NOCH)},	{ANS(RSE::Dir::FW,RSE::State::AUBU)},	{ANS(RSE::Dir::NC,RSE::State::NOCH)}, {ANS(RSE::Dir::NC,RSE::State::AUBU)}, {ANS(RSE::Dir::NC,RSE::State::RADBU)}, {ANS(RSE::Dir::NC,RSE::State::NOCH)} },  //Event BU
		{{ANS(RSE::Dir::NC,RSE::State::NOCH)},	{ANS(RSE::Dir::NC,RSE::State::FADBD)},	{ANS(RSE::Dir::NC,RSE::State::NOCH)},	{ANS(RSE::Dir::NC,RSE::State::RAUBD)},{ANS(RSE::Dir::NC,RSE::State::NOCH)}, {ANS(RSE::Dir::NC,RSE::State::NOCH)}, {ANS(RSE::Dir::NC,RSE::State::RADBD)} }   //Event BD
};

void RotarySelector::handleEvent(ButtonAction::Level level, int pinSelector){
		RSE::Event event = RSE::Event::ER;
		uint8_t newDir = RSE::Dir::NC;				// RSE::Dir
		uint8_t nextState = RSE::State::NOCH;		// RSE::State
		RSE oneItem ;

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


		if(event != RSE::Event::ER){
			oneItem = RotarySelector::rotaryFSM[(int)event][(int)this->state];
			newDir = GetAction(oneItem.actionNextState);
			nextState = GetNextState(oneItem.actionNextState);

//			Serial.print(event_name[event]);//Serial.print(F(","));
//			Serial.print(F(" CurrSt:"));Serial.print(this->state);Serial.print(F(" Ev:"));Serial.print(event_name[event]); Serial.print(F(" Act:"));Serial.print(newDir); Serial.print(F(" NxtSt:"));Serial.println(nextState);

			if(nextState != RSE::State::NOCH) this->state = nextState;
			this->dir = newDir;
		}
}

// Testing Git
