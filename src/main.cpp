/*+===================================================================
  Project:  ESP32 Project template with builtin OTA,
            HTTP Server, WiFi connectivity and About page.
            Only manual OTA updates (/update) are supported.

            There is a second build config that contains the oled
            display: heltec_wifi_kit_32
  
  File:      main.cpp

  Project:   <yourprojectname>
             
  Summary:   Project template that includes plubming and code for 
             a WiFi client + OTA updates via manual update.
             Automatic updates are not yet implemented but
             may be ported over from my legacy projects.

             Architecture: ESP32 specific.
            
  Config:    You must update secrets.h with your WiFi credentials
             and the hostname you choose for this device.
             SPIFFs for HTML requires update.html to be uploaded first.
             https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/
             or use the PIO tasks to upload to SPIFFs.

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
#include <localUpdateServer.h>


// OLED display
#if defined(chia_led)
#include <oled.h>
extern Adafruit_SSD1306 display;
#endif

// externs
extern WebServer httpServer;
extern void startWifi();
extern void startUpdateServer();
extern bool isWiFiConnected();
extern String globalIP;

// prototypes
String checkSPIFFS();

// locals
const int activityLED = 12;
unsigned long lastUpdate = 0;

void setup()
{
    /*--------------------------------------------------------------------
     Boot and I/O initialization.
    ---------------------------------------------------------------------*/
    Serial.begin(115200);
    Serial.println();
    Serial.println("Booting...");
    zUtils::getChipInfo();
    pinMode(activityLED, OUTPUT);
    digitalWrite(activityLED, LOW);

/*--------------------------------------------------------------------
     NoName OLED display initialization.
    ---------------------------------------------------------------------*/
#if defined(chia_led)
    display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("WiFi...");
    display.display();
#endif
    /*--------------------------------------------------------------------
     Start WiFi & OTA HTTP update server
    ---------------------------------------------------------------------*/
    startWifi();
    checkSPIFFS();
    startUpdateServer();

    /*--------------------------------------------------------------------
     Project specific setup code
    ---------------------------------------------------------------------*/
}

void loop()
{

    /*--------------------------------------------------------------------
        Update oled every second with your text
    ---------------------------------------------------------------------*/
#if defined(chia_led)
    if (millis() - lastUpdate > 1000)
    {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println(globalIP);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 17);
        display.println("Uptime: " + zUtils::getMidTime());
        display.display();
        lastUpdate = millis();
    }
#endif

    /*--------------------------------------------------------------------
     Project specific loop code
    ---------------------------------------------------------------------*/

    // Your code here.

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