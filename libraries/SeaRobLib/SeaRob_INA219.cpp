#include <Arduino.h>
#include <Wire.h>
#include "SeaRobLogger.h"
#include "SeaRob_INA219.h"


/*
*/ 
SeaRob_INA219::SeaRob_INA219(int i2cAddress) : 
	_addr(i2cAddress), _sensor(i2cAddress) {
}

bool SeaRob_INA219::Init(unsigned long startTime) {
    if (!_sensor.begin()) {
      	Serial.println(F("Failed to find INA219 chip"));
     	return false;
    } 
    
    bcloggerP(PSTR("INA219 [%d] init addr=%x"), _objId, _addr);
    return true;
}


void SeaRob_INA219::ProcessLoop(unsigned long updateTime) {
	// TODO: consider adding a maxmmum sample rate and casching the samples in between.
}


void SeaRob_INA219::getSample(Sample *sample, unsigned long lastUpdateTime) {
	
	sample->shuntvoltage = _sensor.getShuntVoltage_mV();
	sample->busvoltage   = _sensor.getBusVoltage_V();
	sample->current_mA   = _sensor.getCurrent_mA();
	sample->power_mW     = _sensor.getPower_mW();
	
	sample->loadvoltage  = sample->busvoltage + (sample->shuntvoltage / 1000); 
	sample->sampleTime = lastUpdateTime; 
	bcloggerP(PSTR("INA219 [%d] new sample: %lu, %lu"), _objId, lastUpdateTime, sample->sampleTime);
}



SeaRob_INA219::Sample::Sample() {
    shuntvoltage = 0.0;
    busvoltage = 0.0;
    current_mA = 0.0;
    power_mW = 0.0;
    loadvoltage = 0.0;
    
    sampleTime = 0L;
}
  