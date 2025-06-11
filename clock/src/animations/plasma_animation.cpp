#include "animations_modules.h"
#include "animation_utils.h"
#include "display.h"
#include <FastLED.h>

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
        
        state.plasmaTime += 0.1f;
        
        // Keep plasmaTime within a reasonable range to prevent precision loss
        // and ensure FastLED's sin16 lookup tables work optimally
        if (state.plasmaTime > 628.0f) {  // 2*PI*100 ≈ 628
            state.plasmaTime -= 628.0f;
        }
        
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                // Use FastLED's sin16 for better performance and precision
                // sin16 takes values 0-65535 representing 0-2π
                uint16_t angle1 = (uint16_t)((x * 0.08f + state.plasmaTime) * 10430.0f);  // 65535/(2*PI)
                uint16_t angle2 = (uint16_t)((y * 0.08f + state.plasmaTime * 1.2f) * 10430.0f);
                uint16_t angle3 = (uint16_t)(((x + y) * 0.04f + state.plasmaTime * 0.8f) * 10430.0f);
                
                // sin16 returns -32767 to 32767, convert to -1.0 to 1.0
                float v1 = sin16(angle1) / 32767.0f;
                float v2 = sin16(angle2) / 32767.0f;
                float v3 = sin16(angle3) / 32767.0f;
                float plasma = (v1 + v2 + v3) / 3.0f;
                
                float hue = 280 + plasma * 80;
                float saturation = 0.7f + plasma * 0.3f;
                float lightness = 0.4f + plasma * 0.3f;
                
                // Ensure hue stays within 0-360 range
                while (hue < 0) hue += 360.0f;
                while (hue >= 360.0f) hue -= 360.0f;
                
                uint8_t r, g, b;
                AnimationUtils::hslToRgb(hue / 360.0f, saturation, lightness, &r, &g, &b);
                display.drawPixelRGB888(x, y, r, g, b);
            }
        }
    }
    
    const char* getName() {
        return "Plasma";
    }
}