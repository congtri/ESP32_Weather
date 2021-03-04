#ifndef _OPENWEATHERPARSE_H_
#define _OPENWEATHERPARSE_H_

#include <stdint.h>
#include <iostream>
#include <string>
#include <WString.h>
#include <Arduino.h>

using namespace std;

#define PADDING			(2) /* This is size of charactor :" */
#define LOCATION_SIZE	(20)
typedef struct
{
	int id;
	int weather_icon_code;
	char* description;

	float temp;
	float feels_like;
	float temp_min;
	float temp_max;
	int pressure;
	int humidity;
}weather_st;

typedef struct
{
	float speed; //": 1.5,
	int deg;//" : 350
}wind_st;

typedef struct
{
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
} date_time_st;

typedef struct
{
	char country[20];
	char city[20];
	int timezone;
}zone_st;

typedef enum
{
	PARSE_DONE = 0,
	PARSE_TIME_ERROR,
	PARSE_ID_ERROR,
	PARSE_ERROR_UNDEFINE
}paser_error_code_e;

class OpenWeatherParse
{
private:
	zone_st			location;
	weather_st		weather;
	date_time_st	time;

	void convertUnixTimeToHumanTime(long int seconds);
	
public:
	OpenWeatherParse();
	~OpenWeatherParse();
	static OpenWeatherParse *getInstance();
	void TestFunction();
	paser_error_code_e parseOpenWeatherData(String &weather_data);
	
};

#endif
