#include "display.h"

HUB75_I2S_CFG mxconfig(DISPLAY_WIDTH, DISPLAY_HEIGHT, 1, 
                       {R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN,
                        B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN},
                        HUB75_I2S_CFG::shift_driver::FM6124,
                        false, HUB75_I2S_CFG::HZ_8M, 1, false);

MatrixPanel_I2S_DMA dmaOutput(mxconfig);

// Create a virtual display panel for rendering
BufferMatrixPanel display(1, 1, DISPLAY_WIDTH, DISPLAY_HEIGHT);

void displayInit() {
  dmaOutput.begin();
  dmaOutput.setRotation(2);
  dmaOutput.setBrightness(170);
  display.setDisplay(dmaOutput);
  display.clearScreen();
}