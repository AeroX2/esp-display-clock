#ifndef ANIMATIONS_MODULES_H
#define ANIMATIONS_MODULES_H

#include <Arduino.h>

// Common interface for all animations
// Each animation namespace implements: init(), render(), getName()

namespace PlasmaAnimation {
    void init();
    void render();
    const char* getName();
}

namespace ParticlesAnimation {
    void init();
    void render();
    const char* getName();
}

namespace FireAnimation {
    void init();
    void render();
    const char* getName();
}

namespace GalaxyAnimation {
    void init();
    void render();
    const char* getName();
}

namespace LightningAnimation {
    void init();
    void render();
    const char* getName();
}

namespace BeachAnimation {
    void init();
    void render();
    const char* getName();
}

#endif // ANIMATIONS_MODULES_H 