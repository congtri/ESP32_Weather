typedef struct OpenWeatherCodeMeaning
{
	int id;
	char *description;
	int icon_code;
} OpenWeatherCodeMeaning_st;

#define CODE_TABLE_SIZE 55

const OpenWeatherCodeMeaning_st OpenWeatherCodeMeaningTable[] = {
	// Group 2xx: Thunderstorm
	// ID	Main	Description	Icon
	{200, "thunderstorm with light rain", 11},
	{201, "thunderstorm with rain", 11},
	{202, "thunderstorm with heavy rain", 11},
	{210, "light thunderstorm", 11},
	{211, "thunderstorm", 11},
	{212, "heavy thunderstorm", 11},
	{221, "ragged thunderstorm", 11},
	{230, "thunderstorm with light drizzle", 11},
	{231, "thunderstorm with drizzle", 11},
	{232, "thunderstorm with heavy drizzle", 11},

	//Group 3xx: Drizzle
	//ID	Main	Description	Icon
	{300, "light intensity drizzle", 9},
	{301, "drizzle", 9},
	{302, "heavy intensity drizzle", 9},
	{310, "light intensity drizzle rain", 9},
	{311, "drizzle rain", 9},
	{312, "heavy intensity drizzle rain", 9},
	{313, "shower rain and drizzle", 9},
	{314, "heavy shower rain and drizzle", 9},
	{321, "shower drizzle", 9},

	//Group 5xx: Rain
	//ID	Main	Description	Icon
	{500, "light rain", 10},
	{501, "moderate rain", 10},
	{502, "heavy intensity rain", 10},
	{503, "very heavy rain", 10},
	{504, "extreme rain", 10},
	{511, "freezing rain", 13},
	{520, "light intensity shower rain", 9},
	{521, "shower rain", 9},
	{522, "heavy intensity shower rain", 9},
	{531, "ragged shower rain", 9},
	//Group 6xx: Snow
	//ID	Main	Description	Icon
	{600, "light snow", 13},
	{601, "Snow", 13},
	{602, "Heavy snow", 13},
	{611, "Sleet", 13},
	{612, "Light shower sleet", 13},
	{613, "Shower sleet", 13},
	{615, "Light rain and snow", 13},
	{616, "Rain and snow", 13},
	{620, "Light shower snow", 13},
	{621, "Shower snow", 13},
	{622, "Heavy shower snow", 13},
	//Group 7xx: Atmosphere
	//ID	Main	Description	Icon
	{701, "	mist", 50},
	{711, "e	Smoke", 50},
	{721, "	Haze", 50},
	{731, "	sand/ dust whirls", 50},
	{741, "fog", 50},
	{751, "sand", 50},
	{761, "dust", 50},
	{762, "volcanic ash", 50},
	{771, "ll	squalls", 50},
	{781, "ado	tornado", 50},
	//Group 800: Clear
	//ID	Main	Description	Icon
	{800, "clear sky", 01},

	//Group 80x: Clouds
	//ID	Main	Description	Icon
	{801, "few clouds: 11-25%", 2},
	{802, "scattered clouds: 25-50%", 3},
	{803, "broken clouds: 51-84%", 4},
	{804, "overcast clouds: 85-100%", 4}};
