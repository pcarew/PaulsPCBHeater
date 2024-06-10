
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
typedef struct {
   char description [6];
} descriptionType;

const descriptionType state_name[] PROGMEM =
{
		"ADBD",
		"ADBU",
		"AUBD",
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

const RSE RotarySelector::rotaryFSM[4][7] PROGMEM = {
		//ADBD								ADBU								AUBD								AUBU
		/*
		{{RSE::Dir::FW,RSE::State::AUBD},	{RSE::Dir::RV,RSE::State::AUBU},	{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::NC,RSE::State::NOCH}}, //Event AU
		{{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::RV,RSE::State::ADBD},	{RSE::Dir::FW,RSE::State::ADBU}}, //Event AD
		{{RSE::Dir::RV,RSE::State::ADBU},	{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::FW,RSE::State::AUBU},	{RSE::Dir::NC,RSE::State::NOCH}}, //Event BU
		{{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::FW,RSE::State::ADBD},	{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::RV,RSE::State::AUBD}}  //Event BD
		*/
		// Separate states for forward vs Reverse
		//FADBD								FADBU								FAUBD								AUBU								RRAUBD							RADBD							RADBU
//		{{RSE::Dir::NC,RSE::State::FAUBD},	{RSE::Dir::NC,RSE::State::AUBU},	{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::NC,RSE::State::NOCH}, {RSE::Dir::NC,RSE::State::NOCH}, {RSE::Dir::NC,RSE::State::RAUBD}, {RSE::Dir::RV,RSE::State::AUBU} },  //Event AU
//		{{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::NC,RSE::State::FADBD},	{RSE::Dir::NC,RSE::State::FADBU},{RSE::Dir::NC,RSE::State::RADBD}, {RSE::Dir::NC,RSE::State::NOCH}, {RSE::Dir::NC,RSE::State::NOCH} },  //Event AD
//		{{RSE::Dir::NC,RSE::State::FADBU},	{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::FW,RSE::State::AUBU},	{RSE::Dir::NC,RSE::State::NOCH}, {RSE::Dir::NC,RSE::State::AUBU}, {RSE::Dir::NC,RSE::State::RADBU}, {RSE::Dir::NC,RSE::State::NOCH} },  //Event BU
//		{{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::NC,RSE::State::FADBD},	{RSE::Dir::NC,RSE::State::NOCH},	{RSE::Dir::NC,RSE::State::RAUBD},{RSE::Dir::NC,RSE::State::NOCH}, {RSE::Dir::NC,RSE::State::NOCH}, {RSE::Dir::NC,RSE::State::RADBD} }   //Event BD

		{{(int8_t)RSE::Dir::NC,(int8_t)RSE::State::FAUBD},	{(int8_t)RSE::Dir::NC,(int8_t)RSE::State::AUBU},	{(int8_t)RSE::Dir::NC,(int8_t)RSE::State::NOCH},	{(int8_t)RSE::Dir::NC,(int8_t)RSE::State::NOCH}, {(int8_t)RSE::Dir::NC,(int8_t)RSE::State::NOCH}, {(int8_t)RSE::Dir::NC,(int8_t)RSE::State::RAUBD}, {(int8_t)RSE::Dir::RV,(int8_t)RSE::State::AUBU} },  //Event AU
		{{(int8_t)RSE::Dir::NC,(int8_t)RSE::State::NOCH},	{(int8_t)RSE::Dir::NC,(int8_t)RSE::State::NOCH},	{(int8_t)RSE::Dir::NC,(int8_t)RSE::State::FADBD},	{(int8_t)RSE::Dir::NC,(int8_t)RSE::State::FADBU},{(int8_t)RSE::Dir::NC,(int8_t)RSE::State::RADBD}, {(int8_t)RSE::Dir::NC,(int8_t)RSE::State::NOCH}, {(int8_t)RSE::Dir::NC,(int8_t)RSE::State::NOCH} },  //Event AD
		{{(int8_t)RSE::Dir::NC,(int8_t)RSE::State::FADBU},	{(int8_t)RSE::Dir::NC,(int8_t)RSE::State::NOCH},	{(int8_t)RSE::Dir::FW,(int8_t)RSE::State::AUBU},	{(int8_t)RSE::Dir::NC,(int8_t)RSE::State::NOCH}, {(int8_t)RSE::Dir::NC,(int8_t)RSE::State::AUBU}, {(int8_t)RSE::Dir::NC,(int8_t)RSE::State::RADBU}, {(int8_t)RSE::Dir::NC,(int8_t)RSE::State::NOCH} },  //Event BU
		{{(int8_t)RSE::Dir::NC,(int8_t)RSE::State::NOCH},	{(int8_t)RSE::Dir::NC,(int8_t)RSE::State::FADBD},	{(int8_t)RSE::Dir::NC,(int8_t)RSE::State::NOCH},	{(int8_t)RSE::Dir::NC,(int8_t)RSE::State::RAUBD},{(int8_t)RSE::Dir::NC,(int8_t)RSE::State::NOCH}, {(int8_t)RSE::Dir::NC,(int8_t)RSE::State::NOCH}, {(int8_t)RSE::Dir::NC,(int8_t)RSE::State::RADBD} }   //Event BD
};
void RotarySelector::handleEvent(ButtonAction::Level level, int pinSelector){
		RSE::Event event = RSE::Event::ER;
		int8_t newDir = RSE::Dir::NC;				// RSE::Dir
		int8_t nextState = RSE::State::NOCH;		// RSE::State
		RSE oneItem ;

//		Serial.print(F("Sizeof RSE:"));Serial.println(sizeof(RSE));
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
			    memcpy_P(&oneItem,
			    		&RotarySelector::rotaryFSM[(int)event][(int)this->state],
						sizeof(RSE));
			newDir = oneItem.dir;
			nextState = oneItem.nextState;
//			newDir = pgm_read_word(&RotarySelector::rotaryFSM[(int)event][(int)this->state].dir);
//			nextState = pgm_read_word(&RotarySelector::rotaryFSM[(int)event][(int)this->state].nextState);
//				Serial.print(F("SE:"));Serial.print(this->state);Serial.print(event);Serial.println(nextState);

			if(nextState != RSE::State::NOCH)
				this->state = nextState;
//			if(newDir != RSE::Dir::NC)
				this->dir = newDir;
//				Serial.print(event_name[event]);//Serial.print(F(","));
			/*
			if(event == RSE::Event::AU || event == RSE::Event::AD){
				switch(this->dir){
					case RSE::Dir::FW: counter++; break;
					case RSE::Dir::RV: counter--; break;
					case RSE::Dir::NC:
//						Serial.println(F("EV NC"));
						break;
				}
			}

			if(counter > counterMax) counter = counterMax;
			if(counter < 0) counter = 0;
			*/

//			this->rotaryAction->rotaryAction(this->counter,this->dir, (((int)event)<<8) | (((int)this->state)));
//			this->rotaryAction->rotaryAction(RotaryAction::ROTATE,NULL,this->dir,this->rotaryParam);		// Notify client of this Rotary Selector *** Called within ISR ***
		}
}

