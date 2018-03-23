/*
 Name:		NexaRemote.ino
 Created:	3/23/2018 5:44:33 PM
 Author:	sindr
*/

#include "tx433_Nexa.h"

NexaTransmitter remote(D4, 9728630); // Create the nexa remote on pin2 with remote id

void setup() {
	Serial.begin(9600);
	Serial.println("start");
}

void loop() {

	Serial.println("On");
	remote.setSwitch(true, 2); // switch on the unit 2 to dim level 10 (out of 15)
	delay(3000);
	Serial.println("Off");// wait 3 sec
	remote.setSwitch(false, 2);    // switch off the unit 2
	delay(3000);
	
}