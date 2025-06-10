#include "animation_base.h"

void Animations::initParticles(AnimationManager* manager) {
    // Particles animation doesn't need specific initialization
    // The general frameCount and initialized will be set by the manager
}

void Animations::renderParticles(AnimationManager* manager) {
    // Draw 15 particles matching HTML implementation
    for (int i = 0; i < 15; i++) {
        // Calculate position matching HTML: (animationFrame * 0.3 * (1 + i * 0.1) + i * 25) % 140 - 6
        float x = fmod(manager->state.frameCount * 0.3f * (1 + i * 0.1f) + i * 25, 140) - 6;
        // y = 32 + Math.sin(animationFrame * 0.02 + i) * 25
        float y = 32 + sin16(TO_SIN16(manager->state.frameCount * 0.02f + i)) / 32767.0f * 25;
        
        // Calculate hue: (animationFrame * 0.8 + i * 40) % 360
        float hue = fmod(manager->state.frameCount * 0.8f + i * 40, 360);
        uint8_t r, g, b;
        manager->hslToRgb(hue / 360.0f, 0.9f, 0.6f, &r, &g, &b);
        
        // Draw main particle with drawCircle (radius 2)
        if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
            display.drawCircle((int)x, (int)y, 2, display.color565(r, g, b));
            display.fillCircle((int)x, (int)y, 1, display.color565(r, g, b));
        }
        
        // Draw trail particle (matching HTML: trailX = x - 8, radius 1.2)
        float trailX = x - 8;
        if (trailX > 0 && trailX < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
            // 30% alpha for trail
            uint8_t trailR = r * 0.3f;
            uint8_t trailG = g * 0.3f;
            uint8_t trailB = b * 0.3f;
            display.drawCircle((int)trailX, (int)y, 1, display.color565(trailR, trailG, trailB));
        }
    }
} 