/*+===================================================================
  File:      localUpdateServer.h

  Summary:   Provides an HTTP server and HTTPUploader for OTA updates,
             both automatic and manual, and a detailed about page.

  Kary Wall 2022.
===================================================================+*/

#include <WebServer.h>
#include <Update.h>
#include <strings.h>

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

WebServer httpServer(80);

// Prototypes
void handleAbout();
void bangLED(int);
void handleRestart();
void listAllFiles();
String getUpdateHTML();
String getControlPanelHTML();

String updateHtml;
String controlPanelHtml;

int requestValue = 0; // to inform loop which request was made (needs event).
uint8_t briteValue = 255; // used to inform loop new brightness value.
const char *currentAnimation = "Server ready...";
bool isUpdating = false;

// Start the server
void startUpdateServer()
{
   // listAllFiles();
    updateHtml = getUpdateHTML();
    controlPanelHtml = getControlPanelHTML();
    Serial.println("mDNS responder started");

    // handlers for url paths
    httpServer.on("/", HTTP_GET, []()
                  {
                      httpServer.sendHeader("Connection", "close");
                      httpServer.send(200, "text/html", controlPanelHtml);
                      requestValue = 0; // lights out
                      currentAnimation = "Lights Out";
                  });
    httpServer.on("/1", HTTP_GET, []()
              {
                  httpServer.send(200, "text/plain", "RandomDots");
                  requestValue = 1;
                  currentAnimation = "RandomDots";
              });

    httpServer.on("/2", HTTP_GET, []()
              {
                  httpServer.send(200, "text/plain", "RandomDots2");
                  requestValue = 2;
                  currentAnimation = "RandomDots2";
              });

    httpServer.on("/3", HTTP_GET, []()
              {
                  httpServer.send(200, "text/plain", "AnalogNoise");
                  requestValue = 3;
                  currentAnimation = "AnalogNoise";
              });

    httpServer.on("/4", HTTP_GET, []()
              {
                  httpServer.send(200, "text/plain", "RandomBlueJumper");
                  requestValue = 4;
                  currentAnimation = "RandomBlueJumper";
              });

    httpServer.on("/5", HTTP_GET, []()
              {
                  httpServer.send(200, "text/plain", "RandomPurpleJumper");
                  requestValue = 5;
                  currentAnimation = "RandomRedJumper";
              });

    httpServer.on("/6", HTTP_GET, []()
              {
                  httpServer.send(200, "text/plain", "ScrollColor");
                  requestValue = 6;
                  currentAnimation = "ScrollColor";
              });

    httpServer.on("/7", HTTP_GET, []()
              {
                  httpServer.send(200, "text/plain", "FlashColor");
                  requestValue = 7;
                  currentAnimation = "FlashColor";
              });

    httpServer.on("/8", HTTP_GET, []()
              {
                  httpServer.send(200, "text/plain", "Left 2 Right");
                  requestValue = 8;
                  currentAnimation = "LeftToRight";
              });

// Brightness --------------------------------

    httpServer.on("/10", HTTP_GET, []()
              {
                  httpServer.send(200, "text/plain", "Brightness 20'ish");
                  briteValue = 10;
              });
    httpServer.on("/20", HTTP_GET, []()
              {
                  httpServer.send(200, "text/plain", "Brightness 50");
                  briteValue = 50;
              });
    httpServer.on("/40", HTTP_GET, []()
              {
                  httpServer.send(200, "text/plain", "Brightness 100");
                  briteValue = 100;
              });
    httpServer.on("/60", HTTP_GET, []()
              {
                  httpServer.send(200, "text/plain", "Brightness 150");
                  briteValue = 150;
              });
    httpServer.on("/80", HTTP_GET, []()
              {
                  httpServer.send(200, "text/plain", "Brightness 200");
                  briteValue = 200;
              });
    httpServer.on("/100", HTTP_GET, []()
              {
                  httpServer.send(200, "text/plain", "Brightness 255");
                  briteValue = 255;
              });
        httpServer.onNotFound( []()
              {
                  httpServer.sendHeader("Connection", "close");
                  httpServer.send(404, "text/plain", "404 - Not Found");
                  briteValue = 255;
              });
    httpServer.on("/about", HTTP_GET, handleAbout);
    httpServer.on("/restart", handleRestart);
    httpServer.on(
        "/update", HTTP_ANY, []()
        {
            bangLED(HIGH);
            if (httpServer.method() == HTTP_GET)
            {
                httpServer.sendHeader("Connection", "close");
                httpServer.send(200, "text/html", updateHtml);
            }
            else if (httpServer.method() == HTTP_POST)
            {
                isUpdating = true;
                httpServer.sendHeader("Connection", "close");
                httpServer.send(200, "text/plain", (Update.hasError()) ? "FAIL... Restarting in 5 seconds" : "OK... Restarting in 5 seconds");
                isUpdating = false;
                delay(5000);
                ESP.restart();
            }
            else
            {
                httpServer.sendHeader("Connection", "close");
                httpServer.send(200, "text/html", "Option not allowed.");
            }
            bangLED(LOW);
        },
        []()
        {
            bangLED(HIGH);
            HTTPUpload &upload = httpServer.upload();
            if (upload.status == UPLOAD_FILE_START)
            {
                Serial.printf("Update: %s\n", upload.filename.c_str());
                if (!Update.begin(UPDATE_SIZE_UNKNOWN))
                { //start with max available size
                    Update.printError(Serial);
                }
            }
            else if (upload.status == UPLOAD_FILE_WRITE)
            {
                /* flashing firmware to ESP*/
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
                {
                    Update.printError(Serial);
                }
            }
            else if (upload.status == UPLOAD_FILE_END)
            {
                if (Update.end(true))
                { //true to set the size to the current progress
                    Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
                }
                else
                {
                    Update.printError(Serial);
                }
            }
            bangLED(LOW);
        });
    httpServer.begin();
    Serial.println("HTTP server started! Open your browser and go to http://" + globalIP);
    Serial.println("or http://" + hostName);
}

// return the about page as HTML.
void handleAbout()
{
    bangLED(HIGH);
    String aboutResponse = "<head><style type=\"text/css\">.button:active{background-color:#cccccc;color:#111111}></style><head>"
    "<body style=\"background-color:#141d27;color:#dddddd;font-family:arial\"><b>[About ESP32]</b><br><br>"
    "<b>Device Family:</b> " + deviceFamily + "<br>"
    "<b>ESP Chip Model:</b> " + String(ESP.getChipModel()) + "<br>"
    "<b>CPU Frequency:</b> " + String(ESP.getCpuFreqMHz()) + "<br>"
    "<b>Free Heap Mem:</b> " + String(ESP.getFreeHeap()) + "<br>"
    "<b>Flash Mem Size:</b> " + String(ESP.getFlashChipSize() / 1024 / 1024) + " MB<br>"
    "<b>Hostname:</b> " + hostName + "<br>"
    "<b>IPAddress:</b> " + globalIP + "<br>"
    "<b>MAC Address:</b> " + String(WiFi.macAddress()) + "<br>"
    "<b>SSID: </b> " + ssid + "<br>"
    "<b>RSSI: </b> " + String(WiFi.RSSI()) + " dB<br>"
    "<b>Software Version:</b> " + softwareVersion + "<br>"
    "<b>Description:</b> " + description + "<br>"
    "<b>Uptime:</b> " + zUtils::getMidTime() + "<br>"
    "<b>Update:</b> http://" + hostName + ".ra.local/update<br><br>"
    "<button class=\"button\" style=\"width:100px;height:30px;border:0;background-color:#3c5168;color:#dddddd\" onclick=\"window.location.href='/restart'\">Restart</button></body>"
    "&nbsp;&nbsp;<button class=\"button\" style=\"width:100px;height:30px;border:0;background-color:#3c5168;color:#dddddd\" onclick=\"window.location.href='/update'\">Update</button></body>";
    httpServer.send(200, "text/html", aboutResponse);
    httpServer.sendHeader("Connection", "close");
    bangLED(LOW);
}

// update.html must have previously been uploaded to the SPIFFs partition via Arduino IDE
// before uploading this sketch. This is only needed once per device.
String getUpdateHTML()
{
    String updateHTML = "";
    File file = SPIFFS.open("/update.html");
    if (file && file.available() && file.size() > 0)
    {
        updateHTML = file.readString();
        Serial.println("updateHTML loaded!");
    }
    else
    {
        updateHTML = "Failed to open /update.html for reading. Update.html must be uploaded to SPIFFs partition before uploading this sketch.";
        Serial.println(updateHTML);
    }
    file.close();
    return updateHTML;
}

void handleRestart()
{
    Serial.println("Restarting in 5 seconds...");
    httpServer.send(200, "text/html", metaRedirect);
    delay(5000);
    ESP.restart();
}

void bangLED(int state)
{
    digitalWrite(activityLED, state);
}

String getControlPanelHTML()
{
    String cpHTML = "";\
    
    File file = SPIFFS.open("/index.html");
    if (file && file.available() && file.size() > 0)
    {
        cpHTML = file.readString();
         Serial.println("cpHTML loaded!");
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