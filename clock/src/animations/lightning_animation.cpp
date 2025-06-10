#include "animation_base.h"

void Animations::initLightning(AnimationManager* manager) {
    // Initialize lightning bolts as inactive
    for (int i = 0; i < MAX_LIGHTNING; i++) {
        manager->state.lightnings[i].active = false;
        manager->state.lightnings[i].progress = 0;
        manager->state.lightnings[i].frame = 0;
        manager->state.lightnings[i].reachedGround = false;
        for (int b = 0; b < 3; b++) {
            manager->state.lightnings[i].branchLengths[b] = 0;
        }
    }
}

void Animations::renderLightning(AnimationManager* manager) {
    // Dark stormy background
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        uint8_t storm = map(y, 0, DISPLAY_HEIGHT, 34, 68);
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            manager->drawPixelWithBlend(x, y, 0, 0, storm);
        }
    }
    
    // Create new lightning if we have room and random chance
    if (random(0, 1000) < 10) {  // 1% chance per frame
        for (int i = 0; i < MAX_LIGHTNING; i++) {
            if (!manager->state.lightnings[i].active) {
                // Initialize new lightning
                manager->state.lightnings[i].active = true;
                manager->state.lightnings[i].progress = 0;
                manager->state.lightnings[i].frame = 0;
                manager->state.lightnings[i].reachedGround = false;
                
                // Generate branches
                int startX = random(10, DISPLAY_WIDTH - 10);
                int numBranches = random(1, 4);  // 1-3 branches
                
                for (int b = 0; b < numBranches && b < 3; b++) {
                    float bx = startX + random(-10, 11);
                    float by = 0;
                    int pointCount = 0;
                    
                    // Generate full path
                    while (by < DISPLAY_HEIGHT && pointCount < 64) {
                        bx += random(-2, 3);  // Horizontal zigzag
                        by += random(1, 3);   // Downward progress
                        
                        manager->state.lightnings[i].branches[b][pointCount].x = bx;
                        manager->state.lightnings[i].branches[b][pointCount].y = by;
                        pointCount++;
                    }
                    
                    // Ensure last point reaches ground
                    if (pointCount > 0 && manager->state.lightnings[i].branches[b][pointCount-1].y < DISPLAY_HEIGHT) {
                        manager->state.lightnings[i].branches[b][pointCount].x = manager->state.lightnings[i].branches[b][pointCount-1].x;
                        manager->state.lightnings[i].branches[b][pointCount].y = DISPLAY_HEIGHT;
                        pointCount++;
                    }
                    
                    manager->state.lightnings[i].branchLengths[b] = pointCount;
                }
                break;
            }
        }
    }
    
    // Update and draw active lightning
    for (int i = 0; i < MAX_LIGHTNING; i++) {
        if (!manager->state.lightnings[i].active) continue;
        
        // Calculate fade
        float fade = 1.0f;
        if (manager->state.lightnings[i].reachedGround) {
            // Slow fade after reaching ground
            fade = 1.0f - (float)(manager->state.lightnings[i].frame - 128) / 200.0f;
        } else {
            // Normal fade during propagation
            fade = 1.0f - (float)manager->state.lightnings[i].frame / 1000.0f;
        }
        fade = max(0.0f, fade);
        
        // Draw each branch
        for (int b = 0; b < 3; b++) {
            if (manager->state.lightnings[i].branchLengths[b] == 0) continue;
            
            // Draw lightning segments up to current progress
            for (int p = 0; p < manager->state.lightnings[i].branchLengths[b] - 1; p++) {
                if (manager->state.lightnings[i].branches[b][p].y > manager->state.lightnings[i].progress) break;
                if (manager->state.lightnings[i].branches[b][p+1].y > manager->state.lightnings[i].progress) break;
                
                int x1 = (int)manager->state.lightnings[i].branches[b][p].x;
                int y1 = (int)manager->state.lightnings[i].branches[b][p].y;
                int x2 = (int)manager->state.lightnings[i].branches[b][p+1].x;
                int y2 = (int)manager->state.lightnings[i].branches[b][p+1].y;
                
                // Clamp to screen bounds
                x1 = constrain(x1, 0, DISPLAY_WIDTH - 1);
                y1 = constrain(y1, 0, DISPLAY_HEIGHT - 1);
                x2 = constrain(x2, 0, DISPLAY_WIDTH - 1);
                y2 = constrain(y2, 0, DISPLAY_HEIGHT - 1);
                
                // Draw line with glow
                uint8_t alpha = (uint8_t)(255 * fade);
                
                // Core lightning
                display.drawLine(x1, y1, x2, y2, display.color565(255, 255, 255));
                
                // Add glow around core
                if (x1 > 0) display.drawLine(x1-1, y1, x2-1, y2, display.color565(255, 255, 255));
                if (x1 < DISPLAY_WIDTH-1) display.drawLine(x1+1, y1, x2+1, y2, display.color565(255, 255, 255));
            }
        }
        
        // Update progress
        if (!manager->state.lightnings[i].reachedGround) {
            manager->state.lightnings[i].progress += 0.5f;  // Propagation speed
            if (manager->state.lightnings[i].progress >= DISPLAY_HEIGHT) {
                manager->state.lightnings[i].reachedGround = true;
            }
        }
        
        manager->state.lightnings[i].frame++;
        
        // Remove lightning when faded out
        if (fade <= 0) {
            manager->state.lightnings[i].active = false;
        }
    }
    
    // Improved rain with rectangles
    for (int i = 0; i < 60; i++) {
        if (random(0, 100) < 20) {  // 20% chance
            int x = random(0, DISPLAY_WIDTH);
            int y = random(0, DISPLAY_HEIGHT - 3);
            int length = 2 + random(0, 3);  // 2-4 pixels long
            float alpha = 0.3f + (sin16(TO_SIN16(manager->state.time + x)) / 32767.0f) * 0.1f;
            uint8_t alphaVal = (uint8_t)(alpha * 255);
            
            // Draw rectangular raindrop
            for (int dy = 0; dy < length && y + dy < DISPLAY_HEIGHT; dy++) {
                manager->drawPixelWithBlend(x, y + dy, 255, 255, 255, alphaVal);
                
                // Add subtle trail
                if (random(0, 100) < 30 && dy > 0) {
                    manager->drawPixelWithBlend(x, y + dy - length, 255, 255, 255, alphaVal / 3);
                }
            }
        }
    }
} 