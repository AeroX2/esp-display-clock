#include "animation_base.h"

void Animations::initPipes(AnimationManager* manager) {
    // Initialize pipes
    for (int i = 0; i < MAX_PIPES; i++) {
        manager->state.pipes[i].x = DISPLAY_WIDTH / 2;
        manager->state.pipes[i].y = DISPLAY_HEIGHT / 2;
        manager->state.pipes[i].direction = random(0, 4);
        manager->state.pipes[i].red = random(100, 255);
        manager->state.pipes[i].green = random(100, 255);
        manager->state.pipes[i].blue = random(100, 255);
        manager->state.pipes[i].trailLength = 0;
        manager->state.pipes[i].moveCounter = 0;
        manager->state.pipes[i].lastDirection = manager->state.pipes[i].direction;
    }
}

void Animations::renderPipes(AnimationManager* manager) {
    // Clear screen completely like HTML
    display.fillRect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, display.color565(0, 0, 0));
    
    // Update and draw each pipe (5 pipes like HTML)
    for (int i = 0; i < 5; i++) {  // NUM_BOXES = 5 in HTML
        // Move pipe every 2 frames like HTML
        manager->state.pipes[i].moveCounter++;
        if (manager->state.pipes[i].moveCounter >= 2) {
            // Store current position in trail (shift trail forward)
            for (int t = 199; t > 0; t--) {
                manager->state.pipes[i].trail[t][0] = manager->state.pipes[i].trail[t-1][0];
                manager->state.pipes[i].trail[t][1] = manager->state.pipes[i].trail[t-1][1];
            }
            manager->state.pipes[i].trail[0][0] = manager->state.pipes[i].x;
            manager->state.pipes[i].trail[0][1] = manager->state.pipes[i].y;
            if (manager->state.pipes[i].trailLength < 200) {
                manager->state.pipes[i].trailLength++;
            }
            
            // Remember previous direction
            manager->state.pipes[i].lastDirection = manager->state.pipes[i].direction;
            
            // Move pipe based on direction
            switch (manager->state.pipes[i].direction) {
                case 0: manager->state.pipes[i].y--; break;  // Up
                case 1: manager->state.pipes[i].y++; break;  // Down
                case 2: manager->state.pipes[i].x--; break;  // Left
                case 3: manager->state.pipes[i].x++; break;  // Right
            }
            
            // Boundary check and smart direction change
            if (manager->state.pipes[i].x <= 1) {
                manager->state.pipes[i].x = 1;
                manager->state.pipes[i].direction = random(0, 4);
            } else if (manager->state.pipes[i].x >= DISPLAY_WIDTH - 2) {
                manager->state.pipes[i].x = DISPLAY_WIDTH - 2;
                manager->state.pipes[i].direction = random(0, 4);
            }
            
            if (manager->state.pipes[i].y <= 1) {
                manager->state.pipes[i].y = 1;
                manager->state.pipes[i].direction = random(0, 4);
            } else if (manager->state.pipes[i].y >= DISPLAY_HEIGHT - 2) {
                manager->state.pipes[i].y = DISPLAY_HEIGHT - 2;
                manager->state.pipes[i].direction = random(0, 4);
            }
            
            // Random direction change (2% chance like HTML)
            if (random(0, 100) < 2) {
                manager->state.pipes[i].direction = random(0, 4);
            }
            
            manager->state.pipes[i].moveCounter = 0;
        }
        
        // Draw trail with fading alpha
        for (int t = 0; t < manager->state.pipes[i].trailLength; t++) {
            float alpha = 1.0f - (float)t / 200.0f;
            uint8_t alphaVal = (uint8_t)(alpha * 255);
            
            int trailX = (int)manager->state.pipes[i].trail[t][0];
            int trailY = (int)manager->state.pipes[i].trail[t][1];
            
            // Draw 2x2 trail square (matching HTML fillRect(pos.x - 1, pos.y - 1, 2, 2))
            display.fillRect(trailX - 1, trailY - 1, 2, 2, 
                display.color565(manager->state.pipes[i].red/2, manager->state.pipes[i].green/2, manager->state.pipes[i].blue/2));
        }
        
        // Draw main 2x2 pipe box (matching HTML fillRect(box.x, box.y, 2, 2))
        display.fillRect((int)manager->state.pipes[i].x, (int)manager->state.pipes[i].y, 2, 2, 
            display.color565(manager->state.pipes[i].red, manager->state.pipes[i].green, manager->state.pipes[i].blue));
    }
} 