#ifndef DISPLAY_H
#define DISPLAY_H

#include <PxMatrix.h>

#ifdef ESP8266
  #include <Ticker.h>
  Ticker display_ticker;
  #define P_LAT 16
  #define P_A 5
  #define P_B 4
  #define P_C 15
  #define P_D 12
  #define P_E 0
  #define P_OE 2
#endif

#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 16

// This defines the 'on' time of the display is us. The larger this number,
// the brighter the display. If too large the ESP will crash
#define DISPLAY_DRAW_TIME 60 //30-70 is usually fine

PxMATRIX display(MATRIX_WIDTH,MATRIX_HEIGHT,P_LAT,P_OE,P_A,P_B,P_C);

uint16_t BLUE = display.color565(0, 0, 255);
uint16_t WHITE = display.color565(255, 255, 255);
uint16_t BLACK = display.color565(0, 0, 0);

#ifdef ESP8266
// ISR for display refresh
void displayUpdater() {
  display.display(DISPLAY_DRAW_TIME);
}
#endif

void displayUpdateEnable(bool is_enable) {
#ifdef ESP8266
  if (is_enable) {
    display_ticker.attach(0.002, displayUpdater);
  } else {
    display_ticker.detach();
  }
#endif
}

void displayInit() {
  display.begin(4);
  display.setScanPattern(ZAGZIG);
  displayUpdateEnable(true);
}

uint16_t color = display.color565(random(0, 256), random(0, 256), random(0, 256));
int blub = 0;

void setPixel(int ax, int ay, bool d) {
  display.drawPixel(ax, ay, d ? WHITE : BLACK);
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