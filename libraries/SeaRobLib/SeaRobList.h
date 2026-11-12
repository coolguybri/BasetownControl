#ifndef __searob_list_h__
#define __searob_list_h__

#include "SeaRobObject.h"



/*
*/
class SeaRobList {

  public:
    		SeaRobList(int initialCapacity);
    		virtual ~SeaRobList();
    		
    void 	Add(SeaRobObject *);

private:
	void 	EnsureCapacity(int additional);
			
private:
    int							_currLen;
    int							_currCapacity;
    SeaRobObject **         	_list;
};


#endif // __searob_springbuttonlight_h__
