#include "animation_base.h"

void Animations::initOrbital(AnimationManager* manager) {
    // Initialize orbital particles
    manager->state.gravityPoint.x = DISPLAY_WIDTH / 2;
    manager->state.gravityPoint.y = DISPLAY_HEIGHT / 2;
    manager->state.gravityPoint.velocityX = 0.2f;
    manager->state.gravityPoint.velocityY = 0.15f;
    
    for (int i = 0; i < MAX_ORBITAL_PARTICLES; i++) {
        float angle = random(0, 628) / 100.0f;  // Random angle
        float distance = 5 + random(0, 40);
        // Use sin16/cos16 for optimized trigonometry
        manager->state.orbitalParticles[i].x = manager->state.gravityPoint.x + (cos16(TO_SIN16(angle)) / 32767.0f) * distance;
        manager->state.orbitalParticles[i].y = manager->state.gravityPoint.y + (sin16(TO_SIN16(angle)) / 32767.0f) * distance;
        
        float speed = 0.05f + random(0, 15) / 100.0f;
        float perpAngle = angle + PI/2;
        manager->state.orbitalParticles[i].velocityX = (cos16(TO_SIN16(perpAngle)) / 32767.0f) * speed;
        manager->state.orbitalParticles[i].velocityY = (sin16(TO_SIN16(perpAngle)) / 32767.0f) * speed;
        
        manager->state.orbitalParticles[i].red = random(100, 255);
        manager->state.orbitalParticles[i].green = random(100, 255);
        manager->state.orbitalParticles[i].blue = random(100, 255);
        manager->state.orbitalParticles[i].trailLength = 0;
    }
}

void Animations::renderOrbital(AnimationManager* manager) {
    // Clear with trails
    manager->applyFade(250);
    
    // Update gravity point
    manager->state.gravityPoint.x += manager->state.gravityPoint.velocityX;
    manager->state.gravityPoint.y += manager->state.gravityPoint.velocityY;
    
    // Bounce gravity point off edges
    if (manager->state.gravityPoint.x <= 0 || manager->state.gravityPoint.x >= DISPLAY_WIDTH - 1) {
        manager->state.gravityPoint.velocityX *= -1;
        manager->state.gravityPoint.x = constrain(manager->state.gravityPoint.x, 0, DISPLAY_WIDTH - 1);
    }
    if (manager->state.gravityPoint.y <= 0 || manager->state.gravityPoint.y >= DISPLAY_HEIGHT - 1) {
        manager->state.gravityPoint.velocityY *= -1;
        manager->state.gravityPoint.y = constrain(manager->state.gravityPoint.y, 0, DISPLAY_HEIGHT - 1);
    }
    
    // Update particles
    for (int i = 0; i < MAX_ORBITAL_PARTICLES; i++) {
        // Store trail
        if (manager->state.orbitalParticles[i].trailLength < 200) {
            manager->state.orbitalParticles[i].trail[manager->state.orbitalParticles[i].trailLength][0] = manager->state.orbitalParticles[i].x;
            manager->state.orbitalParticles[i].trail[manager->state.orbitalParticles[i].trailLength][1] = manager->state.orbitalParticles[i].y;
            manager->state.orbitalParticles[i].trailLength++;
        } else {
            // Shift trail
            for (int t = 0; t < 199; t++) {
                manager->state.orbitalParticles[i].trail[t][0] = manager->state.orbitalParticles[i].trail[t + 1][0];
                manager->state.orbitalParticles[i].trail[t][1] = manager->state.orbitalParticles[i].trail[t + 1][1];
            }
            manager->state.orbitalParticles[i].trail[199][0] = manager->state.orbitalParticles[i].x;
            manager->state.orbitalParticles[i].trail[199][1] = manager->state.orbitalParticles[i].y;
        }
        
        // Apply gravity - optimized distance calculation
        float dx = manager->state.gravityPoint.x - manager->state.orbitalParticles[i].x;
        float dy = manager->state.gravityPoint.y - manager->state.orbitalParticles[i].y;
        float distanceSquared = dx * dx + dy * dy;  // Use squared distance to avoid sqrt
        
        if (distanceSquared > 1.0f) {  // Avoid division by zero
            float invDistance = 1.0f / sqrtf(distanceSquared);  // Use single sqrt call
            float force = 0.05f * invDistance * invDistance * invDistance;  // force = 0.05f / (distance^3)
            manager->state.orbitalParticles[i].velocityX += dx * force;
            manager->state.orbitalParticles[i].velocityY += dy * force;
        }
        
        // Update position
        manager->state.orbitalParticles[i].x += manager->state.orbitalParticles[i].velocityX;
        manager->state.orbitalParticles[i].y += manager->state.orbitalParticles[i].velocityY;
        
        // Wrap around screen
        if (manager->state.orbitalParticles[i].x < 0) manager->state.orbitalParticles[i].x = DISPLAY_WIDTH - 1;
        if (manager->state.orbitalParticles[i].x >= DISPLAY_WIDTH) manager->state.orbitalParticles[i].x = 0;
        if (manager->state.orbitalParticles[i].y < 0) manager->state.orbitalParticles[i].y = DISPLAY_HEIGHT - 1;
        if (manager->state.orbitalParticles[i].y >= DISPLAY_HEIGHT) manager->state.orbitalParticles[i].y = 0;
        
        // Limit speed - optimized speed calculation
        float velocitySquared = manager->state.orbitalParticles[i].velocityX * manager->state.orbitalParticles[i].velocityX + 
                               manager->state.orbitalParticles[i].velocityY * manager->state.orbitalParticles[i].velocityY;
        if (velocitySquared > 0.04f) {  // 0.2f squared
            float invSpeed = 0.2f / sqrtf(velocitySquared);  // Use single sqrt call
            manager->state.orbitalParticles[i].velocityX *= invSpeed;
            manager->state.orbitalParticles[i].velocityY *= invSpeed;
        }
        
        // Draw trail
        for (int t = 0; t < manager->state.orbitalParticles[i].trailLength; t++) {
            float alpha = 1.0f - (float)t / 200.0f;
            uint8_t alphaVal = (uint8_t)(alpha * 128);
            manager->drawPixelWithBlend(
                (int)manager->state.orbitalParticles[i].trail[t][0], 
                (int)manager->state.orbitalParticles[i].trail[t][1],
                manager->state.orbitalParticles[i].red / 2, 
                manager->state.orbitalParticles[i].green / 2, 
                manager->state.orbitalParticles[i].blue / 2, 
                alphaVal);
        }
        
        // Draw particle
        manager->drawPixelWithBlend((int)manager->state.orbitalParticles[i].x, (int)manager->state.orbitalParticles[i].y,
            manager->state.orbitalParticles[i].red, manager->state.orbitalParticles[i].green, manager->state.orbitalParticles[i].blue);
    }
    
    // Draw gravity point
    manager->drawPixelWithBlend((int)manager->state.gravityPoint.x, (int)manager->state.gravityPoint.y, 255, 255, 255, 128);
} 