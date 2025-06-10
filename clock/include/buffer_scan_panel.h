
/*
  Patch class for 32x16 RGB Matrix panels

  reimplement all functions which use x,y coordinates

*/

#ifndef ESP_HUB75_32x16MatrixPanel
#define ESP_HUB75_32x16MatrixPanel

#include "display_config.h"
#include <ESP32-HUB75-VirtualMatrixPanel_T.hpp>

class BufferMatrixPanel : public VirtualMatrixPanel_T<CHAIN_NONE> {
  using VirtualMatrixPanel_T<CHAIN_NONE>::VirtualMatrixPanel_T;

 public:
  void drawPixel(int16_t x, int16_t y, uint16_t color) override;
  void drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b);

  inline void clearData() {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
      for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        pixelData[x][y] = 0;
      }
    }
  }

  uint16_t getPixel(int16_t x, int16_t y) { 
    return pixelData[x][y]; 
  }

 private:
  uint16_t pixelData[DISPLAY_WIDTH][DISPLAY_HEIGHT] = {};
};

#endif
