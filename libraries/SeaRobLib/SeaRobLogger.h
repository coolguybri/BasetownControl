#ifndef __searob_logger_h__
#define __searob_logger_h__


/*
	fmt must be the output of PSTR() (PSTR("yo"))
*/
#define bcloggerP bclogger
void bclogger(const char* fmt, ...);

#endif // __searob_logger_h__
