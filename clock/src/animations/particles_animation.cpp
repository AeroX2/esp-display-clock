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
        
        // Reset frame count before it gets too large to prevent overflow
        state.frameCount = (state.frameCount + 1) % 1000000;
        
        // Clear background
        AnimationUtils::applyFade(23);
        
        // Draw 15 particles matching original implementation
        for (int i = 0; i < 15; i++) {
            // Calculate position
            float x = fmod(state.frameCount * 0.3f * (1 + i * 0.1f) + i * 25, 140) - 6;
            float y = 32 + sin(state.frameCount * 0.02f + i) * 25;
            
            // Only draw if within bounds
            if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
                // Calculate hue
                float hue = fmod(state.frameCount * 0.8f + i * 40, 360) / 360.0f;
                uint8_t r, g, b;
                AnimationUtils::hslToRgb(hue, 0.9f, 0.6f, &r, &g, &b);
                AnimationUtils::fillCircle(x, y, 2, display.color565(r,g,b));
                
                float trailX = x - 8;
                if (trailX >= 0 && trailX < DISPLAY_WIDTH) {
                    uint16_t color = AnimationUtils::alphaBlend(trailX, y, r, g, b, 77);
                    AnimationUtils::fillCircle(trailX, y, 1.2, color);
                }
            }
        }
    }
    
    const char* getName() {
        return "Particles";
    }
}