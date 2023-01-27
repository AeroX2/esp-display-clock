#ifndef DISPLAY_H
#define DISPLAY_H

// uncomment one line to select your MatrixHardware configuration - configuration header needs to be included before <SmartMatrix.h>
//#include <MatrixHardware_Teensy3_ShieldV4.h>        // SmartLED Shield for Teensy 3 (V4)
//#include <MatrixHardware_Teensy4_ShieldV5.h>        // SmartLED Shield for Teensy 4 (V5)
//#include <MatrixHardware_Teensy3_ShieldV1toV3.h>    // SmartMatrix Shield for Teensy 3 V1-V3
//#include <MatrixHardware_Teensy4_ShieldV4Adapter.h> // Teensy 4 Adapter attached to SmartLED Shield for Teensy 3 (V4)
//#include <MatrixHardware_ESP32_V0.h>                // This file contains multiple ESP32 hardware configurations, edit the file to define GPIOPINOUT (or add #define GPIOPINOUT with a hardcoded number before this #include)
#include "MatrixHardware_Custom.h"                  // Copy an existing MatrixHardware file to your Sketch directory, rename, customize, and you can include it like this
#include <SmartMatrix.h>

#define COLOR_DEPTH 24                  // Choose the color depth used for storing pixels in the layers: 24 or 48 (24 is good for most sketches - If the sketch uses type `rgb24` directly, COLOR_DEPTH must be 24)
const uint16_t kMatrixWidth = 64;       // Set to the width of your display, must be a multiple of 8
const uint16_t kMatrixHeight = 8;      // Set to the height of your display
const uint8_t kRefreshDepth = 36;       // Tradeoff of color quality vs refresh rate, max brightness, and RAM usage.  36 is typically good, drop down to 24 if you need to.  On Teensy, multiples of 3, up to 48: 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48.  On ESP32: 24, 36, 48
const uint8_t kDmaBufferRows = 4;       // known working: 2-4, use 2 to save RAM, more to keep from dropping frames and automatically lowering refresh rate.  (This isn't used on ESP32, leave as default)
const uint8_t kPanelType = SM_PANELTYPE_HUB75_8ROW_MOD4SCAN;   // Choose the configuration that matches your panels.  See more details in MatrixCommonHub75.h and the docs: https://github.com/pixelmatix/SmartMatrix/wiki
const uint32_t kMatrixOptions = (SM_HUB75_OPTIONS_NONE);        // see docs for options: https://github.com/pixelmatix/SmartMatrix/wiki
const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_OPTIONS_NONE);
const uint8_t kScrollingLayerOptions = (SM_SCROLLING_OPTIONS_NONE);
const uint8_t kIndexedLayerOptions = (SM_INDEXED_OPTIONS_NONE);

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);
SMARTMATRIX_ALLOCATE_SCROLLING_LAYER(scrollingLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kScrollingLayerOptions);
SMARTMATRIX_ALLOCATE_INDEXED_LAYER(indexedLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kIndexedLayerOptions);

#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 16

const int defaultBrightness = (15*255)/100;

void displayInit() {
  // display.begin(4);
  // display.setScanPattern(ZAGZIG);
  // displayUpdateEnable(true);

  matrix.addLayer(&indexedLayer);
  matrix.begin();

  // matrix.setBrightness(defaultBrightness);

  // indexedLayer.fillScreen(0);
  // indexedLayer.setFont(gohufont11b);
  // indexedLayer.drawString(0, kMatrixHeight / 2 - 6, 1, "CLOCK");
  // indexedLayer.swapBuffers(false);
}

void displayUpdate() {
  // indexedLayer.drawString(0, kMatrixHeight / 2 - 6, 1, "CLOCK");
  // indexedLayer.swapBuffers();
      backgroundLayer.fillScreen( {0x40, 0, 0});
    backgroundLayer.swapBuffers();

    scrollingLayer.setColor({0xff, 0xff, 0xff});
    scrollingLayer.setMode(wrapForward);
    scrollingLayer.setSpeed(40);
    scrollingLayer.setFont(font6x10);
    scrollingLayer.start("SmartMatrix Demo", 1);
}

// void setPixel(int ax, int ay, bool d) {
//   display.drawPixel(ax, ay, d ? WHITE : BLACK);
// }

// boolean dec(int number, int x, int y) {
//   return (NUMBERS_FONT[number][y] >> (7-x)) & 1;
// }

// boolean decc(int chr, int x,int y) {
//   return (TEXT_FONT[chr][x] >> (y)) & 1;
// }

// void drawDigit(int number, 
//                int ox, int oy, 
//                bool update = true,
//                bool clear = false) {
//   for (int y = 0; y < NUM_HEIGHT; y++) {
//     for (int x = 0; x < NUM_WIDTH; x++) {
//       bool d = dec(number,x,y);
//       if (d || update) {
//         if (clear) d = false;
//         setPixel(ox+x,oy+y,d);
//       }
//     }
//   }
// }

// void drawChar(char chr, int ox, int oy) {
//   for (int y = 0; y < TEXT_HEIGHT; y++) {
//     for (int x = 0; x < TEXT_WIDTH; x++) {
//       setPixel(ox+x,oy+y,decc(chr,x,y));
//     }
//   }
// }

#endif