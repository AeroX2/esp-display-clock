#include "animations.h"
#include "animations/animation_base.h"
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
    
    // Render current animation
    switch (currentAnimation) {
        case ANIM_PLASMA: Animations::renderPlasma(this); break;
        case ANIM_PARTICLES: Animations::renderParticles(this); break;
        case ANIM_FIRE: Animations::renderFire(this); break;
        case ANIM_SKYLINE: Animations::renderSkyline(this); break;
        case ANIM_GALAXY: Animations::renderGalaxy(this); break;
        case ANIM_LIGHTNING: Animations::renderLightning(this); break;
        case ANIM_PIPES: Animations::renderPipes(this); break;
        case ANIM_ORBITAL: Animations::renderOrbital(this); break;
        case ANIM_STARS: Animations::renderStars(this); break;
        case ANIM_BEACH: Animations::renderBeach(this); break;
    }
    
    // If fading, blend with next animation
    if (inFade) {
        // Store current frame
        // Render next animation with fade
        switch (nextAnimation) {
            case ANIM_PLASMA: Animations::renderPlasma(this); break;
            case ANIM_PARTICLES: Animations::renderParticles(this); break;
            case ANIM_FIRE: Animations::renderFire(this); break;
            case ANIM_SKYLINE: Animations::renderSkyline(this); break;
            case ANIM_GALAXY: Animations::renderGalaxy(this); break;
            case ANIM_LIGHTNING: Animations::renderLightning(this); break;
            case ANIM_PIPES: Animations::renderPipes(this); break;
            case ANIM_ORBITAL: Animations::renderOrbital(this); break;
            case ANIM_STARS: Animations::renderStars(this); break;
            case ANIM_BEACH: Animations::renderBeach(this); break;
        }
        applyFade(fadeProgress);
    }
}

void AnimationManager::initializeAnimation(AnimationType type) {
    state.frameCount = 0;
    state.initialized = false;
    
    // Call the individual animation initialization functions
    switch (type) {
        case ANIM_PLASMA: Animations::initPlasma(this); break;
        case ANIM_PARTICLES: Animations::initParticles(this); break;
        case ANIM_FIRE: Animations::initFire(this); break;
        case ANIM_SKYLINE: Animations::initSkyline(this); break;
        case ANIM_GALAXY: Animations::initGalaxy(this); break;
        case ANIM_LIGHTNING: Animations::initLightning(this); break;
        case ANIM_PIPES: Animations::initPipes(this); break;
        case ANIM_ORBITAL: Animations::initOrbital(this); break;
        case ANIM_STARS: Animations::initStars(this); break;
        case ANIM_BEACH: Animations::initBeach(this); break;
    }
    
    state.initialized = true;
}

// Individual render functions are now implemented in separate files
void AnimationManager::renderPlasma() { Animations::renderPlasma(this); }
void AnimationManager::renderParticles() { Animations::renderParticles(this); }
void AnimationManager::renderFire() { Animations::renderFire(this); }
void AnimationManager::renderSkyline() { Animations::renderSkyline(this); }
void AnimationManager::renderGalaxy() { Animations::renderGalaxy(this); }
void AnimationManager::renderLightning() { Animations::renderLightning(this); }
void AnimationManager::renderPipes() { Animations::renderPipes(this); }
void AnimationManager::renderOrbital() { Animations::renderOrbital(this); }
void AnimationManager::renderStars() { Animations::renderStars(this); }
void AnimationManager::renderBeach() { Animations::renderBeach(this); }

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
    if (display.getPixel(x,y) == 0xFFFF) return;
    
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