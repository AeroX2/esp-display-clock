#include "animation_base.h"

void Animations::initStars(AnimationManager* manager) {
    // Initialize stars
    for (int i = 0; i < MAX_STARS; i++) {
        manager->state.stars[i].x = (i * 17) % DISPLAY_WIDTH;
        manager->state.stars[i].y = (i * 11) % DISPLAY_HEIGHT;
        manager->state.stars[i].size = 1.0f + (random(0, 100) / 100.0f);
        manager->state.stars[i].brightness = random(50, 100) / 100.0f;
        manager->state.stars[i].twinkleSpeed = 0.02f + (random(0, 30) / 1000.0f);
        manager->state.stars[i].phase = random(0, 628) / 100.0f;  // 0 to 2*PI
    }
}

void Animations::renderStars(AnimationManager* manager) {
    // Dark blue background
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            manager->drawPixelWithBlend(x, y, 0, 0, 51);  // #000033
        }
    }
    
    // Draw twinkling stars - use sin16 for optimized brightness calculation
    for (int i = 0; i < MAX_STARS; i++) {
        manager->state.stars[i].phase += manager->state.stars[i].twinkleSpeed;
        float brightness = (sin16(TO_SIN16(manager->state.stars[i].phase)) / 32767.0f + 1.0f) / 2.0f;
        
        uint8_t starBright = (uint8_t)(brightness * 255 * 0.8f);
        
        // Draw star with glow using drawCircle
        int x = (int)manager->state.stars[i].x;
        int y = (int)manager->state.stars[i].y;
        
        // Draw star as circle based on size and brightness  
        int radius = (int)(manager->state.stars[i].size * brightness);
        if (radius < 1) radius = 1;
        
        if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
            // Draw outer glow for bright stars
            if (brightness > 0.7f && radius > 1) {
                display.drawCircle(x, y, radius + 1, display.color565(starBright/4, starBright/4, starBright/4));
            }
            
            // Draw main star circle
            display.drawCircle(x, y, radius, display.color565(starBright, starBright, starBright));
            if (radius > 1) {
                display.fillCircle(x, y, radius - 1, display.color565(starBright, starBright, starBright));
            }
        }
    }
} 