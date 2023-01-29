#include <Arduino.h>
#include <FastLED.h>
#include <WiFiManager.h>

// This has to go after WifiManager to prevent HTTP conflicts
#include <AsyncElegantOTA.h>
#include <time.h>

// Goes before display.h incase we want remote debugging there.
#include <RemoteDebug.h>
RemoteDebug Debug;

#include "display.h"

AsyncWebServer server(80);

char currentTime[10] = {0};
char currentTimeNoColumn[10] = {0};
char currentDate[10] = {0};
struct tm previousTime = {0};

bool showCol = false;
unsigned long prevColTime = 0;

void updateTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    // debugD("Failed to obtain time");
    return;
  }

  // Blick column
  unsigned long currTime = millis();
  if (currTime - prevColTime > 500) {
    showCol = !showCol;
    prevColTime = currTime;
  }

  // Draw the new time
  if (previousTime.tm_min != timeinfo.tm_min) {
    // debugD("Update the time");

    strftime(currentTime, sizeof(currentTime), "%k:%M", &timeinfo);
    if (currentTime[0] == ' ') strcpy(currentTime, &currentTime[1]);
    strftime(currentTimeNoColumn, sizeof(currentTimeNoColumn), "%k %M",
             &timeinfo);
    if (currentTimeNoColumn[0] == ' ')
      strcpy(currentTimeNoColumn, &currentTimeNoColumn[1]);
    strftime(currentDate, sizeof(currentDate), "%b%e", &timeinfo);

    previousTime = timeinfo;
  }
}

void drawCenteredString(const char *buf, int x, int y) {
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buf, x, y, &x1, &y1, &w,
                        &h);  // calc width of new string
  display.setCursor(x - w / 2 - 1, y);
  display.write(buf);
}

unsigned int waveTimeCounter = 0;
void displayUpdate() {
  updateTime();

  display.fillScreen(0);

  display.setTextColor(display.color565(0, 0, 8));
  drawCenteredString(showCol ? currentTime : currentTimeNoColumn, 32 / 2 - 1,
                     1);
  drawCenteredString(currentDate, 32 / 2 - 1, 9);

  for (int x = 0; x < 32; x++) {
    for (int y = 0; y < 16; y++) {
      if (display.getPixel(x, y) > 0) continue;

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

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  setenv("TZ", "AEST-10AEDT,M10.1.0,M4.1.0/3",
         1);  // https://github.com/nayarsystems/posix_tz_db
  tzset();

  // Init the display
  displayInit();

  Serial.println("Waiting for Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  Serial.println("Connected!");

  Debug.begin("james_clock_controller");
  debugD("Ready!");

  delay(3000);
}

int updateTimeCounter = 0;
void loop() {
  if (updateTimeCounter++ > 50) {
    updateTimeCounter = 0;
    displayUpdate();
  }

  Debug.handle();
  delay(1);
}