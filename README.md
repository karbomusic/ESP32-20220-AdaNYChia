# ESP32-2022-Master-AdaNYLEDs 
This is the NYLED ESP32 project adapted for this OLED: https://www.amazon.com/gp/product/B08CDN5PSJ

![image info](./assets/oled.png) 

Works with strips and matrixes.  

###OLED
 OLED_WIDTH 128
 OLED_HEIGHT 32
 OLED_ADDR 0x3C
 PINS: SCL=IO22, SCA=IO21 

###MATRIX
DATA_PIN = IO15
NUM_LEDS = 256
NUM_ROWS = 8;
NUM_COLS = 32;

Set rows=0 for LED Strips.

===========================================================

**Dependency Graph:**
    
    |-- <Adafruit SSD1306> 2.5.1
    |   |-- <Adafruit GFX Library> 1.10.12
    |   |   |-- <Adafruit BusIO> 1.10.1
    |   |   |   |-- <Wire> 1.0.1
    |   |   |   |-- <SPI> 1.0
    |   |   |-- <Wire> 1.0.1
    |   |   |-- <SPI> 1.0
    |   |-- <SPI> 1.0
    |   |-- <Wire> 1.0.1
    |-- <Adafruit GFX Library> 1.10.12
    |   |-- <Adafruit BusIO> 1.10.1
    |   |   |-- <Wire> 1.0.1
    |   |   |-- <SPI> 1.0
    |   |-- <Wire> 1.0.1
    |   |-- <SPI> 1.0
    |-- <SPIFFS> 1.0
    |   |-- <FS> 1.0
    |-- <FastLED> 3.5.0
    |   |-- <SPI> 1.0
    |-- <Update> 1.0
    |-- <WebServer> 1.0
    |   |-- <WiFi> 1.0
    |   |-- <FS> 1.0
    |-- <ESPmDNS> 1.0
    |   |-- <WiFi> 1.0
    |-- <WiFi> 1.0
    |-- <Adafruit BusIO> 1.10.1
    |   |-- <Wire> 1.0.1
    |   |-- <SPI> 1.0
    |-- <Wire> 1.0.1

 **Project**  
 ESP32 Project template with builtin OTA, HTTP Server, WiFi connectivity and About page. Only manual OTA updates (/update) are supported.

 This is specifcally for the wide/skinny oled display as seen in readme.md.

             
  **Summary**   
  Project template that includes plumbing and code for a WiFi client + OTA updates via manual update.Automatic updates are not yet implemented but may be ported over from legacy projects.

             Architecture: ESP32 specific.
            
  **Config**    
  You must update secrets.h with your WiFi credentials and the hostname you choose for this device.Currently using Elegant OTA.

             Pre-deloyment configuration checklist:
             
                1. Set NUM_LEDS, NUM_ROWS and NUM_COLS - ROWS=1 = single strip.
                2. Set <title> in htmlStrings.h
                3. Set power max in main.cpp below (must match PSU used!).
                4. Set hostName in secrets.h
                5. Set ssid and password in secrets.h

  **Building**  
  pio run -t \<target> -e envName

             Examples:
                pio run -t upload, monitor -e heltec_wifi_kit_32
                pio run -t upload, monitor -e fm-dev-kit
                pio run -t upload -e heltec_wifi_kit_32 --upload-port COM6
                pio run -t upload -e fm-dev-kit
                pio run -t upload -e fm-dev-kit --upload-port COM6

             List targets: pio run --list-targets