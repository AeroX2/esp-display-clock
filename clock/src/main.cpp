#include <Arduino.h>
#include <WiFiManager.h>

// This has to go after WifiManager to prevent HTTP conflicts
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>

#include <ezTime.h>
#include <string.h>

#include <FastLED.h>
#include <FastTrig.h>

#include "display.h"

#include "karma_22.h"
#include "karma_10.h"

// #include "bulletin.h"
// #include "terminal.h"
// #include "upheavtt.h"

// #include "extrude.h"
// #include "pixel_game.h"
// #include "inflammable.h"
// #include "roboto.h"

AsyncWebServer server(80);
Timezone timezone;

char currentTime[10] = {0};
char currentTimeNoColumn[10] = {0};
char currentDate[15] = {0};

bool justBooted = true;
bool showCol = false;
unsigned long prevColTime = 0;

void drawCenteredString(const char* buf, int x, int y) {
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);  // calc width of new string
  display.setCursor(x - w / 2, y + h - 1);
  display.write(buf);
}

void updateTime() {
  timeStatus_t status = timeStatus();
  if (status == timeNotSet) {
    Serial.println("Failed to obtain time");
    strcpy(currentTime, "Conn");
    strcpy(currentTimeNoColumn, "Conn");
    strcpy(currentDate, "Fail");
    return;
  }

  if (status == timeNeedsSync) {
    Serial.println("Waiting to sync");
    strcpy(currentTime, "Gett");
    strcpy(currentTimeNoColumn, "ting");
    strcpy(currentDate, "Time");
    return;
  }

  // Blick column
  unsigned long currTime = millis();
  if (currTime - prevColTime > 500) {
    showCol = !showCol;
    prevColTime = currTime;
  }

  // Draw the new time
  if (minuteChanged() || justBooted) {
    justBooted = false;

    strncpy(currentTime, timezone.dateTime("G:i").c_str(), sizeof(currentTime));
    strncpy(currentTimeNoColumn, timezone.dateTime("G i").c_str(), sizeof(currentTimeNoColumn));
    strncpy(currentDate, timezone.dateTime("F j").c_str(), sizeof(currentDate));
  }
}

#define TO_SIN16(x) ((x) * 10430)  // Convert float to FastLED 16-bit angle

void displayUpdate() {
  updateTime();

  display.clearData();
  display.setTextColor(display.color565(10, 10, 10));

  display.setFont(&Karma_Future22pt7b);
  drawCenteredString(
      showCol ? currentTime : currentTimeNoColumn,
      DISPLAY_WIDTH / 2,
      1);
  display.setFont(&Karma_Future10pt7b);
  drawCenteredString(currentDate, DISPLAY_WIDTH / 2, 44);

  float t = millis() / 1000.;
  for (int x = 0; x < DISPLAY_WIDTH; x++) {
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
      if (display.getPixel(x, y) > 0)
        continue;

        uint8_t r,g,b;

        int16_t s1 = sin16(TO_SIN16(x / 18.f + t));
        int16_t s2 = sin16(TO_SIN16(y / 14.f));
        int16_t s3 = sin16(TO_SIN16(y / 18.f + t));
        int16_t s4 = sin16(TO_SIN16(x / 14.f - t));

        float v = ((s1 * s2 + s3 * s4) / 32768.0) * 255.0;
        r = constrain(int(v), 0, 255);

        t += 0.2;

        s1 = sin16(TO_SIN16(x / 18.f + t));
        s3 = sin16(TO_SIN16(y / 18.f + t));
        s4 = sin16(TO_SIN16(x / 14.f - t));

        v = ((s1 * s2 + s3 * s4) / 32768.0) * 255.0;
        g = constrain(int(v), 0, 255);

        t += 0.2;

        s1 = sin16(TO_SIN16(x / 18.f + t));
        s3 = sin16(TO_SIN16(y / 18.f + t));
        s4 = sin16(TO_SIN16(x / 14.f - t));

        v = ((s1 * s2 + s3 * s4) / 32768.0) * 255.0;
        b = constrain(int(v), 180, 255);

        t -= 0.4;

        display.drawPixelRGB888(x, y, r, g, b);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Setup wifi
  WiFi.hostname("james_clock_controller_v2");
  WiFiManager wifiManager;
  wifiManager.setShowStaticFields(true);
  wifiManager.setShowDnsFields(true);
  wifiManager.autoConnect();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", "Hi! I am ESP32 Clock.");
  });

  server.on("/erase", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", "Erasing...");

    WiFiManager wifiManager;
    wifiManager.erase();

    ESP.restart();
  });

  // Bypass ElegantOTA so that it doesn't crash with the display timer.
  server.on("/update/identity", HTTP_GET, [&](AsyncWebServerRequest* request) {
    request->send(200, "application/json",
                  "{\"id\": \"clock\", \"hardware\": \"ESP32\"}");
  });
  server.on("/update", HTTP_GET, [&](AsyncWebServerRequest* request) {
    // Wifi is really unresponsive if the display is still spitting out signals.
    dmaOutput.stopDMAoutput();

#if defined(ASYNCWEBSERVER_VERSION) && ASYNCWEBSERVER_VERSION_MAJOR > 2  // This means we are using recommended fork of AsyncWebServer
    AsyncWebServerResponse* response = request->beginResponse(200, "text/html", ELEGANT_HTML, sizeof(ELEGANT_HTML));
#else
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", ELEGANT_HTML, sizeof(ELEGANT_HTML));
#endif
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  ElegantOTA.begin(&server);
  server.begin();

  int counter = 0;
  Serial.println("Waiting for Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    if (counter++ > 100) {
      Serial.println("WiFi down, :(");
      break;
    }
    delay(100);
  }
  Serial.println("Connected!");

  waitForSync();
  timezone.setLocation("Australia/Sydney");

  // Init the display
  displayInit();

  Serial.println("Display initialised!");

  delay(3000);
}

int updateTimeCounter = 0;
void loop() {
  displayUpdate();

  ElegantOTA.loop();
  // delay(1);
}