#include "Menu.h"
#include "Ram.h"
#include "RotarySelector.h"

#define MENUITEMTEXTSIZE 2
IMPORT bool cancelled;
IMPORT DisplayText displayElement;

Menu::Menu(MenuItem *menu, int menuSize, int portDPinA, int portDPinB, int selectPin, Display *display, int row, int col ){
	this->menuItems				= menu;
	this->currentMenuItemPtr	= menu;
	this->nextMenuItemPtr		= menu;
	this->currentMenuItemId		= -1;			// Indicates that Menu has not been displayed
	this->nextMenuItemId		= 0;
	this->menuItemCount			= menuSize;
	this->display				= display;
	this->posRow				= row;
	this->posCol				= col;
	this->rotary				= new RotarySelector(portDPinA, portDPinB, selectPin, this, 5 ); // A:D5, B:D6 on PCBHeater 'selectPin' button on the rotary selector (D4 on PCBHeater)

	this->menuLine	= &displayElement;

	menu[0].selected	= false;
//	menu->selected	= false;
};

void Menu::showMenu(){

	this->display->tftScreen.background(this->display->br, this->display->bg, this->display->bb);

	this->currentMenuItemId		= 0;
	this->currentMenuItemPtr	= this->menuItems;
	this->menuLine->setCol(this->posCol);
	this->menuLine->setBg(this->display->br, this->display->bg, this->display->bb);
	this->menuLine->setFg(this->display->fr, this->display->fg, this->display->fb);
	for(int i=0;i<this->menuItemCount;i++){
		this->menuLine->setText((char*)(this->menuItems[i].prompt));
		this->menuLine->setRow(i+this->posRow);
		this->menuLine->show();
	}
	this->highlightSelectedMenuLine();
}

void Menu::menuInvoke(){ // Called from System thread
	if(this->currentMenuItemId < 0) this->showMenu();				// 1st Time through, show entire menu

	if(this->currentMenuItemPtr->selected == true){					// Check to see if we have a selected menu line
		this->currentMenuItemPtr->selected = false;
		cancelled = false;
		MenuAction *handler = this->currentMenuItemPtr->handler;	// Setup handler to use for selected menu line

		this->inMenu = false;
		handler->menuAction(this->currentMenuItemPtr->param);		// Invoke selected menu handler. Note: we are running as part of the UI/System thread
		this->inMenu = true;

		this->showMenu();											// Action finished, so re display menu
	}

	if(this->currentMenuItemId != this->nextMenuItemId){			// If a rotary action resulted in a new highlighted menu line, then show it
		this->menuLine->setText((char*)this->currentMenuItemPtr->prompt);
		this->menuLine->setRow(this->currentMenuItemId+this->posRow);
		this->menuLine->setCol(this->posCol);
		this->menuLine->setBg(this->display->br, this->display->bg, this->display->bb);
		this->menuLine->setFg(this->display->fr, this->display->fg, this->display->fb);
		this->menuLine->show();										// Turn off inversion on existing highlighted line

		this->currentMenuItemId = this->nextMenuItemId;				// Setup new menu line to highlight
		this->currentMenuItemPtr = this->nextMenuItemPtr;

		this->highlightSelectedMenuLine();
	}
}

void Menu::highlightSelectedMenuLine(){
	this->menuLine->setBg(this->display->br, this->display->bg, this->display->bb);
	this->menuLine->setFg(this->display->fr, this->display->fg, this->display->fb);

	this->menuLine->setText((char*)this->currentMenuItemPtr->prompt);	// Setup new menu line text and position
	this->menuLine->setRow(this->currentMenuItemId+this->posRow);
	this->menuLine->setCol(this->posCol);

	this->menuLine->invert();
	this->menuLine->show();
	this->menuLine->invert();
}

void Menu::rotaryAction(const int type, int level, RSE::Dir direction, int param){		// type is ROTATE or SELECT, driven from **Interrupt**
	if(this->inMenu){
		switch(type){
			case RotaryAction::ROTATE:
				switch(direction){
					case RSE::FW:
						if(this->nextMenuItemId < this->menuItemCount-1)
							this->nextMenuItemId++;
						break;
					case RSE::RV:
						if(this->nextMenuItemId > 0)
							this->nextMenuItemId--;
						break;
					case RSE::NC:
						break;
				}
				this->nextMenuItemPtr = &this->menuItems[this->nextMenuItemId];
				break;
			case RotaryAction::SELECT:
				if(level == ButtonAction::BUTTONLOW){
					this->currentMenuItemPtr->selected = true;
				}else{
//							// Menu ButtonAction HIGH
				}
			break;
			default: break;
		}
	}else{
		// Pass through to invoked application
		this->currentMenuItemPtr->handler->rotaryAction(type, level, direction, param);
	}
}
