/*+===================================================================
  File:      secrets.h

  Summary:   Wifi SSID and Credentials.

  Kary Wall 2/26/2022.
===================================================================+*/

#include <Arduino.h>

#ifndef STASSID
#define STASSID ""
#define STAPSK ""
#endif

String ssid = STASSID;                         // WiFi ssid
String password = STAPSK;                      // WiFi password
