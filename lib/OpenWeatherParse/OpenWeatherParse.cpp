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
	// Initialize all structures to zero
	memset(&location, 0, sizeof(location));
	memset(&weather, 0, sizeof(weather));
	memset(&time, 0, sizeof(time));
	
	// Set default values
	weather.icon_code = ')'; // Default "not available" icon
}

OpenWeatherParse::~OpenWeatherParse()
{
}

// Helper method for safe string copying
void OpenWeatherParse::safeStringCopy(char* dest, const String& src, size_t destSize)
{
	if (dest == nullptr || destSize == 0) return;
	
	memset(dest, '\0', destSize);
	size_t copyLen = (src.length() < destSize - 1) ? src.length() : destSize - 1;
	strncpy(dest, src.c_str(), copyLen);
}

// Helper method to validate icon codes
bool OpenWeatherParse::isValidIconCode(const String& icon)
{
	if (icon.length() != 3) return false;
	
	for (uint8_t idx = 0; idx < ICON_MAP_TABLE_SIZE; idx++) {
		if (icon.equals(icon_map[idx].icon)) {
			return true;
		}
	}
	return false;
}

// Get error message for debugging
const char* OpenWeatherParse::getErrorMessage(parser_error_code_e error)
{
	switch (error) {
		case PARSE_DONE: return "Success";
		case PARSE_ID_ERROR: return "Weather ID parsing failed";
		case PARSE_DESCRIPTION_ERROR: return "Description parsing failed";
		case PARSE_ICON_ERROR: return "Icon parsing failed";
		case PARSE_TEMP_ERROR: return "Temperature parsing failed";
		case PARSE_FEELS_LIKE_ERROR: return "Feels like temperature parsing failed";
		case PARSE_TEMP_MIN_ERROR: return "Minimum temperature parsing failed";
		case PARSE_TEMP_MAX_ERROR: return "Maximum temperature parsing failed";
		case PARSE_PRESSURE_ERROR: return "Pressure parsing failed";
		case PARSE_HUMIDITY_ERROR: return "Humidity parsing failed";
		case PARSE_COUNTRY_ERROR: return "Country parsing failed";
		case PARSE_CITY_NAME_ERROR: return "City name parsing failed";
		case PARSE_TIMEZONE_ERROR: return "Timezone parsing failed";
		case PARSE_TIME_ERROR: return "Time parsing failed";
		case PARSE_JSON_INVALID: return "Invalid JSON format";
		default: return "Unknown error";
	}
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


parser_error_code_e OpenWeatherParse::parseOpenWeatherData(String &weather_data)
{
	uint8_t idx = 0;
	int i_value = 0;     /* This variable is used to convert from string to int */
	float f_value = 0;   /* This variable is used to convert from string to float */
	
	// Validate input
	if (weather_data.length() == 0) {
		return PARSE_JSON_INVALID;
	}
	
	// Use modern API to set JSON data
	if (!this->setJsonData(&weather_data)) {
		return PARSE_JSON_INVALID;
	}

	/* Parse weather id */
	if (this->parseValue("id") && this->convertToInt(i_value))
	{
		this->weather.id = i_value;
	}
	else
	{
		return PARSE_ID_ERROR;
	}

	/* Parse weather description */
	if (this->parseValueFromJson(weather_data, "description"))
	{
		String description = this->getValueAsString();
		this->safeStringCopy(this->weather.description, description, sizeof(this->weather.description));
	}
	else
	{
		return PARSE_DESCRIPTION_ERROR;
	}

	/* Parse weather icon */
	if (this->parseValueFromJson(weather_data, "icon"))
	{
		String icon = this->getValueAsString();
		
		// Validate and set icon code
		this->weather.icon_code = ')'; /* Default NA data icon */
		if (this->isValidIconCode(icon)) {
			for (idx = 0; idx < ICON_MAP_TABLE_SIZE; idx++)
			{
				if (icon.equals(icon_map[idx].icon))
				{
					this->weather.icon_code = icon_map[idx].code;
					break;
				}
			}
		}
	}
	else
	{
		return PARSE_ICON_ERROR;
	}

	/* Parse temperature data */
	if (this->parseValueFromJson(weather_data, "temp") && this->convertToFloat(f_value))
	{
		this->weather.temp = f_value;
	}
	else
	{
		return PARSE_TEMP_ERROR;
	}

	/* Parse temperature feels_like data */
	if (this->parseValueFromJson(weather_data, "feels_like") && this->convertToFloat(f_value))
	{
		this->weather.feels_like = f_value;
	}
	else
	{
		return PARSE_FEELS_LIKE_ERROR;
	}

	/* Parse temperature temp_min data */
	if (this->parseValueFromJson(weather_data, "temp_min") && this->convertToFloat(f_value))
	{
		this->weather.temp_min = f_value;
	}
	else
	{
		return PARSE_TEMP_MIN_ERROR;
	}

	/* Parse temperature temp_max data */
	if (this->parseValueFromJson(weather_data, "temp_max") && this->convertToFloat(f_value))
	{
		this->weather.temp_max = f_value;
	}
	else
	{
		return PARSE_TEMP_MAX_ERROR;
	}

	/* Parse pressure data */
	if (this->parseValueFromJson(weather_data, "pressure") && this->convertToInt(i_value))
	{
		this->weather.pressure = i_value;
	}
	else
	{
		return PARSE_PRESSURE_ERROR;
	}

	/* Parse humidity data */
	if (this->parseValueFromJson(weather_data, "humidity") && this->convertToInt(i_value))
	{
		this->weather.humidity = i_value;
	}
	else
	{
		return PARSE_HUMIDITY_ERROR;
	}

	/* Parse location data */
	if (this->parseValueFromJson(weather_data, "country"))
	{
		String country = this->getValueAsString();
		this->safeStringCopy(this->location.country, country, sizeof(this->location.country));
	}
	else
	{
		return PARSE_COUNTRY_ERROR;
	}

	/* Parse city name data */
	if (this->parseValueFromJson(weather_data, "name"))
	{
		String city = this->getValueAsString();
		this->safeStringCopy(this->location.city, city, sizeof(this->location.city));
	}
	else
	{
		return PARSE_CITY_NAME_ERROR;
	}

	/* Parse timezone data */
	if (this->parseValueFromJson(weather_data, "timezone") && this->convertToInt(i_value))
	{
		this->location.timezone = i_value;
	}
	else
	{
		return PARSE_TIMEZONE_ERROR;
	}

	/* Get date and time data */
	if (this->parseValueFromJson(weather_data, "dt") && this->convertToInt(i_value))
	{
		i_value = i_value + this->location.timezone;
		convertUnixTimeToHumanTime(i_value);
	}
	else
	{
		return PARSE_TIME_ERROR;
	}
	
	return PARSE_DONE;
}

void OpenWeatherParse::showInfoInConsole()
{
	Serial.printf("\n%s - %s - Timezone: %d", this->location.city, this->location.country, this->location.timezone);
	Serial.printf("\n%02d:%02d:%02d - %02d/%02d/%4d", this->time.hour, this->time.min, this->time.sec, this->time.day, this->time.month, this->time.year);
	Serial.printf("\nWeather ID: %d - icon code: %d - Description: %s", this->weather.id, this->weather.icon_code, this->weather.description);
	Serial.printf("\nTemperature: %.02f - Feel like: %.02f - Min: %.02f - Max: %.02f", this->weather.temp, this->weather.feels_like, this->weather.temp_min, this->weather.temp_max);
	Serial.printf("\nPressure: %d - Humidity: %d", this->weather.pressure, this->weather.humidity);
}
