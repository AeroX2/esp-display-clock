#include <Arduino.h>
#include <AsyncTCP.h>
#include <WiFi.h>

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
}

String getAnimationName(AnimationType type) {
  switch(type) {
    case ANIM_PLASMA: return "Plasma";
    case ANIM_PARTICLES: return "Particles";
    case ANIM_FIRE: return "Fire";
    case ANIM_SKYLINE: return "Skyline";
    case ANIM_GALAXY: return "Galaxy";
    case ANIM_LIGHTNING: return "Lightning";
    case ANIM_PIPES: return "Pipes";
    case ANIM_ORBITAL: return "Orbital";
    case ANIM_STARS: return "Stars";
    case ANIM_BEACH: return "Beach";
    default: return "Unknown";
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
    String html = F("<!DOCTYPE html><html><head>");
    html += F("<title>ESP32 Clock Controller</title>");
    html += F("<meta name='viewport' content='width=device-width, initial-scale=1'>");
    html += F("<style>");
    html += F("body { font-family: Arial, sans-serif; max-width: 600px; margin: 0 auto; padding: 20px; background: #1a1a1a; color: white; }");
    html += F("h1 { color: #4a90e2; text-align: center; }");
    html += F(".status { background: #2a2a2a; padding: 15px; border-radius: 8px; margin: 15px 0; }");
    html += F(".controls { background: #2a2a2a; padding: 15px; border-radius: 8px; margin: 15px 0; }");
    html += F(".animation-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); gap: 10px; margin: 15px 0; }");
    html += F(".anim-btn { padding: 12px; background: #3a3a3a; color: white; border: 1px solid #555; border-radius: 5px; cursor: pointer; text-decoration: none; text-align: center; transition: all 0.2s; }");
    html += F(".anim-btn:hover { background: #4a4a4a; }");
    html += F(".anim-btn.active { background: #4a90e2; border-color: #6bb6ff; }");
    html += F(".toggle-btn { padding: 10px 20px; background: #4a90e2; color: white; border: none; border-radius: 5px; cursor: pointer; margin: 5px; }");
    html += F(".toggle-btn:hover { background: #6bb6ff; }");
    html += F(".auto-mode { background: #2a5a2a; }");
    html += F(".manual-mode { background: #5a2a2a; }");
    html += F("a { color: #4a90e2; text-decoration: none; }");
    html += F("a:hover { text-decoration: underline; }");
    html += F("</style></head><body>");
    
    html += F("<h1>🎨 ESP32 Clock Controller</h1>");
    
    html += F("<div class='status'>");
    html += F("<h3>📊 Status</h3>");
    html += ("<p><strong>Current Animation:</strong> ") + getAnimationName(animationManager.getCurrentAnimation()) + ("</p>");
    
    if (animationManager.isInFade()) {
      html += F("<p><strong>Status:</strong> Transitioning (");
      html += String((int)((animationManager.getFadeProgress() / 255.0f) * 100));
      html += F("%)</p>");
    } else {
      html += F("<p><strong>Status:</strong> Running</p>");
    }
    
    html += F("<p><strong>Mode:</strong> ");
    html += animationManager.isAutoMode() ? F("Auto-Cycling") : F("Manual Control");
    html += F("</p>");
    html += F("</div>");
    
    html += F("<div class='controls'>");
    html += F("<h3>🎛️ Controls</h3>");
    
    html += F("<p>");
    html += F("<button class='toggle-btn auto-mode' onclick=\"fetch('/mode/auto')\">🔄 Auto Mode</button>");
    html += F("<button class='toggle-btn manual-mode' onclick=\"fetch('/mode/manual')\">🎯 Manual Mode</button>");
    html += F("</p>");
    
    html += F("<h4>Select Animation:</h4>");
    html += F("<div class='animation-grid'>");
    
    for (int i = 0; i < ANIM_COUNT; i++) {
      AnimationType anim = (AnimationType)i;
      String animName = getAnimationName(anim);
      String activeClass = (anim == animationManager.getCurrentAnimation()) ? F(" active") : F("");
      
      html += ("<a href='/animation/") + String(i) + ("' class='anim-btn") + activeClass + ("'>");
      
      switch(anim) {
        case ANIM_PLASMA: html += F("🌈 "); break;
        case ANIM_PARTICLES: html += F("✨ "); break;
        case ANIM_FIRE: html += F("🔥 "); break;
        case ANIM_SKYLINE: html += F("🏙️ "); break;
        case ANIM_GALAXY: html += F("🌌 "); break;
        case ANIM_LIGHTNING: html += F("⚡ "); break;
        case ANIM_PIPES: html += F("🔧 "); break;
        case ANIM_ORBITAL: html += F("🪐 "); break;
        case ANIM_STARS: html += F("⭐ "); break;
        case ANIM_BEACH: html += F("🏖️ "); break;
      }
      
      html += animName + F("</a>");
    }
    html += F("</div>");
    html += F("</div>");
    
    html += F("<div class='status'>");
    html += F("<h3>ℹ️ Information</h3>");
    html += F("<p>In <strong>Auto Mode</strong>, animations cycle every 30 seconds with smooth 2-second fade transitions.</p>");
    html += F("<p>In <strong>Manual Mode</strong>, you can select any animation and it will stay active.</p>");
    html += F("<p><a href='/erase'>🗑️ Erase WiFi Settings & Restart</a></p>");
    html += F("</div>");
    
    html += F("<script>");
    html += F("setInterval(() => { if (document.hidden === false) location.reload(); }, 5000);");
    html += F("</script>");
    html += F("</body></html>");
    
    request->send(200, F("text/html"), html);
  });

  // Animation selection endpoint
  server.on("^/animation/([0-9]+)$", HTTP_GET, [](AsyncWebServerRequest* request) {
    String animStr = request->pathArg(0);
    int animIndex = animStr.toInt();
    
    if (animIndex >= 0 && animIndex < ANIM_COUNT) {
      animationManager.setAnimation((AnimationType)animIndex);
      request->redirect("/");
    } else {
      request->send(400, "text/plain", "Invalid animation index");
    }
  });

  // Mode control endpoints
  server.on("/mode/auto", HTTP_GET, [](AsyncWebServerRequest* request) {
    animationManager.setAutoMode(true);
    request->redirect("/");
  });

  server.on("/mode/manual", HTTP_GET, [](AsyncWebServerRequest* request) {
    animationManager.setAutoMode(false);
    request->redirect("/");
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
  Serial.println("Animations will cycle every 30 seconds with 2-second fade transitions");
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