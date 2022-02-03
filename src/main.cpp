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
             Currently using Elegant OTA.

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
#include <asyncWebServer.h>
#include <FastLED.h>
#include <oled.h>

/*-------------------------------------------------------------------

Pre-deloyment configuration

1. Set NUM_LEDS, NUM_ROWS and NUM_COLS - ROWS=1 = single strip.
2. Set <title> in htmlStrings.h
3. Set power max in main.cpp below (must match PSU used!).
4. Set hostName in secrets.h
5. Set ssid and password in secrets.h
-------------------------------------------------------------------*/
const int DATA_PIN = 5;
const int NUM_LEDS = 265;
const int NUM_ROWS = 1;
const int NUM_COLS = 0;

CRGB leds[NUM_LEDS];
int gLeds[NUM_LEDS];

// externs
extern Adafruit_SSD1306 display;
extern void startWifi();
extern void startWebServer();
extern bool isWiFiConnected();
extern String globalIP;
extern int g_lineHeight;
extern int requestValue;
extern CHSV chsvValue;
extern uint8_t briteValue;
extern bool isUpdating;
extern String ssid;
extern String rssi;
extern int *getLtrTransform(int leds[], int ledNum, int rows, int cols);
extern Mode g_ledMode = Mode::Off;

// prototypes
String checkSPIFFS();
void printDisplayMessage(String msg);

// locals
const int activityLED = 12;
unsigned long lastUpdate = 0;
bool isSolidColor = false;
Mode previousMode = Mode::Off;
CHSV previousColor = CHSV(0, 0, 0);

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
    pinMode(activityLED, OUTPUT);
    digitalWrite(activityLED, LOW);

    /*--------------------------------------------------------------------
     Start WiFi & OTA HTTP update server
    ---------------------------------------------------------------------*/
    printDisplayMessage("Wifi...");
    startWifi();
    //printDisplayMessage("SPIFFS..");
    //checkSPIFFS();
    printDisplayMessage("Server...");
    //startUpdateServer();
    startWebServer();

    /*--------------------------------------------------------------------
     Project specific setup code
    ---------------------------------------------------------------------*/
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000);
    FastLED.setBrightness(180);
    FastLED.setCorrection(TypicalLEDStrip);
    pinMode(ANALONG_PIN, INPUT);
    randomSeed(analogRead(ANALONG_PIN));

    // Transposes pixels as needed. Set NUM_ROWS=1 for a single row strip.
    // When using an anmiation that cares about row order, pass gLeds[]
    // and leds[] to your animation, then use leds[gLeds[i]]
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
    EVERY_N_MILLISECONDS(250)
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
    EVERY_N_MILLISECONDS(10) // change mode based on user input
    {
        switch (g_ledMode)
        {
        case Mode::Animation:
            previousMode = Mode::Animation;
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
                dotScrollRandomColor(leds, gLeds, NUM_LEDS);
                break;

            case 7:
                flashColor(leds, NUM_LEDS, CRGB::OrangeRed);
                break;

            case 8:
                ltrDot(leds, gLeds, NUM_LEDS);
                break;
            }
            break;

        case Mode::SolidColor:
            previousMode = Mode::SolidColor;
            currentAnimation = "Solid Color";
            // if (chsvValue == previousColor)
            // {
            //     return;
            // }
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = chsvValue;
            }
            // FastLED.showColor(chsvValue);
          //  FastLED.show(chsvValue.v);
            FastLED.show();
            //previousColor = chsvValue;
            break;

        case Mode::Bright:
            FastLED.setBrightness(briteValue);
            FastLED.show();
            g_ledMode = previousMode;
            break;

        case Mode::Off:
            clearLeds();
            break;
        }
    }
    // FastLED.delay(10);
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