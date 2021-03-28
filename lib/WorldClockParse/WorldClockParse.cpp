#include "WorldClockParse.h"

WorldClockParse::WorldClockParse(/* args */)
{
	this->json_value.data = nullptr;
	this->json_value.length = 0;
}

WorldClockParse::WorldClockParse(String &json)
{
	this->json_value.data = nullptr;
	this->json_value.length = 0;
}

WorldClockParse::~WorldClockParse()
{
}

#define SEC_TO_UNIX_EPOCH 11644473600LL

void WorldClockParse::convertFileTimeToHumanTime(long long int seconds, int8_t GMT_time)
{
	// Number of days in month
	// in normal year
	int daysOfMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	long int currYear, daysTillNow, extraTime, extraDays, index, date, month, hours, minutes, secondss, flag = 0;
	seconds /= (10000000LL);
	seconds -= SEC_TO_UNIX_EPOCH;
	seconds = seconds + (GMT_time * 3600); /* 3600 is the number of second in one hour */

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

int WorldClockParse::parseWorldClockData(String &json)
{
	long long int i_value = 0; /* This variable is used to converd from char to int */
	this->initJsonString(&json);

	/* Parse weather id */
	this->findJsonValue("currentFileTime");
	if (this->json_value.length)
	{
		this->convertDataTollint(this->json_value, i_value);
		this->convertFileTimeToHumanTime(i_value, 7);  /* VN time is GMT +7 */
		Serial.printf("\n%02d:%02d:%02d - %02d/%02d/%4d", this->time.hour, this->time.min, this->time.sec, this->time.day, this->time.month, this->time.year);
	}
	else
	{
		return 1;
	}

	return 0;
}