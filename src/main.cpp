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

  Hardware:  If using bright knob, color switch button, temp sensor, set USE_HARDWARE_INPUT 1
             Oled is always expected to be present.

             OLED: SSD1306 128x32. SDA=21, SC=22
             Bright Knob: Use a 10k Potentiometer to control the brightness
                          of the LEDs with .1 uf cap on lugs 1(gnd) and 2 (data).

             Temp Sensor: MLX90615. SDA=21, SCL=21 (same buss as oled)
             Color Button:
                                  ____
                            3v3<-|    |->NC
                                 | () |
                       GND<-10k<-|____|->DATA

             Bright Knob: Pin 1=GND, Pin 2=DATA, Pin 3=VCC

                                    |
                                  (___)
                                  | | |
                                  G D 3
                                  N T V
                                  D A 3

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
            HEAT_SCL_PIN = 22       : I2C SCL pin for temperature sensor.
            HEAT_SDA_PIN = 21       : I2C SDA pin for temperature sensor.
            OLED SCL = 22           : ESP builtin SCA/SCL pins, don't assign in code!
            OLED SCA = 21           : ESP builtin SCA/SCL pins, don't assign in code!
            FAN_PIN = 33            : PWM controlled heat fan pin.

  Kary Wall 2/20/2022.
===================================================================+*/

#define FASTLED_INTERNAL // Quiets build noise
#include <globalConfig.h>
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
#define CALIBRATION_TEMP_MIN 0
#define CALIBRATION_TEMP_MAX 1.9 // linear calaibration attempt because this one is off by 3-5 degrees F

// Config is in appGlobals.h

// Heat management
const int FAN_PIN = 33;
const int FAN_CHANNEL = 0;
const int FAN_FREQ = 10000;
const int FAN_RES = 8;
const int FAN_SPEED = 220;
const float MAX_HEAT = 130.0;  // F
const float SAFE_HEAT = 110.0; // for hysterisis
const float FAN_HEAT = 100.0;  // F

// Template external and globals
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

// Project specific externs and globals
extern int *getLtrTransform(int leds[], int rows, int cols);
extern Mode g_ledMode = Mode::Off;
extern int g_animationValue;
extern uint8_t g_briteValue;
extern CHSV g_chsvColor;

// Prototypes
String checkSPIFFS();
void printDisplayMessage(String msg);
uint8_t getBrigtnessLimit();
void checkBriteKnob();
float celsiusToFahrenheit(float c);

// Locals
MLX90615 mlx90615;
bool displayInfoToggle = true;
int ledCoreTask = 0;
static int ledTaskCore = 0;
float ambTemp = 0.0;
float objTemp = 0.0;
bool heatWarning = false;
const int activityLED = 12;
unsigned long lastButtonUpdate = 0;
int lastKnobValue = 0;
float EMA_a = 0.8; // Smoothing
int EMA_S = 0;     // Smoothing
int colorSelectPressed = 0;
int currentButtonColor = 0;
Mode previousMode = Mode::Off;
CHSV previousColor = CHSV(0, 0, 0);
CHSV buttonColors[] = {CHSV(85, 76, 254), CHSV(0, 0, 255), CHSV(28, 182, 225), CHSV(28, 182, 255),
                       CHSV(164, 4, 255), CHSV(164, 4, 176), CHSV(85, 61, 254), CHSV(72, 61, 85), CHSV(0, 0, 0)};

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

    ledcSetup(FAN_CHANNEL, FAN_FREQ, FAN_RES);
    ledcAttachPin(FAN_PIN, FAN_CHANNEL);
    ledcWrite(FAN_CHANNEL, 0);
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
    FastLED.setCorrection(Halogen);
    pinMode(RND_PIN, INPUT);
    randomSeed(analogRead(RND_PIN));
    FastLED.clear();
    FastLED.show();

    // pot smoothing
    EMA_S = analogRead(BRITE_KNOB_PIN);

    // Transpose pixels if needed. Set NUM_ROWS=1 for a single row strip.
    // When using an anmiation that cares about row order, pass gLeds[]
    // and leds[] to your animation.
    *gLeds = *getLtrTransform(gLeds, NUM_ROWS, NUM_COLS);

    // init some fastled built-in palletes for various FX.
    gPal = HeatColors_p;
    currentPalette = RainbowColors_p;
    targetPalette = OceanColors_p;
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
            if (displayInfoToggle)
            {
                display.setCursor(0, 25);
                display.println("Playing: " + String(g_currentAnimation));
            }
            else
            {
                display.setCursor(0, 25);
                display.println("Temp:" + String(objTemp) + " | " + String(ambTemp) + " F");
            }
            display.display();
        }
        else
        {
            // If heatWarning is true, we've already set the display to HOT!
            return;
        }
    }

#ifdef USE_TEMPERATURE_SENSOR // Only toggle display if the heat sensor is being used.
    EVERY_N_SECONDS(5)
    {
        displayInfoToggle = !displayInfoToggle;
    }
#endif

    /*--------------------------------------------------------------------
     Project specific loop code
    ---------------------------------------------------------------------*/

    EVERY_N_MILLISECONDS(1)
    {
        // Use installed brite knob and color select button
        switch (g_ledMode) // switch  mode based on user input
        {
        case Mode::Animation:
            previousMode = Mode::Animation;
            switch (g_animationValue)
            {
            case -1:
                clearLeds();
                break;

            case 0:
                // do nothing page loaded
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

            case 10:
                beatWaver(leds);
                break;

            case 11:
                redOcean(leds);
                break;

            case 12:
                inchWorm(leds);
                break;

            case 13:
                starTwinkle(leds);
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
                g_briteValue = g_chsvColor.v;
                FastLED.setBrightness(g_chsvColor.v);
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
    colorSelectPressed = digitalRead(COLOR_SELECT_PIN);
    if (colorSelectPressed == 1 && (millis() - lastButtonUpdate > 300))
    {
        g_chsvColor = buttonColors[currentButtonColor];
        currentButtonColor += 1;
        if (currentButtonColor >= ARRAY_LENGTH(buttonColors))
        {
            currentButtonColor = 0;
        }
        g_ledMode = Mode::SolidColor;
        previousMode = Mode::SolidColor;
        g_briteValue = g_chsvColor.v;
        lastButtonUpdate = millis();
        FastLED.setBrightness(g_chsvColor.v);
        FastLED.show();
        return;
    }
    checkBriteKnob();
#endif

#if USE_TEMPERATURE_SENSOR

    EVERY_N_SECONDS(5)
    {
        // Check temperature
    retry:
        float ambTempF = celsiusToFahrenheit(mlx90615.get_ambient_temp() - CALIBRATION_TEMP_MAX);
        float objTempF = celsiusToFahrenheit(mlx90615.get_object_temp() - CALIBRATION_TEMP_MAX);
        objTemp = objTempF;
        ambTemp = ambTempF;

        // If warm enough, turn on fan
        if (ambTempF > FAN_HEAT || objTempF > FAN_HEAT)
        {
            ledcWrite(FAN_CHANNEL, FAN_SPEED);
        }
        else
        {
            ledcWrite(FAN_CHANNEL, 0);
        }

        // hysterisis for heat warning mode recovery
        if ((ambTempF > SAFE_HEAT || objTempF > SAFE_HEAT) && heatWarning)
        {
            heatWarning = true;
        }
        else
        {
            heatWarning = false;
        }

        // If too hot, knock down brightess, go into heat warning mode
        // in a 10 second loop until cool again.
        if (ambTempF > MAX_HEAT || objTempF > MAX_HEAT || heatWarning)
        {
            heatWarning = true;
            FastLED.setBrightness(15);
            g_briteValue = g_briteValue * 0.7;
            FastLED.show();
            ledcWrite(FAN_CHANNEL, FAN_SPEED); // should already be on by now.

            for (int i = 10; i > 0; i--)
            {
                // oled warning
                display.clearDisplay();
                display.setTextSize(2);
                display.setCursor(0, 0);
                display.println("HOT! " + String(i));
                display.setCursor(0, 16);
                display.println(String(objTempF) + " F");
                display.display();
                display.setTextSize(1);

                // console warning
                Serial.println("HOT!" + String(i));
                Serial.println(String(objTempF) + " F");
                delay(1000);
            }

            goto retry;
        }
        else
        {
            FastLED.setBrightness(g_briteValue);
            FastLED.show();
        }

        g_temperature = "Ambient: " + String(ambTempF) + " Object: " + String(objTempF);
    }
#endif

    delay(1); // tiny inhale
}

/*--------------------------------------------------------------------
     Project specific utility code (otherwise use zUtils.h)
---------------------------------------------------------------------*/

#if USE_HARDWARE_INPUT
// Note: The reason there are so many smoothing tricks...
// 3 analog reads averaged, cap on pot + EMS smoothing algo...
// is because we are reading @ 3V3 volts, not the usual 5V.
// Which puts the ADC closer to the noise floor.
// Which in turn causes more jitter in the readings.
void checkBriteKnob()
{
    // cheap jitter smooth #1
    int v1 = analogRead(BRITE_KNOB_PIN);
    int v2 = analogRead(BRITE_KNOB_PIN);
    int v3 = analogRead(BRITE_KNOB_PIN);
    int mVal = (v1 + v2 + v3) / 3;

    EMA_S = (EMA_a * mVal) + ((1 - EMA_a) * EMA_S); // jitter smooth #2
    uint8_t mappedVal = map(EMA_S, 0, 4095, 0, 255);
    if (mappedVal != lastKnobValue && abs(mappedVal - lastKnobValue) > 2)
    {
        g_briteValue = mappedVal;
        g_ledMode = Mode::Bright;
        lastKnobValue = mappedVal;
    }
}
#endif

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
}