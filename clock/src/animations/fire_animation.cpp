#include "animations_modules.h"
#include "animation_utils.h"
#include "display.h"
#include <cmath>

namespace FireAnimation {
    // Private state - completely contained within this namespace
    struct State {
        float time = 0.0f;
        bool initialized = false;
    };
    
    static State state;
    
    void init() {
        state.time = 0.0f;
        state.initialized = true;
    }
    
    void render() {
        if (!state.initialized) {
            init();
        }
        
        state.time += 0.02f;
        
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                // Create fire effect using sine waves for flame-like movement
                float sinValue = sin(x * 0.08f + state.time * 0.04f);
                float cosValue = cos(y * 0.12f + state.time * 0.03f);
                float noise = sinValue * cosValue;
                
                // Create flame gradient from bottom to top
                float flame = max(0.0f, (float)(DISPLAY_HEIGHT - y + noise * 8) / DISPLAY_HEIGHT);
                
                if (flame > 0.1f) {
                    // Fire colors: red -> orange -> yellow -> white
                    uint8_t r = min(255, (int)(flame * 240));
                    uint8_t g = min(200, max(0, (int)((flame - 0.3f) * 350)));
                    uint8_t b = max(0, (int)((flame - 0.7f) * 600));
                    
                    // Add some flickering
                    float flicker = 0.8f + 0.2f * sin(state.time * 3.0f + x + y);
                    r = (uint8_t)(r * flicker);
                    g = (uint8_t)(g * flicker);
                    b = (uint8_t)(b * flicker);
                    
                    AnimationUtils::drawPixelWithBlend(x, y, r, g, b);
                }
            }
        }
    }
    

    
    const char* getName() {
        return "Fire";
    }
} 