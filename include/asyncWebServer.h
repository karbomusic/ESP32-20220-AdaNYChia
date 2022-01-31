/*+===================================================================
  File:      asyncWebServer.h

  Summary:   Provides an Async HTTP server and HTTPUploader for OTA 
             updates, manual only via Elegant OTA:
             https://github.com/ayushsharma82/ElegantOTA

  Kary Wall 1/30/22.
===================================================================+*/

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <htmlStrings.h>

// externs
extern String ssid;               // WiFi ssid.
extern String password;           // WiFi password.
extern String hostName;           // hostname as seen on network.
extern String softwareVersion;    // used for auto OTA updates & about page.
extern String deviceFamily;       // used for auto OTA updates & about page.
extern String description;        // used for about page.
extern String globalIP;           // needed for about page.
extern const String metaRedirect; // needed for restart redirect.
extern const int activityLED;

AsyncWebServer server(80);

// Prototypes
void handleAbout(AsyncWebServerRequest *request);
void bangLED(int);
void handleRestart(AsyncWebServerRequest *request);
void listAllFiles();
String getControlPanelHTML();

String controlPanelHtml;

int requestValue = 0;     // to inform loop which request was made (needs event).
uint8_t briteValue = 255; // used to inform loop new brightness value.
const char *currentAnimation = "Server ready...";
bool isUpdating = false;

// incoming parameters
const char *ANIMATION_PARAM = "animation";
const char *BRITE_PARAM = "brite";


// Replaces placeholder with section in your web page
String processor(const String &var)
{
    if (var == "[TITLE]")
    {
        return hostName;
    }
    return String();
}
// ----------------------------------------------------------------------------

void startWebServer()
{
    // controlPanelHtml = getControlPanelHTML();
    Serial.println("mDNS responder started");

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { 
                  String paramValue;

                  // Check incoming parameters
                  if (request->hasParam(ANIMATION_PARAM)) // animation
                  {
                      paramValue = request->getParam(ANIMATION_PARAM)->value();
                      uint8_t intVal = atoi(paramValue.c_str());

                      switch (intVal)
                      {
                      case 0:
                          currentAnimation = "Lights Out";
                          requestValue = 0;
                          break;
                      case 1:
                          currentAnimation = "Random Dots";
                          requestValue = 1;
                          break;
                      case 2:
                          currentAnimation = "Random Dots 2";
                          requestValue = 2;
                          break;
                      case 3:
                          currentAnimation = "Analog Noise";
                          requestValue = 3;
                          break;
                      case 4:
                          currentAnimation = "Blue Jumper";
                          requestValue = 4;
                          break;
                      case 5:
                          currentAnimation = "Purple Jumper";
                          requestValue = 5;
                          break;
                      case 6:
                          currentAnimation = "Scroll Color";
                          requestValue = 6;
                          break;
                      case 7:
                          currentAnimation = "Flash Color";
                          requestValue = 7;
                          break;
                      case 8:
                          currentAnimation = "Left to Right";
                          requestValue = 8;
                          break;
                      default:
                          currentAnimation = "Lights Out";
                          requestValue = 0;
                          break;
                      }
                        Serial.println("Animation chosen: " + String(currentAnimation) + " (" + String(requestValue) + ")");
                        request->send(200, "text/plain", "Animation changed to: " + String(currentAnimation));
                  }
                  else if (request->hasParam(BRITE_PARAM)) // brightness
                  {
                      paramValue = request->getParam(BRITE_PARAM)->value();
                      uint8_t intVal = atoi(paramValue.c_str());
                      if (intVal > 0 && intVal <= 255)
                      {
                          briteValue = intVal;
                      }
                      request->send(200, "text/plain", "Brightness: " + paramValue);
                  }
                  else
                  {
                      requestValue = 0; // lights out
                      currentAnimation = "Lights Out";
                      //request->send_P(200, "text/html", index_html, processor);
                      request->send_P(200, "text/html", index_html);
                     // request->send(200, "text/html", controlPanelHtml) ;
                  }
              });

    server.on("/restart", HTTP_GET, [](AsyncWebServerRequest *request)
              { handleRestart(request); });

    server.on("/about", HTTP_GET, [](AsyncWebServerRequest *request)
              { handleAbout(request); });

    server.onNotFound([](AsyncWebServerRequest *request)
                      { request->send(404, "text/plain", "404 - Not found"); });
               
    AsyncElegantOTA.begin(&server); // Start ElegantOTA
    Serial.println("Update server started! Open your browser and go to http://" + globalIP + "/update");
    Serial.println("or http://" + hostName + "/update");

    server.begin();                 // Start web server
    Serial.println("HTTP server started! Open your browser and go to http://" + globalIP);
    Serial.println("or http://" + hostName);
    //listAllFiles();
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
                                                  "<b>Update:</b> http://" +
                           hostName + ".ra.local/update<br><br>"
                                      "<button class=\"button\" style=\"width:100px;height:30px;border:0;background-color:#3c5168;color:#dddddd\" onclick=\"window.location.href='/restart'\">Restart</button></body>"
                                      "&nbsp;&nbsp;<button class=\"button\" style=\"width:100px;height:30px;border:0;background-color:#3c5168;color:#dddddd\" onclick=\"window.location.href='/update'\">Update</button></body>";
    request->send(200, "text/html", aboutResponse);
    bangLED(LOW);
}

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