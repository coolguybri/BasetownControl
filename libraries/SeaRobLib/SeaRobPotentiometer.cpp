#include <Arduino.h>
#include "SeaRobLogger.h"
#include "SeaRobPotentiometer.h"
	  
#define FUDGE_FACTOR 20
	  

/*
 */
SeaRobPotentiometer::SeaRobPotentiometer(int inputPin) : 
	_pin(inputPin), 
	_lastRead(0) {
} 


bool SeaRobPotentiometer::Init(unsigned long startTime) {
	pinMode(_pin, INPUT);
	bcloggerP(PSTR("Potentiometer [%d] p=%d"), _objId, _pin);
	return true;
}


/*
 */
void SeaRobPotentiometer::ProcessLoop(unsigned long updateTime) {
}

/*
	Returns 0.0 -> 1.0
 */
float SeaRobPotentiometer::getValue() {

	int rawRead = analogRead(_pin);
	
	// Pin towards the edges.
	if (rawRead >= 1020) 
		rawRead = 1024;
	if (rawRead <= 10) 
		rawRead = 0;
	
	// Only respond to significant changes; filter out jitter.
	if ((abs(rawRead - _lastRead) > FUDGE_FACTOR) ||
		((rawRead == 0) && (_lastRead != 0)) || 
		((rawRead == 1024) && (_lastRead != 1024))) {
		bcloggerP(PSTR("Potentiometer [%d] read: %d->%d"), _objId, _lastRead, rawRead);
		_lastRead = rawRead;
	}
	
	float floatRead = (float) _lastRead;
	float standardized = floatRead / 1024.0;
	return standardized;
} 

