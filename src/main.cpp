#include "hardware_define.h"
#include "OpenWeatherParse.h"
#include "WorldClockParse.h"
#include "std_data_define.h"
#include <WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include "web_resources.h"

// WiFi configuration constants
#define EEPROM_SIZE 512
#define SSID_ADDR 0
#define PASSWORD_ADDR 100
#define SSID_MAX_LENGTH 32
#define PASSWORD_MAX_LENGTH 64
#define SETUP_BUTTON_TIMEOUT 3000  // 3 seconds timeout for setup button

// Default WiFi credentials (fallback)
const char *default_ssid = "Bao Long Lau 2";
const char *default_password = "25042009";

// Current WiFi credentials (loaded from EEPROM or default)
String current_ssid = "";
String current_password = "";

const String saigon = "http://api.openweathermap.org/data/2.5/weather?&units=metric&q=saigon&appid=e97f1ff2ec74d0bc7f1931e6daf796cb";
const String world_clock = "http://worldclockapi.com/api/json/utc/now";

WiFiMulti wifiMulti;
WebServer server(80);
DNSServer dnsServer;

// Setup mode flag
bool setupMode = false;


/* Declare SPI for e-ink LCD */
GxIO_Class io(SPI, ELINK_SS, ELINK_DC, ELINK_RESET);
GxEPD_Class display(io, ELINK_RESET, ELINK_BUSY);

/* Declare SPI for SD card */
SPIClass SDSPI(VSPI);

// WiFi Configuration Functions
void saveWiFiCredentials(const String& ssid, const String& password) {
	EEPROM.begin(EEPROM_SIZE);
	
	// Clear the EEPROM areas
	for (int i = 0; i < SSID_MAX_LENGTH; i++) {
		EEPROM.write(SSID_ADDR + i, 0);
	}
	for (int i = 0; i < PASSWORD_MAX_LENGTH; i++) {
		EEPROM.write(PASSWORD_ADDR + i, 0);
	}
	
	// Write SSID
	for (int i = 0; i < ssid.length() && i < SSID_MAX_LENGTH - 1; i++) {
		EEPROM.write(SSID_ADDR + i, ssid[i]);
	}
	
	// Write Password
	for (int i = 0; i < password.length() && i < PASSWORD_MAX_LENGTH - 1; i++) {
		EEPROM.write(PASSWORD_ADDR + i, password[i]);
	}
	
	EEPROM.commit();
	EEPROM.end();
	
	SLOG_DEBUG("WiFi credentials saved to EEPROM");
}

void loadWiFiCredentials() {
	EEPROM.begin(EEPROM_SIZE);
	
	// Read SSID
	current_ssid = "";
	for (int i = 0; i < SSID_MAX_LENGTH - 1; i++) {
		char c = EEPROM.read(SSID_ADDR + i);
		if (c == 0) break;
		current_ssid += c;
	}
	
	// Read Password
	current_password = "";
	for (int i = 0; i < PASSWORD_MAX_LENGTH - 1; i++) {
		char c = EEPROM.read(PASSWORD_ADDR + i);
		if (c == 0) break;
		current_password += c;
	}
	
	EEPROM.end();
	
	// Use default credentials if EEPROM is empty
	if (current_ssid.length() == 0) {
		current_ssid = default_ssid;
		current_password = default_password;
		SLOG_DEBUG("Using default WiFi credentials");
	} else {
		SLOG_DEBUG("Loaded WiFi credentials from EEPROM: %s", current_ssid.c_str());
	}
}

void handleRoot() {
	String html = WebResources::getSetupPage();
	server.send(200, WebResources::getContentType(), html);
}

void handleScan() {
	// Scan for available WiFi networks
	int networkCount = WiFi.scanNetworks();
	
	String json = "[";
	for (int i = 0; i < networkCount; i++) {
		if (i > 0) json += ",";
		json += "{";
		json += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
		json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
		json += "\"secure\":" + String(WiFi.encryptionType(i) != WIFI_AUTH_OPEN ? "true" : "false");
		json += "}";
	}
	json += "]";
	
	server.send(200, "application/json", json);
}

void handleSave() {
	String ssid = server.arg("ssid");
	String password = server.arg("password");
	
	if (ssid.length() > 0) {
		saveWiFiCredentials(ssid, password);
		
		String html = WebResources::getSuccessPage(ssid);
		server.send(200, WebResources::getContentType(), html);
		
		// Schedule restart
		delay(100);
		ESP.restart();
	} else {
		server.send(400, "text/plain", "Invalid SSID");
	}
}

void handleRestart() {
	String html = WebResources::getRestartPage();
	server.send(200, WebResources::getContentType(), html);
	delay(100);
	ESP.restart();
}

void startAPMode() {
	WiFi.mode(WIFI_AP);
	WiFi.softAP("ESP32-WeatherStation-Setup", "12345678");
	
	// Setup DNS server for captive portal
	dnsServer.start(53, "*", WiFi.softAPIP());
	
	// Setup web server routes
	server.on("/", handleRoot);
	server.on("/save", HTTP_POST, handleSave);
	server.on("/restart", handleRestart);
	server.on("/scan", handleScan);  // New endpoint for WiFi scanning
	
	// Handle all other requests (captive portal)
	server.onNotFound([]() {
		server.sendHeader("Location", "/", true);
		server.send(302, "text/plain", "");
	});
	
	server.begin();
	
	SLOG_DEBUG("AP Mode started - SSID: ESP32-WeatherStation-Setup");
	SLOG_DEBUG("Web server started on IP: %s", WiFi.softAPIP().toString().c_str());
	SLOG_DEBUG("Available endpoints: /, /save, /restart, /scan");
}

bool checkSetupButton() {
	pinMode(BUTTON_PIN, INPUT_PULLUP);
	
	unsigned long startTime = millis();
	bool buttonPressed = false;
	
	SLOG_DEBUG("Checking for setup button press (3 seconds)...");
	
	// Check button for 3 seconds
	while (millis() - startTime < SETUP_BUTTON_TIMEOUT) {
		if (digitalRead(BUTTON_PIN) == LOW) {
			buttonPressed = true;
			SLOG_DEBUG("Setup button detected!");
			
			// Wait for button release or timeout
			while (digitalRead(BUTTON_PIN) == LOW && (millis() - startTime < SETUP_BUTTON_TIMEOUT)) {
				delay(50);
			}
			break;
		}
		delay(50);
	}
	
	return buttonPressed;
}

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
	
	// Use current WiFi credentials
	wifiMulti.addAP(current_ssid.c_str(), current_password.c_str());
	SLOG_DEBUG("Attempting to connect to WiFi: %s", current_ssid.c_str());
	
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
		SLOG_DEBUG("IP address: %s", WiFi.localIP().toString().c_str());
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
		weather.showInfoInConsole();
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
	
	/* Load WiFi credentials from EEPROM */
	loadWiFiCredentials();

	/* Enable SPI for E-ink LCD */
	SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, ELINK_SS);
	display.init(); // enable diagnostic output on Serial
	display.setTextColor(GxEPD_BLACK);

	display.setRotation(0);
	display.fillScreen(GxEPD_WHITE);
	
	// Display startup message
	display.setFont(&FreeSansBold12pt7b);
	display.setCursor(10, 30);
	display.printf("ESP32 Weather Station");
	display.setFont(&FreeSans9pt7b);
	display.setCursor(10, 60);
	display.printf("Starting up...");
	display.setCursor(10, 80);
	display.printf("Press button for setup");
	display.update();

	/* Check if setup button is pressed within 3 seconds */
	if (checkSetupButton()) {
		setupMode = true;
		SLOG_DEBUG("Entering setup mode...");
		
		// Display setup mode message
		display.fillScreen(GxEPD_WHITE);
		display.setFont(&FreeSansBold12pt7b);
		display.setCursor(10, 30);
		display.printf("Setup Mode");
		display.setFont(&FreeSans9pt7b);
		display.setCursor(10, 60);
		display.printf("Connect to WiFi:");
		display.setCursor(10, 80);
		display.printf("ESP32-WeatherStation-Setup");
		display.setCursor(10, 100);
		display.printf("Password: 12345678");
		display.setCursor(10, 120);
		display.printf("Web interface ready");
		display.update();
		
		startAPMode();
	} else {
		setupMode = false;
		SLOG_DEBUG("Entering normal mode...");
		
		// Display connecting message
		display.fillScreen(GxEPD_WHITE);
		display.setFont(&FreeSansBold12pt7b);
		display.setCursor(10, 30);
		display.printf("Normal Mode");
		display.setFont(&FreeSans9pt7b);
		display.setCursor(10, 60);
		display.printf("Connecting to:");
		display.setCursor(10, 80);
		display.printf("%s", current_ssid.c_str());
		display.update();
		
		connectToWifi();
		
		// Clear screen after connection
		display.fillScreen(GxEPD_WHITE);
		display.update();
	}
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
	if (setupMode) {
		// Handle setup mode - run web server
		dnsServer.processNextRequest();
		server.handleClient();
		delay(100);
	} else {
		// Handle normal mode - weather station functionality
		OpenWeatherParse weather_data;
		WorldClockParse world_time;
		int old_temperature = 0;
		int8_t prv_min = 0;

		// Check WiFi connection
		if (WiFi.status() != WL_CONNECTED) {
			SLOG_DEBUG("WiFi disconnected, attempting to reconnect...");
			display.fillScreen(GxEPD_WHITE);
			display.setFont(&FreeSansBold12pt7b);
			display.setCursor(10, 30);
			display.printf("WiFi Error");
			display.setFont(&FreeSans9pt7b);
			display.setCursor(10, 60);
			display.printf("Reconnecting...");
			display.update();
			
			connectToWifi();
		}

		while (1)
		{
			// Check if user wants to enter setup mode during operation
			// Quick double-tap of button during normal operation
			if (digitalRead(BUTTON_PIN) == LOW) {
				delay(50); // debounce
				if (digitalRead(BUTTON_PIN) == LOW) {
					unsigned long pressStart = millis();
					while (digitalRead(BUTTON_PIN) == LOW && (millis() - pressStart < 2000)) {
						delay(50);
					}
					// If button was held for more than 1 second, enter setup mode
					if (millis() - pressStart > 1000) {
						SLOG_DEBUG("Setup mode requested during operation");
						ESP.restart(); // Restart to enter setup mode on next boot
					}
				}
			}
			
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
				display.fillScreen(GxEPD_WHITE);
				display.setFont(&FreeSansBold12pt7b);
				display.setCursor(10, 30);
				display.printf("Error");
				display.setFont(&FreeSans9pt7b);
				display.setCursor(10, 60);
				display.printf("Failed to get weather data");
				display.setCursor(10, 80);
				display.printf("Retrying in 30 seconds...");
				display.update();
				delay(30000);  // Wait longer on error
				continue;
			}
			delay(10000);
		}
	}
}
