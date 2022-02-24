#include <Arduino.h>

/*-------------------------------------------------------------------
              [Pre-deloyment configuration & globals]

1. Set DATA_PIN, NUM_ROWS and NUM_COLS -> Set ROWS=1 for single strip.
2. Set MAX_CURRENT in milliamps and NUM_VOLTS (must match PSU used!).
3. Set hostName and friendly name. 
4. Set ssid and password in secrets.h.
5. Enable USE_HARDWARE_INPUT if using an analog brightness knob (GPIO35).
6. Set NUM_LEDS to the number of LEDs in the strip/matrix.
-------------------------------------------------------------------*/
#define USE_HARDWARE_INPUT 1 // Use installed hardware (knob, temp, buttons etc.
#define USE_TEMPERATURE_SENSOR 0 // Use temperature sensor
const int RND_PIN = 34;
const int COLOR_SELECT_PIN = 16;
const int BRITE_KNOB_PIN = 35;
const int DATA_PIN = 5;
const int TEMP_SCL_PIN = 22; // display and temperature sensors.
const int TEMP_SDA_PIN = 21; // display and temperature sensors.
const int NUM_ROWS = 1;
const int NUM_COLS = 0;
const int MAX_CURRENT = 3000; // mA
const int NUM_VOLTS = 5;

// was in kanimations.h
#define NUM_LEDS 24

// was in secrets.h
String hostName = "ledman-circle";            // hostname as seen on network and home page
String friendlyName = "Power Testing";        // friendly name for home page
String softwareVersion = "2.20.22";            // used for about page
String deviceFamily = "ESP32-ELOTA-Matrix";    // used for about page
String description = "LEDs baby!"; // used for about page
String globalIP = "";

/*-------------------------------------------------------------------
              Other App Globals
-------------------------------------------------------------------*/
#define ARRAY_LENGTH(array) (sizeof((array)) / sizeof((array)[0]))
String g_temperature="";
String g_pageTitle = hostName + " | " + description; // home page title
String g_friendlyName = friendlyName + " Lights";