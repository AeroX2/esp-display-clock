#include <Arduino.h>
#include <WiFiManager.h>

// This has to go after WifiManager to prevent HTTP conflicts
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>

#include <ezTime.h>
#include <string.h>
// #include <time.h>

#include "display.h"

AsyncWebServer server(80);
Timezone timezone;

char currentTime[10] = {0};
char currentTimeNoColumn[10] = {0};
char currentDate[10] = {0};

bool showCol = false;
unsigned long prevColTime = 0;

void drawCenteredString(const char* buf, int x, int y) {
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buf, x, y, &x1, &y1, &w, &h);  // calc width of new string
  display.setCursor(x - w / 2 - 1, y);
  display.write(buf);
}

void updateTime() {
  if (timeStatus() == timeNotSet) {
    Serial.println("Failed to obtain time");
    strcpy(currentTime, "Conn");
    strcpy(currentTimeNoColumn, "Conn");
    strcpy(currentDate, "Fail");
    return;
  }

  // Blick column
  unsigned long currTime = millis();
  if (currTime - prevColTime > 500) {
    showCol = !showCol;
    prevColTime = currTime;
  }

  // Draw the new time
  if (minuteChanged()) {
    strncpy(currentTime, timezone.dateTime("G:i").c_str(), sizeof(currentTime));
    strncpy(currentTimeNoColumn, timezone.dateTime("G i").c_str(), sizeof(currentTimeNoColumn));
    strncpy(currentDate, timezone.dateTime("Mj").c_str(), sizeof(currentDate));
    if (currentDate[4] == '\0') {
      currentDate[4] = currentDate[3];
      currentDate[3] = ' ';
    }
  }
}

unsigned int waveTimeCounter = 0;
void displayUpdate() {
  updateTime();

  display.clearData();
  display.setTextColor(display.color565(255, 255, 255));

  drawCenteredString(
      showCol ? currentTime : currentTimeNoColumn,
      32 / 2,
      1);
  drawCenteredString(currentDate, 32 / 2, 9);

  for (int x = 0; x < 32; x++) {
    for (int y = 0; y < 16; y++) {
      if (display.getPixel(x, y) > 0)
        continue;

      int16_t v = 0;
      uint8_t wibble = sin8(waveTimeCounter);
      v += sin16(x * wibble * 6 + waveTimeCounter);
      v += cos16(y * (128 - wibble) + waveTimeCounter);
      v += sin16(y * x * cos8(-waveTimeCounter) / 8);

      CRGB currentColor = ColorFromPalette(RainbowColors_p, (v >> 8) + 127);
      display.drawPixelRGB888(x, y, currentColor.r, currentColor.g,
                              currentColor.b);
    }
  }
  waveTimeCounter++;
}

void setup() {
  Serial.begin(115200);

  // Setup wifi
  WiFi.hostname("james_clock_controller");
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

  // configTime(0, 0, "time.cloudflare.com", "pool.ntp.org", "time.nist.gov");
  // setenv("TZ", "AEST-10AEDT,M10.1.0,M4.1.0/3",
  //        1);  // https://github.com/nayarsystems/posix_tz_db
  // tzset();

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
  if (updateTimeCounter++ > 50) {
    updateTimeCounter = 0;
    displayUpdate();
  }

  delay(1);
}