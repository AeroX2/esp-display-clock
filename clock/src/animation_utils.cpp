#include "animation_utils.h"

uint16_t AnimationUtils::rgb888To565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
}

void AnimationUtils::rgb565To888(uint16_t color, uint8_t* r, uint8_t* g, uint8_t* b) {
    *r = (color >> 11) << 3;
    *g = ((color >> 5) & 0x3F) << 2;
    *b = (color & 0x1F) << 3;
}

void AnimationUtils::drawPixelWithBlend(int x, int y, uint16_t color, uint8_t alpha) {
    if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT) return;
    if (display.getPixel(x,y) == 0xFFFF) return;
    
    if (alpha == 255) {
        display.drawPixel(x, y, color);
    } else {
        uint8_t r, g, b;
        rgb565To888(color, &r, &g, &b);
        uint16_t blendedColor = alphaBlend(x, y, r, g, b, alpha);
        display.drawPixel(x, y, blendedColor);
    }
}

uint16_t AnimationUtils::alphaBlend(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) {
        uint16_t existing = display.getPixel(x, y);
        uint8_t er, eg, eb;
        rgb565To888(existing, &er, &eg, &eb);
        
        // Alpha blend
        float alphaF = alpha / 255.0f;
        uint8_t nr = (uint8_t)(r * alphaF + er * (1.0f - alphaF));
        uint8_t ng = (uint8_t)(g * alphaF + eg * (1.0f - alphaF));
        uint8_t nb = (uint8_t)(b * alphaF + eb * (1.0f - alphaF));

        return rgb888To565(nr, ng, nb);
}

void AnimationUtils::hslToRgb(float h, float s, float l, uint8_t* r, uint8_t* g, uint8_t* b) {
    if (s == 0) {
        *r = *g = *b = (uint8_t)(l * 255);
    } else {
        float q = l < 0.5f ? l * (1 + s) : l + s - l * s;
        float p = 2 * l - q;
        *r = (uint8_t)(hueToRgb(p, q, h + 1.0f/3.0f) * 255);
        *g = (uint8_t)(hueToRgb(p, q, h) * 255);
        *b = (uint8_t)(hueToRgb(p, q, h - 1.0f/3.0f) * 255);
    }
}

float AnimationUtils::hueToRgb(float p, float q, float t) {
    if (t < 0) t += 1;
    if (t > 1) t -= 1;
    if (t < 1.0f/6.0f) return p + (q - p) * 6 * t;
    if (t < 1.0f/2.0f) return q;
    if (t < 2.0f/3.0f) return p + (q - p) * (2.0f/3.0f - t) * 6;
    return p;
}

void AnimationUtils::applyFade(uint8_t fadeAmount) {
    // Apply fade by reducing brightness of all pixels
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            uint16_t pixel = display.getPixel(x, y);
            if (pixel != 0 || pixel != 0xFFFF) {
                uint8_t r, g, b;
                rgb565To888(pixel, &r, &g, &b);
                
                r = (r * fadeAmount) / 255;
                g = (g * fadeAmount) / 255;
                b = (b * fadeAmount) / 255;
                
                uint16_t color = rgb888To565(r, g, b);
                display.drawPixel(x, y, color);
            }
        }
    }
}

void AnimationUtils::drawCircle(float xCenter, float yCenter, float radius, uint16_t color, uint8_t alpha) {
    int x0 = round(xCenter);
    int y0 = round(yCenter);
    int r0 = round(radius);
    
    int x = r0;
    int y = 0;
    int err = 0;

    while (x >= y) {
        float weight = 1.0f - (sqrt((x - radius) * (x - radius) + (y) * (y)) / radius);
        uint8_t pixelAlpha = (uint8_t)(alpha * weight);
        
        drawPixelWithBlend(x0 + x, y0 + y, color, pixelAlpha);
        drawPixelWithBlend(x0 + y, y0 + x, color, pixelAlpha);
        drawPixelWithBlend(x0 - y, y0 + x, color, pixelAlpha);
        drawPixelWithBlend(x0 - x, y0 + y, color, pixelAlpha);
        drawPixelWithBlend(x0 - x, y0 - y, color, pixelAlpha);
        drawPixelWithBlend(x0 - y, y0 - x, color, pixelAlpha);
        drawPixelWithBlend(x0 + y, y0 - x, color, pixelAlpha);
        drawPixelWithBlend(x0 + x, y0 - y, color, pixelAlpha);

        if (err <= 0) {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

void AnimationUtils::fillCircle(float xCenter, float yCenter, float radius, uint16_t color, uint8_t alpha) {
    int x0 = round(xCenter);
    int y0 = round(yCenter);
    float radiusSquared = radius * radius;

    for (float dy = -radius; dy <= radius; dy += 1.0f) {
        float dx = sqrt(radiusSquared - dy * dy);
        int y = round(y0 + dy);
        
        for (float x = -dx; x <= dx; x += 1.0f) {
            int xPos = round(x0 + x);
            float distanceFromCenter = sqrt(x * x + dy * dy);
            float weight = 1.0f - (distanceFromCenter / radius);
            weight = max(0.0f, min(1.0f, weight));
            
            uint8_t pixelAlpha = (uint8_t)(alpha * weight);
            drawPixelWithBlend(xPos, y, color, pixelAlpha);
        }
    }
}