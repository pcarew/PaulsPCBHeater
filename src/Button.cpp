#include "Arduino.h"
#include "Button.h"
#include "SystemData.h"

	void PCINTButton::intCheck(unsigned portState, unsigned long timestamp){

		if(timestamp > this->debounceTimeout ){									// Handle wrap-around/overflow??
			this->debounceTimeout = (unsigned long)((unsigned long)timestamp+(unsigned long)this->debounceAmount);

			// Verify against lastPinState XOR??
			int pinState  = (portState & this->pcmskBit);
			bool pinChanged  = (pinState  ^ this->lastPinState)>0?true:false;			// Bit pos value turned into true/false
			if(pinChanged == true){
				this->buttonAction->buttonAction( (pinState>0?ButtonAction::Level::BUTTONHIGH : ButtonAction::Level::BUTTONLOW), this->param);
				this->lastPinState = pinState;				// Save new state
			}
		}
	}

	// Port B PCINT
	int PortBButton::portButtonCount = 0;
	PCINTButton *PortBButton::portButtons[MAXBUTTONS]= {NULL,NULL,NULL,NULL,NULL};
	void PortBButton::install(){ // PortB	PCINT0 - PCINT5		PCMSK0 bits 0-5 D8 - D13
			int pinId = this->pinId ;
			pinMode(pinId, INPUT_PULLUP);
			this->pcmskBit = (1<<(pinId-D8));
			this->lastPinState = PINB&this->pcmskBit;
			PCMSK0 = PCMSK0 | this->pcmskBit;
			PCICR = PCICR | (1<<PCIE0);
			PortBButton::portButtons[PortBButton::portButtonCount++] = this;
	}

	void PortBButton::buttonCheck(unsigned portState,unsigned long timestamp){
//		unsigned portState = PINB;
		for(int buttonId =0 ;buttonId<MAXBUTTONS;buttonId++){
			PCINTButton *button = PortBButton::portButtons[buttonId];
			if(button != NULL) button->intCheck(portState, timestamp);
			else break;													// Stop at 1st null
		}
	}

	// Port C PCINT
	int PortCButton::portButtonCount = 0;
	PCINTButton *PortCButton::portButtons[MAXBUTTONS]= {NULL,NULL,NULL,NULL,NULL};
	void PortCButton::install(){		// PortC	PCINT8 - PCINT13		PCMSK1 bits 0-5 A0 - A5
			int pinId = this->pinId ;
			pinMode(pinId, INPUT_PULLUP);
			this->pcmskBit = (1<<(pinId-A0));
			this->lastPinState = PINC&this->pcmskBit;
			PCMSK1 = PCMSK1 | this->pcmskBit;
			PCICR = PCICR | (1<<PCIE1);
			PortCButton::portButtons[PortCButton::portButtonCount++] = this;
	}

	void PortCButton::buttonCheck(unsigned portState,unsigned long timestamp){
//		unsigned portState = PINC;
		for(int buttonId =0 ;buttonId<MAXBUTTONS;buttonId++){
			PCINTButton *button = PortCButton::portButtons[buttonId];
			if(button != NULL) button->intCheck(portState, timestamp);
			else break;													// Stop at 1st null
		}
	}



	// Port D PCINT
	int PortDButton::portButtonCount = 0;
	PCINTButton *PortDButton::portButtons[MAXBUTTONS]= {NULL,NULL,NULL,NULL,NULL};

	void PortDButton::install(){			// PortD	PCINT16 - PCINT23		PCMSK2 bits 0-7 D0 -D7
				// Port D		D0 - D7
			int pinId = this->pinId ;
			pinMode(pinId, INPUT_PULLUP);
			this->pcmskBit = (1<<(pinId-D0));
			this->lastPinState = PIND&this->pcmskBit;		// ?? AJPC
			PCMSK2 = PCMSK2 | this->pcmskBit;
			PCICR = PCICR | (1<<PCIE2);
			PortDButton::portButtons[PortDButton::portButtonCount++] = this;
	}

	void PortDButton::buttonCheck(unsigned portState,unsigned long timestamp){
		for(int buttonId =0 ;buttonId<MAXBUTTONS;buttonId++){
			PCINTButton *button = PortDButton::portButtons[buttonId];
			if(button != NULL) button->intCheck(portState, timestamp);
			else{
				break;													// Stop at 1st null
			}
		}
	}
