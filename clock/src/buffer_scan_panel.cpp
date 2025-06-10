#include "buffer_scan_panel.h"
#include "animation_utils.h"

void BufferMatrixPanel::drawPixel(int16_t x, int16_t y, uint16_t color) {
  uint8_t r, g, b;
  pixelData[x][y] = color;
}

void BufferMatrixPanel::drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b) {
  uint16_t color = AnimationUtils::rgb888To565(r, g, b);
  pixelData[x][y] = color;
}