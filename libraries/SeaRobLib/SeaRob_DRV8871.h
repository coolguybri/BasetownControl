#ifndef __searob_drv8871_h__
#define __searob_drv8871_h__

#include <Arduino.h>
#include "SeaRobObject.h"


typedef enum {
  MotorDirection_Off,
  MotorDirection_Forward,
  MotorDirection_Reverse,
} MotorDirection;


/*
*/
class SeaRob_DRV8871 : public SeaRobObject {
  public:
        			SeaRob_DRV8871(int pinInput1, int pinInput2);
	 virtual bool 	Init(unsigned long startTime);
     virtual void 	ProcessLoop(unsigned long updateTime);
     
			void 	setDirection(MotorDirection md);
			float 	setPower(float powerLevel); // 0.0 -> 1.0
       
  private:
	  const int       _pin1;
	  const int       _pin2;
	  
	  MotorDirection  _direction;
	  int             _pulseWidth; // Between 0-255.
};


#endif // __searob_drv8871_h__