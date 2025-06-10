#include "animations_coordinator.h"
#include "animations_modules.h"
#include "animation_utils.h"
#include "display.h"

// Minimal global state
static AnimationType currentAnimation = ANIM_PLASMA;
static unsigned long lastCycleTime = 0;
static unsigned long animationDuration = 3 * 60 * 60 * 1000; // 3 hours per animation
static bool enableAutoCycle = true;

// Fade state
static bool fadeActive = false;
static bool fadeOut = false;
static uint8_t fadeLevel = 255;
static unsigned long fadeStartTime = 0;
static const unsigned long FADE_DURATION = 1000; // 1 second

void initAnimations() {
    // Initialize the current animation
    switch(currentAnimation) {
        case ANIM_PLASMA:
            PlasmaAnimation::init();
            break;
        case ANIM_PARTICLES:
            ParticlesAnimation::init();
            break;
        case ANIM_FIRE:
            FireAnimation::init();
            break;

        case ANIM_GALAXY:
            GalaxyAnimation::init();
            break;
        case ANIM_LIGHTNING:
            LightningAnimation::init();
            break;
        case ANIM_PIPES:
            PipesAnimation::init();
            break;
        case ANIM_ORBITAL:
            OrbitalAnimation::init();
            break;
        case ANIM_STARS:
            StarsAnimation::init();
            break;
        case ANIM_BEACH:
            BeachAnimation::init();
            break;
        default:
            PlasmaAnimation::init();
            break;
    }
    
    lastCycleTime = millis();
}

void renderCurrentAnimation() {
    // Handle auto-cycling
    if (enableAutoCycle && (millis() - lastCycleTime > animationDuration)) {
        cycleToNextAnimation();
    }
    
    // Render the current animation
    switch(currentAnimation) {
        case ANIM_PLASMA:
            PlasmaAnimation::render();
            break;
        case ANIM_PARTICLES:
            ParticlesAnimation::render();
            break;
        case ANIM_FIRE:
            FireAnimation::render();
            break;

        case ANIM_GALAXY:
            GalaxyAnimation::render();
            break;
        case ANIM_LIGHTNING:
            LightningAnimation::render();
            break;
        case ANIM_PIPES:
            PipesAnimation::render();
            break;
        case ANIM_ORBITAL:
            OrbitalAnimation::render();
            break;
        case ANIM_STARS:
            StarsAnimation::render();
            break;
        case ANIM_BEACH:
            BeachAnimation::render();
            break;
        default:
            PlasmaAnimation::render();
            break;
    }
    
    // Apply fade if active
    if (fadeActive) {
        unsigned long elapsed = millis() - fadeStartTime;
        if (elapsed < FADE_DURATION) {
            float progress = (float)elapsed / FADE_DURATION;
            if (fadeOut) {
                fadeLevel = 255 - (uint8_t)(progress * 255);
            } else {
                fadeLevel = (uint8_t)(progress * 255);
            }
            AnimationUtils::applyFade(fadeLevel);
        } else {
            fadeActive = false;
            fadeLevel = fadeOut ? 0 : 255;
        }
    }
}

void cycleToNextAnimation() {
    // Move to next animation
    currentAnimation = (AnimationType)((currentAnimation + 1) % ANIM_COUNT);
    
    // Initialize new animation
    initAnimations();
    
    lastCycleTime = millis();
}

void setAnimation(AnimationType type) {
    if (type >= ANIM_COUNT) return;
    
    currentAnimation = type;
    
    // Initialize new animation
    initAnimations();
    
    lastCycleTime = millis();
}

AnimationType getCurrentAnimation() {
    return currentAnimation;
}

const char* getCurrentAnimationName() {
    return getAnimationName(currentAnimation);
}

const char* getAnimationName(AnimationType type) {
    switch(type) {
        case ANIM_PLASMA:
            return PlasmaAnimation::getName();
        case ANIM_PARTICLES:
            return ParticlesAnimation::getName();
        case ANIM_FIRE:
            return FireAnimation::getName();
        case ANIM_GALAXY:
            return GalaxyAnimation::getName();
        case ANIM_LIGHTNING:
            return LightningAnimation::getName();
        case ANIM_PIPES:
            return PipesAnimation::getName();
        case ANIM_ORBITAL:
            return OrbitalAnimation::getName();
        case ANIM_STARS:
            return StarsAnimation::getName();
        case ANIM_BEACH:
            return BeachAnimation::getName();
        default:
            return "Unknown";
    }
}

void startFadeOut() {
    fadeActive = true;
    fadeOut = true;
    fadeStartTime = millis();
}

void startFadeIn() {
    fadeActive = true;
    fadeOut = false;
    fadeStartTime = millis();
}

bool isFading() {
    return fadeActive;
}

bool isAnimationFading() {
    return isFading();
} 