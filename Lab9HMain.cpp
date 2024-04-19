// Lab9HMain.cpp
// Runs on MSPM0G3507
// Lab 9 ECE319H
// Megan Lee, Sanjana Kishore
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
        Data = Sensor.In();
        Sensor.Save(Data);

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

int16_t bitmap[160] = {
                   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

// ALL ST7735 OUTPUT MUST OCCUR IN MAIN

uint32_t now = 0, last = 0, limit = 0;
uint32_t olddata = 0;
uint32_t dataj, randnum;
static uint32_t score = 0;
static uint32_t highscore = 0;
uint32_t pressed;
int exittomain = 0;

struct IBlock{
public:
    int32_t x;
    int32_t y;
    const uint16_t *image;
    int16_t h, w;
    bool settled;
public:
    IBlock(int x, int y, int randnum){
        if(randnum == 0){                                       //cucumber
            ST7735_DrawBitmap(x, y, cucumber, 8, 8);
            ST7735_DrawBitmap(x, y-8, cucumber, 8, 8);

            this->x = x / 8;
            this->y = y;
            this->w = 1;
            this->h = 16;
        }
        else if(randnum == 1){                                  //strawberry
            ST7735_DrawBitmap(x, y, strawberry, 8, 8); //bot left corner
            ST7735_DrawBitmap(x, y-8, strawberry, 8, 8); //top left
            ST7735_DrawBitmap(x+8, y-8, strawberry, 8, 8); //top right
            ST7735_DrawBitmap(x+8, y, strawberry, 8, 8); //bot right

            this->x = x / 8;
            this->y = y;
            this->w = 2;
            this->h = 16;
        }
        else if(randnum == 2){
            ST7735_DrawBitmap(x, y, blueberry, 8, 8);             //blueberry

            this->x = x / 8;
            this->y = y;
            this->w = 1;
            this->h = 8;
        }
        else if(randnum == 3){                                  //banana
            ST7735_DrawBitmap(x, y, banana, 8, 8);
            ST7735_DrawBitmap(x+8, y, banana, 8, 8);
            ST7735_DrawBitmap(x+8, y-8, banana, 8, 8);
            ST7735_DrawBitmap(x+8, y-16, banana, 8, 8);
        }
        settled = false;
    }
    void blocksettled(){
        Sound_Killed();
        settled = true;
    }
    void moveblock(int olddata, int dataj, int y, int randnum){
        if(((dataj/8)<this->x) && collision()==1){
                        dataj = olddata;
                    }
                    else if(((dataj/8)>this->x) && collision()==2){
                        dataj = olddata;
                    }
                    else if(((dataj/8) != this->x) && collision()==3){
                        dataj = olddata;
                    }
                    else if((dataj/8)<this->x){
                        this->x = olddata / 8;              //c
                        while(collision()!=1 && (this->x != dataj/8) && collision()!=3){
                            this->x--;                      //c
                        }
                        dataj = this->x * 8;                //c
                    }
                    else if((dataj/8)>this->x){
                        this->x = olddata / 8;              //c
                        while(collision()!=2 && (this->x != dataj/8) && collision()!=3){
                            this->x++;                      //c
                        }
                        dataj = this->x * 8;                //c
                    }
        if(randnum == 0){                                       //cucumber
            ST7735_FillRect(olddata, y-16, 8, 17, 0);
            ST7735_DrawBitmap(dataj, y, cucumber, 8, 8);
            ST7735_DrawBitmap(dataj, y-8, cucumber, 8, 8);
            this->x = dataj / 8;
            this->y = y;
        }
        else if(randnum == 1){
            ST7735_FillRect(olddata, y-16, 16, 17, 0);
            ST7735_DrawBitmap(dataj, y, strawberry, 8, 8); //bot left corner
            ST7735_DrawBitmap(dataj, y-8, strawberry, 8, 8); //top left
            ST7735_DrawBitmap(dataj+8, y-8, strawberry, 8, 8); //top right
            ST7735_DrawBitmap(dataj+8, y, strawberry, 8, 8); //bot right

            this->x = dataj / 8;
            this->y = y;
        }
        else if(randnum == 2){                  //blueberry
            ST7735_FillRect(olddata, y-8, 8, 9, 0);
            ST7735_DrawBitmap(dataj, y, blueberry, 8, 8);

            this->x = dataj / 8;
            this->y = y;
        }
        else if(randnum == 3){
            ST7735_FillRect(olddata, y-8, 8, 9, 0);
            ST7735_FillRect(olddata+8, y-24, 8, 25, 0);
            ST7735_DrawBitmap(dataj, y, banana, 8, 8); //bot left
            ST7735_DrawBitmap(dataj+8, y, banana, 8, 8);
            ST7735_DrawBitmap(dataj+8, y-8, banana, 8, 8);
            ST7735_DrawBitmap(dataj+8, y-16, banana, 8, 8);

            this->x = dataj / 8;
            this->y = y;
        }
    }

    int collision(){
        for(int i = this->y; i > (this->y - this->h); i--){
            if(this->x < 15 && this->x > 0 && (bitmap[i] & (0x8000 >> (this->x + this->w))) && (bitmap[i] & (0x8000 >> (this->x - 1)))){
                return 3; //collision on both sides
            }
            if(this->x < 15 && (bitmap[i] & (0x8000 >> (this->x + this->w)))){
                return 2; //2 means right collision
            }
            if(this->x > 0 && (bitmap[i] & (0x8000 >> (this->x - 1)))){
                return 1; //1 means left collision
            }
        }
        return 0;
    }
};

//return 0 if has space, 1 if no space (gotta stop)
bool checker(IBlock block){
    for(int i = 0; i < block.w; i++){
        if(((bitmap[block.y + 1]) & (0x8000 >> (block.x + i))) != 0)
            return 1;
    }
    return 0;
}

void setmap(IBlock block){
    for(int j = 0; j < block.h; j++){
        for(int i = 0; i < block.w; i++){
            bitmap[block.y - j] |= (0x8000 >> (block.x + i));
        }
    }
}

void gameover(void){
    exittomain = 0;
    Sound_Shoot(); //game over sound
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(0, 7);
    ST7735_OutString((char *)Phrases[GG][myLanguage]);
    ST7735_SetCursor(0, 8);
    ST7735_OutString((char *)Phrases[SCORE][myLanguage]);
    printf("%d", score);
    if(score>highscore){
        highscore = score;
    }
    ST7735_SetCursor(0, 9);
    ST7735_OutString((char *)Phrases[HS][myLanguage]);
    printf("%d", highscore);
    for(int i = 0; i < 160; i++){
        bitmap[i] = 0;
    }
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
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_DrawFastHLine(0, 16, 127, 11);
    //nested while loop- outer loop generates sprites randomly, inner loop moves sprite
    while(1){
        if(exittomain) break;
        //initialize
        TimerG12_IntArm(80000000/30,1);
        pressed = 0;
        int y = 16;
        randnum = Random(3);
        //if 0, cucumber
        //if 1, strawberry
        //if 2, blueberry
          Sensor.Sync();
          if(randnum == 0) dataj = (Data * 16 / 4095) * 8;
          else if(randnum == 1) dataj = (Data * 15 / 4095) * 8;
          else if(randnum == 2) dataj = (Data * 16 / 4095) * 8;
          else if(randnum == 3) dataj = (Data * 15 / 4095) * 8;


        IBlock block(dataj, y, randnum);
        olddata = dataj;
        while(1){

            if(pressed == 0){
              Sensor.Sync();
              if(randnum == 0) dataj = (Data * 16 / 4095) * 8;
              else if(randnum == 1) dataj = (Data * 15 / 4095) * 8;
              else if(randnum == 2) dataj = (Data * 16 / 4095) * 8;
            }


            block.moveblock(olddata, dataj, y, randnum);
            ST7735_DrawFastHLine(0, 16, 127, 11);

            olddata = dataj;
            //if button pressed, drop
            last = now;
            now = Switch_In();
            if(now != last){
                if(now == 1){
                    TimerG12_IntArm(80000000/3000,1);
                    pressed = 1;
                }
            }
            //check if space below is clear
            if(block.y > 159 || checker(block)){                //block hits bottom or contact with another block
              block.blocksettled();
              score += (block.w * block.h / 4);
              //update bitmap
              setmap(block);
              Clock_Delay(80000000);
              if((block.y - block.h) <= 16) //if block is settled + over the limit
                  exittomain = 1;
              break;
            }
            y++;
        }
    }
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
  TimerG12_IntArm(80000000/30,1);
  // initialize all data structures
  __enable_irq();

  while(1){
    //main title screen
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
  }
}
