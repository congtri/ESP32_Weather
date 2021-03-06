#ifndef _HARDWARE_DEFINE_
#define _HARDWARE_DEFINE_

#include <GxEPD.h> //https://github.com/lewisxhe/GxEPD
#include <SD.h>
#include <FS.h>
#include <Arduino.h>
#include <Wire.h>
#include <GxEPD.h>
#include "SPI.h"
#include <GxGDEH0213B73/GxGDEH0213B73.h> // 2.13" b/w newer panel

#include GxEPD_BitmapExamples

// FreeFonts from Adafruit_GFX
#include "meteocons30pt7b.h"
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/Picopixel.h>
#include <Fonts/Org_01.h>

#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <WiFi.h>

#include <WiFiMulti.h>
#include <HTTPClient.h>

#include "serial_debug.h"

#define SPI_MOSI 23
#define SPI_MISO -1
#define SPI_CLK 18

#define ELINK_SS 5
#define ELINK_BUSY 4
#define ELINK_RESET 16
#define ELINK_DC 17

#define SDCARD_SS 13
#define SDCARD_CLK 14
#define SDCARD_MOSI 15
#define SDCARD_MISO 2

#define BUTTON_PIN 39

#define BUTTON_3 BUTTON_PIN
#define EPD_CS ELINK_SS
#define EPD_DC ELINK_DC
#define EPD_RSET ELINK_RESET
#define EPD_BUSY ELINK_BUSY

#define LED 19

#endif