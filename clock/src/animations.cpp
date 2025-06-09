#include "animations.h"
#include <math.h>

// Global instance
AnimationManager animationManager;

AnimationManager::AnimationManager() :
    currentAnimation(ANIM_PLASMA),
    nextAnimation(ANIM_PARTICLES),
    lastCycleTime(0),
    fadeStartTime(0),
    inFade(false),
    fadeProgress(0),
    autoMode(true) {  // Start in auto mode
    memset(&state, 0, sizeof(state));
}

void AnimationManager::begin() {
    lastCycleTime = millis();
    initializeAnimation(currentAnimation);
}

void AnimationManager::update() {
    uint32_t now = millis();
    state.frameCount++;
    state.time = now * 0.001f;  // Convert to seconds
    
    // Check if it's time to cycle to next animation (only in auto mode)
    if (autoMode && now - lastCycleTime >= ANIMATION_CYCLE_DURATION && !inFade) {
        // Start fade transition
        inFade = true;
        fadeStartTime = now;
        nextAnimation = (AnimationType)((currentAnimation + 1) % ANIM_COUNT);
        initializeAnimation(nextAnimation);
    }
    
    // Handle fade transition
    if (inFade) {
        uint32_t fadeElapsed = now - fadeStartTime;
        if (fadeElapsed >= FADE_DURATION) {
            // Fade complete
            inFade = false;
            currentAnimation = nextAnimation;
            fadeProgress = 0;
            lastCycleTime = now;
        } else {
            // Calculate fade progress (0-255)
            fadeProgress = map(fadeElapsed, 0, FADE_DURATION, 0, 255);
        }
    }
    
    // Clear the display buffer
    clearBackground();
    
    // Render current animation
    switch (currentAnimation) {
        case ANIM_PLASMA: renderPlasma(); break;
        case ANIM_PARTICLES: renderParticles(); break;
        case ANIM_FIRE: renderFire(); break;
        case ANIM_SKYLINE: renderSkyline(); break;
        case ANIM_GALAXY: renderGalaxy(); break;
        case ANIM_LIGHTNING: renderLightning(); break;
        case ANIM_PIPES: renderPipes(); break;
        case ANIM_ORBITAL: renderOrbital(); break;
        case ANIM_STARS: renderStars(); break;
        case ANIM_BEACH: renderBeach(); break;
    }
    
    // If fading, blend with next animation
    if (inFade) {
        // Store current frame
        // Render next animation with fade
        switch (nextAnimation) {
            case ANIM_PLASMA: renderPlasma(); break;
            case ANIM_PARTICLES: renderParticles(); break;
            case ANIM_FIRE: renderFire(); break;
            case ANIM_SKYLINE: renderSkyline(); break;
            case ANIM_GALAXY: renderGalaxy(); break;
            case ANIM_LIGHTNING: renderLightning(); break;
            case ANIM_PIPES: renderPipes(); break;
            case ANIM_ORBITAL: renderOrbital(); break;
            case ANIM_STARS: renderStars(); break;
            case ANIM_BEACH: renderBeach(); break;
        }
        applyFade(fadeProgress);
    }
}

void AnimationManager::initializeAnimation(AnimationType type) {
    state.frameCount = 0;
    state.initialized = false;
    
    switch (type) {
        case ANIM_PARTICLES:
            // Initialize particles
            for (int i = 0; i < MAX_PARTICLES; i++) {
                state.particles[i].x = random(0, DISPLAY_WIDTH);
                state.particles[i].y = random(0, DISPLAY_HEIGHT);
                state.particles[i].velocityX = (random(100, 300) / 100.0f) * (random(0, 2) ? 1 : -1);
                state.particles[i].velocityY = (random(50, 150) / 100.0f) * (random(0, 2) ? 1 : -1);
                state.particles[i].red = random(100, 255);
                state.particles[i].green = random(100, 255);
                state.particles[i].blue = random(100, 255);
                state.particles[i].trailLength = 0;
                state.particles[i].active = true;
            }
            break;
            
        case ANIM_SKYLINE:
            // Initialize buildings
            for (int layer = 0; layer < 3; layer++) {
                int buildingCount = 8 + layer * 2;
                float x = 0;
                for (int i = 0; i < buildingCount; i++) {
                    int idx = layer * 15 + i;
                    if (idx >= 50) break;
                    
                    state.buildings[idx].width = random(8, 20);
                    state.buildings[idx].height = random(10, 45);
                    state.buildings[idx].x = x;
                    state.buildings[idx].y = DISPLAY_HEIGHT - state.buildings[idx].height;
                    state.buildings[idx].slantedTop = random(0, 5) == 0;
                    state.buildings[idx].spireTop = random(0, 14) == 0;
                    state.buildings[idx].antennaTop = random(0, 10) == 0;
                    
                    // Color based on layer
                    uint8_t gray = 60 + layer * 20;
                    state.buildings[idx].color = color565(gray, gray, gray);
                    
                    x += state.buildings[idx].width + random(2, 8);
                }
            }
            break;
            
        case ANIM_STARS:
            // Initialize stars
            for (int i = 0; i < MAX_STARS; i++) {
                state.stars[i].x = (i * 17) % DISPLAY_WIDTH;
                state.stars[i].y = (i * 11) % DISPLAY_HEIGHT;
                state.stars[i].size = 1.0f + (random(0, 100) / 100.0f);
                state.stars[i].brightness = random(50, 100) / 100.0f;
                state.stars[i].twinkleSpeed = 0.02f + (random(0, 30) / 1000.0f);
                state.stars[i].phase = random(0, 628) / 100.0f;  // 0 to 2*PI
            }
            break;
            
        case ANIM_PIPES:
            // Initialize pipes
            for (int i = 0; i < MAX_PIPES; i++) {
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
            break;
            
        case ANIM_ORBITAL:
            // Initialize orbital particles
            state.gravityPoint.x = DISPLAY_WIDTH / 2;
            state.gravityPoint.y = DISPLAY_HEIGHT / 2;
            state.gravityPoint.velocityX = 0.2f;
            state.gravityPoint.velocityY = 0.15f;
            
            for (int i = 0; i < MAX_ORBITAL_PARTICLES; i++) {
                float angle = random(0, 628) / 100.0f;  // Random angle
                float distance = 5 + random(0, 40);
                state.orbitalParticles[i].x = state.gravityPoint.x + cos(angle) * distance;
                state.orbitalParticles[i].y = state.gravityPoint.y + sin(angle) * distance;
                
                float speed = 0.05f + random(0, 15) / 100.0f;
                state.orbitalParticles[i].velocityX = cos(angle + PI/2) * speed;
                state.orbitalParticles[i].velocityY = sin(angle + PI/2) * speed;
                
                state.orbitalParticles[i].red = random(100, 255);
                state.orbitalParticles[i].green = random(100, 255);
                state.orbitalParticles[i].blue = random(100, 255);
                state.orbitalParticles[i].trailLength = 0;
            }
            break;
            
        case ANIM_BEACH:
            state.waveScale = 1.0f;
            state.cyclePosition = 0.0f;
            state.treePos.x = 120;
            state.treePos.y = 30;
            break;
    }
    
    state.initialized = true;
}

void AnimationManager::renderPlasma() {
    float time = state.time * TIME_SCALE;
    
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            float v1 = sin(x * PLASMA_SCALE + time);
            float v2 = sin(y * PLASMA_SCALE + time * 1.2f);
            float v3 = sin((x + y) * 0.04f + time * 0.8f);
            float plasma = (v1 + v2 + v3) / 3.0f;
            
            float hue = 280 + plasma * 80;
            float saturation = 0.7f + plasma * 0.3f;
            float lightness = 0.4f + plasma * 0.3f;
            
            uint8_t r, g, b;
            hslToRgb(fmod(hue, 360.0f) / 360.0f, saturation, lightness, &r, &g, &b);
            drawPixelWithBlend(x, y, r, g, b);
        }
    }
}

void AnimationManager::renderParticles() {
    // Fade background
    applyFade(248);  // Slight fade for trails
    
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!state.particles[i].active) continue;
        
        // Update position
        state.particles[i].x += state.particles[i].velocityX * 0.3f;
        state.particles[i].y += state.particles[i].velocityY * 0.02f * sin(state.time * 0.02f + i);
        
        // Wrap around screen
        if (state.particles[i].x > DISPLAY_WIDTH + 6) {
            state.particles[i].x = -6;
        }
        if (state.particles[i].y < -6 || state.particles[i].y > DISPLAY_HEIGHT + 6) {
            state.particles[i].y = DISPLAY_HEIGHT / 2 + random(-20, 20);
        }
        
        // Draw particle
        int x = (int)state.particles[i].x;
        int y = (int)state.particles[i].y;
        
        if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
            drawPixelWithBlend(x, y, state.particles[i].red, state.particles[i].green, state.particles[i].blue);
            
            // Draw trail
            int trailX = x - 8;
            if (trailX >= 0 && trailX < DISPLAY_WIDTH) {
                drawPixelWithBlend(trailX, y, 
                    state.particles[i].red, 
                    state.particles[i].green, 
                    state.particles[i].blue, 76);  // 30% alpha
            }
        }
    }
}

void AnimationManager::renderFire() {
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            float noise = sin(x * 0.08f + state.time * 0.04f) * cos(y * 0.12f + state.time * 0.03f);
            float flame = max(0.0f, (y + noise * 8) / DISPLAY_HEIGHT);
            
            if (flame > 0.1f) {
                uint8_t r = min(255, (int)(flame * 240));
                uint8_t g = min(200, max(0, (int)((flame - 0.3f) * 350)));
                uint8_t b = max(0, (int)((flame - 0.7f) * 600));
                drawPixelWithBlend(x, y, r, g, b);
            }
        }
    }
}

void AnimationManager::renderSkyline() {
    // Draw sky gradient
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        uint8_t blue = map(y, 0, DISPLAY_HEIGHT, 51, 102);  // #000033 to #000066
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            drawPixelWithBlend(x, y, 0, 0, blue);
        }
    }
    
    // Draw stars
    for (int i = 0; i < 50; i++) {
        int x = (i * 13) % DISPLAY_WIDTH;
        int y = (i * 7) % 40;
        float brightness = sin(state.time * 0.1f + i) * 0.5f + 0.5f;
        uint8_t starBright = (uint8_t)(brightness * 255);
        drawPixelWithBlend(x, y, starBright, starBright, starBright);
    }
    
    // Draw buildings (simplified for memory constraints)
    for (int layer = 0; layer < 3; layer++) {
        float speed = 0.02f + layer * 0.04f;
        uint8_t gray = 60 + layer * 20;
        
        for (int i = 0; i < 10; i++) {
            float x = fmod(i * 15 - state.time * speed * 100, DISPLAY_WIDTH + 20) - 10;
            int height = 10 + (i + layer) * 3;
            int width = 6 + layer * 2;
            
            // Draw simple rectangle building
            for (int bx = 0; bx < width && x + bx < DISPLAY_WIDTH; bx++) {
                for (int by = 0; by < height && DISPLAY_HEIGHT - by >= 0; by++) {
                    if (x + bx >= 0) {
                        drawPixelWithBlend((int)(x + bx), DISPLAY_HEIGHT - by - 1, gray, gray, gray);
                    }
                }
            }
        }
    }
}

void AnimationManager::renderGalaxy() {
    // Dark background
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            float distance = sqrt((x - 64) * (x - 64) + (y - 32) * (y - 32));
            uint8_t bg = max(0, 51 - (int)(distance * 0.5f));
            drawPixelWithBlend(x, y, bg/3, bg/3, bg);
        }
    }
    
    // Draw spiral arms
    float centerX = DISPLAY_WIDTH / 2;
    float centerY = DISPLAY_HEIGHT / 2;
    float time = state.time * 0.01f;
    
    for (int layer = 0; layer < 2; layer++) {
        for (int arm = 0; arm < 2; arm++) {
            float startAngle = arm * PI + layer * PI / 4;
            float rotationSpeed = 0.8f + layer * 0.2f;
            
            for (float angle = 0; angle < PI * 4; angle += 0.2f) {
                float radius = angle * 2 * (0.8f + layer * 0.2f);
                float x = centerX + cos(angle + time * rotationSpeed + startAngle) * radius;
                float y = centerY + sin(angle + time * rotationSpeed + startAngle) * radius;
                
                if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
                    float hue = fmod(layer * 45 + time * 30, 360);
                    uint8_t r, g, b;
                    hslToRgb(hue / 360.0f, 1.0f, 0.6f, &r, &g, &b);
                    drawPixelWithBlend((int)x, (int)y, r, g, b, 153);  // 60% alpha
                }
            }
        }
    }
}

void AnimationManager::renderLightning() {
    // Dark stormy background
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        uint8_t storm = map(y, 0, DISPLAY_HEIGHT, 34, 68);
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            drawPixelWithBlend(x, y, 0, 0, storm);
        }
    }
    
    // Simple lightning simulation
    if (random(0, 100) < 2) {  // 2% chance per frame
        int x = random(10, DISPLAY_WIDTH - 10);
        // Draw vertical lightning bolt
        for (int y = 0; y < DISPLAY_HEIGHT; y += 3) {
            int boltX = x + random(-2, 3);
            if (boltX >= 0 && boltX < DISPLAY_WIDTH) {
                drawPixelWithBlend(boltX, y, 255, 255, 255);
                if (boltX + 1 < DISPLAY_WIDTH) {
                    drawPixelWithBlend(boltX + 1, y, 255, 255, 255, 128);
                }
                if (boltX - 1 >= 0) {
                    drawPixelWithBlend(boltX - 1, y, 255, 255, 255, 128);
                }
            }
        }
    }
    
    // Simple rain
    for (int i = 0; i < 60; i++) {
        if (random(0, 5) == 0) {
            int x = random(0, DISPLAY_WIDTH);
            int y = random(0, DISPLAY_HEIGHT);
            drawPixelWithBlend(x, y, 255, 255, 255, 76);  // 30% alpha
        }
    }
}

void AnimationManager::renderPipes() {
    // Clear with trails
    applyFade(240);
    
    for (int i = 0; i < MAX_PIPES; i++) {
        // Update pipe position
        state.pipes[i].moveCounter++;
        if (state.pipes[i].moveCounter >= 2) {
            // Store trail
            if (state.pipes[i].trailLength < 200) {
                state.pipes[i].trail[state.pipes[i].trailLength][0] = state.pipes[i].x;
                state.pipes[i].trail[state.pipes[i].trailLength][1] = state.pipes[i].y;
                state.pipes[i].trailLength++;
            } else {
                // Shift trail
                for (int t = 0; t < 199; t++) {
                    state.pipes[i].trail[t][0] = state.pipes[i].trail[t + 1][0];
                    state.pipes[i].trail[t][1] = state.pipes[i].trail[t + 1][1];
                }
                state.pipes[i].trail[199][0] = state.pipes[i].x;
                state.pipes[i].trail[199][1] = state.pipes[i].y;
            }
            
            // Move pipe
            switch (state.pipes[i].direction) {
                case 0: state.pipes[i].y--; break;  // Up
                case 1: state.pipes[i].y++; break;  // Down  
                case 2: state.pipes[i].x--; break;  // Left
                case 3: state.pipes[i].x++; break;  // Right
            }
            
            // Boundary check and direction change
            if (state.pipes[i].x <= 1 || state.pipes[i].x >= DISPLAY_WIDTH - 2 ||
                state.pipes[i].y <= 1 || state.pipes[i].y >= DISPLAY_HEIGHT - 2) {
                state.pipes[i].direction = random(0, 4);
                state.pipes[i].x = constrain(state.pipes[i].x, 1, DISPLAY_WIDTH - 2);
                state.pipes[i].y = constrain(state.pipes[i].y, 1, DISPLAY_HEIGHT - 2);
            }
            
            // Random direction change
            if (random(0, 100) < 2) {
                state.pipes[i].direction = random(0, 4);
            }
            
            state.pipes[i].moveCounter = 0;
        }
        
        // Draw trail
        for (int t = 0; t < state.pipes[i].trailLength; t++) {
            float alpha = 1.0f - (float)t / 200.0f;
            uint8_t alphaVal = (uint8_t)(alpha * 128);
            drawPixelWithBlend(
                (int)state.pipes[i].trail[t][0], 
                (int)state.pipes[i].trail[t][1],
                state.pipes[i].red / 2, 
                state.pipes[i].green / 2, 
                state.pipes[i].blue / 2, 
                alphaVal);
        }
        
        // Draw main pipe
        drawPixelWithBlend((int)state.pipes[i].x, (int)state.pipes[i].y, 
            state.pipes[i].red, state.pipes[i].green, state.pipes[i].blue);
        drawPixelWithBlend((int)state.pipes[i].x + 1, (int)state.pipes[i].y, 
            state.pipes[i].red, state.pipes[i].green, state.pipes[i].blue);
        drawPixelWithBlend((int)state.pipes[i].x, (int)state.pipes[i].y + 1, 
            state.pipes[i].red, state.pipes[i].green, state.pipes[i].blue);
        drawPixelWithBlend((int)state.pipes[i].x + 1, (int)state.pipes[i].y + 1, 
            state.pipes[i].red, state.pipes[i].green, state.pipes[i].blue);
    }
}

void AnimationManager::renderOrbital() {
    // Clear with trails
    applyFade(250);
    
    // Update gravity point
    state.gravityPoint.x += state.gravityPoint.velocityX;
    state.gravityPoint.y += state.gravityPoint.velocityY;
    
    // Bounce gravity point off edges
    if (state.gravityPoint.x <= 0 || state.gravityPoint.x >= DISPLAY_WIDTH - 1) {
        state.gravityPoint.velocityX *= -1;
        state.gravityPoint.x = constrain(state.gravityPoint.x, 0, DISPLAY_WIDTH - 1);
    }
    if (state.gravityPoint.y <= 0 || state.gravityPoint.y >= DISPLAY_HEIGHT - 1) {
        state.gravityPoint.velocityY *= -1;
        state.gravityPoint.y = constrain(state.gravityPoint.y, 0, DISPLAY_HEIGHT - 1);
    }
    
    // Update particles
    for (int i = 0; i < MAX_ORBITAL_PARTICLES; i++) {
        // Store trail
        if (state.orbitalParticles[i].trailLength < 200) {
            state.orbitalParticles[i].trail[state.orbitalParticles[i].trailLength][0] = state.orbitalParticles[i].x;
            state.orbitalParticles[i].trail[state.orbitalParticles[i].trailLength][1] = state.orbitalParticles[i].y;
            state.orbitalParticles[i].trailLength++;
        } else {
            // Shift trail
            for (int t = 0; t < 199; t++) {
                state.orbitalParticles[i].trail[t][0] = state.orbitalParticles[i].trail[t + 1][0];
                state.orbitalParticles[i].trail[t][1] = state.orbitalParticles[i].trail[t + 1][1];
            }
            state.orbitalParticles[i].trail[199][0] = state.orbitalParticles[i].x;
            state.orbitalParticles[i].trail[199][1] = state.orbitalParticles[i].y;
        }
        
        // Apply gravity
        float dx = state.gravityPoint.x - state.orbitalParticles[i].x;
        float dy = state.gravityPoint.y - state.orbitalParticles[i].y;
        float distance = sqrt(dx * dx + dy * dy);
        
        if (distance > 0) {
            float force = 0.05f / (distance * distance);
            state.orbitalParticles[i].velocityX += (dx / distance) * force;
            state.orbitalParticles[i].velocityY += (dy / distance) * force;
        }
        
        // Update position
        state.orbitalParticles[i].x += state.orbitalParticles[i].velocityX;
        state.orbitalParticles[i].y += state.orbitalParticles[i].velocityY;
        
        // Wrap around screen
        if (state.orbitalParticles[i].x < 0) state.orbitalParticles[i].x = DISPLAY_WIDTH - 1;
        if (state.orbitalParticles[i].x >= DISPLAY_WIDTH) state.orbitalParticles[i].x = 0;
        if (state.orbitalParticles[i].y < 0) state.orbitalParticles[i].y = DISPLAY_HEIGHT - 1;
        if (state.orbitalParticles[i].y >= DISPLAY_HEIGHT) state.orbitalParticles[i].y = 0;
        
        // Limit speed
        float speed = sqrt(state.orbitalParticles[i].velocityX * state.orbitalParticles[i].velocityX + 
                          state.orbitalParticles[i].velocityY * state.orbitalParticles[i].velocityY);
        if (speed > 0.2f) {
            state.orbitalParticles[i].velocityX = (state.orbitalParticles[i].velocityX / speed) * 0.2f;
            state.orbitalParticles[i].velocityY = (state.orbitalParticles[i].velocityY / speed) * 0.2f;
        }
        
        // Draw trail
        for (int t = 0; t < state.orbitalParticles[i].trailLength; t++) {
            float alpha = 1.0f - (float)t / 200.0f;
            uint8_t alphaVal = (uint8_t)(alpha * 128);
            drawPixelWithBlend(
                (int)state.orbitalParticles[i].trail[t][0], 
                (int)state.orbitalParticles[i].trail[t][1],
                state.orbitalParticles[i].red / 2, 
                state.orbitalParticles[i].green / 2, 
                state.orbitalParticles[i].blue / 2, 
                alphaVal);
        }
        
        // Draw particle
        drawPixelWithBlend((int)state.orbitalParticles[i].x, (int)state.orbitalParticles[i].y,
            state.orbitalParticles[i].red, state.orbitalParticles[i].green, state.orbitalParticles[i].blue);
    }
    
    // Draw gravity point
    drawPixelWithBlend((int)state.gravityPoint.x, (int)state.gravityPoint.y, 255, 255, 255, 128);
}

void AnimationManager::renderStars() {
    // Dark blue background
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            drawPixelWithBlend(x, y, 0, 0, 51);  // #000033
        }
    }
    
    // Draw twinkling stars
    for (int i = 0; i < MAX_STARS; i++) {
        state.stars[i].phase += state.stars[i].twinkleSpeed;
        float brightness = (sin(state.stars[i].phase) + 1.0f) / 2.0f;
        
        uint8_t starBright = (uint8_t)(brightness * 255 * 0.8f);
        
        // Draw star with glow
        int x = (int)state.stars[i].x;
        int y = (int)state.stars[i].y;
        
        drawPixelWithBlend(x, y, starBright, starBright, starBright);
        
        // Small glow around bright stars
        if (brightness > 0.7f) {
            if (x > 0) drawPixelWithBlend(x - 1, y, starBright/4, starBright/4, starBright/4);
            if (x < DISPLAY_WIDTH - 1) drawPixelWithBlend(x + 1, y, starBright/4, starBright/4, starBright/4);
            if (y > 0) drawPixelWithBlend(x, y - 1, starBright/4, starBright/4, starBright/4);
            if (y < DISPLAY_HEIGHT - 1) drawPixelWithBlend(x, y + 1, starBright/4, starBright/4, starBright/4);
        }
    }
}

void AnimationManager::renderBeach() {
    float time = state.time * 0.005f;
    
    // Update wave animation
    state.cyclePosition = fmod(time * 0.1f, 1.0f);
    if (state.cyclePosition <= 0.35f) {
        state.waveScale = 1.0f + (state.cyclePosition / 0.35f) * 0.3f;
    } else if (state.cyclePosition <= 0.69f) {
        state.waveScale = 1.3f - ((state.cyclePosition - 0.35f) / (0.69f - 0.35f)) * 0.3f;
    } else {
        state.waveScale = 1.0f;
    }
    
    // Sky gradient (40% of height)
    for (int y = 0; y < 26; y++) {  // 40% of 64
        uint8_t blue1 = 3;   // #037ccb deep blue component
        uint8_t blue2 = 130; // #82ccef light blue component
        uint8_t blue = map(y, 0, 25, blue1, blue2);
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            drawPixelWithBlend(x, y, blue/3, blue*2/3, blue);
        }
    }
    
    // Dry sand background
    for (int y = 42; y < DISPLAY_HEIGHT; y++) {  // 65% to bottom
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            drawPixelWithBlend(x, y, 253, 241, 215);  // #fdf1d7
        }
    }
    
    // Sea with animation
    int seaTop = 26;  // 40% of 64
    int seaHeight = (int)(19.2f * state.waveScale);  // 30% of 64, scaled
    
    for (int y = seaTop; y < seaTop + seaHeight && y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            // Sea gradient
            float gradient = (float)(y - seaTop) / seaHeight;
            uint8_t r, g, b;
            if (gradient < 0.25f) {
                float t = gradient / 0.25f;
                r = (uint8_t)(8 + t * (18 - 8));
                g = (uint8_t)(122 + t * (156 - 122));
                b = (uint8_t)(193 + t * (192 - 193));
            } else if (gradient < 0.5f) {
                float t = (gradient - 0.25f) / 0.25f;
                r = (uint8_t)(18 + t * (42 - 18));
                g = (uint8_t)(156 + t * (212 - 156));
                b = (uint8_t)(192 + t * (229 - 192));
            } else if (gradient < 0.75f) {
                float t = (gradient - 0.5f) / 0.25f;
                r = (uint8_t)(42 + t * (150 - 42));
                g = (uint8_t)(212 + t * (233 - 212));
                b = (uint8_t)(229 + t * (239 - 229));
            } else {
                float t = (gradient - 0.75f) / 0.25f;
                r = (uint8_t)(150 + t * (222 - 150));
                g = (uint8_t)(233 + t * (236 - 233));
                b = (uint8_t)(239 + t * (211 - 239));
            }
            drawPixelWithBlend(x, y, r, g, b);
        }
    }
    
    // Wet sand animation
    float wetSandOpacity = 0.2f;
    if (state.cyclePosition >= 0.34f && state.cyclePosition <= 0.35f) {
        wetSandOpacity = 0.2f + ((state.cyclePosition - 0.34f) / 0.01f) * 0.2f;  // 0.2 to 0.4
    } else if (state.cyclePosition > 0.35f) {
        wetSandOpacity = 0.4f - ((state.cyclePosition - 0.35f) / 0.65f) * 0.2f;  // 0.4 back to 0.2
    }
    
    // Draw wet sand with opacity
    for (int y = seaTop; y < seaTop + 24 && y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            uint8_t alpha = (uint8_t)(wetSandOpacity * 255);
            drawPixelWithBlend(x, y, 236, 192, 117, alpha);  // #ecc075
        }
    }
    
    // Seabird (render before tree)
    int birdX = (int)(fmod(time * 10, 150)) - 10;
    int birdY = 8 + (int)(sin(time * 0.5f) * 3);
    
    if (birdX >= 0 && birdX < DISPLAY_WIDTH - 4 && birdY >= 0 && birdY < DISPLAY_HEIGHT) {
        // Simple bird shape
        drawPixelWithBlend(birdX, birdY - 1, 80, 80, 80);
        drawPixelWithBlend(birdX - 2, birdY, 80, 80, 80);
        drawPixelWithBlend(birdX + 2, birdY, 80, 80, 80);
        drawPixelWithBlend(birdX + 1, birdY + 1, 80, 80, 80);
    }
    
    // Palm tree
    int treeX = (int)state.treePos.x;
    int treeY = (int)state.treePos.y;
    
    // Palm trunk - curved
    for (int y = 0; y < 52; y++) {  // Height of trunk
        int x = treeX - (int)((y * y) / 50.0f);  // Curve calculation
        if (x >= 0 && x < DISPLAY_WIDTH && treeY - 32 + y >= 0 && treeY - 32 + y < DISPLAY_HEIGHT) {
            // Draw trunk width
            for (int w = 0; w < 6; w++) {
                if (x + w - 3 >= 0 && x + w - 3 < DISPLAY_WIDTH) {
                    drawPixelWithBlend(x + w - 3, treeY - 32 + y, 170, 131, 102);  // #aa8366
                }
            }
        }
    }
    
    // Palm leaves (simplified triangular shapes)
    int leafColors[3][3] = {{57, 77, 0}, {70, 115, 1}, {70, 90, 5}};  // RGB values
    int leafX = treeX - 8;  // Top of curved trunk
    int leafY = treeY - 32;
    
    // Draw 3 palm leaves
    for (int leaf = 0; leaf < 3; leaf++) {
        int startX = leafX + leaf * 8 - 8;
        int startY = leafY + leaf * 6 + 6;
        
        // Draw triangular leaf
        for (int i = 0; i < 15; i++) {  // Leaf length
            int width = max(1, 4 - i / 4);  // Tapering width
            for (int w = 0; w < width; w++) {
                int px = startX + i - leaf * 3;
                int py = startY + w - width / 2;
                if (px >= 0 && px < DISPLAY_WIDTH && py >= 0 && py < DISPLAY_HEIGHT) {
                    drawPixelWithBlend(px, py, leafColors[leaf][0], leafColors[leaf][1], leafColors[leaf][2]);
                }
            }
        }
    }
    
    // Wave ripples
    for (int i = 0; i < 32; i++) {
        int x = i * 4;
        int waveY = seaTop + (int)(sin(time * 3 + i * 0.3f) * 1.5f);
        float rippleOpacity = sin(time * 2 + i * 0.2f) * 0.3f + 0.3f;
        uint8_t alpha = (uint8_t)(rippleOpacity * 255);
        if (x < DISPLAY_WIDTH && waveY >= 0 && waveY < DISPLAY_HEIGHT) {
            drawPixelWithBlend(x, waveY, 255, 255, 255, alpha);
            if (x + 1 < DISPLAY_WIDTH) {
                drawPixelWithBlend(x + 1, waveY, 255, 255, 255, alpha);
            }
        }
    }
}

// Helper functions
void AnimationManager::hslToRgb(float h, float s, float l, uint8_t* r, uint8_t* g, uint8_t* b) {
    if (s == 0) {
        *r = *g = *b = (uint8_t)(l * 255);
    } else {
        float q = l < 0.5f ? l * (1 + s) : l + s - l * s;
        float p = 2 * l - q;
        *r = (uint8_t)(hue2rgb(p, q, h + 1.0f/3.0f) * 255);
        *g = (uint8_t)(hue2rgb(p, q, h) * 255);
        *b = (uint8_t)(hue2rgb(p, q, h - 1.0f/3.0f) * 255);
    }
}

float AnimationManager::hue2rgb(float p, float q, float t) {
    if (t < 0) t += 1;
    if (t > 1) t -= 1;
    if (t < 1.0f/6.0f) return p + (q - p) * 6 * t;
    if (t < 1.0f/2.0f) return q;
    if (t < 2.0f/3.0f) return p + (q - p) * (2.0f/3.0f - t) * 6;
    return p;
}

uint16_t AnimationManager::color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void AnimationManager::drawPixelWithBlend(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) {
    if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT) return;
    
    if (alpha == 255) {
        display.drawPixelRGB888(x, y, r, g, b);
    } else {
        // Get existing pixel and blend
        uint16_t existing = display.getPixel(x, y);
        uint8_t er = (existing >> 8) & 0xF8;
        uint8_t eg = (existing >> 3) & 0xFC;
        uint8_t eb = (existing << 3) & 0xF8;
        
        // Alpha blend
        float alphaF = alpha / 255.0f;
        uint8_t nr = (uint8_t)(r * alphaF + er * (1.0f - alphaF));
        uint8_t ng = (uint8_t)(g * alphaF + eg * (1.0f - alphaF));
        uint8_t nb = (uint8_t)(b * alphaF + eb * (1.0f - alphaF));
        
        display.drawPixelRGB888(x, y, nr, ng, nb);
    }
}

void AnimationManager::clearBackground() {
    display.clearData();
}

void AnimationManager::applyFade(uint8_t fadeAmount) {
    // Apply fade by reducing brightness of all pixels
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            uint16_t pixel = display.getPixel(x, y);
            if (pixel != 0) {
                uint8_t r = (pixel >> 8) & 0xF8;
                uint8_t g = (pixel >> 3) & 0xFC;
                uint8_t b = (pixel << 3) & 0xF8;
                
                r = (r * fadeAmount) / 255;
                g = (g * fadeAmount) / 255;
                b = (b * fadeAmount) / 255;
                
                display.drawPixelRGB888(x, y, r, g, b);
            }
        }
    }
}

// Manual control methods
void AnimationManager::setAnimation(AnimationType type) {
    if (type >= 0 && type < ANIM_COUNT && type != currentAnimation) {
        if (!inFade) {
            // Start immediate transition to selected animation
            inFade = true;
            fadeStartTime = millis();
            nextAnimation = type;
            initializeAnimation(nextAnimation);
        } else {
            // If already fading, just change the target
            nextAnimation = type;
            initializeAnimation(nextAnimation);
        }
    }
}

void AnimationManager::setAutoMode(bool enabled) {
    autoMode = enabled;
    if (enabled) {
        // Reset the cycle timer when entering auto mode
        lastCycleTime = millis();
    }
} 