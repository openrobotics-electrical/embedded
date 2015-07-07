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
	
		Attribute() {};
	
		Attribute(int handle, String attributeName, MESSAGE_TYPE type) {
			
			name = attributeName;
			reference = handle;
			messageType = type;
			data = new byte[type];
		};
		
		bool newData = false;
		String name;
		MESSAGE_TYPE messageType;
		int reference, thing;
		byte *data;
		union numeric {
			float asFloat;
			int32_t asInt;
		} number;
		
		bool newDataAvailable() {
			return this->newData;
		}
		
		void newDataAvailable(bool b) {
			this->newData = b;
		}
		
		int32_t asInteger() {
			return this->number.asInt;
		}
		
		float asFloating() {
			return this->number.asFloat;
		}
		
		void load(int32_t i) {
			this->number.asInt = i;
			memcpy(data, &i, sizeof i);
		}
		
		void load(float f ) {
			this->number.asFloat = f;
			memcpy(data, &f, sizeof f);
		}
};

#endif