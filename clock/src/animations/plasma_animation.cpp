#include "animations_modules.h"
#include "animation_utils.h"
#include "display.h"
#include <cmath>

namespace PlasmaAnimation {
    // Private state - completely contained within this namespace
    struct State {
        float plasmaTime = 0.0f;
        float colorShift = 0.0f;
        uint8_t plasmaTable[64];
        bool initialized = false;
    };
    
    static State state;
    
    void init() {
        // Initialize plasma lookup table
        for (int i = 0; i < 64; i++) {
            state.plasmaTable[i] = (uint8_t)(128 + 127 * sin(i * M_PI / 32.0));
        }
        
        state.plasmaTime = 0.0f;
        state.colorShift = 0.0f;
        state.initialized = true;
    }
    
    void render() {
        if (!state.initialized) {
            init();
        }
        
        state.plasmaTime += 0.05f;
        state.colorShift += 0.02f;
        
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                // Create plasma effect using sine waves
                float v1 = sin((x + state.plasmaTime) * 0.1) * 32 + 32;
                float v2 = sin((y + state.plasmaTime * 0.8) * 0.1) * 32 + 32;
                float v3 = sin((x + y + state.plasmaTime * 0.6) * 0.1) * 32 + 32;
                float v4 = sin(sqrt(x*x + y*y) + state.plasmaTime * 0.4) * 32 + 32;
                
                int plasma = (int)((v1 + v2 + v3 + v4) / 4) & 63;
                
                // Convert to color with shifting hue
                float hue = (plasma + state.colorShift * 50) / 64.0f;
                while (hue > 1.0f) hue -= 1.0f;
                
                uint8_t r, g, b;
                AnimationUtils::hslToRgb(hue, 1.0f, 0.5f, &r, &g, &b);
                
                display.drawPixelRGB888(x, y, r, g, b);
            }
        }
    }
    

    
    const char* getName() {
        return "Plasma";
    }
} 