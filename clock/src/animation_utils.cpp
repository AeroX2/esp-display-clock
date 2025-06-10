#include "animation_utils.h"

void AnimationUtils::drawPixelWithBlend(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) {
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

void AnimationUtils::hslToRgb(float h, float s, float l, uint8_t* r, uint8_t* g, uint8_t* b) {
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

float AnimationUtils::hue2rgb(float p, float q, float t) {
    if (t < 0) t += 1;
    if (t > 1) t -= 1;
    if (t < 1.0f/6.0f) return p + (q - p) * 6 * t;
    if (t < 1.0f/2.0f) return q;
    if (t < 2.0f/3.0f) return p + (q - p) * (2.0f/3.0f - t) * 6;
    return p;
}

uint16_t AnimationUtils::color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void AnimationUtils::applyFade(uint8_t fadeAmount) {
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