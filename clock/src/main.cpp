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

  // Update and render animations first (this will clear the display)
  animationManager.update();

  // Draw text over the animation (original approach)
  display.setTextColor(display.color565(255, 255, 255));

  display.setFont(&Karma_Future22pt7b);
  drawCenteredString(
      showCol ? currentTime : currentTimeNoColumn,
      DISPLAY_WIDTH / 2,
      1);
  display.setFont(&Karma_Future10pt7b);
  drawCenteredString(currentDate, DISPLAY_WIDTH / 2, 44);
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
    String html = "<!DOCTYPE html><html><head>";
    html += "<title>ESP32 Clock Controller</title>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; max-width: 600px; margin: 0 auto; padding: 20px; background: #1a1a1a; color: white; }";
    html += "h1 { color: #4a90e2; text-align: center; }";
    html += ".status { background: #2a2a2a; padding: 15px; border-radius: 8px; margin: 15px 0; }";
    html += ".controls { background: #2a2a2a; padding: 15px; border-radius: 8px; margin: 15px 0; }";
    html += ".animation-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); gap: 10px; margin: 15px 0; }";
    html += ".anim-btn { padding: 12px; background: #3a3a3a; color: white; border: 1px solid #555; border-radius: 5px; cursor: pointer; text-decoration: none; text-align: center; transition: all 0.2s; }";
    html += ".anim-btn:hover { background: #4a4a4a; }";
    html += ".anim-btn.active { background: #4a90e2; border-color: #6bb6ff; }";
    html += ".toggle-btn { padding: 10px 20px; background: #4a90e2; color: white; border: none; border-radius: 5px; cursor: pointer; margin: 5px; }";
    html += ".toggle-btn:hover { background: #6bb6ff; }";
    html += ".auto-mode { background: #2a5a2a; }";
    html += ".manual-mode { background: #5a2a2a; }";
    html += "a { color: #4a90e2; text-decoration: none; }";
    html += "a:hover { text-decoration: underline; }";
    html += "</style></head><body>";
    
    html += "<h1>🎨 ESP32 Clock Controller</h1>";
    
    // Status section
    html += "<div class='status'>";
    html += "<h3>📊 Status</h3>";
    html += "<p><strong>Current Animation:</strong> " + getAnimationName(animationManager.getCurrentAnimation()) + "</p>";
    
    if (animationManager.isInFade()) {
      html += "<p><strong>Status:</strong> Transitioning (";
      html += String((int)((animationManager.getFadeProgress() / 255.0f) * 100));
      html += "%)</p>";
    } else {
      html += "<p><strong>Status:</strong> Running</p>";
    }
    
    html += "<p><strong>Mode:</strong> ";
    html += animationManager.isAutoMode() ? "Auto-Cycling" : "Manual Control";
    html += "</p>";
    html += "</div>";
    
    // Controls section
    html += "<div class='controls'>";
    html += "<h3>🎛️ Controls</h3>";
    
    // Mode toggle buttons
    html += "<p>";
    html += "<button class='toggle-btn auto-mode' onclick=\"fetch('/mode/auto')\">🔄 Auto Mode</button>";
    html += "<button class='toggle-btn manual-mode' onclick=\"fetch('/mode/manual')\">🎯 Manual Mode</button>";
    html += "</p>";
    
    // Animation selection grid
    html += "<h4>Select Animation:</h4>";
    html += "<div class='animation-grid'>";
    
    for (int i = 0; i < ANIM_COUNT; i++) {
      AnimationType anim = (AnimationType)i;
      String animName = getAnimationName(anim);
      String activeClass = (anim == animationManager.getCurrentAnimation()) ? " active" : "";
      
      html += "<a href='/animation/" + String(i) + "' class='anim-btn" + activeClass + "'>";
      
      // Add emoji for each animation
      switch(anim) {
        case ANIM_PLASMA: html += "🌈 "; break;
        case ANIM_PARTICLES: html += "✨ "; break;
        case ANIM_FIRE: html += "🔥 "; break;
        case ANIM_SKYLINE: html += "🏙️ "; break;
        case ANIM_GALAXY: html += "🌌 "; break;
        case ANIM_LIGHTNING: html += "⚡ "; break;
        case ANIM_PIPES: html += "🔧 "; break;
        case ANIM_ORBITAL: html += "🪐 "; break;
        case ANIM_STARS: html += "⭐ "; break;
        case ANIM_BEACH: html += "🏖️ "; break;
      }
      
      html += animName + "</a>";
    }
    html += "</div>";
    html += "</div>";
    
    // Additional info
    html += "<div class='status'>";
    html += "<h3>ℹ️ Information</h3>";
    html += "<p>In <strong>Auto Mode</strong>, animations cycle every 30 seconds with smooth 2-second fade transitions.</p>";
    html += "<p>In <strong>Manual Mode</strong>, you can select any animation and it will stay active.</p>";
    html += "<p><a href='/erase'>🗑️ Erase WiFi Settings & Restart</a></p>";
    html += "</div>";
    
    html += "<script>";
    html += "setInterval(() => { if (document.hidden === false) location.reload(); }, 5000);";  // Auto-refresh every 5 seconds when visible
    html += "</script>";
    html += "</body></html>";
    
    request->send(200, "text/html", html);
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