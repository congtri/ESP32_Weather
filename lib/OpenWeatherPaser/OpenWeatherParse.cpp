#include "OpenWeatherParse.h"
#include "OpenWeatherCodeMeaning.h"

typedef struct
{
	char *data;
	int length;
} sub_data_st;

#define IS_PARSED_DATA_VALID(DATA) (DATA.length)
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

void showData(sub_data_st &dat)
{
	if (dat.data != nullptr)
	{
		cout << "Parsed data: ";
		int i;
		for (i = 0; i < dat.length; i++)
		{
			cout << dat.data[i];
		}
		cout << " - lenght: " << dat.length << endl;
	}
	else
	{
		cout << "\nData is empty" << endl;
	}
}

sub_data_st findJsonValue(String &data, String subdata)
{
	int s_pos = 0, e_pos = 0;
	sub_data_st dat = {nullptr, 0};

	/* Find the key of json */
	s_pos = data.indexOf(subdata);

	if (s_pos != string::npos)
	{
		s_pos = s_pos + subdata.length() + PADDING;
		e_pos = s_pos + 1;

		if (data.charAt(s_pos) == '"')
		{
			s_pos++;
			e_pos++;
		}

		/* Loking for the end position of json string value */
		for (; e_pos < data.length(); e_pos++)
		{
			if (data.charAt(e_pos) == '\n' || data.charAt(e_pos) == ',' || data.charAt(e_pos) == '"')
			{
				dat.data = &data[s_pos];
				dat.length = e_pos - s_pos;
				break;
			}
		}
	}

	return dat;
}

paser_error_code_e OpenWeatherParse::parseOpenWeatherData(String &weather_data)
{
	sub_data_st parsed_value;
	uint8_t idx = 0;
	long int i_value = 0; /* This variable is used to converd from char to int */
	float f_value = 0; /* This variable is used to converd from char to float */

	/* Get date and time data */
	parsed_value = findJsonValue(weather_data, "dt");
	if (IS_PARSED_DATA_VALID(parsed_value))
	{
		i_value = atoi(parsed_value.data);
		convertUnixTimeToHumanTime(i_value);
	}
	else
	{
		return PARSE_TIME_ERROR;
	}

	/* Parse weather data */
	parsed_value = findJsonValue(weather_data, "id");
	if (IS_PARSED_DATA_VALID(parsed_value))
	{
		i_value = atoi(parsed_value.data);
		this->weather.id = (int)i_value;

		for (idx = 0; idx < CODE_TABLE_SIZE; idx++)
		{
			if (i_value == OpenWeatherCodeMeaningTable[idx].id)
			{
				this->weather.description = OpenWeatherCodeMeaningTable[idx].description;
				this->weather.weather_icon_code = OpenWeatherCodeMeaningTable[idx].icon_code;
			}
		}
	}
	else
	{
		return PARSE_ID_ERROR;
	}

	/* Parse tempareture data */
	parsed_value = findJsonValue(weather_data, "temp");
	if (IS_PARSED_DATA_VALID(parsed_value))
	{
		f_value = atof(parsed_value.data);
		this->weather.temp = f_value;
	}
	else
	{
		return PARSE_ID_ERROR;
	}

	/* Parse tempareture feels_like data */
	parsed_value = findJsonValue(weather_data, "feels_like");
	if (IS_PARSED_DATA_VALID(parsed_value))
	{
		f_value = atof(parsed_value.data);
		this->weather.feels_like = f_value;
	}
	else
	{
		return PARSE_ID_ERROR;
	}

	/* Parse tempareture temp_min data */
	parsed_value = findJsonValue(weather_data, "temp_min");
	if (IS_PARSED_DATA_VALID(parsed_value))
	{
		f_value = atof(parsed_value.data);
		this->weather.temp_min = f_value;
	}
	else
	{
		return PARSE_ID_ERROR;
	}

	/* Parse tempareture temp_max data */
	parsed_value = findJsonValue(weather_data, "temp_max");
	if (IS_PARSED_DATA_VALID(parsed_value))
	{
		f_value = atof(parsed_value.data);
		this->weather.temp_max = f_value;
	}
	else
	{
		return PARSE_ID_ERROR;
	}

	/* Parse pressure data */
	parsed_value = findJsonValue(weather_data, "pressure");
	if (IS_PARSED_DATA_VALID(parsed_value))
	{
		i_value = atoi(parsed_value.data);
		this->weather.pressure = i_value;
	}
	else
	{
		return PARSE_ID_ERROR;
	}

	/* Parse humidity data */
	parsed_value = findJsonValue(weather_data, "humidity");
	if (IS_PARSED_DATA_VALID(parsed_value))
	{
		i_value = atoi(parsed_value.data);
		this->weather.humidity = i_value;
	}
	else
	{
		return PARSE_ID_ERROR;
	}

	/* Parse location data */
	parsed_value = findJsonValue(weather_data, "country");
	if (IS_PARSED_DATA_VALID(parsed_value))
	{
		memset(this->location.country, '\0', LOCATION_SIZE);
		memcpy(this->location.country,parsed_value.data,parsed_value.length);
	}
	else
	{
		return PARSE_ID_ERROR;
	}

	/* Parse city name data */
	parsed_value = findJsonValue(weather_data, "name");
	if (IS_PARSED_DATA_VALID(parsed_value))
	{
		memset(this->location.city, '\0', LOCATION_SIZE);
		memcpy(this->location.city,parsed_value.data,parsed_value.length);
	}
	else
	{
		return PARSE_ID_ERROR;
	}

	/* Parse timezone data */
	parsed_value = findJsonValue(weather_data, "timezone");
	if (IS_PARSED_DATA_VALID(parsed_value))
	{
		i_value = atoi(parsed_value.data);
		this->location.timezone = i_value;
	}
	else
	{
		return PARSE_ID_ERROR;
	}
	return PARSE_DONE;
}

void OpenWeatherParse::TestFunction()
{
	Serial.printf("\n%s - %s - Timezone: %d", this->location.city, this->location.country, this->location.timezone);
	Serial.printf("\n%d:%d:%d - %d\/%d\/%d", this->time.hour, this->time.min, this->time.sec, this->time.day, this->time.month, this->time.year);
	Serial.printf("\nWeather ID: %d - icon code: %d - Description: %s", this->weather.id, this->weather.weather_icon_code, this->weather.description);
	Serial.printf("\nTemperature: %f - Feel like: %f - Min: %f - Max: %f", this->weather.temp, this->weather.feels_like, this->weather.temp_min, this->weather.temp_max);
	Serial.printf("\nPessure: %d - Humidity: %d", this->weather.pressure, this->weather.humidity);
}

