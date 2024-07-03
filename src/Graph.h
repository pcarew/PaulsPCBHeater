/*
 * Graph.h
 *
 *  Created on: Jul 2, 2024
 *      Author: PaulCarew
 */

#ifndef GRAPH_H_
#define GRAPH_H_
#include <Arduino.h>
#include "Menu.h"

class Graph: Implements MenuAction{
public:

//	const static int height;
//	const static int width;


	Graph(){}
	virtual ~Graph(){}

//	void update();

	//Used for UI
	void menuAction(int param);
//	void rotaryAction(const int type, int level, RSE::Dir direction, int param);		// type is ROTATE or SELECT
//	bool handleRotary(const int type, int level, RSE::Dir direction, int *counter);	// Used for processing rotary action for local pages
};

#endif /* GRAPH_H_ */
