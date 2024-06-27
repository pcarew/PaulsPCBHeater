/*
 * DisplayControl.cpp
 *
 *  Created on: Jun 27, 2024
 *      Author: PaulCarew
 */

#include "DisplayController.h"

DisplayController::DisplayController() {
	// TODO Auto-generated constructor stub

}

DisplayController::~DisplayController() {
	// TODO Auto-generated destructor stub
}


LEDController::LEDMode LEDController::ledStatus = LEDMode::SelfTest;

void LEDController::setup(){
	pinMode(LED_PIN, OUTPUT);
	LEDController::ledStatus = LEDMode::SelfTest;
	for(int i = 0; i<3; i++){
		digitalWrite(LED_PIN,HIGH);
		delay(125);
		digitalWrite(LED_PIN,LOW);
		delay(125);
	}
}
void LEDController::ledEvent(LEDMode mode){
	switch(mode){
	case LEDMode::Off:
	case LEDMode::ZeroCrossing:
	case LEDMode::HeaterOn:
	case LEDMode::AmbientWarning:
	case LEDMode::AmbientDanger:
		break;
	}
	if(mode >= LEDController::ledStatus){
		LEDController::ledStatus = mode;
	}
}

void LEDController::update(){

}
