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
