#ifndef ANIMATION_UTILS_H
#define ANIMATION_UTILS_H

#include <Arduino.h>
#include "display.h"

class AnimationUtils {
public:
    // Color and drawing utilities
    static void drawPixelWithBlend(int x, int y, uint16_t color, uint8_t alpha = 255);
    static uint16_t alphaBlend(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);
    static void hslToRgb(float h, float s, float l, uint8_t* r, uint8_t* g, uint8_t* b);
    static void applyFade(uint8_t fadeAmount);
    static void drawCircle(float xCenter, float yCenter, float radius, uint16_t color, uint8_t alpha = 255);
    static void fillCircle(float xCenter, float yCenter, float radius, uint16_t color, uint8_t alpha = 255);
    
    // Transparent bitmap drawing - skips empty pixels instead of drawing background
    static void drawBitmapTransparent(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);
    
    // Convert 24-bit RGB (8 bits per channel) to 16-bit RGB565
    static uint16_t rgb888To565(uint8_t r, uint8_t g, uint8_t b);
    // Convert 16-bit RGB565 to 24-bit RGB components
    static void rgb565To888(uint16_t color, uint8_t* r, uint8_t* g, uint8_t* b);

private:
    static float hueToRgb(float p, float q, float t);
};

#endif // ANIMATION_UTILS_H