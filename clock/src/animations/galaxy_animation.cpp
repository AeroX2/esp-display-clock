#include "animations_modules.h"
#include "animation_utils.h"
#include "display.h"
#include <cmath>

namespace GalaxyAnimation {
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
        
        float centerX = 64;
        float centerY = 32;
        float time = state.frameCount * 0.01f;
        
        // Background with radial gradient
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                int dx = x - centerX;
                int dy = y - centerY;
                float distance = sqrt(dx * dx + dy * dy);
                float gradient = distance / 40.0f;
                gradient = constrain(gradient, 0, 1);
                
                uint8_t r = (1 - gradient) * 0x00 + gradient * 0x00;
                uint8_t g = (1 - gradient) * 0x00 + gradient * 0x00;
                uint8_t b = (1 - gradient) * 0x11 + gradient * 0x33;
                
                display.drawPixelRGB888(x, y, r, g, b);
            }
        }
        
        // Spiral arms with 4 layers and 2 arms each
        for (int layer = 0; layer < 4; layer++) {
            for (int arm = 0; arm < 2; arm++) {
                float startAngle = (arm * M_PI) + (layer * M_PI / 4);
                float rotationSpeed = 0.8f + layer * 0.2f;
                
                for (float angle = 0; angle < M_PI * 4; angle += 0.03f) {
                    float depth = 0.8f + layer * 0.2f;
                    float radius = angle * 3 * depth;
                    float totalAngle = angle + time * rotationSpeed + startAngle;
                    float x = centerX + cos(totalAngle) * radius;
                    float y = centerY + sin(totalAngle) * radius;
                    
                    if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
                        float hue = fmod(layer * 45 + time * 30, 360) / 360.0f;
                        uint8_t r, g, b;
                        AnimationUtils::hslToRgb(hue, 1.0f, 0.6f, &r, &g, &b);
                        
                        uint8_t alpha = 0.6f * 255;
                        AnimationUtils::drawPixelWithBlend(x, y, AnimationUtils::rgb888To565(r, g, b), alpha);
                        
                        // Make lines thicker
                        if (x + 1 < DISPLAY_WIDTH) {
                            AnimationUtils::drawPixelWithBlend(x + 1, y, AnimationUtils::rgb888To565(r, g, b), alpha);
                        }
                        if (y + 1 < DISPLAY_HEIGHT) {
                            AnimationUtils::drawPixelWithBlend(x, y + 1, AnimationUtils::rgb888To565(r, g, b), alpha);
                        }
                    }
                }
            }
        }
    }
    
    const char* getName() {
        return "Galaxy";
    }
}