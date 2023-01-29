#ifndef DISPLAY_H
#define DISPLAY_H

#include "quarter_scan_panel.h" // Virtual Display to re-map co-ordinates such that they draw correctly on a 32x16 1/4 Scan panel 
#include <Wire.h>
#include <FastLED.h>

// uncomment to use custom pins, then provide below
#define USE_CUSTOM_PINS 

/* Pin 1,3,5,7,9,11,13,15 */
#define R1_PIN  16
#define B1_PIN  17
#define R2_PIN  5
#define B2_PIN  18
#define A_PIN  19
#define C_PIN  21
#define CLK_PIN  23
#define OE_PIN  33

/* Pin 2,6,10,12,14 */
#define G1_PIN  4
#define G2_PIN  26
#define B_PIN  15
#define D_PIN  13
#define LAT_PIN  25
#define E_PIN  -1 // required for 1/32 scan panels

MatrixPanel_I2S_DMA dmaOutput;

// Create virtual 1/2 to 1/4 scan pixel co-ordinate mapping class.
QuarterScanMatrixPanel display(dmaOutput);

bool displayEnabled = true;

uint16_t BLUE = display.color565(0, 0, 255);
uint16_t WHITE = display.color565(255, 0, 0);
uint16_t BLACK = display.color565(0, 0, 0);

void displayInit() {
  dmaOutput.begin(R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN );  // setup the LED matrix
  dmaOutput.setLatBlanking(2);

  display.fillScreen(display.color444(0, 0, 0));
}

void displayUpdate() {
  display.fillScreen(display.color444(0, 0, 0));
  display.setTextFGColor(display.color565(0,60,255));

  display.setCursor(0,5);
  display.write("HURRA");
  // delay(timeout);
}

void setPixel(int ax, int ay, bool d) {
  display.drawPixel(ax, ay, d ? WHITE : BLACK);
}

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