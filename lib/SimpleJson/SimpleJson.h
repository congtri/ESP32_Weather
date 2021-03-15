#ifndef _SIMPLE_JSON_H_
#define _SIMPLE_JSON_H_
#include <WString.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <WString.h>
#include <Arduino.h>

typedef struct
{
	char *data;
	int length;
} value_data_st;

class SimpleJson
{
private:
	String *json_string;
	
public:
	value_data_st json_value;

	SimpleJson(/* args */);
	SimpleJson(String *json);
	~SimpleJson();
	int findJsonValue(String &json_data, String key);
	int findJsonValue(String key);

	void initJsonString(String *json);
	void showJsonValueData();

	uint8_t convertDataToInt(value_data_st &jdata, int &int_number);
	uint8_t convertDataTollint(value_data_st &jdata, long long int &llint_number);
	uint8_t convertDataToFloat(value_data_st &jdata, float &float_number);

};

#endif