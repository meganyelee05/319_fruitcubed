// Sound.cpp
// Runs on MSPM0
// Sound assets in sounds/sounds.h
// Jonathan Valvano
// 11/15/2021 
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "Sound.h"
#include "sounds/sounds.h"
#include "../inc/DAC5.h"
#include "../inc/Timer.h"
#include "../inc/LaunchPad.h"

static uint32_t ind = 0;
const unsigned char* wave;
unsigned long counter = 0;

void SysTick_IntArm(uint32_t period, uint32_t priority){
  // write this
    SysTick->CTRL = 0x00;
    SysTick->LOAD = period - 1;
    SCB->SHP[1] = (SCB->SHP[1] & (~0xC0000000)) | priority<<30;
    SysTick->VAL = 0;
    SysTick->CTRL = 0x07;
}
// initialize a 11kHz SysTick, however no sound should be started
// initialize any global variables
// Initialize the 5 bit DAC
void Sound_Init(void){
// write this
    SysTick_IntArm(80000000/11000, 0);
    IOMUX->SECCFG.PINCM[PB0INDEX] = 0x0000081;
    IOMUX->SECCFG.PINCM[PB1INDEX] = 0x0000081;
    IOMUX->SECCFG.PINCM[PB2INDEX] = 0x0000081;
    IOMUX->SECCFG.PINCM[PB3INDEX] = 0x0000081;
    IOMUX->SECCFG.PINCM[PB4INDEX] = 0x0000081;
    GPIOB->DOE31_0 |= (1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<4);
}

extern "C" void SysTick_Handler(void);
void SysTick_Handler(void){ // called at 11 kHz
  // output one value to DAC if a sound is active
//    static uint32_t i = 0;
//    GPIOB->DOUT31_0 = (GPIOB->DOUT31_0 & (~0x0F)) | wave[i]; //wave is the sound
//    i = (i+1)&(0x1F);

            DAC5_Out(wave[ind]);
            ind++;
            counter--;
            if(counter == 0){
                SysTick->LOAD = 0;
            }
}


//******* Sound_Start ************
// This function does not output to the DAC. 
// Rather, it sets a pointer and counter, and then enables the SysTick interrupt.
// It starts the sound, and the SysTick ISR does the output
// feel free to change the parameters
// Sound should play once and stop
// Input: pt is a pointer to an array of DAC outputs
//        count is the length of the array
// Output: none
// special cases: as you wish to implement
void Sound_Start(const uint8_t *pt, uint32_t count){
// write this
  wave = pt;
  ind = 0;
  counter = count;
  SysTick->LOAD = 80000000/11000 - 1;
  SysTick->VAL = 0;
}
void Sound_Shoot(void){
// write this
  Sound_Start(shoot, 4080);
}
void Sound_Killed(void){
// write this
  
}
void Sound_Explosion(void){
// write this
  
}
