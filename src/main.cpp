#include "hardware_define.h"
#include "OpenWeatherParse.h"
#include "WorldClockParse.h"
#include "std_data_define.h"

const char *ssid = "Bao Long Lau 2";
const char *password = "25042009";

const String saigon = "http://api.openweathermap.org/data/2.5/weather?&units=metric&q=saigon&appid=e97f1ff2ec74d0bc7f1931e6daf796cb";
const String world_clock = "http://worldclockapi.com/api/json/utc/now";
WiFiMulti wifiMulti;


/* Declare SPI for e-ink LCD */
GxIO_Class io(SPI, ELINK_SS, ELINK_DC, ELINK_RESET);
GxEPD_Class display(io, ELINK_RESET, ELINK_BUSY);

/* Declare SPI for SD card */
SPIClass SDSPI(VSPI);

bool setupSDCard(void)
{
	SDSPI.begin(SDCARD_CLK, SDCARD_MISO, SDCARD_MOSI);
	return SD.begin(SDCARD_SS, SDSPI);
}

uint8_t connectToWifi()
{
	int count = 0;
	int led_val = 1;
	uint8_t wifi_status = WL_CONNECT_FAILED;
	pinMode(LED, OUTPUT);
	digitalWrite(LED, 0);
	wifiMulti.addAP(ssid, password);
	/* wait for WiFi connection */
	do
	{
		led_val ^= 1;
		count++;
		delay(500);
		SLOG_DEBUG("[%8d]Connecting to WiFi..\n", count);
		digitalWrite(LED, led_val);
		wifi_status = wifiMulti.run();
	} while ((wifi_status != WL_CONNECTED) && count < 100);

	if (wifi_status == WL_CONNECTED)
	{
		digitalWrite(LED, 1);
		SLOG_DEBUG("Connected to the WiFi network");
	}
	else
	{
		digitalWrite(LED, 0);
		SLOG_DEBUG("Cannot connect to the WiFi network");
	}

	return wifi_status;
}

int sendRequestAndGetPayloadData(const String &url, String &rx_payload)
{
	HTTPClient http;
	int error = ERROR;
	String payload;

	// wait for WiFi connection
	if ((wifiMulti.run() == WL_CONNECTED))
	{
		SLOG_DEBUG("[HTTP] begin...: %s", url.c_str());
		// configure traged server and url
		http.begin(url);

		SLOG_DEBUG("[HTTP] GET...");
		// start connection and send HTTP header
		int httpCode = http.GET();
		// httpCode will be negative on error
		if (httpCode == HTTP_CODE_OK)
		{
			// HTTP header has been send and Server response header has been handled
			SLOG_DEBUG("[HTTP] GET... code: %d", httpCode);

			rx_payload = http.getString();
			// file found at server

			/* Showing Json data */
			//showPayloadData(payload);
			error = NO_ERROR;
		}
		else
		{
			SLOG_DEBUG("[HTTP] GET... failed, error[%d]: %s", httpCode, http.errorToString(httpCode).c_str());
		}

		http.end();
	}
	else
	{
		error = ERROR;
	}

	return error;
}

int getWeatherStatus(OpenWeatherParse &weather)
{
	int error = NO_ERROR;
	int error_code;
	//String weather_data = "\"{\"coord\":{\"lon\":-122.08,\"lat\":37.39},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clearsky\",\"icon\":\"01d\"}],\"base\":\"stations\",\"main\":{\"temp\":30,\"feels_like\":281.86,\"temp_min\":280.37,\"temp_max\":284.26,\"pressure\":1023,\"humidity\":100},\"visibility\":16093,\"wind\":{\"speed\":1.5,\"deg\":350},\"clouds\":{\"all\":1},\"dt\":1560350645,\"sys\":{\"type\":1,\"id\":5122,\"message\":0.0139,\"country\":\"US\",\"sunrise\":1560343627,\"sunset\":1560396563},\"timezone\":-25200,\"id\":420006353,\"name\":\"MountainView\",\"cod\":200}\"";

	String weather_data;
	error = sendRequestAndGetPayloadData(saigon, weather_data);
	if(error)
	{
		return error;
	}

	//Serial.print(payload);
	delay(1000);
	error_code = weather.parseOpenWeatherData(weather_data);

	if (error_code == PARSE_DONE)
	{
		error = NO_ERROR;
		weather.showInforInConsole();
	}
	else
	{
		error = ERROR;
		SLOG_ERROR("Error code: %d", error_code);
	}

	return error;
}

void setup()
{
	/* Enable Serial port */
	Serial.begin(115200);
	SLOG_DEBUG("Serial port setup");

	/* Enable SPI for E-ink LCD */
	SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, ELINK_SS);
	display.init(); // enable diagnostic output on Serial
	display.setTextColor(GxEPD_BLACK);

	display.setRotation(0);
	display.fillScreen(GxEPD_WHITE);

	//display.drawExampleBitmap(BitmapExample1, 0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, GxEPD_BLACK);

	display.update();

	connectToWifi();

	//delay(5000);
}

void displayToScreen(OpenWeatherParse &weather_data)
{
	int x, y;
	char infor[30];

	display.setRotation(1);
	display.fillScreen(GxEPD_WHITE);
	display.setTextColor(GxEPD_BLACK);
	display.setFont(&meteocons30pt7b);

	/* Display icon */
	display.fillScreen(GxEPD_WHITE);
	display.setTextColor(GxEPD_BLACK);
	display.setFont(&meteocons30pt7b);
	display.setCursor(5, 65);
	display.printf("%c", weather_data.weather.icon_code);

	/* Display content */
	display.setFont(&FreeSansBold9pt7b);
	display.setCursor(80, 20);
	display.printf("%s", weather_data.location.city);

	/* Display temperaturre */
	display.setFont(&FreeSansBold24pt7b);
	display.setCursor(5, 120);
	display.printf("%.0f", weather_data.weather.temp);
	x = display.getCursorX();
	y = display.getCursorY();
	display.setCursor(x, y - 15);
	display.setFont(&FreeSansBold12pt7b);
	display.print('O');

	x = display.getCursorX();
	display.setCursor(x, y);
	display.setFont(&FreeSansBold24pt7b);
	display.print('C');

	x = display.getCursorX();
	y = display.getCursorY();
	display.setFont(&FreeSans9pt7b);

	// display.setCursor(x + 10, y - 40);
	// display.printf("Tri dep trai vl :D");

	display.setCursor(x + 10, y - 20);
	display.printf("Humidity: %d%%", weather_data.weather.humidity);

	display.setCursor(x + 10, y);
	display.printf("Pressure: %d", weather_data.weather.pressure);

	/* Display date and time */
	memset(infor, '\0', 30);
	sprintf(infor, "%02d:%02d - %02d/%02d/%4d", weather_data.time.hour, weather_data.time.min, weather_data.time.day, weather_data.time.month, weather_data.time.year);
	display.setFont(&FreeSans9pt7b);
	display.setCursor(80, 40);
	display.printf("%s", infor);

	/* Display weather description */
	x = display.getCursorX();
	y = display.getCursorY();
	display.setCursor(80, 57);
	display.setFont();
	display.printf("%s", weather_data.weather.description);

	display.update();
}

int getCurrenTime(WorldClockParse &world_time)
{
	String payload;
	int error = 0;
	error = sendRequestAndGetPayloadData(world_clock, payload);
	Serial.println(payload);
	if (error == 0)
	{
		error = world_time.parseWorldClockData(payload);
	}
	return error;
}

void loop()
{
	OpenWeatherParse weather_data;
	WorldClockParse world_time;
	int old_temperature = 0;
	int8_t prv_min = 0;

	// while (1)
	// {
	// 	SLOG_DEBUG("Get weather data");
	// 	getWeatherStatus(weather_data);
	// 	delay(1000);

	// 	SLOG_DEBUG("Get time data");
	// 	getCurrenTime(world_time);
	// 	delay(6000);
	// }

	while (1)
	{
		if (getWeatherStatus(weather_data) == NO_ERROR)
		{
			if (getCurrenTime(world_time) == NO_ERROR)
			{
				if(world_time.time.min != prv_min || old_temperature != weather_data.weather.temp)
				{
					weather_data.time.day = world_time.time.day;
					weather_data.time.month = world_time.time.month;
					weather_data.time.year = world_time.time.year;

					weather_data.time.hour = world_time.time.hour;
					weather_data.time.min = world_time.time.min;

					old_temperature = weather_data.weather.temp;
					prv_min = world_time.time.min;

					displayToScreen(weather_data);
				}
			}
		}
		else
		{
			/* Show error on screen */
		}
		delay(10000);
	}
}
