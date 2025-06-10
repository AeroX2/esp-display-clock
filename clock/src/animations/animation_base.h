#ifndef ANIMATION_BASE_H
#define ANIMATION_BASE_H

#include "../animations.h"

// Forward declaration of AnimationManager
class AnimationManager;

// Common animation interface - all individual animations will use this
namespace Animations {
    // Render functions
    void renderPlasma(AnimationManager* manager);
    void renderParticles(AnimationManager* manager);
    void renderFire(AnimationManager* manager);
    void renderSkyline(AnimationManager* manager);
    void renderGalaxy(AnimationManager* manager);
    void renderLightning(AnimationManager* manager);
    void renderPipes(AnimationManager* manager);
    void renderOrbital(AnimationManager* manager);
    void renderStars(AnimationManager* manager);
    void renderBeach(AnimationManager* manager);
    
    // Initialization functions
    void initPlasma(AnimationManager* manager);
    void initParticles(AnimationManager* manager);
    void initFire(AnimationManager* manager);
    void initSkyline(AnimationManager* manager);
    void initGalaxy(AnimationManager* manager);
    void initLightning(AnimationManager* manager);
    void initPipes(AnimationManager* manager);
    void initOrbital(AnimationManager* manager);
    void initStars(AnimationManager* manager);
    void initBeach(AnimationManager* manager);
}

#endif // ANIMATION_BASE_H 