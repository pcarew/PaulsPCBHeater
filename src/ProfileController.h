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
	enum ProfileState{
		NotActive	= 0,
		Adjusting	= 1,				// Warming or cooling towards target
		Soaking		= 2,					// Target attained, now soaking
		Complete	= 3				// Soaking complete
	};
	enum ManualUpdate{
		Target = 1,
		Guard = 2
	};
	static int targetTemp;
	static int guardTemp;
	static bool profileRunning;
	static Profile *activeProfile;
	static MenuItem localMenuItems[];
	static Menu *localMenu;
	static Menu *profileMenu;
	static char activePage;

	static ProfileState currantState;

	ProfileController();
	virtual ~ProfileController();

	static void update();			//Called by task to update Profile status

	//Used for UI
	static const char *profileNameFmt;
	void menuAction(int param);
	void rotaryAction(const int type, int level, RSE::Dir direction, int param);		// type is ROTATE or SELECT
	bool handleRotary(const int type, int level, RSE::Dir direction, int *counter);	// Used for processing rotary action for local pages
};

#endif /* PROFILECONTROLLER_H_ */
