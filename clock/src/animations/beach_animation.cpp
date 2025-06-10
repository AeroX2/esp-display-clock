#include "animations_modules.h"
#include "animation_utils.h"
#include "display.h"
#include <cmath>

namespace BeachAnimation {
    struct State {
        float waveOffset = 0.0f;
        float time = 0.0f;
        bool initialized = false;
    };
    
    static State state;
    
    void init() {
        state.waveOffset = 0.0f;
        state.time = 0.0f;
        state.initialized = true;
    }
    
    void render() {
        if (!state.initialized) {
            init();
        }
        
        state.time += 0.03f;
        state.waveOffset += 0.05f;
        
        // Sky gradient
        for (int y = 0; y < DISPLAY_HEIGHT / 2; y++) {
            uint8_t skyBlue = map(y, 0, DISPLAY_HEIGHT / 2, 135, 206);
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                display.drawPixelRGB888(x, y, 135, 206, 235); // Sky blue
            }
        }
        
        // Sand
        for (int y = DISPLAY_HEIGHT / 2; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                uint8_t sandBrightness = 194 + sin(x * 0.1f + y * 0.05f + state.time) * 20;
                display.drawPixelRGB888(x, y, sandBrightness, sandBrightness - 20, sandBrightness - 80);
            }
        }
        
        // Animated waves
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            float wave1 = sin((x + state.waveOffset) * 0.1f) * 4;
            float wave2 = sin((x + state.waveOffset * 1.3f) * 0.15f) * 2;
            float wave3 = sin((x + state.waveOffset * 0.8f) * 0.08f) * 3;
            
            int waveY = DISPLAY_HEIGHT / 2 + (int)(wave1 + wave2 + wave3);
            
            // Draw wave line
            if (waveY >= 0 && waveY < DISPLAY_HEIGHT) {
                AnimationUtils::drawPixelWithBlend(x, waveY, 255, 255, 255, 200); // White foam
                if (waveY - 1 >= 0) {
                    AnimationUtils::drawPixelWithBlend(x, waveY - 1, 100, 150, 255, 150); // Blue water
                }
                if (waveY + 1 < DISPLAY_HEIGHT) {
                    AnimationUtils::drawPixelWithBlend(x, waveY + 1, 150, 200, 255, 100); // Lighter blue
                }
            }
        }
        
        // Sun
        int sunX = DISPLAY_WIDTH - 15;
        int sunY = 8;
        
        // Sun rays
        for (int angle = 0; angle < 360; angle += 45) {
            float rad = (angle + state.time * 10) * M_PI / 180.0f;
            for (int r = 8; r <= 12; r++) {
                int rayX = sunX + cos(rad) * r;
                int rayY = sunY + sin(rad) * r;
                if (rayX >= 0 && rayX < DISPLAY_WIDTH && rayY >= 0 && rayY < DISPLAY_HEIGHT) {
                    AnimationUtils::drawPixelWithBlend(rayX, rayY, 255, 255, 0, 100);
                }
            }
        }
        
        // Sun body
        for (int dy = -6; dy <= 6; dy++) {
            for (int dx = -6; dx <= 6; dx++) {
                if (dx*dx + dy*dy <= 36) { // radius 6
                    int px = sunX + dx;
                    int py = sunY + dy;
                    if (px >= 0 && px < DISPLAY_WIDTH && py >= 0 && py < DISPLAY_HEIGHT) {
                        float distance = sqrt(dx*dx + dy*dy);
                        uint8_t intensity = 255 - (distance * 20);
                        display.drawPixelRGB888(px, py, 255, 255, intensity);
                    }
                }
            }
        }
        
        // Palm tree
        int treeX = 10;
        int trunkTop = DISPLAY_HEIGHT / 2;
        
        // Trunk
        for (int y = trunkTop; y < DISPLAY_HEIGHT - 5; y++) {
            display.drawPixelRGB888(treeX, y, 101, 67, 33); // Brown
            if (treeX + 1 < DISPLAY_WIDTH) {
                display.drawPixelRGB888(treeX + 1, y, 101, 67, 33);
            }
        }
        
        // Palm fronds
        for (int frond = 0; frond < 5; frond++) {
            float angle = frond * M_PI * 2 / 5 + state.time * 0.1f;
            for (int len = 1; len <= 8; len++) {
                int frondX = treeX + cos(angle) * len;
                int frondY = trunkTop - 2 + sin(angle) * len * 0.3f;
                if (frondX >= 0 && frondX < DISPLAY_WIDTH && frondY >= 0 && frondY < DISPLAY_HEIGHT) {
                    AnimationUtils::drawPixelWithBlend(frondX, frondY, 34, 139, 34, 200); // Forest green
                }
            }
        }
        
        // Flying seagull
        int birdX = (int)(state.time * 20) % (DISPLAY_WIDTH + 20) - 10;
        int birdY = 15 + sin(state.time * 2) * 3;
        
        if (birdX >= -5 && birdX < DISPLAY_WIDTH + 5 && birdY >= 0 && birdY < DISPLAY_HEIGHT) {
            // Simple bird shape
            AnimationUtils::drawPixelWithBlend(birdX, birdY, 255, 255, 255);
            AnimationUtils::drawPixelWithBlend(birdX - 1, birdY, 255, 255, 255);
            AnimationUtils::drawPixelWithBlend(birdX + 1, birdY, 255, 255, 255);
            AnimationUtils::drawPixelWithBlend(birdX, birdY - 1, 255, 255, 255);
        }
    }
    

    
    const char* getName() {
        return "Beach";
    }
} 