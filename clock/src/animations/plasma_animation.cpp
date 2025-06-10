#include "animation_base.h"

void Animations::initPlasma(AnimationManager* manager) {
    // Plasma animation doesn't need specific initialization
    // The general frameCount and initialized will be set by the manager
}

void Animations::renderPlasma(AnimationManager* manager) {
    float time = manager->state.time;// * TIME_SCALE;
    
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            // Use sin16 for optimized performance
            float v1 = sin16(TO_SIN16(x * PLASMA_SCALE + time)) / 32767.0f;
            float v2 = sin16(TO_SIN16(y * PLASMA_SCALE + time * 1.2f)) / 32767.0f;
            float v3 = sin16(TO_SIN16((x + y) * 0.04f + time * 0.8f)) / 32767.0f;
            float plasma = (v1 + v2 + v3) / 3.0f;
            
            float hue = 280 + plasma * 80;
            float saturation = 0.7f + plasma * 0.3f;
            float lightness = 0.4f + plasma * 0.3f;
            
            uint8_t r, g, b;
            manager->hslToRgb(fmod(hue, 360.0f) / 360.0f, saturation, lightness, &r, &g, &b);
            manager->drawPixelWithBlend(x, y, r, g, b);
        }
    }
} 