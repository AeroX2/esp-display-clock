#ifndef ANIMATIONS_COORDINATOR_H
#define ANIMATIONS_COORDINATOR_H

#include <Arduino.h>

// Animation IDs
enum AnimationType {
    ANIM_PLASMA = 0,
    ANIM_PARTICLES,
    ANIM_FIRE,
    ANIM_GALAXY,
    ANIM_LIGHTNING,
    ANIM_BEACH,
    ANIM_COUNT
};

// Simple global coordination functions
void initAnimations();
void renderCurrentAnimation();
void cycleToNextAnimation();
void setAnimation(AnimationType type);
AnimationType getCurrentAnimation();
const char* getCurrentAnimationName();
const char* getAnimationName(AnimationType type);

// Optional fade states
void startFadeOut();
void startFadeIn();
bool isFading();
bool isAnimationFading(); // Alias for isFading()

#endif // ANIMATIONS_COORDINATOR_H 