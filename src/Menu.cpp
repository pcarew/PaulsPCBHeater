#include "Menu.h"
#include "Ram.h"
#include "RotarySelector.h"

#define MENUITEMTEXTSIZE 2
IMPORT bool cancelled;

Menu::Menu(MenuItem *menu, int menuSize, int portDPinA, int portDPinB, int selectPin, Display *display ){
	this->menuItems				= menu;
	this->currentMenuItemPtr	= menu;
	this->nextMenuItemPtr		= menu;
	this->currentMenuItemId		= -1;
	this->nextMenuItemId		= 0;
	this->menuItemCount			= menuSize;
	this->display				= display;
	this->rotary	= new RotarySelector(portDPinA, portDPinB, selectPin, this, 5 ); // A:D5, B:D6 on PCBHeater 'selectPin' button on the rotary selector (D4 on PCBHeater)

//	this->menuLine	= new DisplayText(this->menuLineBuff,27,this->display, this->posCol , this->posRow, MENUITEMTEXTSIZE );
	this->menuLine	= new DisplayText(NULL,this->display, this->posCol , this->posRow, MENUITEMTEXTSIZE );

	menu->selected	= false;
//	this->showMenu(); // Not needed here AJPC
};

//extern Ram ramApp;
void Menu::showMenu(){

	cli();
	this->display->tftScreen.background(this->display->br, this->display->bg, this->display->bb);
	sei();

	for(int i=0;i<this->menuItemCount;i++){
		this->menuLine->setText((char*)(this->menuItems[i].prompt));
		this->menuLine->setRow(i);
		this->menuLine->setCol(2);
//		this->menuLine->setTextSize(MENUITEMTEXTSIZE);
		this->menuLine->show();
	}
	/*
	this->menuLine->setText((char*)(this->currentMenuItemPtr->prompt));
	this->menuLine->setRow(this->currentMenuItemId);
	this->menuLine->setCol(2);
	this->menuLine->invert();
	this->menuLine->show();
	this->menuLine->invert();
	*/
	this->showMenuLine();
}
/*
void Menu::checkTxtSize(char id){
//	Serial.print(F("Check Here:"));Serial.println(ramApp.freeRam());
	if(this == NULL || this->menuLine == NULL){
		Serial.println(F("Null MenuLine  Id:"));Serial.print(id);
		delay(10);
		while(1);
	}else if(this->menuLine->textSize != 2){
		Serial.print(F("MenuText Size Error Id:"));Serial.print(id);Serial.println(this->menuLine->textSize);
		delay(10);
		Serial.print(F("RAM Remaining:"));Serial.println(ramApp.freeRam());
		delay(10);
		while(1);
	}
}
*/

void Menu::menuInvoke(){ // Called from System thread
	if(this->currentMenuItemId < 0) this->showMenu();

	if(this->currentMenuItemPtr->selected == true){
//		Serial.print(F("Selecting: "));Serial.print(this->currentMenuItemId); Serial.println(this->currentMenuItemPtr->prompt);
		this->currentMenuItemPtr->selected = false;
		cancelled = false;
		MenuAction *handler = this->currentMenuItemPtr->handler;	// AJPC This is on the stack!!!
		this->inMenu = false;
		handler->menuAction(this->currentMenuItemPtr->param);	// Invoke selected menu handler AJPC
		this->inMenu = true;
		this->showMenu();
	}

	if(this->currentMenuItemId != this->nextMenuItemId){
		this->menuLine->show();									// Turn off inversion
		this->currentMenuItemId = this->nextMenuItemId;
		this->currentMenuItemPtr = this->nextMenuItemPtr;

		this->showMenuLine();
	}

	/*
	if(cancelled){
	  	Serial.print(F("Cancel Button "));
	  	cancelled = false;
	}
	*/
}
void Menu::showMenuLine(){
	this->menuLine->setText((char*)this->currentMenuItemPtr->prompt);
	this->menuLine->setRow(this->currentMenuItemId);
	this->menuLine->setCol(2);
	this->menuLine->invert();
	this->menuLine->show();
	this->menuLine->invert();
}

void Menu::rotaryAction(const int type, int level, RSE::Dir direction, int param){		// type is ROTATE or SELECT, driven from **Interrupt**
	if(this->inMenu){
		switch(type){
			case RotaryAction::ROTATE:
				Serial.print(F("Menu Rotate. Cnt: "));Serial.print(level);
				switch(direction){
					case RSE::FW:
						Serial.println(F(" UP"));
						if(this->nextMenuItemId < this->menuItemCount-1)
							this->nextMenuItemId++;
						break;
					case RSE::RV:
						Serial.println(F(" DOWN"));
						if(this->nextMenuItemId > 0)
							this->nextMenuItemId--;
						break;
					case RSE::NC:
						Serial.println(F(" NC"));
						break;
				}
				this->nextMenuItemPtr = &this->menuItems[this->nextMenuItemId];
				break;
			case RotaryAction::SELECT:
				if(level == ButtonAction::BUTTONLOW){
					Serial.println(F("Menu ButtonAction"));
					this->currentMenuItemPtr->selected = true;
				}else{
					Serial.println(F("Menu ButtonAction HIGH"));
				}
			break;
			default: break;
		}
	}else{
		// Pass through to invoked application
		Serial.print(F("Menu RotaryAction NIM T:"));Serial.print(type);Serial.print(F(" L:"));Serial.print(level);Serial.print(F(" D:"));Serial.println(direction);
		this->currentMenuItemPtr->handler->rotaryAction(type, level, direction, param);
	}
}
/*
void Menu::buttonAction(Level level, int param){
	if(this->inMenu){
		if(level == ButtonAction::BUTTONLOW){
//			Serial.println(F("Menu ButtonAction"));
			this->currentMenuItemPtr->selected = true;
		}
	}else{
		// Pass on to page that is currently invoked
		this->currentMenuItemPtr->handler->button(level, this->currentMenuItemPtr->param);
	}
}
*/
