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
#include "animations.h"

#include "karma_22.h"
#include "karma_10.h"

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

  display.clearData();

  display.setTextColor(display.color565(255, 255, 255));

  display.setFont(&Karma_Future22pt7b);
  drawCenteredString(
      showCol ? currentTime : currentTimeNoColumn,
      DISPLAY_WIDTH / 2,
      1);
  display.setFont(&Karma_Future10pt7b);
  drawCenteredString(currentDate, DISPLAY_WIDTH / 2, 44);

  // Update and render animations
  animationManager.update();

  display.flip();
}

String getAnimationName(AnimationType type) {
  switch(type) {
    case ANIM_PLASMA: return F("Plasma");
    case ANIM_PARTICLES: return F("Particles");
    case ANIM_FIRE: return F("Fire");
    case ANIM_SKYLINE: return F("Skyline");
    case ANIM_GALAXY: return F("Galaxy");
    case ANIM_LIGHTNING: return F("Lightning");
    case ANIM_PIPES: return F("Pipes");
    case ANIM_ORBITAL: return F("Orbital");
    case ANIM_STARS: return F("Stars");
    case ANIM_BEACH: return F("Beach");
    default: return F("Unknown");
  }
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
    json += "\"currentAnimation\":" + String((int)animationManager.getCurrentAnimation()) + ",";
    json += "\"inFade\":" + String(animationManager.isInFade() ? "true" : "false") + ",";
    json += "\"fadeProgress\":" + String((int)((animationManager.getFadeProgress() / 255.0f) * 100)) + ",";
    json += "\"autoMode\":" + String(animationManager.isAutoMode() ? "true" : "false");
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
      json += "\"name\":\"" + getAnimationName((AnimationType)i) + "\"";
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
        animationManager.setAnimation((AnimationType)animIndex);
        request->send(200, "application/json", "{\"success\":true,\"animation\":" + String(animIndex) + "}");
      } else {
        request->send(400, "application/json", "{\"success\":false,\"error\":\"Invalid animation index\"}");
      }
    } else {
      request->send(400, "application/json", "{\"success\":false,\"error\":\"Missing animation index\"}");
    }
  });

  // JSON API: Set mode
  server.on("/api/mode/auto", HTTP_POST, [](AsyncWebServerRequest* request) {
    animationManager.setAutoMode(true);
    request->send(200, "application/json", "{\"success\":true,\"mode\":\"auto\"}");
  });

  server.on("/api/mode/manual", HTTP_POST, [](AsyncWebServerRequest* request) {
    animationManager.setAutoMode(false);
    request->send(200, "application/json", "{\"success\":true,\"mode\":\"manual\"}");
  });

  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", "Restarting...");

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
  
  // Initialize animation manager
  animationManager.begin();

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