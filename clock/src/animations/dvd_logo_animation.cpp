#include "animations_modules.h"
#include "animation_utils.h"
#include "dvd_logo.h"
#include <Arduino.h>

namespace DVDLogoAnimation {
    // DVD logo position and velocity
    float logoX = 10.0;
    float logoY = 10.0;
    float velocityX = 1.5;
    float velocityY = 1.0;
    
    // Color cycling
    uint8_t currentColorIndex = 0;
    const uint16_t colors[] = {
        display.color565(0xFF,00,00), // Red
        display.color565(0x00,0xFF,00), // Green
        display.color565(0x00,00,0xFF), // Blue
        display.color565(0xFF,0xFF,00), // Yellow
        display.color565(0xFF,00,0xFF), // Magenta
        display.color565(0x00,0xFF,0xFF), // Cyan
        display.color565(0xFF,0xA5,0x00), // Orange
        display.color565(0x80,0x00,0x80)  // Purple
    };
    const uint8_t numColors = sizeof(colors) / sizeof(colors[0]);
    
    void init() {
        // Initialize position to center-ish
        logoX = (DISPLAY_WIDTH - dvdLogoImageWidth) / 2.0;
        logoY = (DISPLAY_HEIGHT - dvdLogoImageHeight) / 2.0;
        
        // Random initial velocity direction
        velocityX = (random(2) == 0) ? 1.5 : -1.5;
        velocityY = (random(2) == 0) ? 1.0 : -1.0;
        
        // Random starting color
        currentColorIndex = random(numColors);
    }
    
    void render() {
        display.clearData();

        // Update position
        logoX += velocityX;
        logoY += velocityY;
        
        // Check for collisions with edges and bounce
        bool colorChanged = false;
        
        // Left/right edge collision
        if (logoX <= 0) {
            logoX = 0;
            velocityX = -velocityX;
            colorChanged = true;
        } else if (logoX >= DISPLAY_WIDTH - dvdLogoImageWidth) {
            logoX = DISPLAY_WIDTH - dvdLogoImageWidth;
            velocityX = -velocityX;
            colorChanged = true;
        }
        
        // Top/bottom edge collision
        if (logoY <= 0) {
            logoY = 0;
            velocityY = -velocityY;
            colorChanged = true;
        } else if (logoY >= DISPLAY_HEIGHT - dvdLogoImageHeight) {
            logoY = DISPLAY_HEIGHT - dvdLogoImageHeight;
            velocityY = -velocityY;
            colorChanged = true;
        }
        
        // Change color when hitting an edge
        if (colorChanged) {
            currentColorIndex = (currentColorIndex + 1) % numColors;
        }
        
        // Draw the DVD logo bitmap with current color
        AnimationUtils::drawBitmapTransparent((int)logoX, (int)logoY, dvd_logo_bitmap, dvdLogoImageWidth, dvdLogoImageHeight, colors[currentColorIndex]);
    }
    
    const char* getName() {
        return "DVD Logo";
    }
}