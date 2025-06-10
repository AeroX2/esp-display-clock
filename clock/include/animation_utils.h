#ifndef ANIMATION_UTILS_H
#define ANIMATION_UTILS_H

#include <Arduino.h>
#include "display.h"

class AnimationUtils {
public:
    // Color and drawing utilities
    static void drawPixelWithBlend(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha = 255);
    static void hslToRgb(float h, float s, float l, uint8_t* r, uint8_t* g, uint8_t* b);
    static uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
    static void applyFade(uint8_t fadeAmount);
    
private:
    static float hue2rgb(float p, float q, float t);
};

#endif // ANIMATION_UTILS_H 