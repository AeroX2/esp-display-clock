#include "buffer_scan_panel.h"
#include "animation_utils.h"

void BufferMatrixPanel::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT) {
    return;
  }
  pixelData[x][y] = color;
}

void BufferMatrixPanel::drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b) {
  uint16_t color = AnimationUtils::rgb888To565(r, g, b);
  drawPixel(x,y,color);
}

void BufferMatrixPanel::applyAntialiasing() {
  // Create a temporary buffer for the antialiased result
  static uint16_t tempBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT];
  
  // Copy current data to temp buffer
  for (int x = 0; x < DISPLAY_WIDTH; x++) {
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
      tempBuffer[x][y] = pixelData[x][y];
    }
  }
  
  // Apply simple 3x3 box filter antialiasing
  for (int x = 1; x < DISPLAY_WIDTH - 1; x++) {
    for (int y = 1; y < DISPLAY_HEIGHT - 1; y++) {
      // Extract RGB components from surrounding pixels
      uint32_t totalR = 0, totalG = 0, totalB = 0;
      int count = 0;
      
      // Sample 3x3 neighborhood
      for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
          uint16_t pixel = tempBuffer[x + dx][y + dy];
          
          // Convert RGB565 to RGB888 for averaging
          uint8_t r = (pixel >> 11) & 0x1F;
          uint8_t g = (pixel >> 5) & 0x3F;
          uint8_t b = pixel & 0x1F;
          
          // Scale to 8-bit
          r = (r * 255) / 31;
          g = (g * 255) / 63;
          b = (b * 255) / 31;
          
          totalR += r;
          totalG += g;
          totalB += b;
          count++;
        }
      }
      
      // Average the colors
      uint8_t avgR = totalR / count;
      uint8_t avgG = totalG / count;
      uint8_t avgB = totalB / count;
      
      // Blend with original pixel (50% antialiasing strength)
      uint16_t originalPixel = tempBuffer[x][y];
      uint8_t origR = ((originalPixel >> 11) & 0x1F) * 255 / 31;
      uint8_t origG = ((originalPixel >> 5) & 0x3F) * 255 / 63;
      uint8_t origB = (originalPixel & 0x1F) * 255 / 31;
      
      uint8_t finalR = (origR + avgR) / 2;
      uint8_t finalG = (origG + avgG) / 2;
      uint8_t finalB = (origB + avgB) / 2;
      
      // Convert back to RGB565
      pixelData[x][y] = AnimationUtils::rgb888To565(finalR, finalG, finalB);
    }
  }
}