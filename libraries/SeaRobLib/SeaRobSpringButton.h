#ifndef __searob_springbutton_h__
#define __searob_springbutton_h__

#include "SeaRobObject.h"

/*
	Callback prototype for events triggered by the detected button press.
*/
class SeaRobSpringButton;
typedef void (*onButtonAction) (SeaRobSpringButton *button, long updateTime);


/*
	Represents one physical button; when pressed, the onPressDown callback is invoked. 
	The physical button pops back up with a spring when released.
	Requires one input pin per button.
*/
class SeaRobSpringButton : public SeaRobObject {
  public:
  				SeaRobSpringButton(String name, int pin, bool useInternalPullUp,
  							onButtonAction downHandler, onButtonAction upHandler = NULL, void *opaque = NULL);
  				
  		virtual bool 	Init(unsigned long startTime);
  		virtual void 	ProcessLoop(unsigned long updateTime);
  		
  		const void * 	GetOpaque() { return _opaque; }
  		const int		GetPin() { return _pin; }
  		const String 	GetName() { return _name; }
  		
  private:
		const String          	_name;
		const int             	_pin;
		 
		const onButtonAction   	_downHandler;
		const onButtonAction   	_upHandler;
		const void *          	_opaque;
		const bool				_usePullup;
		
		int             		_levelPrev;
		int						_downLevel;
};


#endif // __searob_springbutton_h__
