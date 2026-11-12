#ifndef __searob_light_h__
#define __searob_light_h__

#include "SeaRobObject.h"

/*
 * Represents one led that can be either on or off. One output pin is required per light. 
 *  It can be set to blink overtime, or just stay in its current state until set again.
 */
class SeaRobLight : public SeaRobObject {

  public:
  	typedef enum {
  	  Off = 0,
  	  On,
  	  UniformBlink,
  	} LightState;
  	
  	typedef enum {
  	  FadeOff = 0,
  	  FadeOut,
  	  FadeIn,
  	} FadeState;

  public:
  					SeaRobLight(int pin, int blinkOffset=0);
  					SeaRobLight(int pin, bool dimmable, int blinkOffset=0);
  			virtual ~SeaRobLight();
  					
  		void		UpdateState(LightState state);
  		void		UpdateDimLevel(int dimLevel);
  		int			GetDimLevel();
  		
  		void		UpdateBlinkConfig(unsigned long startTime, int offset, int durationOn, int durationOff, 
  						boolean startOn = false, int fadeInDelay = 0, int fadeOutDelay = 0);
  		void		UpdateBlinkSequenceConfig(unsigned long startTime, int offset, int durationCount, int *durations, 
  						boolean startOn = false, int fadeInDelay = 0, int fadeOutDelay = 0);
  		
  		void		ToggleOnOff();
      	void    	SetDebugLogging(bool setter);
  					
  		bool		IsOn();
  		String		GetStateName();
  		int			GetPin() { return _pin; }
  					
  		virtual bool 	Init(unsigned long startTime);
  		virtual void	ProcessLoop(unsigned long updateTime);
  		
  protected:
  		virtual void	ProcessLoopDimmable(unsigned long updateTime);
  		virtual void	ProcessLoopNonDimmable(unsigned long updateTime);
  		virtual void	RescheduleBlink();
  							
  private:
	  const int        	_pin;
	  const bool		_dimmable;
	  
	  LightState       	_state;
	  LightState       	_lastToggleState;
	  FadeState			_fadeState;
	  unsigned long    	_fadeStart;
	  int				_fadeInTime;
	  int				_fadeOutTime;
	  
	  int              	_blinkOffset;
	  int				_blinkDurationCount;
	  int				_blinkDurationIndex;
	  int *				_blinkDurations;
	  unsigned long    	_blinkTimeNext;
	  
	  int				_dimLevel; // (0-255)
	  
	  bool             	_litState;
      bool             	_loggingState;
};

#endif // __searob_light_h__
