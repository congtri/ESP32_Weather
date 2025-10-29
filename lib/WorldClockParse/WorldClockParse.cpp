#include "WorldClockParse.h"

#define SEC_TO_UNIX_EPOCH 11644473600LL
#define SECONDS_PER_DAY (24 * 60 * 60)
#define SECONDS_PER_HOUR 3600
#define SECONDS_PER_MINUTE 60

WorldClockParse::WorldClockParse()
{
	this->initializeTimeStruct();
}

WorldClockParse::WorldClockParse(const String &json)
{
	this->initializeTimeStruct();
	String jsonCopy = json;  // Create a non-const copy
	this->setJsonData(&jsonCopy);
}

WorldClockParse::~WorldClockParse()
{
}

/* Helper method to initialize time structure */
void WorldClockParse::initializeTimeStruct()
{
	this->time.year = 0;
	this->time.month = 0;
	this->time.day = 0;
	this->time.hour = 0;
	this->time.min = 0;
	this->time.sec = 0;
}

/* Helper method to validate timezone offset */
bool WorldClockParse::isValidTimezone(int8_t timezone)
{
	return (timezone >= -12 && timezone <= 14);
}

/* Helper method to check leap year */
bool WorldClockParse::isLeapYear(uint16_t year)
{
	return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
}

#define SEC_TO_UNIX_EPOCH 11644473600LL
#define SECONDS_PER_DAY (24 * 60 * 60)
#define SECONDS_PER_HOUR 3600
#define SECONDS_PER_MINUTE 60

void WorldClockParse::convertFileTimeToHumanTime(long long int seconds, int8_t timezoneOffset)
{
	// Validate timezone offset
	if (!this->isValidTimezone(timezoneOffset)) {
		timezoneOffset = 0; // Default to UTC
	}

	// Number of days in month for normal year
	int daysOfMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	// Convert Windows file time to Unix timestamp
	seconds /= 10000000LL;  // Convert from 100-nanosecond intervals to seconds
	seconds -= SEC_TO_UNIX_EPOCH;  // Adjust from Windows epoch to Unix epoch
	seconds += (timezoneOffset * SECONDS_PER_HOUR);  // Apply timezone offset

	// Calculate total days since Unix epoch
	long int daysTillNow = seconds / SECONDS_PER_DAY;
	long int extraTime = seconds % SECONDS_PER_DAY;
	
	// Start from Unix epoch (1970)
	long int currYear = 1970;

	// Calculate current year
	while (daysTillNow >= 365) {
		if (this->isLeapYear(currYear)) {
			daysTillNow -= 366;
		} else {
			daysTillNow -= 365;
		}
		currYear++;
	}

	// Calculate days remaining for current year (1-based)
	long int extraDays = daysTillNow + 1;
	bool isCurrentYearLeap = this->isLeapYear(currYear);

	// Update February days for leap year
	if (isCurrentYearLeap) {
		daysOfMonth[1] = 29;
	}

	// Calculate month and day
	int month = 0;
	for (int index = 0; index < 12; index++) {
		if (extraDays <= daysOfMonth[index]) {
			month = index + 1;  // 1-based month
			break;
		}
		extraDays -= daysOfMonth[index];
	}

	// Calculate time components
	long int hours = extraTime / SECONDS_PER_HOUR;
	long int minutes = (extraTime % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE;
	long int secondsRem = extraTime % SECONDS_PER_MINUTE;

	// Store results in time structure
	this->time.year = currYear;
	this->time.month = month;
	this->time.day = extraDays;
	this->time.hour = hours;
	this->time.min = minutes;
	this->time.sec = secondsRem;
}

world_clock_error_code_e WorldClockParse::parseWorldClockData(const String &json, int8_t timezoneOffset)
{
	// Validate timezone offset
	if (!this->isValidTimezone(timezoneOffset)) {
		return WORLD_CLOCK_PARSE_TIMEZONE_INVALID;
	}

	// Validate input JSON is not empty
	if (json.length() == 0) {
		return WORLD_CLOCK_PARSE_JSON_INVALID;
	}

	// Create a non-const copy for setJsonData
	String jsonCopy = json;
	
	// Set JSON data using modern API
	if (!this->setJsonData(&jsonCopy)) {
		return WORLD_CLOCK_PARSE_JSON_INVALID;
	}

	// Parse currentFileTime - this is the field from worldclockapi.com
	long long int timeValue = 0;
	if (!this->parseValueFromJson(jsonCopy, "currentFileTime")) {
		return WORLD_CLOCK_PARSE_TIME_NOT_FOUND;
	}

	if (!this->convertToLongLong(timeValue)) {
		return WORLD_CLOCK_PARSE_TIME_INVALID;
	}

	// Validate that time value is reasonable (not zero or negative)
	if (timeValue <= 0) {
		return WORLD_CLOCK_PARSE_TIME_INVALID;
	}

	// Convert file time to human readable time
	this->convertFileTimeToHumanTime(timeValue, timezoneOffset);

	return WORLD_CLOCK_PARSE_SUCCESS;
}

/* Legacy compatibility method - returns int for backward compatibility */
int WorldClockParse::parseWorldClockData(String &json)
{
	world_clock_error_code_e result = this->parseWorldClockData((const String&)json, 7);
	
	// Convert modern error codes to legacy return values
	switch (result) {
		case WORLD_CLOCK_PARSE_SUCCESS:
			return 0;  // Success
		default:
			return 1;  // Error
	}
}

/* Helper method to display time in console */
void WorldClockParse::showTimeInConsole() const
{
	Serial.printf("World Clock: %02d:%02d:%02d - %02d/%02d/%4d (GMT%+d)", 
		this->time.hour, this->time.min, this->time.sec, 
		this->time.day, this->time.month, this->time.year,
		7); // Assuming Vietnam time for display
}

/* Helper method to get error message */
const char* WorldClockParse::getErrorMessage(world_clock_error_code_e errorCode) const
{
	switch (errorCode) {
		case WORLD_CLOCK_PARSE_SUCCESS: return "Success";
		case WORLD_CLOCK_PARSE_JSON_INVALID: return "Invalid JSON data";
		case WORLD_CLOCK_PARSE_TIME_NOT_FOUND: return "Time data not found in JSON";
		case WORLD_CLOCK_PARSE_TIME_INVALID: return "Invalid time format";
		case WORLD_CLOCK_PARSE_TIMEZONE_INVALID: return "Invalid timezone offset";
		default: return "Unknown error";
	}
}