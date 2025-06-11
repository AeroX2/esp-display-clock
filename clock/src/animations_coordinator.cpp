#include "animations_coordinator.h"
#include "animations_modules.h"
#include "animation_utils.h"
#include "display.h"

// Minimal global state
static AnimationType currentAnimation = ANIM_PLASMA;
static AnimationType targetAnimation = ANIM_PLASMA;
static unsigned long lastCycleTime = 0;
static unsigned long animationDuration = 3 * 60 * 60 * 1000; // 3 hours per animation

// Fade state
static bool fadeActive = false;
static bool fadeOut = false;
static unsigned long fadeStartTime = 0;
static const unsigned long FADE_DURATION = 3000; // 3 second

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
        case ANIM_STARS:
            StarAnimation::init();
            break;
        case ANIM_BEACH:
            BeachAnimation::init();
            break;
        case ANIM_DVD_LOGO:
            DVDLogoAnimation::init();
            break;
        default:
            PlasmaAnimation::init();
            break;
    }
    
    lastCycleTime = millis();
}

void renderCurrentAnimation() {
    // Handle auto-cycling
    if (millis() - lastCycleTime > animationDuration) {
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
        case ANIM_STARS:
            StarAnimation::render();
            break;
        case ANIM_BEACH:
            BeachAnimation::render();
            break;
        case ANIM_DVD_LOGO:
            DVDLogoAnimation::render();
            break;
        default:
            PlasmaAnimation::render();
            break;
    }
    
    // Apply fade if active
    if (fadeActive) {
        unsigned long elapsed = millis() - fadeStartTime;

        uint8_t fadeLevel;
        float progress = (float)elapsed / FADE_DURATION;
        if (fadeOut) {
            fadeLevel = 255 - (uint8_t)(progress * 255);
            if (elapsed > FADE_DURATION) {
                currentAnimation = targetAnimation;
                initAnimations();

                // Screen should be blank by now but just in case.
                display.clearData();

                startFadeIn();
            }
        } else {
            fadeLevel = (uint8_t)(progress * 255);
            if (elapsed > FADE_DURATION) {
                fadeActive = false;
                fadeLevel = 255;
            }
        }

        AnimationUtils::applyFade(fadeLevel);
    }
}

void cycleToNextAnimation() {
    // Move to next animation
    targetAnimation = (AnimationType)((currentAnimation + 1) % ANIM_COUNT);
    
    lastCycleTime = millis();
    startFadeOut();
}

void setAnimation(AnimationType type) {
    if (type >= ANIM_COUNT) return;
    
    targetAnimation = type;
    
    lastCycleTime = millis();
    startFadeOut();
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
        case ANIM_STARS:
            return StarAnimation::getName();
        case ANIM_BEACH:
            return BeachAnimation::getName();
        case ANIM_DVD_LOGO:
            return DVDLogoAnimation::getName();
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