
/*
  Patch class for 32x16 RGB Matrix panels

  reimplement all functions which use x,y coordinates

*/

#ifndef ESP_HUB75_32x16MatrixPanel
#define ESP_HUB75_32x16MatrixPanel

#include <ESP32-VirtualMatrixPanel-I2S-DMA.h>

class BufferMatrixPanel : public VirtualMatrixPanel {
  using VirtualMatrixPanel::VirtualMatrixPanel;

 public:
  void drawPixel(int16_t x, int16_t y, uint16_t color) override;
  void drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b);
  // inline VirtualCoords getCoords(int16_t x, int16_t y) override;

  inline void clearData() {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
      for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        pixelData[x][y] = 0;
      }
    }
  }

  uint16_t getPixel(int16_t x, int16_t y) { return pixelData[x][y]; }

 private:
  uint32_t pixelData[DISPLAY_WIDTH][DISPLAY_HEIGHT] = {0};
};

void BufferMatrixPanel::drawPixel(int16_t x, int16_t y, uint16_t color) {
  // getCoords(x, y);

  uint8_t r, g, b;
  MatrixPanel_I2S_DMA::color565to888(color, r, g, b);
  pixelData[x][y] = (r << 16) + (g << 8) + b;

  VirtualMatrixPanel::drawPixelRGB888(x, y, r, g, b);
}

void BufferMatrixPanel::drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b) {
  // getCoords(x, y);
  pixelData[x][y] = (r << 16) + (g << 8) + b;

  VirtualMatrixPanel::drawPixelRGB888(x, y, r, g, b);
}

// inline VirtualCoords QuarterScanMatrixPanel::getCoords(int16_t x, int16_t y) {
//   coords.x = x + (x / 8) * 8 + (y % 8 / 4) * 8;
//   coords.y = y % 4 + (y / 8) * 4;
//   return coords;
// }

#endif
