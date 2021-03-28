#ifndef _WORLD_CLOCK_PARSE_H
#define _WORLD_CLOCK_PARSE_H

#include <stdint.h>
#include <iostream>
#include <string>
#include <WString.h>
#include <Arduino.h>
#include "SimpleJson.h"

using namespace std;

typedef struct
{
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
} world_time_st;

class WorldClockParse:public SimpleJson
{
private:
	/* data */
public:
	world_time_st time;

	WorldClockParse(/* args */);
	WorldClockParse(String &json);
	~WorldClockParse();

	int parseWorldClockData(String &json);
	void convertFileTimeToHumanTime(long long int seconds, int8_t GMT_time);
};



#endif