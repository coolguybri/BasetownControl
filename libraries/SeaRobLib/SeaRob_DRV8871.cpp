#include "Arduino.h"
#include "SeaRobLogger.h"
#include "SeaRob_DRV8871.h"


/*
 * 
 */
SeaRob_DRV8871::SeaRob_DRV8871(int pinOutput1, int pinOutput2) :
	_pin1(pinOutput1), 
	_pin2(pinOutput2), 
	_direction(MotorDirection_Off), 
	_pulseWidth(0) {
}

bool SeaRob_DRV8871::Init(unsigned long startTime) {
	pinMode(_pin1, OUTPUT);
	pinMode(_pin2, OUTPUT);
	bcloggerP(PSTR("DRV8871 [%d] init p1=%d p2=%d"), _objId, _pin1, _pin2);
	return true;
}


/*
 */
void SeaRob_DRV8871::ProcessLoop(unsigned long updateTime) {

	switch (_direction) {
	
	case MotorDirection_Forward:
		analogWrite(_pin1, _pulseWidth);
		digitalWrite(_pin2, LOW);
		break;
	
	case MotorDirection_Reverse:
		digitalWrite(_pin1, LOW);
		analogWrite(_pin2, _pulseWidth);
		break;
	
	case MotorDirection_Off:
	default:
		digitalWrite(_pin1, LOW);
		digitalWrite(_pin2, LOW);
		break;
	}
}


/*
// 0.0 -> 1.0
*/
float SeaRob_DRV8871::setPower(float powerLevel) {
	if (powerLevel > 1.0)
		powerLevel = 1.0;
	else if (powerLevel < 0.0)
		powerLevel = 0.0;
		
	int newLevel = (int) (powerLevel * 255);
	if (_pulseWidth != newLevel) {
		_pulseWidth = newLevel;
		bcloggerP(PSTR("DRV8871 [%d] set: p=%d w=%d"), _objId, (int)(powerLevel * 100), _pulseWidth);
	}
	return _pulseWidth;
}; 
	
			
/*
 * 
 */
void SeaRob_DRV8871::setDirection(MotorDirection md) {
	if (_direction != md) {
		_direction = md;
  		bcloggerP(PSTR("DRV8871 [%d] dir: %d"), _objId, _direction);
  	}
}
