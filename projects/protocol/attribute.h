#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <Arduino.h>

typedef enum MESSAGE_TYPE {

	UINT32_T = 4,
	FLOAT32_T = 4,
	CHAR = 1

} MESSAGE_TYPE;

class Attribute {
	
	public:
	
	Attribute(int handle, String attributeName, MESSAGE_TYPE type) {
		
		name = attributeName;
		reference = handle;
		messageType = type;
		data = new byte[type];
	};
	
	bool newData = false;
	String name;
	MESSAGE_TYPE messageType;
	int reference;
	byte *data;
	
	bool newDataAvailable() {
		return newData;
	}
	
	void newDataAvailable(bool b) {
		this->newData = b;
	}
};

#endif