#include "hardware_define.h"
#include "OpenWeatherParse.h"

const char *ssid = "Bao Long Lau 2";
const char *password = "25042009";

const String saigon = "http://api.openweathermap.org/data/2.5/weather?&units=metric&q=saigon&appid=e97f1ff2ec74d0bc7f1931e6daf796cb";

WiFiMulti wifiMulti;
//string weather_data = "\"{\"coord\":{\"lon\":-122.08,\"lat\":37.39},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clearsky\",\"icon\":\"01d\"}],\"base\":\"stations\",\"main\":{\"temp\":282.55,\"feels_like\":281.86,\"temp_min\":280.37,\"temp_max\":284.26,\"pressure\":1023,\"humidity\":100},\"visibility\":16093,\"wind\":{\"speed\":1.5,\"deg\":350},\"clouds\":{\"all\":1},\"dt\":1560350645,\"sys\":{\"type\":1,\"id\":5122,\"message\":0.0139,\"country\":\"US\",\"sunrise\":1560343627,\"sunset\":1560396563},\"timezone\":-25200,\"id\":420006353,\"name\":\"MountainView\",\"cod\":200}\"";

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
		Serial.printf("[%8d]Connecting to WiFi..\n", count);
		digitalWrite(LED, led_val);
		wifi_status = wifiMulti.run();
	} while ((wifi_status != WL_CONNECTED) && count < 100);

	if (wifi_status == WL_CONNECTED)
	{
		digitalWrite(LED, 1);
		Serial.println("Connected to the WiFi network");
	}
	else
	{
		digitalWrite(LED, 0);
		Serial.println("Cannot connect to the WiFi network");
	}

	return wifi_status;
}

void showPayloadData(String &payload)
{
	for (int i = 0; i < payload.length(); i++)
	{
		if (payload[i] == '{' || payload[i] == '[')
			Serial.printf("\n%c\n\t", payload[i]);
		else if (payload[i] == ',')
			Serial.printf("%c\n\t", payload[i]);
		else if (payload[i] == '}' || payload[i] == ']')
			Serial.printf("\n\b%c", payload[i]);
		else
			Serial.print(payload[i]);
	}
}
void getWeatherStatus()
{
	// wait for WiFi connection
	if ((wifiMulti.run() == WL_CONNECTED))
	{
		HTTPClient http;
		OpenWeatherParse weather;

		Serial.print("[HTTP] begin...\n");
		// configure traged server and url
		http.begin(saigon); //HTTP

		Serial.print("[HTTP] GET...\n");
		// start connection and send HTTP header
		int httpCode = http.GET();
		String payload;
		// httpCode will be negative on error
		if (httpCode > 0)
		{
			// HTTP header has been send and Server response header has been handled
			Serial.printf("[HTTP] GET... code: %d\n", httpCode);

			payload = http.getString();

			// file found at server
			if (httpCode == HTTP_CODE_OK)
			{
				/* Showing Json data */
				//showPayloadData(payload);
				int error_code = weather.parseOpenWeatherData(payload);
				Serial.printf("\nDebug code: %d", error_code);
				if (error_code == PARSE_DONE)
				{
					weather.TestFunction();
				}
			}
			else if (httpCode == HTTP_CODE_TOO_MANY_REQUESTS)
			{
				Serial.println(payload);
			}
		}
		else
		{
			Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
		}

		http.end();
	}
}

void setup()
{
	/* Enable Serial port */
	bool rlst = false;
	Serial.begin(115200);
	Serial.println();
	Serial.println("setup");

	/* Enable SPI for E-ink LCD */
	SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, ELINK_SS);
	display.init(); // enable diagnostic output on Serial
	display.setTextColor(GxEPD_BLACK);

	rlst = setupSDCard();

	display.setRotation(0);
	display.fillScreen(GxEPD_WHITE);

	display.drawExampleBitmap(BitmapExample1, 0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, GxEPD_BLACK);

	display.setRotation(1);
	display.setCursor(20, display.height() - 15);
	String sizeString = "SD:" + String(SD.cardSize() / 1024.0 / 1024.0 / 1024.0) + "G";
	display.println(rlst ? sizeString : "SD:N/A");

	int16_t x1, x2;
	uint16_t w, h;
	String str = GxEPD_BitmapExamplesQ;
	str = str.substring(2, str.lastIndexOf("/"));
	display.getTextBounds(str, 0, 0, &x1, &x2, &w, &h);
	display.setCursor(display.width() - w - 5, display.height() - 15);
	display.println(str);
	display.update();

	connectToWifi();

	delay(5000);
}

void displayTest()
{
	display.setRotation(1);
	display.fillScreen(GxEPD_WHITE);
	display.setTextColor(GxEPD_BLACK);
}
void loop()
{
	// OpenWeatherParse weather;
	// //weather.TestFunction(weather_data); while(1);
	// int error_code = weather.parseOpenWeatherData(weather_data);
	// Serial.printf("\nDebug code: %d", error_code);
	// if(error_code == PARSE_DONE)
	// {
	// 	weather.TestFunction(weather_data);
	// }

	// while(1);
	display.setRotation(0);
	display.fillScreen(GxEPD_WHITE);
	uint16_t w, h;
	int i = 1;
	while (1)
	{

		//display.fillScreen(GxEPD_WHITE);
		display.setCursor(display.width() - 5, display.height() - 15);
		display.printf("Test: %d", i);
		i++;
		//display.update();
		delay(1000);
	}
	

	while (1)
	{
		getWeatherStatus();
		delay(1000);
	}
}
