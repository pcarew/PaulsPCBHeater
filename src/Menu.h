/*
 * Menu.h
 *
 *  Created on: Feb 11, 2024
 *      Author: PaulCarew
 */

#ifndef MENU_H
#define MENU_H

#include "Button.h"
#include "Display.h"
#include "RotarySelector.h"

class Menu;		// foward declaration
Interface MenuAction;

class MenuItem {
private:
public:
	const char *prompt;
	MenuAction *handler;
	int param;
	bool selected;
};

Interface MenuAction: Implements RotaryAction{
private:
public:
	~MenuAction(){}
	virtual void menuAction(int param) =0;
	void rotaryAction(const int type, int level, RSE::Dir direction, int param){};		// type is ROTATE or SELECT. 'level only used for SELECT
};


class Menu : Implements RotaryAction{

private:
public:
	DisplayText *menuLine ;
	MenuItem *menuItems			= NULL;
	MenuItem *currentMenuItemPtr= NULL;
	MenuItem *nextMenuItemPtr	= NULL;
	int currentMenuItemId		= 0;
	int nextMenuItemId			= 0;
	int menuItemCount			= 0;
	Display *display			= NULL;
	int posRow					= 0;
	int posCol					= 0;
	RotarySelector *rotary		= NULL;
	bool inMenu					= true;

	Menu(MenuItem *menu, int menuSize, RotarySelector *rotary, Display *display, int row, int col  );
	Menu(MenuItem *menu, int menuSize, int portDPinA, int portDPinB, int selectPin, Display *display):
							Menu(menu, menuSize, portDPinA, portDPinB, selectPin, display, 0, 0  ){}
	Menu(MenuItem *menu, int menuSize, int portDPinA, int portDPinB, int selectPin, Display *display, int row, int col  ):
							Menu(menu,menuSize, new RotarySelector(portDPinA, portDPinB, selectPin, this, 5 ) ,display,row,col){}			// Create a 'RotarySelector'
	void showMenu();
	void highlightCurrentMenuLine();

	void rotaryAction(const int type, int counterOrLevel, RSE::Dir direction, int param);		// type is ROTATE or SELECT
	void menuInvoke();
};

#endif /* MENU_H */
