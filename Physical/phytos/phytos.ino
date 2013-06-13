/*
 * An Arduino controller for LED strings based on the SM16716 controller.
 *
 * By Mike Tsao (github.com/sowbug). Copyright 2012, all rights reserved.
 *
 * You might see these being sold as WS2801 strands, and you're wondering why
 * your off-the-shelf WS2801 code isn't working. This sketch will restore
 * your confidence that your lights aren't broken.
 *
 * This code isn't the fastest, but it is just about the simplest you'll
 * find. For performance, inv estigate code that uses hardware SPI.
 *
 * Beware that everyone seems to use different wiring color schemes. I
 * found the order of the wires is more reliable. Looking at the IC side
 * of the board, you'll find from left to right: V+, Data, Clock, Ground.
 */
 

#include <Adafruit_WS2801.h>
#include "SPI.h"

#define DATA_PIN (2)
#define CLOCK_PIN (3)

#define LIGHT_COUNT (72)
#define SB_BUFFER_SIZE (64)
#define DEVICE_ID (1)
#define PARAMETER_MAX (255)

Adafruit_WS2801 strip = Adafruit_WS2801(LIGHT_COUNT, DATA_PIN, CLOCK_PIN);

uint32_t pixels[LIGHT_COUNT];

struct led
{
  int x;
  int y;
  //char r;
  //char g;
  //char b;
  char id;
  char type;
};

//#define pinModeFast(x, y) pinMode(x, y)
//#define digitalWriteFast(x, y) digitalWrite(x, y)
//SerialCommand SCmd;   // The demo SerialCommand object

//pseudo led class field values
/*
enum fields{
  X,
  Y,
  R,
  G,
  B,
  T   //0 for plankton, 1 for element
};
*/
//state values
enum states{
  RESTING,
  GAME,
  CALIBRATE
};

enum types{
  PHYTOS,
  CHEMS 
};

float windowY = 48;
float windowX = 26;

char sb [SB_BUFFER_SIZE]; //sb = serialbuffer
int sbpos = 0;

int state = 0;

led leds [LIGHT_COUNT];
int top = -1;
int buffer [LIGHT_COUNT];

int nitrogen = 0;
int ph = 0;
int temperature = 0;

int TIME = 0;

void setup() {
  
  Serial.begin(9600);

  //int leds[LIGHT_COUNT]; // (x,y,r,g,b)
  init_leds();
      
//  Serial.write("Set up!!!");
  //pinModeFast(DATA_PIN, OUTPUT);
  //pinModeFast(CLOCK_PIN, OUTPUT);
  //digitalWriteFast(DATA_PIN, LOW);
  //digitalWriteFast(CLOCK_PIN, LOW);
  Serial.println("Ready"); 
  
  strip.begin();

  // Update LED contents, to start they are all 'off'
  strip.show();

}


void loop() {
  //delay(1000);
  //Serial.println("hello");
  readNextCommand();
  Serial.println(state);
  TIME = millis(); //will be changed to take time from dispatch arduino
  switch(state)
  {
    case RESTING: solid(CHEMS);
                  solid(PHYTOS);
                  break;
    case    GAME: game(nitrogen, ph, temperature);
                  break;
    case CALIBRATE: calibrate();
                  break;
    default     : tyker(PHYTOS);
                  tyker(CHEMS);
  }
  
  /*
  while(!isEmpty())
  {
     //int popped = pop();
     //set_pixel_rgb(popped, leds[popped][B], leds[popped][G], leds[popped][R]);
     //strip.setPixelColor(popped, leds[popped].r, leds[popped].g, leds[popped].b);
  }
  */
  //show();
  //Serial.println("showing");
  strip.show();
}

void game(int nitrogen, int ph, int temperature)
{
   for(int i = 0; i < LIGHT_COUNT; ++i)
   {
      //temperature_control(i, temperature);
   }
}

void wipe()
{
  for(int i = 0; i < LIGHT_COUNT; ++i)
  {
   strip.setPixelColor(i, 0, 0, 0); 
  }
}

//shows BLUE
void solid(int type)
{
  
   for(int i = 0; i < LIGHT_COUNT; ++i)
   {
     //if(leds[i][X] == 0 || leds[i][T] != type) continue;
     //leds[i].r = 0;
     //leds[i].g = 0;
     //leds[i].b = 255;
     //push(i);
     strip.setPixelColor(i, 0, 0, 255);
   }
   
}

//float counter = (millis() / 5000) % 400;
void tyker(int type)
{      
     delay(10);
     float control =  (TIME / 2000.0) ;
     float center = sin((PI / 2) * control) * windowY * 2 - (windowY / 2);
     for(int i = 0; i < LIGHT_COUNT; ++i)
     {
        if (leds[i].y == 0 || leds[i].type != type) continue;
        float distance = abs(leds[i].y - center);
        //Serial.println(distance);
        //int v = (int) ((distance / (windowX * 2)) * 255) + (int) (20 * (float) ((i % 7) - 3));
        int v = (int) ((distance / (windowY * 2)) * 127);
//       Serial.println(v);
        //leds[i].r = v;
        push(i);
     }
     
}

//waves on the beach
void tyker2(int type)
{      
//     Serial.println("Tyker");
     delay(10);
     float control =  (TIME / 5000.0) ;
     //Serial.println(counter);
     float center = sin(TWO_PI * control) * windowY;
     //Serial.print("center = ");
     //Serial.println(center);
     for(int i = 0; i < LIGHT_COUNT; ++i)
     {
        if (leds[i].y == 0 || leds[i].type != type) continue;
        float distance = abs(leds[i].y - center);
        //Serial.println(distance);
        //int v = (int) ((distance / (windowX * 2)) * 255) + (int) (20 * (float) ((i % 7) - 3));
        int v = (int) ((distance / (windowY * 2)) * 255);
//       Serial.println(v);
        //leds[i].r = v;
        //leds[i].g = v;
        //leds[i].b = v;
        //push(i);
     }
     
}

void calibrate(){
  wipe();
  for(int i = 0; i < LIGHT_COUNT; )
  {
   if(Serial.read() != -1){
     wipe();
     strip.setPixelColor(i, 255,255,255);
     ++i;
   }
   strip.show();
   delay(15); 
  }
  Serial.write("calibration over");
  state = 0;
}

////////////////////

/*
a machine that uses this method will receive serial commands
in this form, minus the quotation marks.
    
  ------number of bytes in this message  
  | --- device ID number (0 means all, if first character isn't 0 this string
  | |   is parsed to determine if command is designated for this machine.  
  | | ---- state ID
  | | |  ------- additonal arguments (optional, determined by state ID)      
  | | |  |  |  |           
"15 0 1 45 36 64"

The individual arguments here are given in order of preference.  If a buffer is
in danger of overflowing, the byte size allows that machine to trash those
bytes if it needs to.  This would result in a loss of resolution, but it avoids
having to develop a handshake system which could be fatally time consuming.  

For questions ask Tyler.

*/
void readNextCommand_arduino()
{
 char term = '\n';
 if(Serial.available() == 0) return;
 char msgSize = Serial.read();
 char targetid = Serial.read();
 if(targetid != 0 && !isCommandForMe_arduino()) return;
 state = Serial.read();
 parseExtraArgs_arduino(state);
}

boolean isCommandForMe_arduino()
{
 char inByte = Serial.read();
 while(inByte != ':')
 {
   if (inByte == DEVICE_ID) return true; 
 }
 return false;
}

void parseExtraArgs_arduino(int state)
{
  switch(state)
  {
    case GAME: nitrogen = Serial.read();
               ph = Serial.read();
               temperature = Serial.read();
               break;
    default  : return;
  } 
}

void readNextCommand()
{
  //Serial.println(sb);
  char term = '\n';
  if(Serial.available() > 0)
  {
    char inChar = Serial.read();
    if (inChar == term)
    {
      //Serial.println("Received newline");
      //Serial.println(sb);
      char* cmdbuf;
      cmdbuf = strtok(sb, " ");
      //Serial.println(1);
      int cmdsize = atoi(cmdbuf);
      cmdbuf = strtok(NULL, " ");
      //Serial.println(2);
      if (!isCommandForMe(cmdbuf)) return;
      cmdbuf = strtok(NULL, " ");
      //Serial.println(3);
      state = atoi(cmdbuf);
      Serial.print("State =");
      Serial.println(state);
      clearsb();
    }
    else 
    {
     if (sbpos < SB_BUFFER_SIZE)
     {
       sb[sbpos++] = inChar;
       sb[sbpos] = '\0';
     }
     else 
     {
       Serial.println("Buffer overflow"); 
     }
    }
  }
}

void clearsb()
{
  sb[0] = '\0';
  sbpos = 0; 
}

boolean isCommandForMe(char* str)
{
  //Serial.print("icfm");
  //Serial.println(str);
  if (str[0] == '0') return true; 
  int i = 0;
  while(str[i] != NULL)
  {
    //Serial.println(str[i]);
    if(str[i] == DEVICE_ID) return true;
    ++i; 
  }
  return false;
}

void parseExtraArgs(int state)
{
  char* argbuf;
  switch(state)
  {
   case GAME: argbuf = strtok(NULL , " ");
              nitrogen = atoi(argbuf);
              argbuf = strtok(NULL, " ");
              ph = atoi(argbuf);
              argbuf = strtok(NULL, " ");
              temperature = atoi(argbuf);
   default:   return; 
  }
  
}

boolean isEmpty()
{
   return top == -1; 
}
 
void push(int led)
{
  buffer[++top] = led;
}
 
int pop()
{
  return buffer[top--]; 
}

void init_leds()
{
  
  
  for(int i = 0; i < LIGHT_COUNT; ++i)
  {
    
    switch(i){
      case 0: leds[i].x = 4;
              leds[i].y = 16;
              leds[i].id = 0;
              break;
      case 1: leds[i].x = 6;
              leds[i].y = 10;
              leds[i].id = 1;
              break;
      case 2: leds[i].x = 4;
              leds[i].y = 4;
              leds[i].id = 2;
              break;
      case 3: leds[i].x = 10;
              leds[i].y = 8;
              leds[i].id = 3;
              break;
      case 4: leds[i].x = 16;
              leds[i].y = 7;
              leds[i].id = 4;
              break;
      case 5: leds[i].x = 23;
              leds[i].y = 5;
              leds[i].id = 5;
              break;
      case 6: leds[i].x = 21;
              leds[i].y = 10;
              leds[i].id = 6;
              break;
      case 7: leds[i].x = 23;
              leds[i].y = 17;
              leds[i].id = 7;
              break;
      case 8: leds[i].x = 22;
              leds[i].y = 23;
              leds[i].id = 8;
              break; 
      case 9: leds[i].x = 18;
              leds[i].y = 29;
              leds[i].id = 9;
              break;
      case 10: leds[i].x = 23;
               leds[i].y = 39;
               leds[i].id = 10;
               break;
      case 11: leds[i].x = 19;
               leds[i].y = 36;
               leds[i].id = 11;
               break;
      case 12: leds[i].x = 15;
               leds[i].y = 34;
               leds[i].id = 12;
               break;
      case 13: leds[i].x = 14;
               leds[i].y = 31;
               leds[i].id = 13;
               break;
      case 14: leds[i].x = 10;
               leds[i].y = 33;
               leds[i].id = 14;
               break;
      case 15: leds[i].x = 12;
               leds[i].y = 38;
               leds[i].id = 15;
               break;
      case 16: leds[i].x = 15;
               leds[i].y = 45;
               leds[i].id = 16;
               break;
      case 17: leds[i].x = 10;
               leds[i].y = 43;
               leds[i].id = 17;
               break;
      case 18: leds[i].x = 4;
               leds[i].y = 39;
               leds[i].id = 18;
               break;
      case 19: leds[i].x = 5;
               leds[i].y = 32;
               leds[i].id = 19;
               break;
      case 20: leds[i].x = 11;
               leds[i].y = 25;
               leds[i].id = 20;
               break;
      case 21: leds[i].x = 16;
               leds[i].y = 20;
               leds[i].id = 21;
               break;
      case 22: leds[i].x = 13;
               leds[i].y = 17;
               leds[i].id = 22;
               break;
      case 23: leds[i].x = 5;
               leds[i].y = 23;
               leds[i].id = 23;
               break;
      case 24: leds[i].x = 22;
               leds[i].y = 10;
               leds[i].id = 24;
               break;
      case 25: leds[i].x = 20;
               leds[i].y = 14;
               leds[i].id = 25;
               break;
      case 26: leds[i].x = 21;
               leds[i].y = 18;
               leds[i].id = 26;
               break;
      case 27: leds[i].x = 22;
               leds[i].y = 23;
               leds[i].id = 27;
               break;
      case 28: leds[i].x = 19;
               leds[i].y = 27;
               leds[i].id = 28;
               break;
      case 29: leds[i].x = 19;
               leds[i].y = 32;
               leds[i].id = 29;
               break;
      case 30: leds[i].x = 14;
               leds[i].y = 35;
               leds[i].id = 30;
               break;
      case 31: leds[i].x = 19;
               leds[i].y = 39;
               leds[i].id = 31;
               break;
      case 32: leds[i].x = 15;
               leds[i].y = 42;
               leds[i].id = 32;
               break;
      case 33: leds[i].x = 11;
               leds[i].y = 44;
               leds[i].id = 33;
               break;
      case 34: leds[i].x = 5;
               leds[i].y = 38;
               leds[i].id = 34;
               break;
      case 35: leds[i].x = 6;
               leds[i].y = 33;
               leds[i].id = 35;
               break;
      case 36: leds[i].x = 4;
               leds[i].y = 27;
               leds[i].id = 36;
               break;
      case 37: leds[i].x = 11;
               leds[i].y = 28;
               leds[i].id = 37;
               break;
      case 38: leds[i].x = 14;
               leds[i].y = 24;
               leds[i].id = 38;
               break;
      case 39: leds[i].x = 13;
               leds[i].y = 19;
               leds[i].id = 39;
               break;
      case 40: leds[i].x = 14;
               leds[i].y = 13;
               leds[i].id = 40;
               break;
      case 41: leds[i].x = 15;
               leds[i].y = 7;
               leds[i].id = 41;
               break;
      case 42: leds[i].x = 8;
               leds[i].y = 6;
               leds[i].id = 42;
               break;
      case 43: leds[i].x = 8;
               leds[i].y = 3;
               leds[i].id = 43;
               break;
      case 44: leds[i].x = 3;
               leds[i].y = 8;
               leds[i].id = 44;
               break;
      case 45: leds[i].x = 3;
               leds[i].y = 16;
               leds[i].id = 45;
               break;
      case 46: leds[i].x = 7;
               leds[i].y = 19;
               leds[i].id = 46;
               break;
      case 47: leds[i].x = 9;
               leds[i].y = 14;
               leds[i].id = 47;
               break;
      case 48: leds[i].x = 23;
               leds[i].y = 26;
               leds[i].id = 48;
               break;
      case 49: leds[i].x = 21;
               leds[i].y = 32;
               leds[i].id = 49;
               break;
      case 50: leds[i].x = 18;
               leds[i].y = 37;
               leds[i].id = 50;
               break;


    }    
  //}
  
  /*
  4: 16, 7
  5: 23, 5
  6: 21, 10
  7: 23, 17
  8: 22, 23
  9: 18, 29
  10: 23, 39
  11: 19, 36
  12: 15, 34
  13: 14, 31
  14: 10, 33
  15: 12, 38
  16: 15, 45
  17: 10, 43
  18: 4, 39
  19: 5, 32
  20: 11, 25
  21: 16, 20
  22: 13, 17
  23: 5, 23
  24: 22, 10
  25: 20, 14
  26: 21, 18
  27: 22, 23
  28: 19, 27
  29: 19, 32
  30: 14, 35
  31: 19, 39
  32: 15, 42
  33: 11, 44
  34: 5, 38
  35: 6, 33
  36: 4, 27
  37: 11, 28
  38: 14, 24
  39: 13, 19
  40: 14, 13
  41: 15, 7
  42: 8, 6
  43: 8, 3
  44: 3, 8
  45: 3, 16
  46: 7, 19
  47: 9, 14
  48: 23, 26
  49: 21, 32
  50: 18, 37
  51: 12, 40
  52: 13, 44
  53: 7, 38
  54: 12, 32
  55: 6, 30
  56: 3, 26
  57: 9, 25
  58: 12, 19
  59: 6, 19
  60: 4, 14
  61: 6, 10
  62: 4, 6
  63: 11, 7
  64: 11, 14
  65: 17, 21
  66: 18, 28
  67: 17, 16
  68: 17, 5
  69: 22, 4
  70: 22, 14
  
  */
  
}
