#ifndef DISPLAY_H
#define DISPLAY_H

#include "quarter_scan_panel.h"  // Virtual Display to re-map co-ordinates such that they draw correctly on a 32x16 1/4 Scan panel

/* Pin 1,3,5,7,9,11,13,15 */
#define R1_PIN 16
#define B1_PIN 17
#define R2_PIN 5
#define B2_PIN 18
#define A_PIN 19
#define C_PIN 21
#define CLK_PIN 23
#define OE_PIN 33

/* Pin 2,6,10,12,14 */
#define G1_PIN 4
#define G2_PIN 26
#define B_PIN 15
#define D_PIN 13
#define LAT_PIN 25
#define E_PIN -1  // required for 1/32 scan panels

HUB75_I2S_CFG mxconfig(64, 8, 1,
                       {R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN,
                        B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN},
                       HUB75_I2S_CFG::SHIFTREG, false, HUB75_I2S_CFG::HZ_10M, 1,
                       false);

MatrixPanel_I2S_DMA dmaOutput(mxconfig);

// Create virtual 1/2 to 1/4 scan pixel co-ordinate mapping class.
QuarterScanMatrixPanel display(dmaOutput, 1, 1, 32, 16);

void displayInit() {
  dmaOutput.begin();
  dmaOutput.setBrightness(100);
  display.clearScreen();
}

#endif