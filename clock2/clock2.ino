#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoOTA.h>
#include <LEDMatrixDriver.hpp>

#include <time.h>
#include <sys/time.h> 
#include <coredecls.h>

#include "font.h"
#include "display.h"

extern "C" int clock_gettime(clockid_t unused, struct timespec *tp);

//const char* DAYS[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char* DAYS[] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};

bool transistion = false;

bool showCol = false;
unsigned long prevColTime = 0;
unsigned long prevTimeUpdate = 0;
struct tm previousTime;
void updateTime() {
  time_t now;
  struct tm timeinfo;
  time(&now);
  timeinfo = *localtime(&now);

  // Draw the new time
  if (previousTime.tm_min != timeinfo.tm_min) {
    Serial.println("Update the time");

    if (timeinfo.tm_hour/10 != 0) drawDigit(timeinfo.tm_hour/10,0,-1);
    else drawDigit(timeinfo.tm_hour/10,0,-1,true,true);
    
    drawDigit(timeinfo.tm_hour%10,7,-1);

    drawDigit(timeinfo.tm_min/10,17,-1);
    drawDigit(timeinfo.tm_min%10,24,-1);

    // Draw the new date
    const char* day = DAYS[timeinfo.tm_wday];

    drawChar(day[0]-'a'+10+26,0,10);
    drawChar(day[1]-'a'+10,4,10);
    drawChar(day[2]-'a'+10,8,10);

    drawChar(timeinfo.tm_mday/10,13,10);
    drawChar(timeinfo.tm_mday%10,17,10);
    drawChar(10+26+26,21,10);
    drawChar(timeinfo.tm_mon/10,24,10);
    drawChar(timeinfo.tm_mon%10,28,10);
    
    previousTime = timeinfo;
  }

  // Blick column
  unsigned long currTime = millis();
  if (currTime - prevColTime > 500) {
    showCol = !showCol;
    prevColTime = currTime;
  }
  drawDigit(10,12,-1,false,showCol);
}

void setup() {
  Serial.begin(115200);

  // Setup wifi
  WiFi.hostname("james_clock_controller");
  WiFiManager wifiManager;
  wifiManager.autoConnect();

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  setenv("TZ", "AEST-10AEDT,M10.1.0,M4.1.0/3", 0); // https://github.com/nayarsystems/posix_tz_db

  // JamesClockUpdatePassword
  ArduinoOTA.setPasswordHash("fa6cecc079e1bd670f416301907e1f0b");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  
	// Init the display
  displayInit();
  
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  delay(3000);
}

void loop() {
  ArduinoOTA.handle();
  
  updateTime();

  delay(50);
}
