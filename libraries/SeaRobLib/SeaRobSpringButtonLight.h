#ifndef __searob_springbuttonlight_h__
#define __searob_springbuttonlight_h__

#include "SeaRobSpringButton.h"
#include "SeaRobLight.h"
#include "SeaRobObject.h"


/*
	Callback prototype for events triggered by the detected button press.
*/
class SeaRobSpringButtonLight;
typedef void (*onStateChange) (SeaRobSpringButtonLight *bl, long updateTime);


/*
	Represents the pairing of one button to one LED; the button will auto switch it on and off.
*/
class SeaRobSpringButtonLight : public SeaRobObject {

  public:
    		SeaRobSpringButtonLight(String name, int buttonPin, int ledPin, 
    				bool dimmable, bool useInternalPullUp,
    				onStateChange downHandler, onStateChange upHandler = NULL, void *opaque = NULL);
    		virtual ~SeaRobSpringButtonLight();

	virtual bool 			Init(unsigned long startTime);
    virtual void 			ProcessLoop(unsigned long updateTime);
    
    void					AddExtraLedPin(int ledPin);
    
    SeaRobSpringButton * 	GetButton() { return _button; }
    SeaRobLight *  			GetLight() { return _light; }
   	int  					GetExtraLightLen() { return _extraLightLen; }
    SeaRobLight **	  		GetExtraLights() { return _extraLights; }
    
    bool 					IsOn() { return _light->IsOn(); }
    void *					GetOpaque() { return _opaque; }
    String					GetName() { return _name; }

  protected:
    void 					OnButtonDown(long updateTime);
    void 					OnButtonUp(long updateTime);

  private:
    const String                _name;
    const SeaRobSpringButton *  _button;
    const bool					_dimmable;
    const onStateChange    	  	_downHandler;
    const onStateChange    	  	_upHandler;
    const void *          		_opaque;
    
    const SeaRobLight *         _light;
    int							_extraLightLen;
    int							_extraLightCapacity;
    SeaRobLight **         		_extraLights;
    
  protected:
	static void StaticOnButtonDown(SeaRobSpringButton *button, long updateTime) {
	  SeaRobSpringButtonLight *bl = (SeaRobSpringButtonLight *) button->GetOpaque();
	  bl->OnButtonDown(updateTime);
	}
	
	static void StaticOnButtonUp(SeaRobSpringButton *button, long updateTime) {
	  SeaRobSpringButtonLight *bl = (SeaRobSpringButtonLight *) button->GetOpaque();
	  bl->OnButtonUp(updateTime);
	}
};


#endif // __searob_springbuttonlight_h__
