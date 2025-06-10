#include "animation_base.h"

void Animations::initFire(AnimationManager* manager) {
    // Fire animation doesn't need specific initialization
    // The general frameCount and initialized will be set by the manager
}

void Animations::renderFire(AnimationManager* manager) {
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            // Use sin16 and cos16 for optimized noise generation
            float sinValue = sin16(TO_SIN16(x * 0.08f + manager->state.time * 0.04f)) / 32767.0f;
            float cosValue = cos16(TO_SIN16(y * 0.12f + manager->state.time * 0.03f)) / 32767.0f;
            float noise = sinValue * cosValue;
            float flame = max(0.0f, (y + noise * 8) / DISPLAY_HEIGHT);
            
            if (flame > 0.1f) {
                uint8_t r = min(255, (int)(flame * 240));
                uint8_t g = min(200, max(0, (int)((flame - 0.3f) * 350)));
                uint8_t b = max(0, (int)((flame - 0.7f) * 600));
                manager->drawPixelWithBlend(x, y, r, g, b);
            }
        }
    }
} 