#include "OpenWeatherParse.h"

#define IS_PARSED_DATA_VALID(DATA) (DATA.length)

typedef struct
{
	const char *icon;
	char code;
} icon_map_t;

#define ICON_MAP_TABLE_SIZE (18)
const icon_map_t icon_map[] = {
	{"01d", 'B'},
	{"01n", 'C'},
	{"02d", 'H'},
	{"02n", 'I'},
	{"03d", 'N'},
	{"03n", '5'},
	{"04d", 'Y'},
	{"04n", '%'},
	{"09d", 'Q'},
	{"09n", '7'},
	{"10d", 'R'},
	{"10n", '8'},
	{"11d", 'Z'},
	{"11n", '&'},
	{"13d", 'W'},
	{"13n", '#'},
	{"50d", 'J'},
	{"50n", 'K'}};

OpenWeatherParse::OpenWeatherParse()
{
}

OpenWeatherParse::~OpenWeatherParse()
{
}

// Unix time is in seconds and
// Humar Readable Format:
// DATE:MONTH:YEAR:HOUR:MINUTES:SECONDS,
// Start of unix time:01 Jan 1970, 00:00:00
// Function to convert unix time to
// Human readable format
void OpenWeatherParse::convertUnixTimeToHumanTime(long int seconds)
{
	// Number of days in month
	// in normal year
	int daysOfMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	long int currYear, daysTillNow, extraTime, extraDays, index, date, month, hours, minutes, secondss, flag = 0;

	// Calculate total days unix time T
	daysTillNow = seconds / (24 * 60 * 60);
	extraTime = seconds % (24 * 60 * 60);
	currYear = 1970;

	// Calculating currrent year
	while (daysTillNow >= 365)
	{
		if (currYear % 400 == 0 || (currYear % 4 == 0 && currYear % 100 != 0))
		{
			daysTillNow -= 366;
		}
		else
		{
			daysTillNow -= 365;
		}
		currYear += 1;
	}

	// Updating extradays because it
	// will give days till previous day
	// and we have include current day
	extraDays = daysTillNow + 1;

	if (currYear % 400 == 0 || (currYear % 4 == 0 && currYear % 100 != 0))
		flag = 1;

	// Calculating MONTH and DATE
	month = 0, index = 0;
	if (flag == 1)
	{
		while (true)
		{
			if (index == 1)
			{
				if (extraDays - 29 < 0)
					break;
				month += 1;
				extraDays -= 29;
			}
			else
			{
				if (extraDays - daysOfMonth[index] < 0)
				{
					break;
				}
				month += 1;
				extraDays -= daysOfMonth[index];
			}
			index += 1;
		}
	}
	else
	{
		while (true)
		{

			if (extraDays - daysOfMonth[index] < 0)
			{
				break;
			}
			month += 1;
			extraDays -= daysOfMonth[index];
			index += 1;
		}
	}

	// Current Month
	if (extraDays > 0)
	{
		month += 1;
		date = extraDays;
	}
	else
	{
		if (month == 2 && flag == 1)
			date = 29;
		else
		{
			date = daysOfMonth[month - 1];
		}
	}

	// Calculating HH:MM:YYYY
	hours = extraTime / 3600;
	minutes = (extraTime % 3600) / 60;
	secondss = (extraTime % 3600) % 60;

	this->time.day = date;
	this->time.month = month;
	this->time.year = currYear;

	this->time.hour = hours;
	this->time.min = minutes;
	this->time.sec = secondss;
}


paser_error_code_e OpenWeatherParse::parseOpenWeatherData(String &weather_data)
{
	uint8_t idx = 0;
	char icon_buf[4] = {0};
	long int i_value = 0; /* This variable is used to converd from char to int */
	float f_value = 0;	  /* This variable is used to converd from char to float */
	this->initJsonString(&weather_data);

	/* Parse weather id */
	this->findJsonValue("id");
	if (this->json_value.length)
	{
		i_value = atoi(this->json_value.data);
		this->weather.id = (int)i_value;
	}
	else
	{
		return PARSE_ID_ERROR;
	}

	/* Parse weather description */
	this->findJsonValue(weather_data, "description");
	if (this->json_value.length)
	{
		memset(this->weather.description, '\0', 40);
		memcpy(this->weather.description, this->json_value.data, this->json_value.length);
	}
	else
	{
		return PARSE_DES_ERROR;
	}

	/* Parse weather icon */
	this->findJsonValue(weather_data, "icon");
	if (this->json_value.length)
	{
		memset(icon_buf, '\0', sizeof(icon_buf));
		memcpy(icon_buf, this->json_value.data, this->json_value.length); /* expected lenght is 3 */
		this->weather.icon_code = ')';							  /* NA data icon */
		for (idx = 0; idx < ICON_MAP_TABLE_SIZE; idx++)
		{
			if (strcmp(icon_buf, icon_map[idx].icon) == 0)
			{
				this->weather.icon_code = icon_map[idx].code;
				break;
			}
		}
	}
	else
	{
		return PARSE_ICON_ERROR;
	}

	/* Parse tempareture data */
	this->findJsonValue(weather_data, "temp");
	if (this->json_value.length)
	{
		f_value = atof(this->json_value.data);
		this->weather.temp = f_value;
	}
	else
	{
		return PARSE_TEMP_ERROR;
	}

	/* Parse tempareture feels_like data */
	this->findJsonValue(weather_data, "feels_like");
	if (this->json_value.length)
	{
		f_value = atof(this->json_value.data);
		this->weather.feels_like = f_value;
	}
	else
	{
		return PARSE_FEELLIKE_ERROR;
	}

	/* Parse tempareture temp_min data */
	this->findJsonValue(weather_data, "temp_min");
	if (this->json_value.length)
	{
		f_value = atof(this->json_value.data);
		this->weather.temp_min = f_value;
	}
	else
	{
		return PARSE_TEMP_MIN_ERROR;
	}

	/* Parse tempareture temp_max data */
	this->findJsonValue(weather_data, "temp_max");
	if (this->json_value.length)
	{
		f_value = atof(this->json_value.data);
		this->weather.temp_max = f_value;
	}
	else
	{
		return PARSE_TEMP_MAX_ERROR;
	}

	/* Parse pressure data */
	this->findJsonValue(weather_data, "pressure");
	if (this->json_value.length)
	{
		i_value = atoi(this->json_value.data);
		this->weather.pressure = i_value;
	}
	else
	{
		return PARSE_PRESSURE_ERROR;
	}

	/* Parse humidity data */
	this->findJsonValue(weather_data, "humidity");
	if (this->json_value.length)
	{
		i_value = atoi(this->json_value.data);
		this->weather.humidity = i_value;
	}
	else
	{
		return PARSE_HUMIDITY_ERROR;
	}

	/* Parse location data */
	this->findJsonValue(weather_data, "country");
	if (this->json_value.length)
	{
		memset(this->location.country, '\0', LOCATION_SIZE);
		memcpy(this->location.country, this->json_value.data, this->json_value.length);
	}
	else
	{
		return PARSE_COUNTRY_ERROR;
	}

	/* Parse city name data */
	this->findJsonValue(weather_data, "name");
	if (this->json_value.length)
	{
		memset(this->location.city, '\0', LOCATION_SIZE);
		memcpy(this->location.city, this->json_value.data, this->json_value.length);
	}
	else
	{
		return PARSE_CITYNAME_ERROR;
	}

	/* Parse timezone data */
	this->findJsonValue(weather_data, "timezone");
	if (this->json_value.length)
	{
		i_value = atoi(this->json_value.data);
		this->location.timezone = i_value;
	}
	else
	{
		return PARSE_TIMEZONE_ERROR;
	}

	/* Get date and time data */
	this->findJsonValue(weather_data, "dt");
	if (this->json_value.length)
	{
		i_value = atoi(this->json_value.data);
		i_value = i_value + this->location.timezone;
		convertUnixTimeToHumanTime(i_value);
	}
	else
	{
		return PARSE_TIME_ERROR;
	}
	return PARSE_DONE;
}

void OpenWeatherParse::showInforInConsole()
{
	Serial.printf("\n%s - %s - Timezone: %d", this->location.city, this->location.country, this->location.timezone);
	Serial.printf("\n%02d:%02d:%02d - %02d/%02d/%4d", this->time.hour, this->time.min, this->time.sec, this->time.day, this->time.month, this->time.year);
	Serial.printf("\nWeather ID: %d - icon code: %d - Description: %s", this->weather.id, this->weather.icon_code, this->weather.description);
	Serial.printf("\nTemperature: %.02f - Feel like: %.02f - Min: %.02f - Max: %.02f", this->weather.temp, this->weather.feels_like, this->weather.temp_min, this->weather.temp_max);
	Serial.printf("\nPessure: %d - Humidity: %d", this->weather.pressure, this->weather.humidity);
}
