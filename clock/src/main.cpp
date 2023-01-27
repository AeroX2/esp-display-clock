#include <Arduino.h>

// #include <WiFiManager.h>
// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>
// #include <AsyncElegantOTA.h>
// #include <time.h>

#include "font.h"
#include "display.h"

// AsyncWebServer server(80);

// //const char* DAYS[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
// const char* DAYS[] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};

// bool transistion = false;

// bool showCol = false;
// unsigned long prevColTime = 0;
// unsigned long prevTimeUpdate = 0;
// struct tm previousTime;
// void updateTime() {
//   struct tm timeinfo;
//   if (!getLocalTime(&timeinfo)) {
//     Serial.println("Failed to obtain time");
//     return;
//   }

//   // Draw the new time
//   if (previousTime.tm_min != timeinfo.tm_min) {
//     Serial.println("Update the time");

//     if (timeinfo.tm_hour/10 != 0) drawDigit(timeinfo.tm_hour/10,0,-1);
//     else drawDigit(timeinfo.tm_hour/10,0,-1,true,true);
    
//     drawDigit(timeinfo.tm_hour%10,7,-1);

//     drawDigit(timeinfo.tm_min/10,17,-1);
//     drawDigit(timeinfo.tm_min%10,24,-1);

//     // Draw the new date
//     const char* day = DAYS[timeinfo.tm_wday];

//     drawChar(day[0]-'a'+10+26,0,10);
//     drawChar(day[1]-'a'+10,4,10);
//     drawChar(day[2]-'a'+10,8,10);

//     drawChar(timeinfo.tm_mday/10,13,10);
//     drawChar(timeinfo.tm_mday%10,17,10);
//     drawChar(10+26+26,21,10);
//     drawChar(timeinfo.tm_mon/10,24,10);
//     drawChar(timeinfo.tm_mon%10,28,10);
    
//     previousTime = timeinfo;
//   }

//   // Blick column
//   unsigned long currTime = millis();
//   if (currTime - prevColTime > 500) {
//     showCol = !showCol;
//     prevColTime = currTime;
//   }
//   drawDigit(10,12,-1,false,showCol);
// }

boolean otaStarted = false;
void setup() {
  Serial.begin(115200);

  // // Setup wifi
  // WiFi.hostname("james_clock_controller");
  // WiFiManager wifiManager;
  // wifiManager.autoConnect();
  
  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send(200, "text/plain", "Hi! I am ESP32.");
  // });

  // // Bypass ElegantOTA so that it doesn't crash with the display timer.
  // server.on("/update/identity", HTTP_GET, [&](AsyncWebServerRequest *request) {
  //   #if defined(ESP8266)
  //     request->send(200, "application/json", "{\"id\": \"clock\", \"hardware\": \"ESP8266\"}");
  //   #elif defined(ESP32)
  //     request->send(200, "application/json", "{\"id\": \"clock\", \"hardware\": \"ESP32\"}");
  //   #endif
  // });
  // server.on("/update", HTTP_GET, [&](AsyncWebServerRequest *request){
  //   Serial.println("Cancel display update");
  //   displayUpdateEnable(false);

  //   AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", ELEGANT_HTML, ELEGANT_HTML_SIZE);
  //   response->addHeader("Content-Encoding", "gzip");
  //   request->send(response);
  // });
  
  // AsyncElegantOTA.begin(&server);
  // server.begin();
  // Serial.println("HTTP server started");
  
  // configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  // setenv("TZ", "AEST-10AEDT,M10.1.0,M4.1.0/3", 0); // https://github.com/nayarsystems/posix_tz_db
  
	// Init the display
  displayInit();

  Serial.println("Ready");
  // Serial.print("IP address: ");
  // Serial.println(WiFi.localIP());
}

void loop() {
  // updateTime();
  displayUpdate();
  delay(50);
}