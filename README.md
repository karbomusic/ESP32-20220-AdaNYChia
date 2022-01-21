# ESP32-2022-Master
This is a master template for the ESP32 using this OLED:

https://www.amazon.com/gp/product/B08CDN5PSJ

![image info](./assets/oled.png)

===================

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
`