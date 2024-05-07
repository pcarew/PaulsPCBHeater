/*
 * ProfileController.h
 *
 *  Created on: May 2, 2024
 *      Author: PaulCarew
 */

#ifndef PROFILECONTROLLER_H
#define PROFILECONTROLLER_H

#include "Profile.h"
#include "Menu.h"

class ProfileController: Implements MenuAction {
public:
	static unsigned char targetTemp;
	static Profile activeProfile;
	static MenuItem *localMenuItems;
	static Menu *localMenu;
	static char activePage;

	ProfileController();
	virtual ~ProfileController();

	//Used for UI
	static const char *fmt;
	void menuAction(int param);
	void rotaryAction(const int type, int level, RSE::Dir direction, int param);		// type is ROTATE or SELECT
};

#endif /* PROFILECONTROLLER_H_ */
