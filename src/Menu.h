/*
 * Menu.h
 *
 *  Created on: Feb 11, 2024
 *      Author: PaulCarew
 */

#ifndef MENU_H
#define MENU_H

#include "Button.h"
#include "RotarySelector.h"
#include "Display.h"

//typedef void (*SelectAction)(int param);
//typedef void (*SelectButton)(int param);

Interface MenuAction{
private:
public:
	~MenuAction(){}
	virtual void action(int param) =0;
//	virtual void button(int level, int param) =0;
};

class MenuItem{
private:
public:
	const char *prompt;
	MenuAction *handler;
	int param;
	bool selected;
};

//class Menu : Implements RotaryAction,ButtonAction {
class Menu : Implements RotaryAction{

private:
	DisplayText *menuLine ;
//	char menuLineBuff[27];
	MenuItem *menuItems = NULL;
	MenuItem *currentMenuItemPtr = NULL;
	MenuItem *nextMenuItemPtr = NULL;
	int currentMenuItemId = 0;
	int nextMenuItemId = 0;
	int menuItemCount;
	Display *display;
	int posRow = 0;
	int posCol = 0;
	RotarySelector *rotary = NULL;
//	Button *select = NULL;
	bool inMenu = true;

public:
	Menu(MenuItem *menu, int menuSize, int portDPinA, int portDPinB, int selectPin, Display *display);
	void showMenu();
	void showMenuLine();

	void rotaryAction(const int type, int counterOrLevel, RSE::Dir direction, int param);		// type is ROTATE or SELECT
//	void buttonAction(Level level, int param);
	void update();
	void checkTxtSize(char id);
};

#endif /* MENU_H */
