/*+===================================================================
  File:      secrets.h

  Summary:   Device and WiFi specfic details go here such as
             SSID, password and hostname. Also some are used
             for OTA updates and the about page.

  Kary Wall 2/20/2022.
===================================================================+*/

#include <Arduino.h>

#ifndef STASSID
#define STASSID "IOTNET2G"
#define STAPSK "creekvalley124"
#endif

String ssid = STASSID;                         // WiFi ssid
String password = STAPSK;                      // WiFi password
