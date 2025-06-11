#include "animations_modules.h"
#include "animation_utils.h"
#include "display.h"
#include <FastLED.h>
#include <cmath>

namespace StarAnimation {
    // Private state - completely contained within this namespace
    struct Star {
        float x, y;
        float size;
        float phase;
        float twinkleSpeed;
        bool active;
    };
    
    struct State {
        Star stars[50];  // 50 stars
        uint32_t frameCount = 0;
        bool initialized = false;
    };
    
    static State state;
    
    void init() {
        // Initialize stars
        for (int i = 0; i < 50; i++) {
            state.stars[i].x = random(0, DISPLAY_WIDTH);
            state.stars[i].y = random(0, DISPLAY_HEIGHT);
            state.stars[i].size = 1.0f + (random(0, 100) / 100.0f) * 2.0f; // Size 1-3
            state.stars[i].phase = random(0, 628) / 100.0f; // Random phase 0-2π
            state.stars[i].twinkleSpeed = 0.05f + (random(0, 100) / 100.0f) * 0.1f; // Speed 0.05-0.15
            state.stars[i].active = true;
        }
        
        state.frameCount = 0;
        state.initialized = true;
    }
    
    void render() {
        if (!state.initialized) {
            init();
        }
        
        state.frameCount++;
        
        // Dark blue background
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                display.drawPixel(x, y, AnimationUtils::rgb888To565(0, 0, 20)); // #000033
            }
        }
        
        // Draw and update stars
        for (int i = 0; i < 50; i++) {
            Star& star = state.stars[i];
            if (!star.active) continue;
            
            // Update twinkle phase
            star.phase += star.twinkleSpeed;
            if (star.phase > 2 * M_PI) {
                star.phase -= 2 * M_PI;
            }
            
            // Calculate brightness (0-1)
            float brightness = (sin(star.phase) + 1.0f) / 2.0f;
            
            // Skip drawing if star is too dim to avoid black artifacts
            if (brightness < 0.15f) {
                continue;
            }
            
            // Draw star glow (larger, dimmer circle)
            float glowRadius = star.size * 2.0f;
            for (float dy = -glowRadius; dy <= glowRadius; dy += 0.5f) {
                for (float dx = -glowRadius; dx <= glowRadius; dx += 0.5f) {
                    float distance = sqrt(dx * dx + dy * dy);
                    if (distance <= glowRadius) {
                        int px = (int)(star.x + dx);
                        int py = (int)(star.y + dy);
                        
                        if (px >= 0 && px < DISPLAY_WIDTH && py >= 0 && py < DISPLAY_HEIGHT) {
                            float glowIntensity = (1.0f - (distance / glowRadius)) * brightness * 0.4f;
                            if (glowIntensity > 0.1f) { // Only draw if intensity is significant
                                // Get background color and blend with it
                                uint16_t bgColor = display.getPixel(px, py);
                                uint8_t bgR, bgG, bgB;
                                AnimationUtils::rgb565To888(bgColor, &bgR, &bgG, &bgB);
                                
                                // Add glow to background
                                uint8_t glowAmount = (uint8_t)(glowIntensity * 200);
                                uint8_t newR = min(255, bgR + glowAmount);
                                uint8_t newG = min(255, bgG + glowAmount);
                                uint8_t newB = min(255, bgB + glowAmount);
                                
                                uint16_t glowColor = AnimationUtils::rgb888To565(newR, newG, newB);
                                display.drawPixel(px, py, glowColor);
                            }
                        }
                    }
                }
            }
            
            // Draw star core (bright center)
            float coreRadius = star.size * 0.8f;
            for (float dy = -coreRadius; dy <= coreRadius; dy += 0.5f) {
                for (float dx = -coreRadius; dx <= coreRadius; dx += 0.5f) {
                    float distance = sqrt(dx * dx + dy * dy);
                    if (distance <= coreRadius) {
                        int px = (int)(star.x + dx);
                        int py = (int)(star.y + dy);
                        
                        if (px >= 0 && px < DISPLAY_WIDTH && py >= 0 && py < DISPLAY_HEIGHT) {
                            uint8_t coreBrightness = (uint8_t)(brightness * 255);
                            uint16_t coreColor = AnimationUtils::rgb888To565(coreBrightness, coreBrightness, coreBrightness);
                            display.drawPixel(px, py, coreColor);
                        }
                    }
                }
            }
        }
    }
    
    const char* getName() {
        return "Stars";
    }
}