#include <Arduino.h>
#include <Wire.h>
#include "SeaRob_SSD1306.h"
#include "SeaRobLogger.h"


/*
	Constants
*/
#define OLED_RESET -1
const int circleRadius = 4;
const int moveDuration = 3000;


/*
*/ 
SeaRob_SSD1306::SeaRob_SSD1306(const char *timestamp, int i2cAddress) : 
  	_display(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &Wire, OLED_RESET),
  	_timestamp(timestamp) {
  
	// This is called at app setup time, and through tinkering have discovered that a slight delay
  	// prevents flakiness.
  	delay(1000);
  
    // SSD1306_SWITCHCAPVCC enables internal charge pump to illuminate display
	if (!_display.begin(SSD1306_SWITCHCAPVCC, i2cAddress)) {
      Serial.println(F("SSD1306 allocation failed: need more available memory!"));
      for (;;); // Loop forever if allocation fails
     }
     
   	bcloggerP(PSTR("SeaRob_SSD1306 started on i2c %x"), i2cAddress);
}
  
  
/*
	Fun little animation to prove that we are not locked up; horizontal bouncing ball.
	nonInverse= false puts the animation in reverse (by offsetting of half the duration)
	Scroll position is based on a modulo from system clock, so will be same animation no matter when you start it.
*/
void SeaRob_SSD1306::drawCircleAnimation(unsigned long lastUpdateTime, int maxRight, int maxLeft, int topY, bool nonInverse) {
	int numFrames = maxRight - maxLeft;
	int numFramesDouble = numFrames * 2;
	int timePerFrame = moveDuration / numFramesDouble;
	
	int currentFrame = (lastUpdateTime / timePerFrame);
	if (!nonInverse)
		currentFrame += numFrames;
	currentFrame %= numFramesDouble;
	
	int circleCenterY = topY + circleRadius;
	int circleCenterX = (currentFrame < numFrames) ? (maxRight - currentFrame) : (maxLeft + (currentFrame - numFrames));
	_display.drawCircle(circleCenterX, circleCenterY, circleRadius, WHITE);
}


void SeaRob_SSD1306::drawTitle(const char * title, int x, int y) {
	_display.setTextSize(1);
	_display.setCursor(x, y);
	_display.println(title);
}
