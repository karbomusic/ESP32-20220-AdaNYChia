/*+===================================================================
  File:      asyncWebServer.h

  Summary:   Provides an Async HTTP server and HTTPUploader for OTA
             updates, manual only via Elegant OTA:
             https://github.com/ayushsharma82/ElegantOTA

  Kary Wall 2/20/22.
===================================================================+*/

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <htmlStrings.h>

// externs
extern String ssid;               // WiFi ssid.
extern String hostName;           // hostname as seen on network.
extern String softwareVersion;    // used in about page and your custom needs.
extern String deviceFamily;       // used in about page and your custom needs.
extern String description;        // used in about page and your custom needs.
extern String globalIP;           // used in about page.
extern const String metaRedirect; // used for restart redirect.
extern const int activityLED;
extern Mode g_ledMode;

// Prototypes
void handleAbout(AsyncWebServerRequest *request);
void bangLED(int);
void handleRestart(AsyncWebServerRequest *request);
void listAllFiles();
String getControlPanelHTML();

// locals
String controlPanelHtml;
AsyncWebServer server(80);

// globals
const char *g_currentAnimation = "Ready...";

// incoming parameters
const char *ANIMATION_PARAM = "animation";
const char *BRITE_PARAM = "brite";
const char *HUE_PARAM = "hue";
const char *SAT_PARAM = "sat";
const char *BRI_PARAM = "bri";
const char *SWATCH_PARAM = "swat";

// Replaces placeholder with section in your web page
String processor(const String &var)
{
    // not working becasue not sure how it works yet
    if (var == "[TITLE]")
    {
        return hostName;
    }
    return String();
}

void startWebServer()
{
    Serial.println("mDNS responder started");
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  String animationVal;
                  String hueValue;
                  String satValue;
                  String briValue;
                  String swatValue;

                  if (request->hasParam(ANIMATION_PARAM)) // animation
                  {
                      g_ledMode = Mode::Animation;
                      animationVal = request->getParam(ANIMATION_PARAM)->value();
                      int intVal = atoi(animationVal.c_str());

                      switch (intVal)
                      {
                      case 0:
                          g_currentAnimation = "Lights Out";
                          g_animationValue = 0;
                          break;
                      case 1:
                          g_currentAnimation = "Color Streams";
                          g_animationValue = 1;
                          break;
                      case 2:
                          g_currentAnimation = "Random Dots";
                          g_animationValue = 2;
                          break;
                      case 3:
                          g_currentAnimation = "Analog Noise";
                          g_animationValue = 3;
                          break;
                      case 4:
                          g_currentAnimation = "Blue Jumper";
                          g_animationValue = 4;
                          break;
                      case 5:
                          g_currentAnimation = "Purple Jumper";
                          g_animationValue = 5;
                          break;
                      case 6:
                          g_currentAnimation = "Scroll Color";
                          g_animationValue = 6;
                          break;
                      case 7:
                          g_currentAnimation = "Flash Color";
                          g_animationValue = 7;
                          break;
                      case 8:
                          g_currentAnimation = "Left to Right";
                          g_animationValue = 8;
                          break;
                      case 9:
                          g_currentAnimation = "Campfire";
                          g_animationValue = 9;
                          break;
                      case 10:
                          g_currentAnimation = "Color Waves";
                          g_animationValue = 10;
                          break;
                      case 11:
                          g_currentAnimation = "Red Ocean";
                          g_animationValue = 11;
                          break;
                      case 12:
                          g_currentAnimation = "Inchworm";
                          g_animationValue = 12;
                          break;
                      case 13:
                          g_currentAnimation = "Twinkle Stars";
                          g_animationValue = 13;
                          break;
                       case -1:
                          g_currentAnimation = "Lights Out";
                          g_animationValue = -1;
                          break;
                      default:                          
                            g_currentAnimation = "Ready...";
                            g_animationValue = 0;
                          break;
                      }
                      Serial.println("Animation chosen: " + String(g_currentAnimation) + " (" + String(g_animationValue) + ")");
                      request->send(200, "text/plain", "Animation changed to: " + String(g_currentAnimation));
                  }
                  else if (request->hasParam(HUE_PARAM))
                  {
                      g_ledMode = Mode::SolidColor;
                      hueValue = request->getParam(HUE_PARAM)->value();
                      uint8_t intHueVal = hueValue.toInt();
                      g_chsvColor.h = intHueVal; 
                      request->send(200, "text/plain", "Hue: " + hueValue);
                  }
                  else if (request->hasParam(SAT_PARAM))
                  {
                      g_ledMode = Mode::SolidColor;
                      satValue = request->getParam(SAT_PARAM)->value();
                      uint8_t intSatVal = satValue.toInt();
                      g_chsvColor.s = intSatVal;
                      request->send(200, "text/plain", "Sat:" + satValue);
                  }
                  else if (request->hasParam(BRI_PARAM)) 
                  {
                      g_ledMode = Mode::Bright;
                      briValue = request->getParam(BRI_PARAM)->value();
                      int intVal = atoi(briValue.c_str());
                      if (intVal >= 24 && intVal <= 255) // limit minimum brightness to prevent sudden darkness
                      {
                          g_briteValue = uint8_t(intVal);
                      }
                      Serial.println(String(g_briteValue));
                      request->send(200, "text/plain", "Brightness: " + briValue);
                  }
                  else if (request->hasParam(SWATCH_PARAM)) // it's a swatch
                  {
                      g_ledMode = Mode::SolidColor;
                      swatValue = request->getParam(SWATCH_PARAM)->value();
                      String *hsvColors = zUtils::splitHSVParams(swatValue, ',', 3);
                      uint8_t hueValue = hsvColors[0].toInt();
                      uint8_t satValue = hsvColors[1].toInt();
                      uint8_t briValue = hsvColors[2].toInt();
                      g_chsvColor = CHSV(hueValue, satValue, briValue);
                      request->send(200, "text/plain", "Swatch: " + swatValue);
                  }
                  else
                  {
                    //   g_ledMode = Mode::Off;
                    //   g_animationValue = 0; 
                      g_currentAnimation = "Ready";
                    //request->send_P(200, "text/html", index_html, processor);
                    //request->send_P(200, "text/html", index_html);
                     index_html.replace("{TITLE}", g_pageTitle);
                     index_html.replace("{HEADING}", g_friendlyName);
                     request->send(200, "text/html", index_html) ;
                  } });

    server.on("/restart", HTTP_GET, [](AsyncWebServerRequest *request)
              { handleRestart(request); });

    server.on("/about", HTTP_GET, [](AsyncWebServerRequest *request)
              { handleAbout(request); });

    server.onNotFound([](AsyncWebServerRequest *request)
                      { request->send(404, "text/plain", "404 - Not found"); });

    AsyncElegantOTA.begin(&server); // Start ElegantOTA
    Serial.println("Update server started! Open your browser and go to http://" + globalIP + "/update");
    Serial.println("or http://" + hostName + "/update");

    server.begin(); // Start web server
    Serial.println("HTTP server started! Open your browser and go to http://" + globalIP);
    Serial.println("or http://" + hostName);
    // listAllFiles();
}

void handleRestart(AsyncWebServerRequest *request)
{
    Serial.println("Restarting in 5 seconds...");
    request->send(200, "text/html", metaRedirect);
    delay(5000);
    ESP.restart();
}

void bangLED(int state)
{
    digitalWrite(activityLED, state);
}

// return the about page as HTML.
void handleAbout(AsyncWebServerRequest *request)
{
    bangLED(HIGH);
    String aboutResponse = "<head><style type=\"text/css\">.button:active{background-color:#cccccc;color:#111111}></style><head>"
                           "<meta http-equiv=\"refresh\" content=\"10\"/>"
                           "<body style=\"background-color:#141d27;color:#dddddd;font-family:arial\"><b>[About ESP32]</b><br><br>"
                           "<b>Device Family:</b> " +
                           deviceFamily + "<br>"
                                          "<b>ESP Chip Model:</b> " +
                           String(ESP.getChipModel()) + "<br>"
                                                        "<b>CPU Frequency:</b> " +
                           String(ESP.getCpuFreqMHz()) + "<br>"
                                                         "<b>Free Heap Mem:</b> " +
                           String(ESP.getFreeHeap()) + "<br>"
                                                       "<b>Flash Mem Size:</b> " +
                           String(ESP.getFlashChipSize() / 1024 / 1024) + " MB<br>"
                                                                          "<b>Chip ID: </b>" +
                           String(zUtils::getChipID()) + "<br>"
                                                         "<b>Hostname:</b> " +
                           hostName + "<br>"
                                      "<b>IPAddress:</b> " +
                           globalIP + "<br>"
                                      "<b>MAC Address:</b> " +
                           String(WiFi.macAddress()) + "<br>"
                                                       "<b>SSID: </b> " +
                           ssid + "<br>"
                                  "<b>RSSI: </b> " +
                           String(WiFi.RSSI()) + " dB<br>"
                                                 "<b>Software Version:</b> " +
                           softwareVersion + "<br>"
                                             "<b>Description:</b> " +
                           description + "<br>"
                                         "<b>Uptime:</b> " +
                           zUtils::getMidTime() + "<br>"
                                                  "<b>Temperature:</b> " +
                           g_temperature + "<br>"
                                           "<b>Update:</b> http://" +
                           hostName + ".ra.local/update<br><br>"
                                      "<button class=\"button\" style=\"width:100px;height:30px;border:0;background-color:#3c5168;color:#dddddd\" onclick=\"window.location.href='/restart'\">Restart</button></body>"
                                      "&nbsp;&nbsp;<button class=\"button\" style=\"width:100px;height:30px;border:0;background-color:#3c5168;color:#dddddd\" onclick=\"window.location.href='/update'\">Update</button></body>";
    request->send(200, "text/html", aboutResponse);
    bangLED(LOW);
}

// not currently used.
String getControlPanelHTML()
{
    String cpHTML = "";

    File file = SPIFFS.open("/index.html");
    if (file && file.available() && file.size() > 0)
    {
        cpHTML = file.readString();
        Serial.println("Control panel HTML loaded!");
    }
    else
    {
        cpHTML = "Failed to open /index.html for reading. Index.html must be uploaded to SPIFFs partition before uploading this sketch.";
        Serial.println(cpHTML);
    }
    file.close();
    return cpHTML;
}

void listAllFiles()
{
    // List files in the root directory
    Serial.println("Listing files in the root directory");
    Serial.println("-------------------------");
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while (file)
    {
        Serial.print("FILE: ");
        Serial.println(file.name());
        file = root.openNextFile();
    }
    file.close();
    root.close();
    Serial.println("-------------------------");
}