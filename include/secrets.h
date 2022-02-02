/*+===================================================================
  File:      secrets.h

  Summary:   Device and WiFi specfic details go here such as 
             SSID, password and hostname. Also some are used
             for OTA updates and the about page.

  Kary Wall 1/20/2022.            
===================================================================+*/

#include <Arduino.h>

#ifndef STASSID
#define STASSID "IOTNET2G"
#define STAPSK "creekvalley124"
#endif

String ssid = STASSID;                         // WiFi ssid
String password = STAPSK;                      // WiFi password
String hostName = "ledman";                // hostname as seen on network
String softwareVersion = "1.30.22";            // used for OTA updates & about page
String deviceFamily = "ESP32-ELOTA-Matrix";    // used for OTA updates & about page
String description = "I'm a led matrix baby."; // used for about page
String globalIP = "";
