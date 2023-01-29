#include <Arduino.h>
#include <WiFiManager.h>

// This has to go after WifiManager to prevent HTTP conflicts
#include <AsyncElegantOTA.h>
#include <time.h>

#include "display.h"
#include "font.h"

AsyncWebServer server(80);

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

void setup() {
  Serial.begin(115200);

  // Setup wifi
  WiFi.hostname("james_clock_controller");
  WiFiManager wifiManager;
  wifiManager.autoConnect();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! I am ESP32.");
  });

  // Bypass ElegantOTA so that it doesn't crash with the display timer.
  server.on("/update/identity", HTTP_GET, [&](AsyncWebServerRequest *request) {
    request->send(200, "application/json",
                  "{\"id\": \"clock\", \"hardware\": \"ESP32\"}");
  });
  server.on("/update", HTTP_GET, [&](AsyncWebServerRequest *request) {
    // Wifi is really unresponsive if the display is still spitting out signals.
    dmaOutput.stopDMAoutput();

    AsyncWebServerResponse *response = request->beginResponse_P(
        200, "text/html", ELEGANT_HTML, ELEGANT_HTML_SIZE);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  AsyncElegantOTA.begin(&server);
  server.begin();
  Serial.println("HTTP server started");

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  setenv("TZ", "AEST-10AEDT,M10.1.0,M4.1.0/3",
         0);  // https://github.com/nayarsystems/posix_tz_db

  // Init the display
  displayInit();

  Serial.println("Ready");
  // Serial.print("IP address: ");
  // Serial.println(WiFi.localIP());
}

int time_counter = 0;
void loop() {
  // if (displayEnabled) {
  // delay(5000);
  // // for (int y = 0; y < 16; y++) {
  // //   for (int x = 0; x < 32; x++) {
  // //     setPixel(x, y, 1);
  // //     delay(50);
  // //   }
  // // }
  // // display.fillScreen(display.color444(0, 0, 0));
  // // updateTime();
  // displayUpdate();
  // }

  for (int x = 0; x < 32; x++) {
    for (int y = 0; y < 16; y++) {
      int16_t v = 0;
      uint8_t wibble = sin8(time_counter);
      v += sin16(x * wibble * 6 + time_counter);
      v += cos16(y * (128 - wibble) + time_counter);
      v += sin16(y * x * cos8(-time_counter) / 8);

      CRGB currentColor = ColorFromPalette(RainbowColors_p, (v >> 8) + 127);
      display.drawPixelRGB888(x, y, currentColor.r, currentColor.g,
                              currentColor.b);
    }
  }

  display.setTextColor(0);
  display.setCursor(0,5);
  display.write("12:54 PM");

  time_counter++;
  delay(50);
}