/*+===================================================================
  File:      secrets.h

  Summary:   Device and WiFi specfic details go here such as 
             SSID, password and hostname. Also some are used
             for OTA updates and the about page.

  Kary Wall 1/20/2022.            
===================================================================+*/

#include <Arduino.h>

#ifndef STASSID
#define STASSID "YOURSSID"
#define STAPSK "YOURPWD"
#endif

String ssid = STASSID;                         // WiFi ssid
String password = STAPSK;                      // WiFi password
String hostName = "YOURHOSTNAME";              // hostname as seen on network
String softwareVersion = "2.3.22";             // used for about page
String deviceFamily = "ESP32-ELOTA-Matrix";    // used for about page
String description = "I'm a led matrix baby."; // used for about page
String globalIP = "";
