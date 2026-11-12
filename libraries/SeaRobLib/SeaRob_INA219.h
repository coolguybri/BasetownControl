#ifndef __searob_ina219_h__
#define __searob_ina219_h__

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_INA219.h>
#include "SeaRobObject.h"


/* 
*/
class SeaRob_INA219 : public SeaRobObject {

	public:
		class Sample {
			public:
				Sample();
		  
			float shuntvoltage;
			float busvoltage;
			float current_mA;
			float power_mW;
			float loadvoltage;
		
			unsigned long sampleTime;
		};

  public:
          			SeaRob_INA219(int i2cAddress);
	virtual bool 	Init(unsigned long startTime);
    virtual void 	ProcessLoop(unsigned long updateTime);

    void  			getSample(Sample * outbuf, unsigned long lastUpdateTime);	
    
  private:
  	int					_addr;
	Adafruit_INA219 	_sensor;
};

#endif // __searob_ina219_h__

