#include "animations_modules.h"
#include "animation_utils.h"
#include "display.h"
#include <FastLED.h>

namespace LightningAnimation {
    // Private state - completely contained within this namespace
    struct LightningBolt {
        struct Point {
            float x, y;
        };
        Point branches[3][64];  // 3 branches, max 64 points each
        uint8_t branchLengths[3];
        float progress;
        uint32_t frame;
        bool reachedGround;
        bool active;
    };
    
    struct Raindrop {
        float x, y;
        float speed;
        float length;
        float angle;
        bool active;
    };
    
    struct State {
        LightningBolt bolts[3];  // Max 3 lightning bolts
        Raindrop raindrops[60];  // Rain drops
        uint32_t frameCount = 0;
        bool initialized = false;
    };
    
    static State state;
    
    void init() {
        // Initialize lightning bolts
        for (int i = 0; i < 3; i++) {
            state.bolts[i].active = false;
            state.bolts[i].progress = 0.0f;
            state.bolts[i].frame = 0;
            state.bolts[i].reachedGround = false;
            
            for (int j = 0; j < 3; j++) {
                state.bolts[i].branchLengths[j] = 0;
            }
        }
        
        // Initialize raindrops
        for (int i = 0; i < 60; i++) {
            state.raindrops[i].active = false;
        }
        
        state.frameCount = 0;
        state.initialized = true;
    }
    
    void render() {
        if (!state.initialized) {
            init();
        }
        
        state.frameCount++;
        
        // Dark stormy background gradient (matches HTML)
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                // Gradient from #000022 to #000044
                uint8_t r = 0;
                uint8_t g = 0;
                uint8_t b = 0x22 + ((y * (0x44 - 0x22)) / DISPLAY_HEIGHT);
                display.drawPixelRGB888(x, y, r, g, b);
            }
        }
        
        // Spawn new lightning with proper spacing (matches HTML logic)
        if (random(100) == 0) {  // 1% chance per frame
            int activeLightning = 0;
            for (int i = 0; i < 3; i++) {
                if (state.bolts[i].active) activeLightning++;
            }
            
            if (activeLightning < 3) {  // Max 3 simultaneous lightning
                for (int i = 0; i < 3; i++) {
                    if (!state.bolts[i].active) {
                        // Check spacing from existing lightning
                        float startX = random(10, DISPLAY_WIDTH - 10);
                        bool validPosition = true;
                        
                        for (int j = 0; j < 3; j++) {
                            if (state.bolts[j].active && state.bolts[j].branchLengths[0] > 0) {
                                float existingX = state.bolts[j].branches[0][0].x;
                                if (abs(startX - existingX) < 30) {  // MIN_SPACING = 30
                                    validPosition = false;
                                    break;
                                }
                            }
                        }
                        
                        if (validPosition) {
                            state.bolts[i].active = true;
                            state.bolts[i].progress = 0.0f;
                            state.bolts[i].frame = 0;
                            state.bolts[i].reachedGround = false;
                            
                            // Generate complete lightning path (like HTML)
                            float bx = startX;
                            float by = 0;
                            int pointCount = 0;
                            
                            while (by < DISPLAY_HEIGHT && pointCount < 63) {
                                state.bolts[i].branches[0][pointCount] = {bx, by};
                                bx += (random(-4, 5));
                                by += random(1, 3);
                                pointCount++;
                            }
                            
                            // Ensure last point reaches ground
                            if (pointCount > 0) {
                                state.bolts[i].branches[0][pointCount-1].y = DISPLAY_HEIGHT - 1;
                            }
                            
                            state.bolts[i].branchLengths[0] = pointCount;
                            state.bolts[i].branchLengths[1] = 0;
                            state.bolts[i].branchLengths[2] = 0;
                            break;
                        }
                    }
                }
            }
        }
        
        // Update and draw active lightning bolts with slow motion (matches HTML)
        for (int i = 0; i < 3; i++) {
            if (!state.bolts[i].active) continue;
            
            LightningBolt& bolt = state.bolts[i];
            bolt.frame++;
            
            // Calculate fade based on HTML logic
            float fade = 1.0f;
            const float PROPAGATION_SPEED = 0.5f;  // Pixels per frame (slow motion)
            const float FADE_DURATION = 200.0f;    // Long fade duration
            
            if (bolt.reachedGround) {
                // Very slow fade out after reaching ground
                fade = 1.0f - ((bolt.frame - (DISPLAY_HEIGHT / PROPAGATION_SPEED)) / FADE_DURATION);
            } else {
                // Normal fade during propagation
                fade = 1.0f - (bolt.frame / 1000.0f);  // LIGHTNING_DURATION = 1000
            }
            
            fade = max(0.0f, fade);
            
            // Update progress with slow motion
            if (!bolt.reachedGround) {
                bolt.progress += PROPAGATION_SPEED;
                if (bolt.progress >= DISPLAY_HEIGHT) {
                    bolt.reachedGround = true;
                }
            }
            
            // Draw lightning up to current progress
            if (bolt.branchLengths[0] > 0) {
                for (int p = 0; p < bolt.branchLengths[0] - 1; p++) {
                    float x1 = bolt.branches[0][p].x;
                    float y1 = bolt.branches[0][p].y;
                    float x2 = bolt.branches[0][p + 1].x;
                    float y2 = bolt.branches[0][p + 1].y;
                    
                    // Only draw points up to current progress
                    if (y1 <= bolt.progress && y2 <= bolt.progress) {
                        uint8_t alpha = (uint8_t)(255 * fade);
                        
                        // Outer glow (matches HTML)
                        AnimationUtils::drawPixelWithBlend((int)x1, (int)y1, AnimationUtils::rgb888To565(255, 255, 255), alpha * 0.3f);
                        AnimationUtils::drawPixelWithBlend((int)x2, (int)y2, AnimationUtils::rgb888To565(255, 255, 255), alpha * 0.3f);
                        
                        // Inner glow
                        AnimationUtils::drawPixelWithBlend((int)x1, (int)y1, AnimationUtils::rgb888To565(255, 255, 255), alpha * 0.6f);
                        AnimationUtils::drawPixelWithBlend((int)x2, (int)y2, AnimationUtils::rgb888To565(255, 255, 255), alpha * 0.6f);
                        
                        // Core
                        AnimationUtils::drawPixelWithBlend((int)x1, (int)y1, AnimationUtils::rgb888To565(255, 255, 255), alpha);
                        AnimationUtils::drawPixelWithBlend((int)x2, (int)y2, AnimationUtils::rgb888To565(255, 255, 255), alpha);
                    }
                }
            }
            
            // Remove lightning when completely faded
            if (fade <= 0) {
                bolt.active = false;
            }
        }
        
        // Rain animation (matches HTML)
        // Spawn new raindrops
        for (int i = 0; i < 60; i++) {
            if (!state.raindrops[i].active && random(5) == 0) {  // 20% chance
                state.raindrops[i].x = random(DISPLAY_WIDTH);
                state.raindrops[i].y = random(DISPLAY_HEIGHT);
                state.raindrops[i].speed = 1.0f + (random(150) / 100.0f);  // 1 to 2.5
                state.raindrops[i].length = 2 + random(3);  // 2 to 4
                state.raindrops[i].angle = (random(40) - 20) / 100.0f;  // -0.2 to 0.2
                state.raindrops[i].active = true;
            }
        }
        
        // Update and draw raindrops
        for (int i = 0; i < 60; i++) {
            if (!state.raindrops[i].active) continue;
            
            Raindrop& drop = state.raindrops[i];
            drop.y += drop.speed;
            drop.x += drop.angle;
            
            if (drop.y > DISPLAY_HEIGHT) {
                drop.active = false;
                continue;
            }
            
            // Draw drop with transparency variation
            float alpha = 0.3f + sin16(state.frameCount * 1000 + drop.x * 1000) / 65535.0f * 0.1f;
            uint8_t alphaVal = (uint8_t)(alpha * 255);
            
            AnimationUtils::drawPixelWithBlend((int)drop.x, (int)drop.y, AnimationUtils::rgb888To565(255, 255, 255), alphaVal);
            
            // Draw length
            for (int l = 1; l < drop.length; l++) {
                if (drop.y - l >= 0) {
                    AnimationUtils::drawPixelWithBlend((int)drop.x, (int)(drop.y - l), AnimationUtils::rgb888To565(255, 255, 255), alphaVal);
                }
            }
            
            // Add subtle trail
            if (random(3) == 0 && drop.y - drop.length >= 0) {
                AnimationUtils::drawPixelWithBlend((int)drop.x, (int)(drop.y - drop.length), AnimationUtils::rgb888To565(255, 255, 255), alphaVal * 0.3f);
            }
        }
    }
    
    const char* getName() {
        return "Lightning";
    }
}