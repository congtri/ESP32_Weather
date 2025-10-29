#ifndef _SIMPLE_JSON_H_
#define _SIMPLE_JSON_H_
#include <WString.h>
#include <stdint.h>
#include <Arduino.h>

class SimpleJson
{
private:
	String *json_string;
	String temp_value; // Safe storage for extracted values
	
	// Helper method for proper key matching
	int findKeyPosition(String &json, String key);
	
public:
	// Constructors and destructor
	SimpleJson();
	SimpleJson(String *json);
	~SimpleJson();
	
	// Modern API with clear naming conventions
	bool setJsonData(String *json);                    
	bool parseValue(String key);                       
	bool parseValueFromJson(String &json_data, String key); 
	void printParsedValue();                          
	
	// Type conversion methods with clear names
	bool convertToInt(int &result);                   
	bool convertToLongLong(long long &result);        
	bool convertToFloat(float &result);               
	
	// Utility methods
	bool hasValidData();                              
	int getValueLength();                             
	String getValueAsString();                        
};

#endif