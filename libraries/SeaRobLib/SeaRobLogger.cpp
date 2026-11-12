#include <Arduino.h>
#include "SeaRobLogger.h"

/*
 * Global log buffer, lazy init. 
 * WARNING: Making LOGBUF_SIZE too large will result in failures on the Nano.
 */
#define LOGBUF_SIZE 100
char *logbuf = NULL;


/*
 * Global logger; currently outputs to the Serial Monitor.
 	fmt must be the output of PSTR()
 */
void bclogger(const char* fmt, ...) {
    va_list params;
    va_start(params, fmt);
    
    if (logbuf == NULL) {
    	logbuf = new char[LOGBUF_SIZE + 2];
    }

//#if defined(ARDUINO_ARCH_RENESAS) 
	// Specific to R4; no seperate flash memory
  	vsnprintf(logbuf, LOGBUF_SIZE, fmt, params); 
//#else
 //	 vsnprintf_P(logbuf, LOGBUF_SIZE, fmt, params); 
//#endif
  
    Serial.println(logbuf);

    va_end(params);
}
