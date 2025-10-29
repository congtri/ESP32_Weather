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

typedef enum
{
	WORLD_CLOCK_PARSE_SUCCESS = 0,
	WORLD_CLOCK_PARSE_JSON_INVALID,
	WORLD_CLOCK_PARSE_TIME_NOT_FOUND,
	WORLD_CLOCK_PARSE_TIME_INVALID,
	WORLD_CLOCK_PARSE_TIMEZONE_INVALID,
	WORLD_CLOCK_PARSE_ERROR_UNDEFINED
} world_clock_error_code_e;

class WorldClockParse : public SimpleJson
{
private:
	/* Helper methods */
	bool isValidTimezone(int8_t timezone);
	bool isLeapYear(uint16_t year);
	void initializeTimeStruct();

public:
	world_time_st time;

	WorldClockParse();
	WorldClockParse(const String &json);
	~WorldClockParse();

	world_clock_error_code_e parseWorldClockData(const String &json, int8_t timezoneOffset = 7);
	int parseWorldClockData(String &json);  // Legacy compatibility method
	void convertFileTimeToHumanTime(long long int seconds, int8_t timezoneOffset);
	void showTimeInConsole() const;
	const char* getErrorMessage(world_clock_error_code_e errorCode) const;
};

#endif