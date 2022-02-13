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
                6. Enable USE_HARDWARE_INPUT if using an analog brightness knob (GPIO35).
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

  PINS
            USE_HARDWARE_INPUT 1|0  : Enable brignt knob, temp meter, buttons etc.
            RND_PIN 34              : Random number seed from analog pin.
            BRITE_KNOB_PIN = 35     : Brightness knob.
            DATA_PIN = 5            : Data pin for the LED strip.
            COLOR_SELECT_PIN = 16   : Color selection button.
            HEAT_SCL_PIN = 32       : I2C SCL pin for temperature sensor.
            HEAT_SDA_PIN = 33       : I2C SDA pin for temperature sensor.
            OLED SCL = 22           : OLED pins for the LED strip.
            OLED SCA = 21           : ESP builtin SCA/SCL pins, don't assign in code!

  Kary Wall 2/11/2022.
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
#include "MLX90615.h"

// Heat sensor
//#define SDA_PORT PORTC
//#define SDA_PIN_TEMP 33
//#define SCL_PORT PORTC
//#define SCL_PIN_TEMP 32
#define CALIBRATION_TEMP_MIN 0
#define CALIBRATION_TEMP_MAX 1.9 // linear calaibration attempt because this one is off by 3-5 degrees F
/*-------------------------------------------------------------------
Pre-deloyment configuration
1. Set DATA_PIN, NUM_ROWS and NUM_COLS - ROWS=1 = single strip.
2. Set <title> in htmlStrings.h and <h1> header on line 302
3. Set MAX_CURRENT in milliamps and NUM_VOLTS (must match PSU used!).
4. Set hostName in secrets.h
5. Set ssid and password in secrets.h
6. Enable USE_HARDWARE_INPUT if using an analog brightness knob (GPIO35).
7. NEW: Set NUM_LEDS in Kanimations.h
-------------------------------------------------------------------*/
#define ARRAY_LENGTH(array) (sizeof((array)) / sizeof((array)[0]))
#define USE_HARDWARE_INPUT 1 // Use installed brite knob

const int RND_PIN = 34;
const int COLOR_SELECT_PIN = 16;
const int BRITE_KNOB_PIN = 35;
const int DATA_PIN = 5;
const int TEMP_SCL_PIN = 22; // display and temp sensor
const int TEMP_SDA_PIN = 21; // display and temp sensor
const int NUM_ROWS = 1;
const int NUM_COLS = 0;
const int MAX_CURRENT = 500; // mA
const int NUM_VOLTS = 5;
const float MAX_HEAT = 110.0; // F

// template externs and globals
extern CRGB leds[];
extern int gLeds[];
extern Adafruit_SSD1306 display;
extern void startWifi();
extern void startWebServer();
extern bool isWiFiConnected();
extern String ssid;
extern String rssi;
extern String globalIP;
extern int g_lineHeight;

// project specific externs and globals
extern int *getLtrTransform(int leds[], int rows, int cols);
extern Mode g_ledMode = Mode::Off;
extern uint8_t g_animationValue;
extern uint8_t g_briteValue;
extern CHSV g_chsvColor;

// prototypes
String checkSPIFFS();
void printDisplayMessage(String msg);
uint8_t getBrigtnessLimit();
void checkBriteKnob();
float celsiusToFahrenheit(float c);

// locals
MLX90615 mlx90615;
int ledCoreTask = 0;
static int ledTaskCore = 0;
float temperature = 0.0;
bool heatWarning = false;
const int activityLED = 12;
unsigned long lastButtonUpdate = 0;
int lastKnobValue = 0;
int colorSelectPressed = 0;
int colorSelectState = 0;
int currentButtonColor = 0;
Mode previousMode = Mode::Off;
CHSV previousColor = CHSV(0, 0, 0);
CHSV buttonColors[] = {CHSV(0, 0, 225), CHSV(0, 0, 255), CHSV(28, 182, 225), CHSV(28, 182, 255),
                       CHSV(164, 4, 255), CHSV(164, 4, 176), CHSV(72, 61, 255), CHSV(72, 61, 85)};

void setup()
{
    /*--------------------------------------------------------------------
     Boot, OLED and I/O initialization.
    ---------------------------------------------------------------------*/
    Serial.begin(115200);
    Serial.println();
    Serial.println("Booting...");
    display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
    printDisplayMessage("Boot...");
    zUtils::getChipInfo();

    pinMode(activityLED, OUTPUT);
    digitalWrite(activityLED, LOW);
    pinMode(BRITE_KNOB_PIN, INPUT);
    pinMode(COLOR_SELECT_PIN, INPUT);
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
    mlx90615.begin(TEMP_SDA_PIN, TEMP_SCL_PIN);
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(NUM_VOLTS, MAX_CURRENT);
    FastLED.setBrightness(180);
    pinMode(RND_PIN, INPUT);
    randomSeed(analogRead(RND_PIN));

    // Transposes pixels as needed. Set NUM_ROWS=1 for a single row strip.
    // When using an anmiation that cares about row order, pass gLeds[]
    // and leds[] to your animation, then use leds[gLeds[i]]
    // you only have to do this once in setup
    *gLeds = *getLtrTransform(gLeds, NUM_ROWS, NUM_COLS);

    // init some fastled built-ins for various FX.
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
        if (!heatWarning)
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
            // display.setCursor(0, 25);
            // display.println(g_currentAnimation);
            display.setCursor(0, 25);
            display.println("Temp:" + String(temperature) + " F");
            display.display();
        }
        else
        {
            return;
        }
    }
    /*--------------------------------------------------------------------
     Project specific loop code
    ---------------------------------------------------------------------*/

    EVERY_N_MILLISECONDS(1)
    {

// Use installed brite knob and color select button
#if USE_HARDWARE_INPUT
        checkBriteKnob();
        colorSelectPressed = digitalRead(COLOR_SELECT_PIN);
        if (colorSelectPressed == 1 && (millis() - lastButtonUpdate > 300))
        {
            Serial.println("HIGH");
            g_chsvColor = buttonColors[currentButtonColor];
            currentButtonColor += 1;
            if (currentButtonColor >= ARRAY_LENGTH(buttonColors))
            {
                currentButtonColor = 0;
            }
            g_ledMode = Mode::SolidColor;
            lastButtonUpdate = millis();
        }
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
                randomDots(leds);
                break;

            case 2:
                randomDots2(leds);
                break;

            case 3:
                randomNoise(leds);
                break;

            case 4:
                randomBlueJumper(leds);
                break;

            case 5:
                randomPurpleJumper(leds);
                break;

            case 6:
                dotScrollRandomColor(leds, gLeds);
                break;

            case 7:
                flashColor(leds, CRGB::OrangeRed);
                break;

            case 8:
                ltrDot(leds, gLeds);
                break;

            case 9:
                Fire2012WithPalette(leds);
                break;
            }
            break;

        case Mode::SolidColor:
            if (previousColor != g_chsvColor)
            {
                previousMode = Mode::SolidColor;
                g_currentAnimation = "Solid Color";
                previousColor = g_chsvColor;
                for (int i = 0; i < NUM_LEDS; i++)
                {
                    leds[i] = g_chsvColor;
                }
                FastLED.show();
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

#if USE_HARDWARE_INPUT

    EVERY_N_SECONDS(5) // check temperature, throttle brightness if hot.
    {
        float ambTempF = celsiusToFahrenheit(mlx90615.get_ambient_temp() - CALIBRATION_TEMP_MAX);
        float objTempF = celsiusToFahrenheit(mlx90615.get_object_temp() - CALIBRATION_TEMP_MAX);
        temperature = objTempF;
        if (ambTempF > MAX_HEAT || objTempF > MAX_HEAT)
        {
            // dim leds
            FastLED.setBrightness(30);
            FastLED.show();

            // oled warning
            display.clearDisplay();
            display.setTextSize(2);
            display.setCursor(0, 0);
            display.println("HOT!");
            display.setCursor(0, 15);
            display.println(String(objTempF) + " F");
            display.display();
            display.setTextSize(1);

            // console warning
            Serial.println("HOT!");
            Serial.println(String(objTempF) + " F");
            heatWarning = true;
            delay(10000);
        }
        else
        {
            heatWarning = false;
            FastLED.setBrightness(g_briteValue);
            FastLED.show();
        }
        Serial.println("Ambient: " + String(ambTempF) + " Object: " + String(objTempF));
    }
#endif

    delay(1); // tiny inhale
}

/*--------------------------------------------------------------------
     Project specific utility code (otherwise use zUtils.h)
---------------------------------------------------------------------*/
#if USE_HARDWARE_INPUT
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

// FastLED.showColor which I really need doesn't trigger the current bright
// limter code. This is a workaround to calculate it for solid colors so
// we can limit max brightness in the SolidColor mode.

// NOTE 2/12/22: not currently used because FastLED.showColor() flickers terribly when changing brightness.
// All this did was save me from the for loop to address all LEDs anyway.
// But doing the loop myself seems just as fast and no flicker so bailing on this idea.
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

float celsiusToFahrenheit(float c)
{
    return (c * 9.0 / 5.0) + 32.0;
}

/*--------------------------------------------------------------------
     Multi-threading for LEDS
---------------------------------------------------------------------*/

void createLedTask(TaskFunction_t pFunction)
{
    xTaskCreatePinnedToCore(
        pFunction,    /* Function to implement the task */
        "coreTask",   /* Name of the task */
        10000,        /* Stack size in words */
        NULL,         /* Task input parameter */
        0,            /* Priority of the task */
        NULL,         /* Task handle. */
        ledTaskCore); /* Core where the task should run */

    Serial.println("Task created...");
}

void ledTask(void *pvParameters)
{
    while (1)
    {
        // Serial.println("Task created...");
    }
}