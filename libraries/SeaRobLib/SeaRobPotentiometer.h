#ifndef __searob_potentiometer_h__
#define __searob_potentiometer_h__

#include "SeaRobObject.h"


/*
	Represents one potentiometer, where two of its leads are hooked up to the 5v and CG
	lines, and the center lead is hooked up to one of the arduino's analog input pins. 
*/
class SeaRobPotentiometer : public SeaRobObject {
  public:
  						SeaRobPotentiometer(int inputPin);
  						
  		virtual bool 	Init(unsigned long startTime);
  		virtual void 	ProcessLoop(unsigned long updateTime);
  		
  		float 			getValue(); // 0.0 -> 1.0
  		
  private:
		const int             	_pin;
		int						_lastRead;
};


#endif // __searob_potentiometer_h__
