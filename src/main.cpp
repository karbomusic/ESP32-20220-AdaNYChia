/*+===================================================================
  Project:  ESP32 Project template with builtin OTA,
            HTTP Server, WiFi connectivity and About page.
            Only manual OTA updates (/update) are supported.

            This is specifcally for the wide/skinny
            oled display as seen in readme.md.

  File:      main.cpp

  Project:   <yourprojectname>

  Summary:   Project template that includes plumbing and code for
             a WiFi client + OTA updates via manual update.
             Automatic updates are not yet implemented but
             may be ported over from legacy projects.

             Architecture: ESP32 specific.

  Config:    You must update secrets.h with your WiFi credentials
             and the hostname you choose for this device.
             Currently using Elegant OTA.

             Pre-deloyment configuration checklist:

                1. Set DATA_PIN, NUM_ROWS and NUM_COLS - ROWS=1 = single strip.
                2. Set <title> in htmlStrings.h and <h1> header on line 302
                3. Set MAX_CURRENT in milliamps and NUM_VOLTS (must match PSU used!).
                4. Set hostName in secrets.h
                5. Set ssid and password in secrets.h
                6. Enable USE_BRITE_KNOB if using an analog brightness knob (GPIO35).
                7. NEW: Set NUM_LEDS in Kanimations.h

  Brite Knob: You use a 10k Potentiometer to control the brightness
              of the LEDs. To eanble set #define BRITE_KNOB = 1

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

#define FASTLED_INTERNAL // Quiets build noise
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
1. Set DATA_PIN, NUM_ROWS and NUM_COLS - ROWS=1 = single strip.
2. Set <title> in htmlStrings.h and <h1> header on line 302
3. Set MAX_CURRENT in milliamps and NUM_VOLTS (must match PSU used!).
4. Set hostName in secrets.h
5. Set ssid and password in secrets.h
6. Enable USE_BRITE_KNOB if using an analog brightness knob (GPIO35).
7. NEW: Set NUM_LEDS in Kanimations.h
-------------------------------------------------------------------*/
#define USE_BRITE_KNOB 0 // Use installed brite knob
#define RND_PIN 34
const int BRITE_KNOB_PIN = 35;
const int DATA_PIN = 5;
const int NUM_ROWS = 1;
const int NUM_COLS = 0;
const int MAX_CURRENT = 6000; // mA
const int NUM_VOLTS = 5;

// #define USE_BRITE_KNOB 1 // Use installed brite knob
// #define RND_PIN 34
// const int BRITE_KNOB_PIN = 35;
// const int DATA_PIN = 15;
// const int NUM_ROWS = 8;
// const int NUM_COLS = 32;
// const int MAX_CURRENT = 4000; // mA
// const int NUM_VOLTS = 5;

CRGB leds[NUM_LEDS];
int gLeds[NUM_LEDS];

// template externs and globals
extern Adafruit_SSD1306 display;
extern void startWifi();
extern void startWebServer();
extern bool isWiFiConnected();
extern String ssid;
extern String rssi;
extern String globalIP;
extern int g_lineHeight;

// project specific externs and globals
extern int *getLtrTransform(int leds[], int ledNum, int rows, int cols);
extern Mode g_ledMode = Mode::Off;
extern uint8_t g_animationValue;
extern uint8_t g_briteValue;
extern CHSV g_chsvColor;

// prototypes
String checkSPIFFS();
void printDisplayMessage(String msg);
uint8_t getBrigtnessLimit();
void checkBriteKnob();

// locals
const int activityLED = 12;
unsigned long lastUpdate = 0;
bool isSolidColor = false;
Mode previousMode = Mode::Off;
CHSV previousColor = CHSV(0, 0, 0);
int lastKnobValue = 0;

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
    pinMode(BRITE_KNOB_PIN, INPUT);

    /*--------------------------------------------------------------------
     Start WiFi & OTA HTTP update server
    ---------------------------------------------------------------------*/
    printDisplayMessage("Wifi...");
    startWifi();
    printDisplayMessage("Server...");
    startWebServer();

    /*--------------------------------------------------------------------
     Project specific setup code
    ---------------------------------------------------------------------*/
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(NUM_VOLTS, MAX_CURRENT);
    FastLED.setBrightness(180);
    pinMode(RND_PIN, INPUT);
    randomSeed(analogRead(RND_PIN));

    // Transposes pixels as needed. Set NUM_ROWS=1 for a single row strip.
    // When using an anmiation that cares about row order, pass gLeds[]
    // and leds[] to your animation, then use leds[gLeds[i]]
    *gLeds = *getLtrTransform(gLeds, NUM_LEDS, NUM_ROWS, NUM_COLS);

    // some fastl led built-ins for various FX.
    gPal = HeatColors_p;
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
    random16_add_entropy(random(255));
    /*--------------------------------------------------------------------
        Update oled every interval with your text
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
        display.println(g_currentAnimation);
        display.display();
        lastUpdate = millis();
    }
    /*--------------------------------------------------------------------
     Project specific loop code
    ---------------------------------------------------------------------*/

    EVERY_N_MILLISECONDS(1)
    {

#if USE_BRITE_KNOB
        checkBriteKnob();
#endif

        switch (g_ledMode) // switch  mode based on user input
        {
        case Mode::Animation:
            previousMode = Mode::Animation;
            switch (g_animationValue)
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

            case 9:
                Fire2012WithPalette(leds); // if thsi works change all to NUM_LEDS
                break;
            }
            break;

        case Mode::SolidColor:
            if (previousColor != g_chsvColor)
            {
                previousMode = Mode::SolidColor;
                g_currentAnimation = "Solid Color";
                previousColor = g_chsvColor;
                g_chsvColor.v = getBrigtnessLimit();
                FastLED.showColor(g_chsvColor);
            }
            break;

        case Mode::Bright:
            FastLED.setBrightness(g_briteValue);
            FastLED.show();
            g_chsvColor.v = g_briteValue;
            g_ledMode = previousMode;
            break;

        case Mode::Off:
            clearLeds();
            break;
        }
    }
    delay(1); // inhale
}

/*--------------------------------------------------------------------
     Project specific utility code (otherwise use zUtils.h)
     ** not currently used **
---------------------------------------------------------------------*/
#if USE_BRITE_KNOB
void checkBriteKnob()
{
    int mVal = map(analogRead(BRITE_KNOB_PIN), 0, 4095, 0, 255);
    if (mVal != lastKnobValue)
    {
        if (abs(mVal - lastKnobValue) > 2) // faked smoothing (includes .1 UF cap on knob)
        {
            g_ledMode = Mode::Bright;
            g_briteValue = mVal;
            lastKnobValue = mVal;
        }
    }
}
#endif

// FastLED.showColor which I really need doesn't trigger the current limter code,
// this is a workaround to calculate it for solid colors.
uint8_t getBrigtnessLimit()
{
    return calculate_max_brightness_for_power_mW(leds, NUM_LEDS,
                                                 g_chsvColor.v, calculate_unscaled_power_mW(leds, NUM_LEDS));
}

String checkSPIFFS()
{
    // Mount SPIFFS if we are using it
    if (!SPIFFS.begin(true))
    {
        return "An Error has occurred while mounting SPIFFS";
    }
    else
    {
        return "SPIFFS mounted OK.";
    }
}