#include "animations_modules.h"
#include "animation_utils.h"
#include "display.h"
#include <cmath>

namespace OrbitalAnimation {
    struct Particle {
        float x, y;
        float velocityX, velocityY;
        uint8_t red, green, blue;
        float trail[200][2];
        uint8_t trailLength;
    };
    
    struct State {
        Particle particles[20];
        float gravityX = 64.0f;
        float gravityY = 32.0f;
        bool initialized = false;
    };
    
    static State state;
    
    void init() {
        // Initialize orbital particles
        for (int i = 0; i < 20; i++) {
            state.particles[i].x = random(0, DISPLAY_WIDTH);
            state.particles[i].y = random(0, DISPLAY_HEIGHT);
            state.particles[i].velocityX = random(-200, 200) / 100.0f;
            state.particles[i].velocityY = random(-200, 200) / 100.0f;
            state.particles[i].red = random(100, 255);
            state.particles[i].green = random(100, 255);
            state.particles[i].blue = random(100, 255);
            state.particles[i].trailLength = 0;
        }
        state.initialized = true;
    }
    
    void render() {
        if (!state.initialized) {
            init();
        }
        
        // Clear background
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                display.drawPixelRGB888(x, y, 0, 0, 5);
            }
        }
        
        // Draw gravity center
        AnimationUtils::drawPixelWithBlend((int)state.gravityX, (int)state.gravityY, 255, 255, 255);
        for (int r = 1; r <= 3; r++) {
            for (int angle = 0; angle < 360; angle += 30) {
                float rad = angle * M_PI / 180.0f;
                int x = state.gravityX + cos(rad) * r;
                int y = state.gravityY + sin(rad) * r;
                if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
                    AnimationUtils::drawPixelWithBlend(x, y, 200, 200, 200, 100);
                }
            }
        }
        
        // Update and draw particles
        for (int i = 0; i < 20; i++) {
            // Calculate gravity effect
            float dx = state.gravityX - state.particles[i].x;
            float dy = state.gravityY - state.particles[i].y;
            float distance = sqrt(dx * dx + dy * dy);
            if (distance > 1.0f) {
                float force = 0.5f / (distance * distance);
                state.particles[i].velocityX += (dx / distance) * force;
                state.particles[i].velocityY += (dy / distance) * force;
            }
            
            // Apply velocity damping
            state.particles[i].velocityX *= 0.99f;
            state.particles[i].velocityY *= 0.99f;
            
            // Update position
            state.particles[i].x += state.particles[i].velocityX;
            state.particles[i].y += state.particles[i].velocityY;
            
            // Boundary bounce
            if (state.particles[i].x <= 0 || state.particles[i].x >= DISPLAY_WIDTH) {
                state.particles[i].velocityX *= -0.8f;
                state.particles[i].x = constrain(state.particles[i].x, 0, DISPLAY_WIDTH - 1);
            }
            if (state.particles[i].y <= 0 || state.particles[i].y >= DISPLAY_HEIGHT) {
                state.particles[i].velocityY *= -0.8f;
                state.particles[i].y = constrain(state.particles[i].y, 0, DISPLAY_HEIGHT - 1);
            }
            
            // Store trail
            for (int t = 199; t > 0; t--) {
                state.particles[i].trail[t][0] = state.particles[i].trail[t-1][0];
                state.particles[i].trail[t][1] = state.particles[i].trail[t-1][1];
            }
            state.particles[i].trail[0][0] = state.particles[i].x;
            state.particles[i].trail[0][1] = state.particles[i].y;
            if (state.particles[i].trailLength < 200) {
                state.particles[i].trailLength++;
            }
            
            // Draw trail
            for (int t = 1; t < state.particles[i].trailLength; t++) {
                float alpha = 1.0f - (float)t / 200.0f;
                int tx = (int)state.particles[i].trail[t][0];
                int ty = (int)state.particles[i].trail[t][1];
                if (tx >= 0 && tx < DISPLAY_WIDTH && ty >= 0 && ty < DISPLAY_HEIGHT) {
                    AnimationUtils::drawPixelWithBlend(tx, ty, 
                        state.particles[i].red, 
                        state.particles[i].green, 
                        state.particles[i].blue, 
                        (uint8_t)(alpha * 255));
                }
            }
            
            // Draw particle
            if (state.particles[i].x >= 0 && state.particles[i].x < DISPLAY_WIDTH &&
                state.particles[i].y >= 0 && state.particles[i].y < DISPLAY_HEIGHT) {
                AnimationUtils::drawPixelWithBlend((int)state.particles[i].x, (int)state.particles[i].y, 
                    state.particles[i].red, state.particles[i].green, state.particles[i].blue);
            }
        }
    }
    

    
    const char* getName() {
        return "Orbital";
    }
} 