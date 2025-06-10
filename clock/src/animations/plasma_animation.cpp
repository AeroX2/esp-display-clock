#include "animations_modules.h"
#include "animation_utils.h"
#include "display.h"
#include <cmath>
#include <FastLED.h>

#define TO_SIN16(x) (x * 10430)

namespace PlasmaAnimation {
    // Private state - completely contained within this namespace
    struct State {
        float plasmaTime = 0.0f;
        bool initialized = false;
    };
    
    static State state;
    
    void init() {
        state.plasmaTime = 0;
        state.initialized = true;
    }
    
    void render() {
        if (!state.initialized) {
            init();
        }
        
        state.plasmaTime = fmod(state.plasmaTime + 0.1f, 1000.0f);
        
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                float v1 = sin(x * 0.08f + state.plasmaTime);
                float v2 = sin(y * 0.08f + state.plasmaTime * 1.2f);
                float v3 = sin((x + y) * 0.04f + state.plasmaTime * 0.8f);
                float plasma = (v1 + v2 + v3) / 3.0f;
                
                float hue = 280 + plasma * 80;
                float saturation = 0.7f + plasma * 0.3f;
                float lightness = 0.4f + plasma * 0.3f;
                
                uint8_t r, g, b;
                AnimationUtils::hslToRgb(fmod(hue, 360.0f) / 360.0f, saturation, lightness, &r, &g, &b);
                display.drawPixelRGB888(x, y, r, g, b);
            }
        }
    }
    
    const char* getName() {
        return "Plasma";
    }
}