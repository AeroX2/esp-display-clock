#include "animations_modules.h"
#include "animation_utils.h"
#include "display.h"
#include <algorithm>
#include <FastLED.h>

namespace FireAnimation {
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

        display.clearData();
        
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                // Convert coordinates to FastLED angles with fixed-point math
                int16_t noiseX = ((x * 10430) / 18 + (state.frameCount * (0.2 * 10430))); 
                int16_t noiseY = ((y * 10430) / 12 + (state.frameCount * (0.15 * 10430))); 
                
                // Use FastLED's 16-bit sine approximations
                int32_t noise = ((int32_t)sin16(noiseX) * cos16(noiseY)) >> 15;
                
                // Fixed-point arithmetic for flame calculations (8-bit fraction)
                int16_t flameHeight = (y + ((noise * 8) >> 15));
                int16_t flame = (flameHeight << 8) / 64;
                
                if (flame > 25) { // 0.1 in fixed point (25 = 0.1 * 256)
                    uint8_t r = min(255, (flame * 240) >> 8);
                    uint8_t g = min(200, max(0, ((flame - 77) * 350) >> 8)); // 77 = 0.3 * 256
                    uint8_t b = max(0, ((flame - 179) * 600) >> 8); // 179 = 0.7 * 256
                    AnimationUtils::drawPixelWithBlend(x, y, AnimationUtils::rgb888To565(r, g, b));
                }
            }
        }
    }
    
    const char* getName() {
        return "Fire";
    }
}