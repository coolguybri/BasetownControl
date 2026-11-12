#include "Arduino.h"
#include "SeaRobList.h"


/*
 */
SeaRobList::SeaRobList(int initialCapacity) 
		: _currLen(0), _currCapacity(0), _list(NULL)  {
	EnsureCapacity(initialCapacity);
}


/*
*/
SeaRobList::~SeaRobList() {
	for (int i = 0 ; i < _currLen ; i++) {
		delete _list[i];
		_list[i] = NULL;
	}
	delete[] _list;
}


/*
*/
void SeaRobList::EnsureCapacity(int additionalCapacity) {

	int newCapacity = _currCapacity + additionalCapacity;
	if (_currCapacity >= newCapacity)
		return;
		
	int allocCapacity = (_currCapacity == 0) ? newCapacity : (_currCapacity * 2);
		
	SeaRobObject ** newList = new SeaRobObject *[allocCapacity];
	for (int i = 0 ; i < _currLen ; i++) {
		newList[i] = _list[i];
	}
	for (int i = _currLen ; i < allocCapacity ; i++) {
		newList[i] = NULL;
	}
	
	delete[] _list;
	_list = newList;
	_currCapacity = allocCapacity;
}
	
/*
*/
void SeaRobList::Add(SeaRobObject *s) {
	EnsureCapacity(1);
	_list[_currLen++] = s;
}


