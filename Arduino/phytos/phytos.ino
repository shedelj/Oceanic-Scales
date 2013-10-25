
#include <Adafruit_WS2801.h>
#include "SPI.h"

#define DATA_PIN (2)
#define CLOCK_PIN (3)

#define LIGHT_COUNT (130)
#define SB_BUFFER_SIZE (64)
#define DEVICE_ID (1)
#define PARAMETER_MAX (255)

#define WINDOW_X (26)
#define WINDOW_Y (800)

Adafruit_WS2801 strip = Adafruit_WS2801(LIGHT_COUNT, DATA_PIN, CLOCK_PIN);

int balance = 0;
int state = 0;
int living_count = 65;


struct led
{
  int x;
  int y;
  char type;
  uint8_t id;
  boolean alive;
};

struct chem
{
  float value;
  int prev;
  int diff;
  float velocity;
};

void updateChem(int pin, chem* c);


chem temperature;
chem nitrogen;
chem ph;



//state values
//0 - Rest
//1 - Game
//2 - Sleep
//3 - Calibrate
//4 - Pulldown
//5 - Display_Paramaters

enum states{
  GAME,   
  SLEEP,
  PULLDOWN,
};

enum types{
  PLANKTON,
  TEMPERATURE,
  PH,
  NITROGEN
};

led leds [LIGHT_COUNT];

void setup() {
  
  Serial.begin(9600);
  state = GAME;
  //initialize LEDS
  init_leds();

  Serial.println("Ready"); 

  strip.begin();
  strip.show();
  
  temperature.value = 127;
  temperature.prev = 0;
  temperature.diff = 0;
  temperature.velocity = 0;
  
  ph.value = 127;
  nitrogen.value = 127;
  //sets up easytransfer for mydata and the serial port address
//  ET.begin(details(mydata), &Serial);

//  mydata.state = SLEEP;
  //mydata.state = 5;
}


//---------------------------------
// Main Body
//---------------------------------

void loop() {
  //  Serial.print("state = ");
    //Serial.println(state);
  
//  balance = abs(temperature.value - 127) + abs(nitrogen.value - 127)  + abs(ph.value -127);
  balance = abs(temperature.value - 127);
  updateChem(49, &temperature);
  
  switch(state)
  { 
    case GAME     :
      //Serial.println("GAME!"); 
      game();
      break;
    case SLEEP    : 
      Serial.println("sleep!");
      break;
    case PULLDOWN : 
      Serial.println("pulldown!");
      break;
    default       : 
      Serial.println("calibrate!");
      calibrate();
      break;
  }
  
  strip.show();
}
/*
int rgbShiftCalc (int next, int current, int sep){
  int value1;
  value1 = (((next - current)/sep)*mydata.nitrogen + current); 
  return (int) value1;
};


void nitroColor(int i){
   int r=0;
   int b=0;
   int g=0;
   int nitro = mydata.nitrogen;
   if (nitro < 37){
     r = rgbShiftCalc(144,96,38);
     g =  rgbShiftCalc(56,62,38);
     b = rgbShiftCalc(238,66,38);
   }
   else if (nitro < 74){
     r = rgbShiftCalc(238,144,38);
     g =  rgbShiftCalc(66,62,38);
     b = rgbShiftCalc(39,31,38);
   }
   else if (nitro < 111){
     r = rgbShiftCalc(240,238,38);
     g =  rgbShiftCalc(90,66,38);
     b = rgbShiftCalc(40,39,38);
   }
   else if (nitro < 148){
     r = rgbShiftCalc(247,240,38);
     g =  rgbShiftCalc(147,90,38);
     b = rgbShiftCalc(29,40,38);
   }
   else if (nitro < 185){
     r = rgbShiftCalc(255,247,38);
     g =  rgbShiftCalc(221,147,38);
     b = rgbShiftCalc(22,29,38);
   }
   else if (nitro < 222){
     r = rgbShiftCalc(0,255,38);
     g =  rgbShiftCalc(165,221,38);
     b = rgbShiftCalc(80,22,38);
   }
   
//        96,62,66   
//        144,62,31
//        238,66,39
//        240,90,40
//        247,147,29
//        255,221,22
//        0,165,80
  strip.setPixelColor(i, r, g, b);
};
*/

float control = 0; 
float twinklecontrol = 0;
void game()
{
  
  //balance - this basically determines what state we go into
  // balance < 25 -> win
  // balance > 300 -> loing
  //Serial.println(random(10));
  int rand = random(100);
  for(int i = 0; i < LIGHT_COUNT; i += 2)
  {
    
   
    if(leds[i].alive)
    {
       if(shall_die(i, rand))
       {
         leds[i].alive = false;
         living_count--;
         strip.setPixelColor(i, 0, 0, 0);
       }
    }
    
    if(!leds[i].alive)
    {
       if(shall_live_again(i, rand))
       {
          leds[i].alive = true;
          living_count++;
       }
          continue;
       
    }
    
    
    char type = leds[i].type;
    uint8_t brightness = 0;
    switch(type)
    {
     case PLANKTON:    
       brightness = get_brightness_plankton(i);
       strip.setPixelColor(i, brightness, brightness, brightness);
       break;
     case TEMPERATURE: 
       brightness = get_brightness_chem(i);
       strip.setPixelColor(i, 255 - brightness, 10, brightness);
       break; 
     default:
       break;
      
    }
    
  }
/*
    else
    {
      switch(leds[i].type)
      {
        case TEMPERATURE:
          //value between RED and BLUE
          strip.setPixelColor(i, mydata.temperature, 15 , 255- mydata.temperature);
          break;
        case PH:
          //value between BLUE and GREEN
          strip.setPixelColor(i, 15, 255 - mydata.ph, mydata.ph);
          break;
        case NITROGEN: 
//          if (mydata.nitrogen <  127) strip.setPixelColor(i, (mydata.nitrogen * 2) , 0, 5);
//          else strip.setPixelColor(i, 255 - (mydata.nitrogen - 127) * 2, (mydata.nitrogen - 127)* 2, 5);
        //  strip.setPixelColor(i,255 -  mydata.nitrogen, ((mydata.nitrogen+ 1) / .6667 + 83) , 5);
            strip.setPixelColor(i,255 -  mydata.nitrogen, mydata.nitrogen / 2 + 127 , 5);

          break;
*/
//   control = control + (( (float) balance) / (float) 4000);
   control = control + .019;
   if (control >= 1) 
   {
     control -= 1;
   }
   twinklecontrol = twinklecontrol + ((temperature.value * 3) / 10000.0);
   if (twinklecontrol >= 1)
   {
     twinklecontrol -=1;
   }
}

boolean shall_die(int i, int rand)
{
   if(balance * rand > 11000) {
     Serial.println("DIE!");
     return true;
   }
   return false;
}

boolean shall_live_again(int i, int rand)
{
  if(balance > 30)
  {
    return false;
  }
  if(balance * rand > 29000)
  {
//    Serial.println("revived");
    return true;
  }
  return false;
}

/*
void tint_plankton(id)
{
  uint32_t x = strip.getPixelColor(i);
  uint8_t oldb = x & 0xff;
  uint8_t oldg = (x >> 8) & 0xff;
  uint8_t oldr = (x >> 16) & 0xff;
  //uint8_t bleh = x >> 24;   // WHAT IS THIS VALUE??
  
  
}
*/
uint8_t get_brightness_plankton(int id)
{
//  float shift = (float) balance / (127 * 3);
  float shift = (float) balance / (127 );
  float weighted_average = ((float) twinkle_value(id) * shift) + ((float) pulse_value(id) * (1- shift));
  return (uint8_t) weighted_average;
}

uint8_t get_brightness_chem(int id)
{  
   uint8_t relevant_value;
   switch(leds[id].type)
   {
     case NITROGEN: relevant_value = nitrogen.value;
                    break;
     case PH:       relevant_value = ph.value;
                    break;
     case TEMPERATURE: relevant_value = temperature.value;
                       break;
    default: relevant_value = nitrogen.value;    
   }
   int distance =  relevant_value - leds[id].y * (double) ( 255.0 / WINDOW_Y) ;
   
   if(distance < -25)
   {
      return 255; 
   }
   else if (distance > 25)
   {
      return 0; 
   }
   else
   {
      return (255 / 50.0) * (50 -  (distance + 25)); 
   }
   
}

uint8_t pulse_value(int id)
{
   float t = control * TWO_PI  ;
   float d = ((float) leds[id].y / (float) (WINDOW_Y * 2) ) * TWO_PI;
  //brightness level based on the difference of the (x) of sin(x) and d
  float level = cos(t - d) + 1;
  return level * 127;
}


uint8_t twinkle_value(int id)
{
   switch(id % 5)
   {
     case 0: 
      return 127 + (int) (127 * sin(TWO_PI * twinklecontrol));
      break;
    case 1: 
      return 127 + (int) (127 * sin(TWO_PI * (twinklecontrol + .2)));
      break;
    case 2: 
      return 127 + (int) (127 * sin(TWO_PI * (twinklecontrol + .4)));
      break;
    case 3: 
      return 127 + (int) (127 * sin(TWO_PI * (twinklecontrol + .6)));
      break;
    case 4: 
      return 127 + (int) (127 * sin(TWO_PI * (twinklecontrol + .8)));
      break; 
   }
   
 
}


//uint8_t average_value(uint8_t v1, uint8_t v2)
//{
//  uint32_t x = strip.getPixelColor(i);
//  uint8_t oldb = x & 0xff;
//  uint8_t oldg = (x >> 8) & 0xff;
//  uint8_t oldr = (x >> 16) & 0xff;
//  uint8_t bleh = x >> 24;
//  
//  return oldg * (1 - victorycontrol) + brightness * victorycontrol;
//
//
//}

//wipe LEDs blank
void wipe()
{
  for(int i = 0; i < LIGHT_COUNT; ++i)
  {
    strip.setPixelColor(i, 0, 0, 0); 
  }
}

//shows BLUE
void solid()
{
  //wipe();
  for(int i = 0; i < LIGHT_COUNT; ++i)
  {
    switch(state)
    {
    case 0: 
      strip.setPixelColor(i, 255, 0, 0);
      break;
    case 1: 
      strip.setPixelColor(i, 0, 255, 0);
      break;
    case 2: 
      strip.setPixelColor(i, 0, 0, 255);
      break;
    case 3: 
      strip.setPixelColor(i, 255, 255, 0);
      break;
    case 4: 
      strip.setPixelColor(i, 255, 0, 255);
      break;
    case 5: 
      strip.setPixelColor(i, 0, 255, 255);
      break;
    default: 
      strip.setPixelColor(i, 255, 255, 255);
    }
  }
}


void calibrate(){
  wipe();
  for(int i = 0; i < LIGHT_COUNT; )
  {
    if(Serial.read() != -1){
      wipe();
      strip.setPixelColor(i, 255,255,255);
      Serial.println(i);

      ++i;
    }
    strip.show();
    delay(15); 
  }
  Serial.println("calibration over");
  state = 0;
}

 
void updateChem(int pin, chem* c){
  int sensorValue = pulseIn(pin, HIGH);
  //int sensorValue2 = pulseIn(51, HIGH);
  //int sensorValue3 = pulseIn(53, HIGH);

#ifdef DEBUG
  Serial.println(sensorValue);
#endif
 // Serial.println(sensorValue);

  c->diff = c->diff + (sensorValue - c->prev) ;

  if (abs(c->diff) > 600) {
      c->diff = 0;
  }

  float weighted = c->diff / 6;
  
  float max_velocity = 4.0;
  
  if ((c->velocity + (weighted / 100.0)) < -max_velocity) {
      c->velocity = -max_velocity;
      #ifdef DEBUG
      Serial.println("temperature reset ");
      #endif
  }
  else if ((c->velocity + (weighted / 100.0)) > max_velocity) {
      c->velocity = max_velocity;
      #ifdef DEBUG
      Serial.println("temperature reset ");
      #endif
  }
  else{
    c->velocity += (weighted /100.0) ;
  }
//  Serial.println(c->velocity);
//  c->velocity = (raw_velocity - 127) / 100.0;
  c->value += c->velocity;
  if(c->value > 255)
  {
    c->value = 255;
    c->velocity = 0; 
  }
  if(c->value < 0)
  {
    c->value = 0; 
    c->velocity = 0;
  }
  
  c->diff = c->diff % 8; 
  
  c->prev = sensorValue;
  //prevValue2 = sensorValue2;
  //prevValue3 = sensorValue3;
//  Serial.println(c->velocity);
}


void init_leds()
{
  for(int i = 0; i < LIGHT_COUNT; ++i)
  {
    leds[i].id = i;
    leds[i].alive = true;
    switch(i){
    case 0: 
      //leds[i].x = 215;
      leds[i].y = 24;
      leds[i].type = PLANKTON;
     ;
      break;
    case 2: 
      //leds[i].x = 241;
      leds[i].y = 79;
      leds[i].type = TEMPERATURE;
      break;
    case 4: 
      //leds[i].x = ;
      leds[i].y = 123;
      leds[i].type = PLANKTON;
      break;
    case 6: 
      //leds[i].x = 21;
      leds[i].y = 140;
      leds[i].type = TEMPERATURE;
      break;
    case 8: 
      //leds[i].x = 25;
      leds[i].y = 95;
      leds[i].type = PLANKTON;
      break; 
    case 10: 
      //leds[i].x = 27;
      leds[i].y = 69;
      leds[i].type = PLANKTON;
      break;
    case 12: 
      //leds[i].x = 23;
      leds[i].y = 31;
      leds[i].type = TEMPERATURE;
      break;
    case 14: 
      //leds[i].x = 18;
      leds[i].y = 60;
      leds[i].type = TEMPERATURE;
      break;
    case 16: 
      //leds[i].x = 15;
      leds[i].y = 34;
      leds[i].type = PLANKTON;
      break;
    case 18: 
      //leds[i].x = 4;
      leds[i].y = 20;
      leds[i].type = TEMPERATURE;
      break;
    case 20: 
      //leds[i].x = 11;
      leds[i].y = 83;
      leds[i].type = TEMPERATURE;
      break;
    case 22: 
      //leds[i].x = 13;
      leds[i].y = 55;
      leds[i].type = PLANKTON;
      break;
    case 24: 
      //leds[i].x = 22;
      leds[i].y = 89;
      leds[i].type = TEMPERATURE;
      break;
    case 26: 
      //leds[i].x = 21;
      leds[i].y = 140;
      leds[i].type = TEMPERATURE;
      break;
    case 28: 
      //leds[i].x = 19;
      leds[i].y = 123;
      leds[i].type = PLANKTON;
      break;
    case 30: 
      //leds[i].x = 14;
      leds[i].y = 132;
      leds[i].type = TEMPERATURE;
      break;
    case 32: 
      //leds[i].x = 15;
      leds[i].y = 166;
      leds[i].type = PLANKTON;
      break;
    case 34: 
      //leds[i].x = 5;
      leds[i].y = 179;
      leds[i].type = TEMPERATURE;
      break;
    case 36: 
      //leds[i].x = 4;
      leds[i].y = 204;
      leds[i].type = TEMPERATURE;
      break;
    case 38: 
      //leds[i].x = 14;
      leds[i].y = 235;
      leds[i].type = PLANKTON;
      break;
    case 40: 
      //leds[i].x = 14;
      leds[i].y = 274;
      leds[i].type = TEMPERATURE;
      break;
    case 42: 
      //leds[i].x = 8;
      leds[i].y = 251;
      leds[i].type = TEMPERATURE;
      break;
    case 44: 
      //leds[i].x = 3;
      leds[i].y = 280;
      leds[i].type = PLANKTON;
      break;
    case 46: 
      //leds[i].x = 7;
      leds[i].y = 290;
      leds[i].type = TEMPERATURE;
      break;
    case 48: 
      //leds[i].x = 23;
      leds[i].y = 263;
      leds[i].type = PLANKTON;
      break;
    case 50: 
      //leds[i].x = 18;
      leds[i].y = 228;
      leds[i].type = PLANKTON;
      break;
    case 52: 
      //leds[i].x = 13;
      leds[i].y = 235;
      leds[i].type = TEMPERATURE;
      break;
    case 54: 
      //leds[i].x = 12;
      leds[i].y = 200;
      leds[i].type = PLANKTON;
      break;
    case 56: 
      //leds[i].x = 3;
      leds[i].y = 198;
      leds[i].type = TEMPERATURE;
      break;
    case 58: 
      //leds[i].x = 12;
      leds[i].y = 171;
      leds[i].type = PLANKTON;
      break;
    case 60: 
      //leds[i].x = 4;
      leds[i].y = 225;
      leds[i].type = TEMPERATURE;
      break;
    case 62: 
      //leds[i].x = 4;
      leds[i].y = 275;
      leds[i].type = TEMPERATURE;
      break;
    case 64: 
      //leds[i].x = 11;
      leds[i].y = 55;
      leds[i].type = PLANKTON;
      break;
    case 66: 
      //leds[i].x = 18;
      leds[i].y = 90;
      leds[i].type = TEMPERATURE;
      break;
    case 68: 
      //leds[i].x = 17;
      leds[i].y = 110;
      leds[i].type = TEMPERATURE;
      break;
    case 70: 
      //leds[i].x = 22;
      leds[i].y = 157;
      leds[i].type = TEMPERATURE;
      break;
    case 72: 
      //leds[i].x = 22;
      leds[i].y = 170;
      leds[i].type = PLANKTON;
      break;
    case 74: 
      //leds[i].x = 22;
      leds[i].y = 131;
      leds[i].type = PLANKTON;
      break;
    case 76: 
      //leds[i].x = 22;
      leds[i].y = 93;
      leds[i].type = PLANKTON;
      break;
    case 78: 
      //leds[i].x = 22;
      leds[i].y = 52;
      leds[i].type = TEMPERATURE;
      break; 
    case 80: 
      //leds[i].x = 22;
      leds[i].y = 58;
      leds[i].type = TEMPERATURE;
      break;
    case 82: 
      //leds[i].x = 22;
      leds[i].y = 111;
      leds[i].type = PLANKTON;
      break;
    case 84: 
      //leds[i].x = 22;
      leds[i].y = 51;
      leds[i].type = PLANKTON;
      break;
    case 86: 
      //leds[i].x = 22;
      leds[i].y = 78;
      leds[i].type = TEMPERATURE;
      break;
    case 88: 
      //leds[i].x = 22;
      leds[i].y = 118;
      leds[i].type = TEMPERATURE;
      break;
    case 90: 
      //leds[i].x = 22;
      leds[i].y = 154;
      leds[i].type = PLANKTON;
      break;
    case 92: 
      //leds[i].x = 22;
      leds[i].y = 140;
      leds[i].type = TEMPERATURE;
      break;
    case 94: 
      //leds[i].x = 22;
      leds[i].y = 167;
      leds[i].type = TEMPERATURE;
      break;
    //case 96: THERE'S NOTHING HERE
    case 98: 
      //leds[i].x = 22;
      leds[i].y = 55;
      leds[i].type = TEMPERATURE;
      break;
    case 100: 
      //leds[i].x = 22;
      leds[i].y = 88;
      leds[i].type = TEMPERATURE;
      break;
    case 102: 
      //leds[i].x = 22;
      leds[i].y = 98;
      leds[i].type = PLANKTON;
      break;
    case 104: 
      //leds[i].x = 22;
      leds[i].y = 129;
      leds[i].type = TEMPERATURE;
      break;
    case 106: 
      //leds[i].x = 22;
      leds[i].y = 160;
      leds[i].type = TEMPERATURE;
      break;
    case 108: 
      //leds[i].x = 22;
      leds[i].y = 167;
      leds[i].type = PLANKTON;
      break;
    case 110: 
      //leds[i].x = 22;
      leds[i].y = 121;
      leds[i].type = TEMPERATURE;
      break;
    case 112: 
      //leds[i].x = 22;
      leds[i].y = 73;
      leds[i].type = PLANKTON;
      break;
    case 114: 
      //leds[i].x = 22;
      leds[i].y = 49;
      leds[i].type = TEMPERATURE;
      break;
    //case 96: THERE'S NOTHING HERE
    case 116: 
      //leds[i].x = 22;
      leds[i].y = 104;
      leds[i].type = TEMPERATURE;
      break;
    case 118: 
      //leds[i].x = 22;
      leds[i].y = 148;
      leds[i].type = PLANKTON;
      break;
    case 120: 
      //leds[i].x = 22;
      leds[i].y = 125;
      leds[i].type = TEMPERATURE;
      break;
    case 122: 
      //leds[i].x = 22;
      leds[i].y = 78;
      leds[i].type = TEMPERATURE;
      break;
    case 124: 
      //leds[i].x = 22;
      leds[i].y = 53;
      leds[i].type = PLANKTON;
      break;
    case 126: 
      //leds[i].x = 22;
      leds[i].y = 127;
      leds[i].type = PLANKTON;
      break;
    case 128: 
      //leds[i].x = 22;
      leds[i].y = 170;
      leds[i].type = TEMPERATURE;
      break;
    default: 
      break;
    }    

  if(i >= 64) {
    leds[i].y += 300;
  }
  if(i >= 98){
    leds[i].y += 150; 
  }
//NEXT TIME: OFFSET BLRUGH
  }

}








