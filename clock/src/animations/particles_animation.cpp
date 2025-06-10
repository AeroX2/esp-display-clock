#include "animations_modules.h"
#include "animation_utils.h"
#include "display.h"
#include <cmath>

namespace ParticlesAnimation {
    // Private state - completely contained within this namespace
    struct State {
        uint32_t frameCount = 0;
        bool initialized = false;
    };
    
    static State state;
    
    void init() {
        state.frameCount = 0;
        state.initialized = true;
    }
    
    void render() {
        if (!state.initialized) {
            init();
        }
        
        state.frameCount++;
        
        // Clear background
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                display.drawPixelRGB888(x, y, 0, 0, 0);
            }
        }
        
        // Draw 15 particles matching original implementation
        for (int i = 0; i < 15; i++) {
            // Calculate position
            float x = fmod(state.frameCount * 0.3f * (1 + i * 0.1f) + i * 25, 140) - 6;
            float y = 32 + sin(state.frameCount * 0.02f + i) * 25;
            
            // Calculate hue
            float hue = fmod(state.frameCount * 0.8f + i * 40, 360) / 360.0f;
            uint8_t r, g, b;
            AnimationUtils::hslToRgb(hue, 0.9f, 0.6f, &r, &g, &b);
            
            // Draw main particle (radius 2)
            if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
                // Draw filled circle
                for (int dy = -2; dy <= 2; dy++) {
                    for (int dx = -2; dx <= 2; dx++) {
                        if (dx*dx + dy*dy <= 4) { // radius 2
                            int px = (int)x + dx;
                            int py = (int)y + dy;
                            if (px >= 0 && px < DISPLAY_WIDTH && py >= 0 && py < DISPLAY_HEIGHT) {
                                uint8_t intensity = (dx*dx + dy*dy <= 1) ? 255 : 128; // brighter center
                                AnimationUtils::drawPixelWithBlend(px, py, 
                                    (r * intensity) / 255, 
                                    (g * intensity) / 255, 
                                    (b * intensity) / 255);
                            }
                        }
                    }
                }
            }
            
            // Draw trail particle (30% alpha, offset by -8)
            float trailX = x - 8;
            if (trailX >= 0 && trailX < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
                // Draw smaller trail circle (radius 1)
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (dx*dx + dy*dy <= 1) { // radius 1
                            int px = (int)trailX + dx;
                            int py = (int)y + dy;
                            if (px >= 0 && px < DISPLAY_WIDTH && py >= 0 && py < DISPLAY_HEIGHT) {
                                AnimationUtils::drawPixelWithBlend(px, py, 
                                    (r * 30) / 100, 
                                    (g * 30) / 100, 
                                    (b * 30) / 100, 
                                    76); // 30% alpha
                            }
                        }
                    }
                }
            }
        }
    }
    

    
    const char* getName() {
        return "Particles";
    }
} 