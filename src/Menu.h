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

//typedef void (*SelectAction)(int param);
//typedef void (*SelectButton)(int param);

class Menu;		// foward declaration
Interface MenuAction: Implements RotaryAction{
private:
public:
	~MenuAction(){}
	virtual void menuAction(int param) =0;
	void rotaryAction(const int type, int level, RSE::Dir direction, int param){};		// type is ROTATE or SELECT. 'level only used for SELECT
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
public:
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

	Menu(MenuItem *menu, int menuSize, int portDPinA, int portDPinB, int selectPin, Display *display);
	void showMenu();
	void showMenuLine();

	void rotaryAction(const int type, int counterOrLevel, RSE::Dir direction, int param);		// type is ROTATE or SELECT
//	void buttonAction(Level level, int param);
	void menuInvoke();
//	void checkTxtSize(char id);
};

#endif /* MENU_H */
