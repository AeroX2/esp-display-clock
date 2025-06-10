#ifndef DISPLAY_H
#define DISPLAY_H

#include "display_config.h"
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

extern HUB75_I2S_CFG mxconfig;
extern MatrixPanel_I2S_DMA dmaOutput;
extern BufferMatrixPanel display;

void displayInit();

#endif