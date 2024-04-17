// Lab9HMain.cpp
// Runs on MSPM0G3507
// Lab 9 ECE319H
// Your name
// Last Modified: 1/1/2024

#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/SlidePot.h"
#include "../inc/DAC5.h"
#include "SmallFont.h"
#include "LED.h"
#include "Switch.h"
#include "Sound.h"
#include "images/images.h"
extern "C" void __disable_irq(void);
extern "C" void __enable_irq(void);
extern "C" void TIMG12_IRQHandler(void);
// ****note to ECE319K students****
// the data sheet says the ADC does not work when clock is 80 MHz
// however, the ADC seems to work on my boards at 80 MHz
// I suggest you try 80MHz, but if it doesn't work, switch to 40MHz
void PLL_Init(void){ // set phase lock loop (PLL)
  // Clock_Init40MHz(); // run this line for 40MHz
  Clock_Init80MHz(0);   // run this line for 80MHz
}

uint32_t M=1;
uint32_t Random32(void){
  M = 1664525*M+1013904223;
  return M;
}
uint32_t Random(uint32_t n){
  return (Random32()>>16)%n;
}

SlidePot Sensor(1500,0); // copy calibration from Lab 7

uint32_t Data; //12-bit ADC

// games  engine runs at 30Hz
void TIMG12_IRQHandler(void){uint32_t pos,msg;
  if((TIMG12->CPU_INT.IIDX) == 1){ // this will acknowledge
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
// game engine goes here
    // 1) sample slide pot
    // 2) read input switches
    // 3) move sprites
    // 4) start sounds
    // 5) set semaphore
    // NO LCD OUTPUT IN INTERRUPT SERVICE ROUTINES
    if((TIMG12->CPU_INT.IIDX) == 1){
        Data = Sensor.In();
        Sensor.Save(Data);
    }
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
  }
}
uint8_t TExaS_LaunchPadLogicPB27PB26(void){
  return (0x80|((GPIOB->DOUT31_0>>26)&0x03));
}

typedef enum {English, Spanish} Language_t;
Language_t myLanguage=English;
typedef enum {PLAY, INSTRUCT, LANGUAGE, SCORE, LINE1, LINE2, LINE3, LINE4, GG, HS} phrase_t;
const char Play_English[] ="Play";
const char Play_Spanish[] ="Juega";
const char Score_Spanish[]="Puntuaci\xA2n: ";
const char Score_English[]="Score: ";
const char Language_English[]="English";
const char Language_Spanish[]="Espa\xA4ol";
const char Instruct_English[]="How to Play";
const char Instruct_Spanish[]="C\xA2mo Jugar";
const char Line1_English[]="Move blocks with";
const char Line1_Spanish[]="Mover bloques con";
const char Line2_English[]="slidepot, use SW3";
const char Line2_Spanish[]="slidepot, usar SW3";
const char Line3_English[]="to rotate and SW1";
const char Line3_Spanish[]="para rotar y SW1";
const char Line4_English[]="to drop blocks";
const char Line4_Spanish[]="para soltar";
const char GG_English[]="Game Over";
const char GG_Spanish[]="Fin";
const char HS_English[]="High Score: ";
const char HS_Spanish[]="Puntuaci\xA2n alta: ";

const char *Phrases[10][2]={
  {Play_English,Play_Spanish},
  {Instruct_English,Instruct_Spanish},
  {Language_English,Language_Spanish},
  {Score_English,Score_Spanish},
  {Line1_English, Line1_Spanish},
  {Line2_English, Line2_Spanish},
  {Line3_English, Line3_Spanish},
  {Line4_English, Line4_Spanish},
  {GG_English, GG_Spanish},
  {HS_English, HS_Spanish}
};
// use main1 to observe special characters
int main1(void){ // main1
    char l;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
  ST7735_FillScreen(0x0000);            // set screen to black
  for(int myPhrase=0; myPhrase<= 2; myPhrase++){
    for(int myL=0; myL<= 1; myL++){
         ST7735_OutString((char *)Phrases[LANGUAGE][myL]);
      ST7735_OutChar(' ');
         ST7735_OutString((char *)Phrases[myPhrase][myL]);
      ST7735_OutChar(13);
    }
  }
  Clock_Delay1ms(3000);
  ST7735_FillScreen(0x0000);       // set screen to black
  l = 128;
  while(1){
    Clock_Delay1ms(2000);
    for(int j=0; j < 3; j++){
      for(int i=0;i<16;i++){
        ST7735_SetCursor(7*j+0,i);
        ST7735_OutUDec(l);
        ST7735_OutChar(' ');
        ST7735_OutChar(' ');
        ST7735_SetCursor(7*j+4,i);
        ST7735_OutChar(l);
        l++;
      }
    }
  }
}

// use main2 to observe graphics
int main2(void){ // main2
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
  Switch_Init();

  uint32_t now = 0;
  uint32_t last = 0;
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  while(1){
      ST7735_FillScreen(ST7735_BLACK);
      ST7735_DrawBitmap(0, 57, fruitcubedbannertest, 128,57); // player ship bottom
      ST7735_SetCursor(5, 7);
      ST7735_OutString((char *)Phrases[PLAY][myLanguage]);
      ST7735_SetCursor(5, 9);
      ST7735_OutString((char *)Phrases[INSTRUCT][myLanguage]);
      ST7735_SetCursor(5, 11);
      ST7735_OutString((char *)Phrases[LANGUAGE][myLanguage]);
      while(1){
          last = now;
          Clock_Delay(800000);
          now = Switch_In();
          if(now != last){
              if(now == 4){
                  if(myLanguage == English){
                      myLanguage = Spanish;
                  }
                  else{
                      myLanguage = English;
                  }
                  break;
              }
          }
      }
  }

//  ST7735_DrawBitmap(53, 151, Bunker0, 18,5);
//  ST7735_DrawBitmap(42, 159, PlayerShip1, 18,8); // player ship bottom
//  ST7735_DrawBitmap(62, 159, PlayerShip2, 18,8); // player ship bottom
//  ST7735_DrawBitmap(82, 159, PlayerShip3, 18,8); // player ship bottom
//  ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
//  ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
//  ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
//  ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
//  ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);
//
//  for(uint32_t t=500;t>0;t=t-5){
//    SmallFont_OutVertical(t,104,6); // top left
//    Clock_Delay1ms(50);              // delay 50 msec
//  }
//  ST7735_FillScreen(0x0000);   // set screen to black
//  ST7735_SetCursor(1, 1);
//  ST7735_OutString((char *)"GAME OVER");
//  ST7735_SetCursor(1, 2);
//  ST7735_OutString((char *)"Nice try,");
//  ST7735_SetCursor(1, 3);
//  ST7735_OutString((char *)"Earthling!");
//  ST7735_SetCursor(2, 4);
//  ST7735_OutUDec(1234);
  while(1){
  }
}

// use main3 to test switches and LEDs
int main3(void){ // main3
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  ST7735_FillScreen(ST7735_BLACK);
  uint32_t last = 0, now;
  ST7735_SetCursor(0, 0);
  while(1){
    // write code to test switches and LEDs
   now = Switch_In();
   if(now != last){
       if(now == 2){
//           GPIOB->DOUTTGL31_0 = (1<<12);
           LED_Toggle(12);
           ST7735_OutString((char *)"Switch 2 pressed\n");
       }
       else if(now == 4){
//           GPIOB->DOUTTGL31_0 = (1<<13);
           LED_Toggle(13);
           ST7735_OutString((char *)"Switch 3 pressed\n");
       }
       else if(now == 0){
           ST7735_OutString((char *)"Nothing pressed\n");
       }
       else if(now == 1){
//           GPIOB->DOUTTGL31_0 = (1<<16);
           LED_Toggle(16);
           ST7735_OutString((char *)"Switch 1 pressed\n");
       }
   }
   last = now;
   Clock_Delay(800000);
  }
}
// use main4 to test sound outputs
int main4(void){ uint32_t last=0,now;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(ADC0,6,0); // ADC1 channel 6 is PB20, TExaS scope
  __enable_irq();

  // testing




  while(1){
    now = Switch_In(); // one of your buttons
    if((last == 0)&&(now == 1)){
      Sound_Shoot(); // call one of your sounds
      LED_Toggle(16);
    }
    if((last == 0)&&(now == 2)){
      Sound_Killed(); // call one of your sounds
    }
    if((last == 0)&&(now == 4)){
      Sound_Explosion(); // call one of your sounds
    }
    if((last == 0)&&(now == 8)){
      //Sound_Fastinvader1(); // call one of your sounds
    }
    // modify this to test all your sounds
    last = now;
  }
}
// ALL ST7735 OUTPUT MUST OCCUR IN MAIN

uint32_t now = 0, last = 0, limit = 0;
uint32_t olddata = 0;
uint32_t dataj, randnum;
static uint32_t score = 0;
static uint32_t highscore = 0;

struct IBlock{
private:
    int32_t x;
    int32_t y;
    const uint16_t *image;
    int16_t h, w;
    bool settled;
public:
    IBlock(int x, int y, int randnum){
        if(randnum == 0){                                       //cucumber
            ST7735_DrawBitmap(x, y, cucumber, 8, 8);
            ST7735_DrawBitmap(x+8, y, cucumber, 8, 8);
            ST7735_DrawBitmap(x+16, y, cucumber, 8, 8);
            ST7735_DrawBitmap(x+24, y, cucumber, 8, 8);
        }
        else if(randnum == 1){
            ST7735_DrawBitmap(x, y, strawberry, 8, 8); //bot left corner
            ST7735_DrawBitmap(x, y-8, strawberry, 8, 8); //top left
            ST7735_DrawBitmap(x+8, y-8, strawberry, 8, 8); //top right
            ST7735_DrawBitmap(x+8, y, strawberry, 8, 8); //bot right
        }
        else if(randnum == 2){
            ST7735_DrawBitmap(x, y, banana, 8, 8);
            ST7735_DrawBitmap(x+8, y, banana, 8, 8);
            ST7735_DrawBitmap(x+8, y-8, banana, 8, 8);
            ST7735_DrawBitmap(x+8, y-16, banana, 8, 8);
        }
        //fill in rest
        settled = false;
    }
    void blocksettled(){
        settled = true;
    }
    void moveblock(int olddata, int dataj, int y, int randnum){
        if(randnum == 0){                                       //cucumber
            ST7735_FillRect(olddata, y-8, 32, 9, 0);
            ST7735_DrawBitmap(dataj, y, cucumber, 8, 8);
            ST7735_DrawBitmap(dataj+8, y, cucumber, 8, 8);
            ST7735_DrawBitmap(dataj+16, y, cucumber, 8, 8);
            ST7735_DrawBitmap(dataj+24, y, cucumber, 8, 8);
        }
        else if(randnum == 1){
            ST7735_FillRect(olddata, y-16, 16, 18, 0);
            ST7735_DrawBitmap(dataj, y, strawberry, 8, 8); //bot left corner
            ST7735_DrawBitmap(dataj, y-8, strawberry, 8, 8); //top left
            ST7735_DrawBitmap(dataj+8, y-8, strawberry, 8, 8); //top right
            ST7735_DrawBitmap(dataj+8, y, strawberry, 8, 8); //bot right
        }
        else if(randnum == 2){
            ST7735_FillRect(olddata, y-8, 8, 9, 0);
            ST7735_FillRect(olddata+8, y-24, 8, 25, 0);
            ST7735_DrawBitmap(dataj, y, banana, 8, 8);
            ST7735_DrawBitmap(dataj+8, y, banana, 8, 8);
            ST7735_DrawBitmap(dataj+8, y-8, banana, 8, 8);
            ST7735_DrawBitmap(dataj+8, y-16, banana, 8, 8);
        }
        //fill in rest
    }
};

void gameover(void){
    Sound_Shoot(); //game over sound
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(2, 7);
    ST7735_OutString((char *)Phrases[GG][myLanguage]);
    ST7735_SetCursor(2, 8);
    ST7735_OutString((char *)Phrases[SCORE][myLanguage]);
    printf("%d", score);
    if(score>highscore){
        highscore = score;
    }
    ST7735_SetCursor(2, 9);
    ST7735_OutString((char *)Phrases[HS][myLanguage]);
    printf("%d", highscore);
    while(1){
            last = now;
            Clock_Delay(800000);
            now = Switch_In();
            if(now != last){
                if(now == 1 || now == 2 || now == 4) break;
            }
        }
}

void gameplay(void){
    score = 0;
    int spdelay = 800000; int velocity = 8000000;
    ST7735_FillScreen(ST7735_BLACK);
    //nested while loop- outer loop generates sprites randomly, inner loop moves sprite
    while(1){
        spdelay = 800000;
        velocity = 8000000;
        int y = 16;
        randnum = Random(3);
        //if 0, cucumber
        //if 1, strawberry
        if(randnum == 0) dataj = (Sensor.In() * 13 / 4095) * 8;
        else if(randnum == 1) dataj = (Sensor.In() * 15 / 4095) * 8;
        else if(randnum == 2) dataj = (Sensor.In() * 15 / 4095) * 8;
        IBlock block(dataj, y, randnum);
        olddata = dataj;
        while(1){
            if(randnum == 0) dataj = (Sensor.In() * 13 / 4095) * 8;
            else if(randnum == 1) dataj = (Sensor.In() * 15 / 4095) * 8;
            else if(randnum == 2) dataj = (Sensor.In() * 15 / 4095) * 8;
            //call movement based on olddata, y, dataj, randnum
            block.moveblock(olddata, dataj, y, randnum);

            olddata = dataj;
            //if button pressed change x and y
            last = now;
            now = Switch_In();
            if(now != last){
                if(now == 1){
                    spdelay = 800;
                    velocity = 800;
                }
            }
            Clock_Delay(spdelay);
            y++;
            if(y > 159){                //block hits bottom or contact with another block
              block.blocksettled();
              Clock_Delay(80000000);
              break;
            }
            Clock_Delay(velocity);
        }
    }
    //IF LESS THAN 16
    gameover();
}

void HowTo(void){
    uint32_t inputs;
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(0, 0);
    ST7735_OutString((char *)Phrases[LINE1][myLanguage]);
    ST7735_SetCursor(0, 1);
    ST7735_OutString((char *)Phrases[LINE2][myLanguage]);
    ST7735_SetCursor(0, 2);
    ST7735_OutString((char *)Phrases[LINE3][myLanguage]);
    ST7735_SetCursor(0, 3);
    ST7735_OutString((char *)Phrases[LINE4][myLanguage]);
    ST7735_SetCursor(0, 4);
    ST7735_OutString((char *)"GLHF :P");
    while(1){
        last = now;
        Clock_Delay(800000);
        now = Switch_In();
        if(now != last){
            if(now == 1 || now == 2 || now == 4) break;
        }
    }
    return;
}

int main(void){ // final main
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  ST7735_FillScreen(ST7735_BLACK);
  Sensor.Init(); // PB18 = ADC1 channel 5, slidepot
  Switch_Init(); // initialize switches
  LED_Init();    // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(0,0,&TExaS_LaunchPadLogicPB27PB26); // PB27 and PB26
    // initialize interrupts on TimerG12 at 30 Hz
  TimerG12_IntArm(80000000/30,2);
  // initialize all data structures
  __enable_irq();

  while(1){
    // wait for semaphore
      // clear semaphore
      //Sensor.Sync();

       // update ST7735R
       ST7735_FillScreen(ST7735_BLACK);
       ST7735_DrawBitmap(0, 57, fruitcubedbannertest, 128,57); // title banner
       ST7735_SetCursor(5, 7);
       ST7735_OutString((char *)Phrases[PLAY][myLanguage]);
       ST7735_SetCursor(5, 9);
       ST7735_OutString((char *)Phrases[INSTRUCT][myLanguage]);
       ST7735_SetCursor(5, 11);
       ST7735_OutString((char *)Phrases[LANGUAGE][myLanguage]);
       while(1){
          last = now;
          Clock_Delay1ms(10);
          now = Switch_In();
          if(now != last){
              break;
          }
       }
       if(now == 2){
           if(myLanguage == English){
               myLanguage = Spanish;
           }
           else{
               myLanguage = English;
           }
       }
       else if(now == 1){
           ST7735_FillScreen(ST7735_BLACK);
           gameplay();
       }
       else if(now == 4){
           HowTo();
       }
    // check for end game or level switch
  }
}
