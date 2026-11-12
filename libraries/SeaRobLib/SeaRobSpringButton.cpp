#include "Arduino.h"
#include "SeaRobLogger.h"
#include "SeaRobSpringButton.h"
	  

/*
 */
SeaRobSpringButton::SeaRobSpringButton(String name, int pin, bool useInternalPullUp, 
		onButtonAction downHandler, onButtonAction upHandler, void *opaque) : 
	_name(name), 
	_pin(pin), 
	_usePullup(useInternalPullUp),
	_downHandler(downHandler), 
	_upHandler(upHandler), 
	_opaque(opaque) {
	_downLevel = useInternalPullUp ? LOW : HIGH;
	_levelPrev = useInternalPullUp ? HIGH : LOW;
} 

/*
 */
bool SeaRobSpringButton::Init(unsigned long startTime) {
	pinMode(_pin, _usePullup ? INPUT_PULLUP : INPUT);
	bclogger("SeaRobSpringButton [%d:%s] started on pin %d, internal-pullup=%d", 
		_objId, _name.c_str(), _pin, _usePullup);
	return true;
}

/*
 */
void SeaRobSpringButton::ProcessLoop(unsigned long updateTime) {

	// Light Button Control: Detect if the voltage level on the button has changed.
	int currRead = digitalRead(_pin);   
	if (currRead == _levelPrev) {
		//bclogger("SeaRobSpringButton [%d:%s] level on pin %d is still the same (%d)", 
		//	_objId, _name.c_str(), _pin, currRead);
		return;
	}
	
	//bclogger("SeaRobSpringButton [%d:%s] level on pin %d CHANGE (%d -> %d)", 
	//	_objId, _name.c_str(), _pin, _levelPrev, currRead);
	
	// A transition just happened; lets figure out which type it is, and ripple the event up.
	if (currRead == _downLevel) {
	//	bclogger("SeaRobSpringButton [%d:%s] triggered buttonDown on pin %d", 
	//		_objId, _name.c_str(), _pin);
		if (_downHandler) {
			_downHandler(this, updateTime);
		}
	} else {
		//bclogger("SeaRobSpringButton [%d:%s] triggered buttonUp on pin %d", 
		//	_objId, _name.c_str(), _pin);
		if (_upHandler) {
			_upHandler(this, updateTime);
		}
	}

  	_levelPrev = currRead;
} 
