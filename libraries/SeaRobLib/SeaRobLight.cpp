#include "Arduino.h"
#include "SeaRobLight.h"
#include "SeaRobLogger.h"


/*
 */
SeaRobLight::SeaRobLight(int pin, int blinkOffset) 
	: SeaRobLight(pin, false, blinkOffset) {
}

/*
*/
SeaRobLight::SeaRobLight(int pin, bool dimmable, int blinkOffset) : _pin(pin), _dimmable(dimmable) {
  _state = LightState::Off;
  _lastToggleState = LightState::On;
  _fadeState = FadeState::FadeOff;
  _fadeStart = 0;
  _fadeInTime = 0;
  _fadeOutTime = 0;
  
  _blinkOffset = blinkOffset;
  _blinkDurationCount = 0;
  _blinkDurationIndex = 0;
  _blinkDurations = NULL;
  _blinkTimeNext = 0;
  
  _dimLevel = 255;
  _litState = false;
  _loggingState = false;
}


bool SeaRobLight::Init(unsigned long startTime) {
	if (_blinkOffset > 0) {
		_blinkTimeNext = (startTime + _blinkOffset);
	}
	
	pinMode(_pin, OUTPUT);
	
	bclogger("SeaRobLight [%d] pin=%d, dimmable=%d, dimLevel=%d, state=%d, offset=%d, nextblink=%lu, ", 
		_objId, _pin, _dimmable, _dimLevel, _state, _blinkOffset, _blinkTimeNext);
	return true;
}

/*
 */
SeaRobLight::~SeaRobLight() {
	delete[] _blinkDurations;
}

/*
 */
void SeaRobLight::SetDebugLogging(bool setter) {
  bool prev = _loggingState;
  _loggingState = setter;

  if (_loggingState != prev) {
    bclogger("SeaRobLight [%d] pin=%d, logging=%d", _objId, _pin, _loggingState);
  }
}

/*
 */
void SeaRobLight::UpdateState(LightState state) {
   _state = state;

   if (_loggingState) {
      bclogger("SeaRobLight::UpdateState: [%d] pin=%d, state=%d", _objId, _pin, _state);
   }
}

/*
 */
void SeaRobLight::UpdateDimLevel(int dimLevel) {
   _dimLevel = dimLevel;

   if (_loggingState) {
      bclogger("SeaRobLight::UpdateDimLevel: [%d] pin=%d, state=%d, level=%d", _objId, _pin, _state, _dimLevel);
   }
}

/*
 */
void SeaRobLight::UpdateBlinkConfig(unsigned long startTime, int offset, int durationOn, int durationOff, 
				boolean startOn, int fadeInDelay, int fadeOutDelay) {
	int durations[2] = { durationOn, durationOff };
	UpdateBlinkSequenceConfig(startTime, offset, 2, durations, startOn, fadeInDelay, fadeOutDelay);
}


/*
 */
void SeaRobLight::UpdateBlinkSequenceConfig(unsigned long startTime, int offset, int durationCount, int *durations, 
				boolean startOn, int fadeInDelay, int fadeOutDelay) {
			
	// Set initial state.
	_blinkOffset = offset;
	_blinkTimeNext = startTime + _blinkOffset;
	_litState = startOn;
	_fadeInTime = fadeInDelay;
	_fadeOutTime = fadeOutDelay;
	
	// Setup the blink state, deleting any previous state.
	if (_blinkDurations) {
		delete[] _blinkDurations;
		_blinkDurations = NULL;
	}
	
	_blinkDurationCount = durationCount;
	_blinkDurationIndex = 0;
	_blinkDurations = new int[durationCount];
	for (int i = 0 ; i < _blinkDurationCount ; i++) {
		_blinkDurations[i] = durations[i];
		if (_loggingState) {
			bclogger("SeaRobLight::UpdateBlinkConfig: [%d] pin=%d, state=%d, durations_index=%d, duration=%d", 
				_objId, _pin, _state, i, _blinkDurations[i]);
		}
	}
	
	if (_loggingState) {
		bclogger("SeaRobLight::UpdateBlinkConfig [%d] pin=%d, state=%d, durations=%d, offset=%d, startTime=%lu, nextblink=%lu", 
			_objId, _pin, _state, _blinkDurationCount, _blinkOffset, startTime, _blinkTimeNext);
	}
}


/*
 */
void SeaRobLight::ToggleOnOff() {
   switch (_state) {
    case LightState::Off:
      _state = _lastToggleState;
      _litState = true;
      break;
      
    case LightState::On: 
    case LightState::UniformBlink:
      _lastToggleState = _state;
      _state = LightState::Off;
      _litState = false;
      break;
    
    default:
      if (_loggingState) {
    	bclogger("SeaRobLight::ToggleOnOff [%d] pin=%d, jumping to off from %d", 
        	_objId, _pin, _state);
      }
      _state = LightState::Off;
      _litState = false;
      break;
   }

   if (_loggingState) {
      bclogger("SeaRobLight::ToggleOnOff [%d] pin=%d, state=%d, litState=%d, dimLevel=%d", 
      	_objId, _pin, _state, _litState, _dimLevel);
   }
}


/* 
 */
boolean SeaRobLight::IsOn() {
  return _litState;
}

/* 
 */
int SeaRobLight::GetDimLevel() {
  return _dimLevel;
}

/*
 */
String SeaRobLight::GetStateName() {
  switch (_state) {
    case LightState::Off:
      return "off";
      
    case LightState::On: 
     return "on";

    case LightState::UniformBlink:
      return "blink";
    
    default:
      bclogger("SeaRobLight::GetStateName [%d] pin=%d, ILLEGAL STATE CHANGE", 
      	_objId, _pin);
      return "illegal-state";
   }
}


/*
 */
void SeaRobLight::ProcessLoop(unsigned long updateTime) {
  if (_dimmable) {
  	ProcessLoopDimmable(updateTime);
  } else {
  	ProcessLoopNonDimmable(updateTime);
  }
}

#define DELAY_TIME 1000

/*
 */
void SeaRobLight::ProcessLoopDimmable(unsigned long updateTime) {
  
  switch (_state) {
    case LightState::Off:
		_litState = false;
		break;
      
    case LightState::On: 
		_litState = true;
		break;
    
    case LightState::UniformBlink:
    	switch (_fadeState) {
    		case FadeState::FadeOff: {
				if (updateTime >= _blinkTimeNext) {		
					_fadeState = _litState ? FadeState::FadeOut : FadeState::FadeIn;
					_fadeStart = _blinkTimeNext;
					_blinkTimeNext = _blinkTimeNext + ((_fadeState == FadeState::FadeOut) ? _fadeOutTime : _fadeInTime);
				}
			}
			break;
				
			case FadeState::FadeOut: {
				// calculate current dim level
				unsigned long millisDone = updateTime - _fadeStart;
				float partDone = (float) millisDone / (float) _fadeOutTime;
				int level = 256 - (int) (partDone * 256);
				_dimLevel = level;
				if (_loggingState) {
					bclogger("SeaRobLight:ProcessLoopDimmable [%d] fadeout part=%0.1f dimlevel = %d", 
						_objId, partDone, _dimLevel);
				}
				
				if (updateTime >= _blinkTimeNext) {	
					if (_loggingState) {
						bclogger("SeaRobLight:ProcessLoopDimmable [%d] fadeout complete", _objId);
					}
					_fadeState = FadeState::FadeOff;
					_litState = false;
					RescheduleBlink();
				}
			}
			break;
					
			case FadeState::FadeIn: {
				// calculate current dim level
				unsigned long millisDone = updateTime - _fadeStart;
				float partDone = (float) millisDone / (float) _fadeInTime;
				int level = (int) (partDone * 256);
				_dimLevel = level;
				_litState = true;
				if (_loggingState) {
					bclogger("SeaRobLight:ProcessLoopDimmable [%d] fadein part=%0.1f dimlevel = %d", 
						_objId, partDone, _dimLevel);
				}
				
				if (updateTime >= _blinkTimeNext) {	
					if (_loggingState) {
						bclogger("SeaRobLight:ProcessLoopDimmable [%d] fadein complete", _objId);
					}
					_fadeState = FadeState::FadeOff;
					RescheduleBlink();
				}
			}
			break;
	 }
  }

  // Write out current state to the led.
  int writeValue = _litState ? _dimLevel : 0;
  if (writeValue < 0) writeValue = 0;
  if (writeValue > 255) writeValue = 255;
  analogWrite(_pin, writeValue);
}

/*
 */
void SeaRobLight::ProcessLoopNonDimmable(unsigned long updateTime) {
  switch (_state) {
    case LightState::Off:
		_litState = false;
		break;
      
    case LightState::On: 
		_litState = true;
		break;
    
    case LightState::UniformBlink:
		if (updateTime >= _blinkTimeNext) {
			_litState = !_litState;
			RescheduleBlink();
		}
      	break;
  }

  // Write out current state to the led.
  digitalWrite(_pin, _litState);
}


/*
*/
void SeaRobLight::RescheduleBlink() {
	unsigned long thisTime = _blinkTimeNext;

	int nextDuration = 1000;
	if (!_blinkDurations) {
		bclogger("SeaRobLight:RescheduleBlink [%d] pin=%d, state=%d, ILLEGAL STATE - no durations", 
			_objId, _pin, _state);
	} else {
		nextDuration = _blinkDurations[_blinkDurationIndex];
		if (_loggingState) {
			bclogger("SeaRobLight:RescheduleBlink [%d] duration index %d = %d", 
				_objId, _blinkDurationIndex, nextDuration);
		}

		_blinkDurationIndex++;
		if (_blinkDurationIndex >= _blinkDurationCount)
			_blinkDurationIndex = 0;
			
		if (_loggingState) {
			bclogger("SeaRobLight:RescheduleBlink [%d] next index is %d", 
				_objId, _blinkDurationIndex);
		}
	}

	_blinkTimeNext = _blinkTimeNext + nextDuration;
	
	if (_loggingState) {
		bclogger("SeaRobLight:RescheduleBlink [%d] pin=%d, state=%d, nextDur=%d, thisFrameStart=%lu, nextFrameStart=%lu", 
			_objId, _pin, _state, nextDuration, thisTime, _blinkTimeNext);
	}
}
