/*
 * Switch.cpp
 *
 *  Created on: Nov 5, 2023
 *      Author:
 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
// LaunchPad.h defines all the indices into the PINCM table
void Switch_Init(void){
    // write this
    //PA18, PA17, PA16
    IOMUX->SECCFG.PINCM[PA18INDEX] = 0x00050081;
    IOMUX->SECCFG.PINCM[PA17INDEX] = 0x00050081;
    IOMUX->SECCFG.PINCM[PA16INDEX] = 0x00050081;
}
// return current state of switches
uint32_t Switch_In(void){
    // write this
  return ((GPIOA->DIN31_0 >> 16) & 7); //replace this your code
}
