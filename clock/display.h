#ifndef DISPLAY_H
#define DISPLAY_H

#include <LEDMatrixDriver.hpp>

// Define the ChipSelect pin for the led matrix (Dont use the SS or MISO pin of your Arduino!)
// Other pins are Arduino specific SPI pins (MOSI=DIN, SCK=CLK of the LEDMatrix) see https://www.arduino.cc/en/Reference/SPI
const uint8_t LEDMATRIX_CS_PIN = D3;

// Number of 8x8 segments you are connecting
const int LEDMATRIX_SEGMENTS = 8;
const int LEDMATRIX_WIDTH    = LEDMATRIX_SEGMENTS * 8;

const int DISPLAY_WIDTH = 8*4;

// The LEDMatrixDriver class instance
LEDMatrixDriver lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN);

void displayInit() {
  lmd.setEnabled(true);
  lmd.setIntensity(2);   // 0 = low, 10 = high
  lmd.clear();
}

void displayUpdate() {
  lmd.display();
}

void setPixel(int ax, int ay, bool d) {
//  int ax = ox+x;
//  int ay = oy+y;
//  
  ax += (ay/8)*32;
  ay %= 8;
  lmd.setPixel(ax,ay,d);
}

boolean dec(int number, int x, int y) {
  return (NUMBERS_FONT[number][y] >> (7-x)) & 1;
}

boolean decc(int chr, int x,int y) {
  return (TEXT_FONT[chr][x] >> (y)) & 1;
}

void drawDigit(int number, 
               int ox, int oy, 
               bool update = true,
               bool clear = false) {
  for (int y = 0; y < NUM_HEIGHT; y++) {
    for (int x = 0; x < NUM_WIDTH; x++) {
      bool d = dec(number,x,y);
      if (d || update) {
        if (clear) d = false;
        setPixel(ox+x,oy+y,d);
      }
    }
  }
}

void drawChar(char chr, int ox, int oy) {
  for (int y = 0; y < TEXT_HEIGHT; y++) {
    for (int x = 0; x < TEXT_WIDTH; x++) {
      setPixel(ox+x,oy+y,decc(chr,x,y));
    }
  }
}

#endif
