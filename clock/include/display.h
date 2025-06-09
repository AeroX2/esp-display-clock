#ifndef DISPLAY_H
#define DISPLAY_H

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

#include "buffer_scan_panel.h"

#define R1_PIN  GPIO_NUM_2
#define G1_PIN  GPIO_NUM_15
#define B1_PIN  GPIO_NUM_4
#define R2_PIN  GPIO_NUM_16
#define G2_PIN  GPIO_NUM_27
#define B2_PIN  GPIO_NUM_17

#define A_PIN   GPIO_NUM_5
#define B_PIN   GPIO_NUM_18
#define C_PIN   GPIO_NUM_19
#define D_PIN   GPIO_NUM_21
#define E_PIN   GPIO_NUM_12
#define LAT_PIN GPIO_NUM_26
#define OE_PIN  GPIO_NUM_25

#define CLK_PIN GPIO_NUM_22

HUB75_I2S_CFG mxconfig(DISPLAY_WIDTH, DISPLAY_HEIGHT, 1, 
                       {R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN,
                        B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN},
                        HUB75_I2S_CFG::shift_driver::FM6124,
                        false, HUB75_I2S_CFG::HZ_8M, 1, false);

MatrixPanel_I2S_DMA dmaOutput(mxconfig);

// Create a virtual scan matrix to keep track of the pixel data
BufferMatrixPanel display(1, 1, DISPLAY_WIDTH, DISPLAY_HEIGHT);

void displayInit() {
  dmaOutput.begin();
  dmaOutput.setRotation(2);
  dmaOutput.setBrightness(170);
  display.setDisplay(dmaOutput);
  display.clearScreen();
}

#endif