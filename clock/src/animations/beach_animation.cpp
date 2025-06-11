#include "animations_modules.h"
#include "animation_utils.h"
#include "display.h"
#include <FastLED.h>

namespace BeachAnimation {
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
        float time = state.frameCount * 0.005f;  // Matches HTML timing
        
        // Sky gradient (40% of height = 25.6px, but we'll use 26px)
        for (int y = 0; y < 26; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                // Gradient from #037ccb (deep sky blue) to #82ccef (light sky blue)
                uint8_t r = 0x03 + ((y * (0x82 - 0x03)) / 26);
                uint8_t g = 0x7c + ((y * (0xcc - 0x7c)) / 26);
                uint8_t b = 0xcb + ((y * (0xef - 0xcb)) / 26);
                display.drawPixelRGB888(x, y, r, g, b);
            }
        }
        
        // Dry sand background (from 65% = 41.6px to bottom, we'll use 42px)
        for (int y = 42; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                display.drawPixelRGB888(x, y, 0xfd, 0xf1, 0xd7);  // #fdf1d7 dry sand
            }
        }
        
        // Wave animation (matches CSS waveanim keyframes)
        float waveScale = 1.0f;
        float cyclePosition = fmod(time * 0.1f, 1.0f);  // 10s cycle
        
        if (cyclePosition <= 0.35f) {
            // 0% to 35%: scale from 1.0 to 1.3
            waveScale = 1.0f + (cyclePosition / 0.35f) * 0.3f;
        } else if (cyclePosition <= 0.69f) {
            // 35% to 69%: scale from 1.3 back to 1.0
            waveScale = 1.3f - ((cyclePosition - 0.35f) / (0.69f - 0.35f)) * 0.3f;
        } else {
            // 69% to 100%: stay at 1.0
            waveScale = 1.0f;
        }
        
        // Sea parameters (30% height scaled, 200% width, -50% left, top at 40%)
        int seaHeight = (int)(19.2f * waveScale);  // 30% of 64px scaled
        int seaWidth = 256;   // 200% of 128px
        int seaLeft = -64;    // -50% of 128px
        int seaTop = 26;      // 40% of 64px (approximately)
        
        // Draw curved sea using simple ellipse approximation
        for (int y = seaTop; y < seaTop + seaHeight && y < DISPLAY_HEIGHT; y++) {
            for (int x = max(0, seaLeft); x < min(DISPLAY_WIDTH, seaLeft + seaWidth); x++) {
                // Check if point is within elliptical sea area
                float centerX = seaLeft + seaWidth / 2.0f;
                float centerY = seaTop + seaHeight / 2.0f;
                float dx = (x - centerX) / (seaWidth / 2.0f);
                float dy = (y - centerY) / (seaHeight / 2.0f);
                
                if (dx * dx + dy * dy <= 1.0f) {
                    // Sea gradient (matches CSS)
                    float gradientPos = (float)(y - seaTop) / seaHeight;
                    uint8_t r, g, b;
                    
                    if (gradientPos <= 0.25f) {
                        float t = gradientPos / 0.25f;
                        r = 8 + (uint8_t)(t * (18 - 8));
                        g = 122 + (uint8_t)(t * (156 - 122));
                        b = 193 + (uint8_t)(t * (192 - 193));
                    } else if (gradientPos <= 0.5f) {
                        float t = (gradientPos - 0.25f) / 0.25f;
                        r = 18 + (uint8_t)(t * (42 - 18));
                        g = 156 + (uint8_t)(t * (212 - 156));
                        b = 192 + (uint8_t)(t * (229 - 192));
                    } else if (gradientPos <= 0.75f) {
                        float t = (gradientPos - 0.5f) / 0.25f;
                        r = 42 + (uint8_t)(t * (150 - 42));
                        g = 212 + (uint8_t)(t * (233 - 212));
                        b = 229 + (uint8_t)(t * (239 - 229));
                    } else {
                        float t = (gradientPos - 0.75f) / 0.25f;
                        r = 150 + (uint8_t)(t * (222 - 150));
                        g = 233 + (uint8_t)(t * (236 - 233));
                        b = 239 + (uint8_t)(t * (211 - 239));
                    }
                    
                    display.drawPixelRGB888(x, y, r, g, b);
                }
            }
        }
        
        // Wet sand animation (matches CSS wetsand keyframes)
        float wetSandOpacity = 0.2f;
        if (cyclePosition >= 0.34f && cyclePosition <= 0.35f) {
            wetSandOpacity = 0.2f + ((cyclePosition - 0.34f) / 0.01f) * 0.2f;
        } else if (cyclePosition > 0.35f) {
            wetSandOpacity = 0.4f - ((cyclePosition - 0.35f) / 0.65f) * 0.2f;
        }
        
        // Wet sand (37.5% height = 24px)
        int wetSandHeight = 24;
        for (int y = seaTop; y < seaTop + wetSandHeight && y < DISPLAY_HEIGHT; y++) {
            for (int x = max(0, seaLeft); x < min(DISPLAY_WIDTH, seaLeft + seaWidth); x++) {
                float centerX = seaLeft + seaWidth / 2.0f;
                float centerY = seaTop + wetSandHeight / 2.0f;
                float dx = (x - centerX) / (seaWidth / 2.0f);
                float dy = (y - centerY) / (wetSandHeight / 2.0f);
                
                if (dx * dx + dy * dy <= 1.0f) {
                    // Wet sand color #ecc075 blended with existing
                    uint8_t wetR = 0xec;
                    uint8_t wetG = 0xc0;
                    uint8_t wetB = 0x75;
                    uint8_t alpha = (uint8_t)(wetSandOpacity * 255);
                    AnimationUtils::drawPixelWithBlend(x, y, AnimationUtils::rgb888To565(wetR, wetG, wetB), alpha);
                }
            }
        }
        
        // Seabirds in the distance
        int birdX = (int)((time * 10) + 40) % 150 - 10;
        int birdY = 8 + (int)(sin16(time * 32768) / 65535.0f * 6);
        
        if (birdX >= 0 && birdX < DISPLAY_WIDTH && birdY >= 0 && birdY < DISPLAY_HEIGHT) {
            uint16_t birdColor = AnimationUtils::rgb888To565(80, 80, 80);
            uint8_t alpha = 153;  // 0.6 opacity
            
            // Simple bird shape (matches HTML)
            AnimationUtils::drawPixelWithBlend(birdX - 2, birdY, birdColor, alpha);
            AnimationUtils::drawPixelWithBlend(birdX, birdY - 1, birdColor, alpha);
            AnimationUtils::drawPixelWithBlend(birdX + 2, birdY, birdColor, alpha);
            AnimationUtils::drawPixelWithBlend(birdX + 1, birdY + 1, birdColor, alpha);
        }
    }
    
    const char* getName() {
        return "Beach";
    }
}