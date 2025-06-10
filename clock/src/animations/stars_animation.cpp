#include "animations_modules.h"
#include "animation_utils.h"
#include "display.h"
#include <cmath>

namespace StarsAnimation {
    struct Star {
        float x, y;
        float size;
        float brightness;
        float twinkleSpeed;
        float phase;
    };
    
    struct State {
        Star stars[50];
        float time = 0.0f;
        bool initialized = false;
    };
    
    static State state;
    
    void init() {
        // Initialize twinkling stars
        for (int i = 0; i < 50; i++) {
            state.stars[i].x = random(0, DISPLAY_WIDTH);
            state.stars[i].y = random(0, DISPLAY_HEIGHT);
            state.stars[i].size = random(1, 4);
            state.stars[i].brightness = random(50, 255) / 255.0f;
            state.stars[i].twinkleSpeed = random(50, 200) / 100.0f;
            state.stars[i].phase = random(0, 628) / 100.0f;
        }
        state.time = 0.0f;
        state.initialized = true;
    }
    
    void render() {
        if (!state.initialized) {
            init();
        }
        
        state.time += 0.02f;
        
        // Dark space background
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                display.drawPixelRGB888(x, y, 0, 0, 10);
            }
        }
        
        // Draw twinkling stars
        for (int i = 0; i < 50; i++) {
            float twinkle = sin(state.time * state.stars[i].twinkleSpeed + state.stars[i].phase) * 0.5f + 0.5f;
            float currentBrightness = state.stars[i].brightness * twinkle;
            
            uint8_t brightness = (uint8_t)(currentBrightness * 255);
            uint8_t r, g, b;
            
            // Different star colors based on index
            if (i % 3 == 0) {
                r = brightness; g = brightness; b = brightness; // White
            } else if (i % 3 == 1) {
                r = brightness; g = brightness * 0.8f; b = brightness * 0.6f; // Warm
            } else {
                r = brightness * 0.8f; g = brightness * 0.9f; b = brightness; // Cool
            }
            
            // Draw star with size
            for (int sy = 0; sy < state.stars[i].size; sy++) {
                for (int sx = 0; sx < state.stars[i].size; sx++) {
                    int px = (int)state.stars[i].x + sx - state.stars[i].size/2;
                    int py = (int)state.stars[i].y + sy - state.stars[i].size/2;
                    if (px >= 0 && px < DISPLAY_WIDTH && py >= 0 && py < DISPLAY_HEIGHT) {
                        float distance = sqrt((sx - state.stars[i].size/2.0f) * (sx - state.stars[i].size/2.0f) + 
                                            (sy - state.stars[i].size/2.0f) * (sy - state.stars[i].size/2.0f));
                        if (distance <= state.stars[i].size/2.0f) {
                            float intensity = 1.0f - (distance / (state.stars[i].size/2.0f));
                            AnimationUtils::drawPixelWithBlend(px, py, 
                                (uint8_t)(r * intensity), 
                                (uint8_t)(g * intensity), 
                                (uint8_t)(b * intensity));
                        }
                    }
                }
            }
            
            // Add cross pattern for brighter stars
            if (state.stars[i].size >= 3 && currentBrightness > 0.7f) {
                // Horizontal line
                for (int dx = -2; dx <= 2; dx++) {
                    int px = (int)state.stars[i].x + dx;
                    int py = (int)state.stars[i].y;
                    if (px >= 0 && px < DISPLAY_WIDTH && py >= 0 && py < DISPLAY_HEIGHT) {
                        AnimationUtils::drawPixelWithBlend(px, py, r/3, g/3, b/3, 100);
                    }
                }
                // Vertical line
                for (int dy = -2; dy <= 2; dy++) {
                    int px = (int)state.stars[i].x;
                    int py = (int)state.stars[i].y + dy;
                    if (px >= 0 && px < DISPLAY_WIDTH && py >= 0 && py < DISPLAY_HEIGHT) {
                        AnimationUtils::drawPixelWithBlend(px, py, r/3, g/3, b/3, 100);
                    }
                }
            }
        }
        
        // Add some shooting stars occasionally
        if (random(1000) == 0) {
            int startX = random(0, DISPLAY_WIDTH);
            int startY = random(0, DISPLAY_HEIGHT/2);
            for (int trail = 0; trail < 8; trail++) {
                int x = startX + trail * 2;
                int y = startY + trail;
                if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
                    uint8_t alpha = 255 - (trail * 30);
                    AnimationUtils::drawPixelWithBlend(x, y, 255, 255, 200, alpha);
                }
            }
        }
    }
    

    
    const char* getName() {
        return "Stars";
    }
} 