#include <Arduino.h>
#include <AsyncTCP.h>
#include <WiFi.h>
#include <LittleFS.h>

#include <WiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>

#include <ezTime.h>
#include <string.h>

#include <FastLED.h>
#include <FastTrig.h>

#include "display.h"
#include "animations_coordinator.h"

// #include "karma_22.h"
// #include "karma_10.h"
// #include "karma_22_inside.h"
// #include "karma_10_inside.h"

#include "courier_new_10.h"
#include "courier_new_20.h"
#include "courier_new_outside_10.h"
#include "courier_new_outside_20.h"

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

  // Blink column
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

void displayUpdate() {
  updateTime();

  // Update and render animations
  renderCurrentAnimation();

  display.setFont(&Courier_New_Outside_20pt7b);
  display.setTextColor(display.color565(255,255,255));
  drawCenteredString(
      showCol ? currentTime : currentTimeNoColumn,
      DISPLAY_WIDTH / 2 + 2,
      10 - 4);
  display.setFont(&Courier_New20pt7b);
  display.setTextColor(display.color565(0,0,0));
  drawCenteredString(
      showCol ? currentTime : currentTimeNoColumn,
      DISPLAY_WIDTH / 2,
      10);

  display.setFont(&Courier_New_Outside_10pt7b);
  display.setTextColor(display.color565(255,255,255));
  drawCenteredString(currentDate, DISPLAY_WIDTH / 2 + 2, 44 - 4);
  display.setFont(&Courier_New10pt7b);
  display.setTextColor(display.color565(0,0,0));
  drawCenteredString(currentDate, DISPLAY_WIDTH / 2, 44);

  display.flip();
}

void setupWebServer() {
  LittleFS.begin();

  // Serve main web interface
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(LittleFS, "/index.html");
  });

  // JSON API: Get current status
  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest* request) {
    String json = "{";
    json += "\"currentAnimation\":" + String((int)getCurrentAnimation()) + ",";
    json += "\"inFade\":" + String(isAnimationFading() ? "true" : "false") + ",";
    json += "\"fadeProgress\":50";  // Simplified for now
    json += "}";
    
    request->send(200, "application/json", json);
  });

  // API: Get available animations list
  server.on("/api/animations", HTTP_GET, [](AsyncWebServerRequest* request) {
    String json = "[";
    for (int i = 0; i < ANIM_COUNT; i++) {
      if (i > 0) json += ",";
      json += "{";
      json += "\"id\":" + String(i) + ",";
      json += "\"name\":\"" + String(getAnimationName((AnimationType)i)) + "\"";
      json += "}";
    }
    json += "]";
    
    request->send(200, "application/json", json);
  });

  // JSON API: Set animation
  server.on("/api/animation", HTTP_POST, [](AsyncWebServerRequest* request) {
    if (request->hasParam("index", true)) {
      String indexStr = request->getParam("index", true)->value();
      int animIndex = indexStr.toInt();
  
      if (animIndex >= 0 && animIndex < ANIM_COUNT) {
        setAnimation((AnimationType)animIndex);
        request->send(200, "application/json", "{\"success\":true,\"animation\":" + String(animIndex) + "}");
      } else {
        request->send(400, "application/json", "{\"success\":false,\"error\":\"Invalid animation index\"}");
      }
    } else {
      request->send(400, "application/json", "{\"success\":false,\"error\":\"Missing animation index\"}");
    }
  });

  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->redirect("/");

    ESP.restart();
  });

  server.on("/erase", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", "Erasing WiFi settings and restarting...");

    WiFiManager wifiManager;
    wifiManager.erase();

    ESP.restart();
  });

  // Bypass ElegantOTA so that it doesn't crash with the display timer.
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


  // Handle 404
  server.onNotFound([](AsyncWebServerRequest* request) {
    request->send(404, "text/plain", "Not Found");
  });
}

void setup() {
  Serial.begin(115200);

  // Setup wifi
  WiFi.hostname("james_clock_controller_v2");
  WiFiManager wifiManager;
  wifiManager.setShowStaticFields(true);
  wifiManager.setShowDnsFields(true);
  wifiManager.autoConnect();

  // Setup web server with all endpoints
  setupWebServer();

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
  
  // Initialize animations
  initAnimations();

  Serial.println("Display and animations initialized!");
  Serial.println("Animations will cycle every 3 hours with 2-second fade transitions");
  Serial.println("Visit the web interface to control animations manually");

  delay(3000);
}

unsigned long lastUpdate = 0;
const unsigned long FRAME_INTERVAL = 16;  // ~60fps
void loop() {
  if (millis() - lastUpdate >= FRAME_INTERVAL) {
    displayUpdate();
    lastUpdate = millis();
  }

  ElegantOTA.loop();
  // delay(1);
}