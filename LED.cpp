/*
 * LED.cpp
 *
 *  Created on: Nov 5, 2023
 *      Author:
 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
// LaunchPad.h defines all the indices into the PINCM table

// initialize your LEDs
void LED_Init(void){
    // write this
    //PB12, PB13, PB16
    IOMUX->SECCFG.PINCM[PB12INDEX] = 0x00000081;
    IOMUX->SECCFG.PINCM[PB13INDEX] = 0x00000081;
    IOMUX->SECCFG.PINCM[PB16INDEX] = 0x00000081;

    GPIOB->DOE31_0 |= (1<<12) | (1<<13) | (1<<16);
    
}
// data specifies which LED to turn on
void LED_On(uint32_t data){
    // write this
    // use DOUTSET31_0 register so it does not interfere with other GPIO
  GPIOB->DOUTSET31_0 = (1<<data);
}

// data specifies which LED to turn off
void LED_Off(uint32_t data){
    // write this
    // use DOUTCLR31_0 register so it does not interfere with other GPIO
    GPIOB->DOUTCLR31_0 = (1<<data);
}

// data specifies which LED to toggle
void LED_Toggle(uint32_t data){
    // write this
    // use DOUTTGL31_0 register so it does not interfere with other GPIO
    GPIOB->DOUTTGL31_0 = (1<<data);
}
