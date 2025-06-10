#include "buffer_scan_panel.h"

void BufferMatrixPanel::drawPixel(int16_t x, int16_t y, uint16_t color) {
  uint8_t r, g, b;
  pixelData[x][y] = color;
}

void BufferMatrixPanel::drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b) {
  uint16_t color = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
  pixelData[x][y] = color;
}