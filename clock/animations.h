#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "display.h"
#include "font.h"

bool sleep = false;
int sleepTime = 0;

void sleep(void (*ptr)()) 


void tetris(int ox, int oy, int number) {
  for (let i = NUM_HEIGHT-1; i >= 0; i--) {
    let line = NUMBERS_FONT[number][i];
    if (line == 0) continue;

    for (let y = 0; y <= i; y++) {
      if (y > 0) {
        for (let x = 0; x < NUM_WIDTH; x++) {
          setPixel(ox+x,oy+y,false);
        }
      }

      for (let x = 0; x < NUM_WIDTH; x++) {
        setPixel(ox+x,oy+y,(line >> WIDTH-x) & 1);
      }

      // TODO Figure out a solution for this sleep?
      //await sleep(50);
    }
  }
}
