/*+===================================================================
  Project:  ESP32 Project template with builtin OTA,
            HTTP Server, WiFi connectivity and About page.
            Only manual OTA updates (/update) are supported.

            This is specifcally for the chinese wide/skinny
            oled display as seen in readme.md.
  
  File:      main.cpp

  Project:   <yourprojectname>
             
  Summary:   Project template that includes plubming and code for 
             a WiFi client + OTA updates via manual update.
             Automatic updates are not yet implemented but
             may be ported over from my legacy projects.

             Architecture: ESP32 specific.
            
  Config:    You must update secrets.h with your WiFi credentials
             and the hostname you choose for this device.
             SPIFFs for HTML requires index.html and update.html to be uploaded first.
             Place files in /data then VSCode > View > PlatformIO > UploadFilsystemImage
             Or via Arduino plugin:
             https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/

  Building:  pio run -t <target> -e envName

             Examples:
                pio run -t upload, monitor -e heltec_wifi_kit_32
                pio run -t upload, monitor -e fm-dev-kit
                pio run -t upload -e heltec_wifi_kit_32 --upload-port COM6
                pio run -t upload -e fm-dev-kit
                pio run -t upload -e fm-dev-kit --upload-port COM6

             List targets: pio run --list-targets

  Kary Wall 1/20/2022.
===================================================================+*/

#include <Arduino.h>
#include "SPIFFS.h"
#include <zUtils.h>
#include <localWiFi.h>
#include <Kanimations.h>
#include <localUpdateServer.h>
#include <FastLED.h>
#include <oled.h>

// LED Matric Config - 256x8 Matrix
const int DATA_PIN = 15;
const int PWR_PIN = 23; //  not always needed but is 3v3 volts.
const int NUM_LEDS = 256;
const int NUM_ROWS = 16;
const int NUM_COLS = 16;

// LED Matric Config- 8 LED Strip
// const int DATA_PIN = 5;
// const int PWR_PIN = 23; //  not always needed but is 3v3 volts.
// const int NUM_LEDS = 8;
// const int NUM_ROWS = 0;
// const int NUM_COLS = 32;

CRGB leds[NUM_LEDS];
int gLeds[NUM_LEDS];

// externs
extern Adafruit_SSD1306 display;
extern WebServer httpServer;
extern void startWifi();
extern void startUpdateServer();
extern bool isWiFiConnected();
extern String globalIP;
extern int g_lineHeight;
extern int requestValue;
extern uint8_t briteValue;
extern bool isUpdating;
extern String ssid;
extern String rssi;

// prototypes
String checkSPIFFS();
void printDisplayMessage(String msg);

// locals
const int activityLED = 12;
unsigned long lastUpdate = 0;

void setup()
{
    /*--------------------------------------------------------------------
     Boot, Oled and I/O initialization.
    ---------------------------------------------------------------------*/
    Serial.begin(115200);
    display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
    printDisplayMessage("Boot...");
    Serial.println();
    Serial.println("Booting...");
    zUtils::getChipInfo();
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, HIGH);
    pinMode(activityLED, OUTPUT);
    digitalWrite(activityLED, LOW);

    /*--------------------------------------------------------------------
     Start WiFi & OTA HTTP update server
    ---------------------------------------------------------------------*/
    printDisplayMessage("Wifi...");
    startWifi();
    printDisplayMessage("SPIFFS..");
    checkSPIFFS();
    printDisplayMessage("Server...");
    startUpdateServer();

    /*--------------------------------------------------------------------
     Project specific setup code
    ---------------------------------------------------------------------*/
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 5000);
    FastLED.setBrightness(255);
    FastLED.setCorrection(TypicalLEDStrip);
    pinMode(ANALONG_PIN, INPUT);
    randomSeed(analogRead(ANALONG_PIN));

    // Reamap pixels to matrix, call this now if using a matrix
    *gLeds = *getLtrTransform(gLeds, NUM_LEDS, NUM_ROWS, NUM_COLS);

}

void printDisplayMessage(String msg)
{
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println(msg);
    display.display();
}

void loop()
{

    /*--------------------------------------------------------------------
        Update oled every second with your text
    ---------------------------------------------------------------------*/
    if (millis() - lastUpdate > 1000)
    {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println(globalIP);
        display.setCursor(0, 9);
        display.println("Up: " + zUtils::getMidTime());
        display.setCursor(0, 17);
        display.println("Signal: " + String(WiFi.RSSI()) + " dBm");
        display.setCursor(0, 25);
        display.println(currentAnimation);
        display.display();
        lastUpdate = millis();
    }
    /*--------------------------------------------------------------------
     Project specific loop code
    ---------------------------------------------------------------------*/

    EVERY_N_MILLISECONDS(150)
    {
        if (briteValue > 0)
        {
            FastLED.setBrightness(briteValue);
            FastLED.show();
            briteValue = 0;
        }
    }

    EVERY_N_MILLISECONDS(10) // check requestValue from localWebServer and choose animation or brightness
    {

        switch (requestValue)
        {
        case 0:
            clearLeds();
            break;

        case 1:
            randomDots(leds, NUM_LEDS);
            break;

        case 2:
            randomDots2(leds, NUM_LEDS);
            break;

        case 3:
            randomNoise(leds, NUM_LEDS);
            break;

        case 4:
            randomBlueJumper(leds, NUM_LEDS);
            break;

        case 5:
            randomPurpleJumper(leds, NUM_LEDS);
            break;

        case 6:
            dotScrollRandomColor(leds, NUM_LEDS);
            break;

        case 7:
            flashColor(leds, NUM_LEDS, CRGB::OrangeRed);
            break;

        case 8:
            ltrDot(leds, gLeds, NUM_LEDS);
            break;
        }
    }

    /*--------------------------------------------------------------------
     Required for web server and OTA updates
    ---------------------------------------------------------------------*/
    httpServer.handleClient();
    delay(1);
}

/*--------------------------------------------------------------------
     Project specific utility code (otherwise use zUtils.h)
    ---------------------------------------------------------------------*/
String checkSPIFFS()
{
    // SPIFFs support
    if (!SPIFFS.begin(true))
    {
        return "An Error has occurred while mounting SPIFFS";
    }
    else
    {
        return "SPIFFS mounted OK.";
    }
}