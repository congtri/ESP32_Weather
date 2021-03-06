#include "hardware_define.h"
#include "OpenWeatherParse.h"
#include "WorldClockParse.h"

const char *ssid = "Bao Long Lau 2";
const char *password = "25042009";

const String saigon = "http://api.openweathermap.org/data/2.5/weather?&units=metric&q=saigon&appid=e97f1ff2ec74d0bc7f1931e6daf796cb";
const String world_clock = "http://worldclockapi.com/api/json/est/now";
WiFiMulti wifiMulti;
HTTPClient http;

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



int sendRequestAndGetPayloadData(const String &api, String &rx_payload)
{
	int error = 0;
	String payload;

	// wait for WiFi connection
	if ((wifiMulti.run() == WL_CONNECTED))
	{
		SLOG_DEBUG("[HTTP] begin...");
		// configure traged server and url
		http.begin(api);

		SLOG_DEBUG("[HTTP] GET...");
		// start connection and send HTTP header
		int httpCode = http.GET();
		// httpCode will be negative on error
		if (httpCode > 0)
		{
			// HTTP header has been send and Server response header has been handled
			SLOG_DEBUG("[HTTP] GET... code: %d", httpCode);

			rx_payload = http.getString();
			// file found at server
			if (httpCode == HTTP_CODE_OK)
			{
				/* Showing Json data */
				//showPayloadData(payload);
				error = 0;
			}
		}
		else
		{
			error = 1;
			SLOG_DEBUG("[HTTP] GET... failed, error: %s", http.errorToString(httpCode).c_str());
		}

		http.end();
	}
	else
	{
		error = 1;
	}

	return error;
}

int getWeatherStatus(OpenWeatherParse &weather)
{
	int error = 1;
	String weather_data;
	int error_code;
	//String weather_data = "\"{\"coord\":{\"lon\":-122.08,\"lat\":37.39},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clearsky\",\"icon\":\"01d\"}],\"base\":\"stations\",\"main\":{\"temp\":30,\"feels_like\":281.86,\"temp_min\":280.37,\"temp_max\":284.26,\"pressure\":1023,\"humidity\":100},\"visibility\":16093,\"wind\":{\"speed\":1.5,\"deg\":350},\"clouds\":{\"all\":1},\"dt\":1560350645,\"sys\":{\"type\":1,\"id\":5122,\"message\":0.0139,\"country\":\"US\",\"sunrise\":1560343627,\"sunset\":1560396563},\"timezone\":-25200,\"id\":420006353,\"name\":\"MountainView\",\"cod\":200}\"";

	sendRequestAndGetPayloadData(saigon, weather_data);
	//Serial.print(payload);
	delay(1000);
	error_code = weather.parseOpenWeatherData(weather_data);
	
	if (error_code == PARSE_DONE)
	{
		error = 0;
		weather.showInforInConsole();
	}
	else
	{
		error = 1;
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
	display.printf("Humidity: %d", weather_data.weather.humidity);

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

void testWorldClock()
{
	while (1)
	{
		String payload;
		sendRequestAndGetPayloadData(world_clock, payload);
		Serial.println(payload);
		delay(1000);
	}
}
void loop()
{
	//testWorldClock();
	OpenWeatherParse weather_data;

	while (1)
	{
		delay(1000);
		if (getWeatherStatus(weather_data) == 0)
		{
			displayToScreen(weather_data);
		}
		else
		{
			/* Show error on screen */
		}
	}
}
