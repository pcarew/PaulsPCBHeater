
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
}

void RotarySelector::buttonAction(ButtonAction::Level level, int pinSelector) {		// This is called whenever the rotary selector is rotated causing an interrupt
//	Serial.print("Rot BA:");Serial.print(pinSelector);Serial.print(F(" Level:"));Serial.println(level);
	switch(pinSelector){
	case RotarySelector::S:
//		Serial.println("Selector");
		this->rotaryAction->rotaryAction(RotaryAction::SELECT,level,RSE::NC,this->rotaryParam);	// Notify client of this Rotary Selector *** Called within ISR ***
		break; // End of selector handling
	case RotarySelector::A:
	case RotarySelector::B:
		this->tick();
		if(this->dir != RSE::NC)
				this->rotaryAction->rotaryAction(RotaryAction::ROTATE,(int)NULL,this->dir,this->rotaryParam);		// Notify client of this Rotary Selector *** Called within ISR ***
		break; // End of Rotator handling
	}
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
	     } else {
	    	 this->dir = RSE::Dir::RV;
	     }
	   }else{
//		 Serial.println(F("No AChange"));
	   	 this->dir = RSE::Dir::NC;
	   }
	   aLastState = aState; // Updates the previous state of the outputA with the current state

}

// FSM version
/*
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

RSE RotarySelector::rotaryFSM[4][4] = {
		//ADBD								ADBU								AUBD								AUBU
		{{RSE::Dir::FW,RSE::State::AUBD},	{RSE::Dir::RV,RSE::State::AUBU},	{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::NC,RSE::State::NOCH}}, //Event AU
		{{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::RV,RSE::State::ADBD},	{RSE::Dir::FW,RSE::State::ADBU}}, //Event AD
		{{RSE::Dir::RV,RSE::State::ADBU},	{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::FW,RSE::State::AUBU},	{RSE::Dir::NC,RSE::State::NOCH}}, //Event BU
		{{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::FW,RSE::State::ADBD},	{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::RV,RSE::State::AUBD}}  //Event BD
};
void RotarySelector::rotaryFSM(ButtonAction::Level level, int pinSelector){
		RSE::Event event = RSE::Event::ER;
		RSE::Dir newDir = RSE::Dir::NC;
		RSE::State nextState = RSE::State::NOCH;
		static int counter  = 0;
		static int counterMax  = 0;

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
			newDir = RotarySelector::rotaryFSM[(int)event][(int)this->state].dir;
			nextState = RotarySelector::rotaryFSM[(int)event][(int)this->state].nextState;
//				Serial.print("SE:");Serial.print(this->state);Serial.print(event);Serial.println(nextState);

			if(nextState != RSE::State::NOCH)
				this->state = nextState;
			if(newDir != RSE::Dir::NC)
				this->dir = newDir;
			if(event == RSE::Event::AU || event == RSE::Event::AD){
				switch(this->dir){
					case RSE::Dir::FW: counter++; break;
					case RSE::Dir::RV: counter--; break;
					case RSE::Dir::NC:
//						Serial.println("EV NC");
						break;
				}
			}

			if(counter > counterMax) counter = counterMax;
			if(counter < 0) counter = 0;

//			this->rotaryAction->rotaryAction(this->counter,this->dir, (((int)event)<<8) | (((int)this->state)));
//			this->rotaryAction->rotaryAction(RotaryAction::ROTATE,NULL,this->dir,this->rotaryParam);		// Notify client of this Rotary Selector *** Called within ISR ***
		}
}
*/
