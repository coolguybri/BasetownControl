#ifndef __searob_ssd1306_h__
#define __searob_ssd1306_h__

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// Constants
#define OLED_SCREEN_WIDTH 128 // OLED display width, in pixels
#define OLED_SCREEN_HEIGHT 64 // OLED display height, in pixels
  
/* 
*/
class SeaRob_SSD1306 {
  public:
        SeaRob_SSD1306(const char *timestamp, int i2cAddress);
          
        // Common drawing routines.
        void drawTitle(const char * title, int x, int y);	
    	void drawCircleAnimation(unsigned long lastUpdateTime, int maxRight, int maxLeft, int topY, bool nonInverse);
    
    Adafruit_SSD1306 &	getDisplayRef() { return _display; }
    
  private:
	Adafruit_SSD1306 	_display;
	const char *		_timestamp;
};


#endif // __searob_ssd1306_h__
