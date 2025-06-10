#include "animations_modules.h"
#include "animation_utils.h"
#include "display.h"
#include <cmath>

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
    
    struct State {
        LightningBolt bolts[3];  // Max 3 lightning bolts
        uint32_t frameCount = 0;
        float cloudY = 0.0f;
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
        
        state.frameCount = 0;
        state.cloudY = 5.0f;
        state.initialized = true;
    }
    
    void render() {
        if (!state.initialized) {
            init();
        }
        
        state.frameCount++;
        
        // Clear with dark background
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                display.drawPixelRGB888(x, y, 5, 5, 15);  // Dark stormy blue
            }
        }
        
        // Draw storm clouds at top
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            int cloudHeight = (int)(state.cloudY + 3 * sin(x * 0.3f + state.frameCount * 0.05f));
            for (int y = 0; y < cloudHeight && y < DISPLAY_HEIGHT; y++) {
                uint8_t intensity = 40 + 20 * sin(x * 0.2f + y * 0.1f);
                display.drawPixelRGB888(x, y, intensity/3, intensity/3, intensity/2);
            }
        }
        
        // Randomly spawn new lightning
        if ((state.frameCount % 120) == 0 || random(200) == 0) {
            for (int i = 0; i < 3; i++) {
                if (!state.bolts[i].active) {
                    state.bolts[i].active = true;
                    state.bolts[i].progress = 0.0f;
                    state.bolts[i].frame = 0;
                    state.bolts[i].reachedGround = false;
                    
                    // Generate main branch starting from cloud
                    float startX = random(5, DISPLAY_WIDTH - 5);
                    float startY = state.cloudY;
                    
                    state.bolts[i].branches[0][0] = {startX, startY};
                    state.bolts[i].branchLengths[0] = 1;
                    
                    // Clear other branches
                    state.bolts[i].branchLengths[1] = 0;
                    state.bolts[i].branchLengths[2] = 0;
                    break;
                }
            }
        }
        
        // Update and draw active lightning bolts
        for (int i = 0; i < 3; i++) {
            if (!state.bolts[i].active) continue;
            
            LightningBolt& bolt = state.bolts[i];
            bolt.frame++;
            
            // Grow main branch
            if (bolt.branchLengths[0] > 0 && bolt.branchLengths[0] < 63 && !bolt.reachedGround) {
                auto& lastPoint = bolt.branches[0][bolt.branchLengths[0] - 1];
                
                // Add some randomness to the path
                float newX = lastPoint.x + random(-2, 3);
                float newY = lastPoint.y + 1 + random(0, 2);
                
                // Clamp to screen bounds
                newX = constrain(newX, 0, DISPLAY_WIDTH - 1);
                
                bolt.branches[0][bolt.branchLengths[0]] = {newX, newY};
                bolt.branchLengths[0]++;
                
                if (newY >= DISPLAY_HEIGHT - 1) {
                    bolt.reachedGround = true;
                }
                
                // Randomly create side branches
                if (random(3) == 0 && bolt.branchLengths[1] == 0) {
                    bolt.branches[1][0] = {newX, newY};
                    bolt.branchLengths[1] = 1;
                }
            }
            
            // Grow side branches
            for (int branch = 1; branch < 3; branch++) {
                if (bolt.branchLengths[branch] > 0 && bolt.branchLengths[branch] < 20) {
                    auto& lastPoint = bolt.branches[branch][bolt.branchLengths[branch] - 1];
                    
                    float newX = lastPoint.x + random(-3, 4);
                    float newY = lastPoint.y + random(-1, 3);
                    
                    newX = constrain(newX, 0, DISPLAY_WIDTH - 1);
                    newY = constrain(newY, 0, DISPLAY_HEIGHT - 1);
                    
                    bolt.branches[branch][bolt.branchLengths[branch]] = {newX, newY};
                    bolt.branchLengths[branch]++;
                }
            }
            
            // Draw all branches
            for (int branch = 0; branch < 3; branch++) {
                if (bolt.branchLengths[branch] == 0) continue;
                
                for (int p = 0; p < bolt.branchLengths[branch] - 1; p++) {
                    float x1 = bolt.branches[branch][p].x;
                    float y1 = bolt.branches[branch][p].y;
                    float x2 = bolt.branches[branch][p + 1].x;
                    float y2 = bolt.branches[branch][p + 1].y;
                    
                    // Draw lightning line with glow
                    uint8_t intensity = 255 - (p * 3);  // Fade with distance
                    
                    // Core lightning
                    AnimationUtils::drawPixelWithBlend((int)x1, (int)y1, 255, 255, 255, intensity);
                    AnimationUtils::drawPixelWithBlend((int)x2, (int)y2, 255, 255, 255, intensity);
                    
                    // Glow around lightning
                    for (int gx = -1; gx <= 1; gx++) {
                        for (int gy = -1; gy <= 1; gy++) {
                            if (gx == 0 && gy == 0) continue;
                            AnimationUtils::drawPixelWithBlend((int)x1 + gx, (int)y1 + gy, 
                                                             200, 200, 255, intensity/3);
                        }
                    }
                }
            }
            
            // Remove lightning after some time
            if (bolt.frame > 15 || (bolt.reachedGround && bolt.frame > 8)) {
                bolt.active = false;
            }
        }
        
        // Add some random flicker to enhance the storm effect
        if (random(10) == 0) {
            for (int i = 0; i < 5; i++) {
                int x = random(DISPLAY_WIDTH);
                int y = random(DISPLAY_HEIGHT);
                AnimationUtils::drawPixelWithBlend(x, y, 255, 255, 200, 100);
            }
        }
    }
    

    
    const char* getName() {
        return "Lightning";
    }
} 