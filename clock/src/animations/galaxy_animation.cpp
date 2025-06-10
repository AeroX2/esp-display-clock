#include "animation_base.h"

void Animations::initGalaxy(AnimationManager* manager) {
    // Galaxy animation doesn't need specific initialization
    // The general frameCount and initialized will be set by the manager
}

void Animations::renderGalaxy(AnimationManager* manager) {
    float centerX = 64;
    float centerY = 32;
    float time = manager->state.frameCount * 0.01f;
    
    // Background with radial gradient (matching HTML version)
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            int dx = x - centerX;
            int dy = y - centerY;
            float distance = sqrt(dx * dx + dy * dy);
            float gradient = distance / 40.0f;
            gradient = constrain(gradient, 0, 1);
            
            // Interpolate between #000011 (inner) and #000033 (outer)
            uint8_t r = (1 - gradient) * 0x00 + gradient * 0x00;
            uint8_t g = (1 - gradient) * 0x00 + gradient * 0x00;
            uint8_t b = (1 - gradient) * 0x11 + gradient * 0x33;
            
            display.drawPixel(x, y, display.color565(r, g, b));
        }
    }
    
    // Spiral arms with 4 layers and 2 arms each (matching HTML)
    for (int layer = 0; layer < 4; layer++) {  // 4 layers like HTML
        for (int arm = 0; arm < 2; arm++) {
            // Stagger the starting angle for each arm
            float startAngle = (arm * PI) + (layer * PI / 4);
            // Different rotation speeds for each layer
            float rotationSpeed = 0.8f + layer * 0.2f;
            
            // Draw continuous spiral arms (matching HTML step of 0.03)
            for (float angle = 0; angle < PI * 4; angle += 0.03f) {
                float depth = 0.8f + layer * 0.2f;
                float radius = angle * 3 * depth;  // radius = angle * 3 * depth like HTML
                float totalAngle = angle + time * rotationSpeed + startAngle;
                float x = centerX + cos(totalAngle) * radius;
                float y = centerY + sin(totalAngle) * radius;
                
                if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
                    float hue = fmod(layer * 45 + time * 30, 360);
                    uint8_t r, g, b;
                    manager->hslToRgb(hue / 360.0f, 1.0f, 0.6f, &r, &g, &b);
                    
                    // Draw with 60% alpha like HTML
                    uint8_t alpha = 0.6f * 255;
                    manager->drawPixelWithBlend((int)x, (int)y, r, g, b, alpha);
                    
                    // Also draw adjacent pixels to make lines thicker (lineWidth = 2 in HTML)
                    if (x + 1 < DISPLAY_WIDTH) {
                        manager->drawPixelWithBlend((int)x + 1, (int)y, r, g, b, alpha);
                    }
                    if (y + 1 < DISPLAY_HEIGHT) {
                        manager->drawPixelWithBlend((int)x, (int)y + 1, r, g, b, alpha);
                    }
                }
            }
        }
    }
} 