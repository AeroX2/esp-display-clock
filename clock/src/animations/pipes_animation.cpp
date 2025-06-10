#include "animations_modules.h"
#include "animation_utils.h"
#include "display.h"
#include <cmath>

namespace PipesAnimation {
    struct Pipe {
        float x, y;
        uint8_t direction;
        uint8_t red, green, blue;
        float trail[200][2];
        uint8_t trailLength;
        uint8_t moveCounter;
        uint8_t lastDirection;
    };
    
    struct State {
        Pipe pipes[5];
        bool initialized = false;
    };
    
    static State state;
    
    void init() {
        for (int i = 0; i < 5; i++) {
            state.pipes[i].x = DISPLAY_WIDTH / 2;
            state.pipes[i].y = DISPLAY_HEIGHT / 2;
            state.pipes[i].direction = random(0, 4);
            state.pipes[i].red = random(100, 255);
            state.pipes[i].green = random(100, 255);
            state.pipes[i].blue = random(100, 255);
            state.pipes[i].trailLength = 0;
            state.pipes[i].moveCounter = 0;
            state.pipes[i].lastDirection = state.pipes[i].direction;
        }
        state.initialized = true;
    }
    
    void render() {
        if (!state.initialized) {
            init();
        }
        
        // Clear screen
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                display.drawPixelRGB888(x, y, 0, 0, 0);
            }
        }
        
        // Update and draw each pipe
        for (int i = 0; i < 5; i++) {
            // Move pipe every 2 frames
            state.pipes[i].moveCounter++;
            if (state.pipes[i].moveCounter >= 2) {
                // Store current position in trail
                for (int t = 199; t > 0; t--) {
                    state.pipes[i].trail[t][0] = state.pipes[i].trail[t-1][0];
                    state.pipes[i].trail[t][1] = state.pipes[i].trail[t-1][1];
                }
                state.pipes[i].trail[0][0] = state.pipes[i].x;
                state.pipes[i].trail[0][1] = state.pipes[i].y;
                if (state.pipes[i].trailLength < 200) {
                    state.pipes[i].trailLength++;
                }
                
                state.pipes[i].lastDirection = state.pipes[i].direction;
                
                // Move pipe based on direction
                switch (state.pipes[i].direction) {
                    case 0: state.pipes[i].y--; break;  // Up
                    case 1: state.pipes[i].y++; break;  // Down
                    case 2: state.pipes[i].x--; break;  // Left
                    case 3: state.pipes[i].x++; break;  // Right
                }
                
                // Boundary check and direction change
                if (state.pipes[i].x <= 1) {
                    state.pipes[i].x = 1;
                    state.pipes[i].direction = random(0, 4);
                } else if (state.pipes[i].x >= DISPLAY_WIDTH - 2) {
                    state.pipes[i].x = DISPLAY_WIDTH - 2;
                    state.pipes[i].direction = random(0, 4);
                }
                
                if (state.pipes[i].y <= 1) {
                    state.pipes[i].y = 1;
                    state.pipes[i].direction = random(0, 4);
                } else if (state.pipes[i].y >= DISPLAY_HEIGHT - 2) {
                    state.pipes[i].y = DISPLAY_HEIGHT - 2;
                    state.pipes[i].direction = random(0, 4);
                }
                
                // Random direction change (2% chance)
                if (random(0, 100) < 2) {
                    state.pipes[i].direction = random(0, 4);
                }
                
                state.pipes[i].moveCounter = 0;
            }
            
            // Draw trail with fading
            for (int t = 0; t < state.pipes[i].trailLength; t++) {
                float alpha = 1.0f - (float)t / 200.0f;
                int trailX = (int)state.pipes[i].trail[t][0];
                int trailY = (int)state.pipes[i].trail[t][1];
                
                // Draw 2x2 trail square
                for (int dx = 0; dx < 2; dx++) {
                    for (int dy = 0; dy < 2; dy++) {
                        if (trailX - 1 + dx >= 0 && trailX - 1 + dx < DISPLAY_WIDTH &&
                            trailY - 1 + dy >= 0 && trailY - 1 + dy < DISPLAY_HEIGHT) {
                            display.drawPixelRGB888(trailX - 1 + dx, trailY - 1 + dy, 
                                state.pipes[i].red/2, state.pipes[i].green/2, state.pipes[i].blue/2);
                        }
                    }
                }
            }
            
            // Draw main 2x2 pipe box
            for (int dx = 0; dx < 2; dx++) {
                for (int dy = 0; dy < 2; dy++) {
                    if ((int)state.pipes[i].x + dx >= 0 && (int)state.pipes[i].x + dx < DISPLAY_WIDTH &&
                        (int)state.pipes[i].y + dy >= 0 && (int)state.pipes[i].y + dy < DISPLAY_HEIGHT) {
                        display.drawPixelRGB888((int)state.pipes[i].x + dx, (int)state.pipes[i].y + dy, 
                            state.pipes[i].red, state.pipes[i].green, state.pipes[i].blue);
                    }
                }
            }
        }
    }
    

    
    const char* getName() {
        return "Pipes";
    }
} 