#include <EasyTransfer.h>

#include <Adafruit_WS2801.h>
#include "SPI.h"

#define DATA_PIN (2)
#define CLOCK_PIN (3)

#define LIGHT_COUNT (72)
#define SB_BUFFER_SIZE (64)
#define DEVICE_ID (1)
#define PARAMETER_MAX (255)

#define WINDOW_X (26)
#define WINDOW_Y (48)
#define DEBUG

Adafruit_WS2801 strip = Adafruit_WS2801(LIGHT_COUNT, DATA_PIN, CLOCK_PIN);

uint32_t pixels[LIGHT_COUNT];
EasyTransfer ET; 

struct RECEIVE_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to receive
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  uint8_t state;
  uint8_t nitrogen;
  uint8_t ph;
  uint8_t temperature;

};

RECEIVE_DATA_STRUCTURE mydata;


struct led
{
  int x; //coordinate positions 
  int y; //on the board
  char type; //phyto, ph, or nitrogen
  uint8_t id; //position on strip
};

//state values
//0 - Rest
//1 - Game
//2 - Sleep
//3 - Calibrate
//4 - Pulldown
//5 - Display_Paramaters

enum states{
  RESTING,
  GAME,   
  SLEEP,
  CALIBRATE,
  PULLDOWN,
  DISPLAY_PARAMETERS
};

enum types{
  PLANKTON,
  TEMPERATURE,
  PH,
  NITROGEN
};



led leds [LIGHT_COUNT];

int TIME = 0;
//start without winning or losing
boolean winning = false; 
boolean losing = false;

void setup() {
  
  Serial.begin(9600);

  //initialize LEDS
  init_leds();

  #ifdef DEBUG
    Serial.println("Ready"); 
  #endif

  strip.begin();
  strip.show();

  //sets up easytransfer for mydata and the serial port address
  ET.begin(details(mydata), &Serial);
}


//---------------------------------
// Main Body
//---------------------------------

void loop() {

  //recieve data from mega
  ET.receiveData();


  mydata.state = GAME;
  switch(mydata.state)
  {
    case RESTING:   
      solid();
      break;
    case GAME     : 
      game();
      break;
    case SLEEP    : 
      break;
    case CALIBRATE: 
      calibrate();
      break;
    case PULLDOWN : 
      //victory();
      break;
    default       : 
      solid();
      break;
  }
  strip.show();
}

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

float control = 0; 
//basically, victorycontrol determines how long the victory state displays
float victorycontrol = .00;
//t controls sine wave (it is the x)
float t = 0;

void game()
{
  
  
  //balance - this basically determines what state we go into
  // balance < 25 -> win
  // balance > 300 -> loing
  int balance = abs(mydata.temperature - 127) + abs(mydata.ph - 127)  + abs(mydata.nitrogen -127);
  
  
  //incriments (speed of blink) depending on how in/out of balance
  //this makes the LEDs speed up in the regular game (blinky blink)
  control = control + (( (float) balance) / (float) 4000);
  
  //should not be greater than 1
  if(control >= 1) control = control - 1;

  if(balance < 30) winning = true;

  for(int i = 0; i < LIGHT_COUNT; ++i)
  {
    //win
     if(winning) victory(i, t, victorycontrol);

    //otherwise, update the LEDs 
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
            strip.setPixelColor(i,255 -  mydata.nitrogen, mydata.nitrogen / 2 + 127 , 5);

          break;
        //otherwise, update plankton LEDs
        default: 
          game_plankton(i, control);
          break;
      }
      //if (victorycontrol > 0) victorycontrol -= .005;
    }
  }

  if(winning)
  {
    t += (PI / 128) + victorycontrol;  
    if(victorycontrol >= 1) 
      {
        victorycontrol = 0;
        t = 0;
        winning =  false; 
      }
    victorycontrol += .003;
  }
}


//death variables
float dcontrol = 0;
float drate = 30;
uint8_t dbrightness = 0;

void death(int i) {
    //moving X moves a "line" across the X axis at the rate determined by dcontrol
    float movingX = dcontrol*(WINDOW_X/2);
    //moving Y moves a "line" down the y axis by doing some trig (based on dcontrol and drate (angle))
    float movingY = (WINDOW_Y - (abs((dcontrol*(WINDOW_X)/2)*tan(drate))));
      //brightness is determined by how far away y is multiplied by the movingY rate
      dbrightness = (uint8_t) 48+((WINDOW_Y - leds[i].y)*(movingY));
      if (leds[i].x < movingX) {
        strip.setPixelColor(i, dbrightness, dbrightness, 0);
      }
}

void game_plankton(int index, float control)
{
  #ifdef DEBUG
    Serial.println(control);
    Serial.println(balance);
  #endif
  uint8_t value;
  switch(index % 5)
  {
    case 0: 
      value = 127 + (int) (127 * sin(TWO_PI * control));
      break;
    case 1: 
      value = 127 + (int) (127 * sin(TWO_PI * (control + .2)));
      break;
    case 2: 
      value = 127 + (int) (127 * sin(TWO_PI * (control + .4)));
      break;
    case 3: 
      value = 127 + (int) (127 * sin(TWO_PI * (control + .6)));
      break;
    case 4: 
      value = 127 + (int) (127 * sin(TWO_PI * (control + .8)));
      break;
  }

  strip.setPixelColor(index, value, value, value);
}

//wipe LEDs blank
void wipe()
{
  for(int i = 0; i < LIGHT_COUNT; ++i)
  {
    strip.setPixelColor(i, 0, 0, 0); 
  }
}

void solid()
{
  //wipe();
  for(int i = 0; i < LIGHT_COUNT; ++i)
  {
    switch(mydata.state)
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

//nitrogen: brown orange yellow green
//temperature: red purple blue
//ph: yellowish green aquamarine cyan blue

boolean victory(int index, float t, float victorycontrol)
{
  uint32_t pixelColor = strip.getPixelColor(index);
  uint8_t oldb = pixelColor & 0xff;
  uint8_t oldg = (pixelColor >> 8) & 0xff;
  uint8_t oldr = (pixelColor >> 16) & 0xff;
  float d = ((float) leds[index].y / (float) (WINDOW_Y * 2) ) * TWO_PI;
  //brightness level based on the difference of the (x) of sin(x) and d
  float level = cos(t - d) + 1;
  uint8_t brightness = (uint8_t) (level * 127);
  #ifdef DEBUG
    Serial.println(oldr * (1 - victorycontrol));
  #endif 
  float weightedr = oldr * (1 - victorycontrol) + brightness * victorycontrol;
  float weightedg = oldg * (1 - victorycontrol) + brightness * victorycontrol;
  float weightedb = oldb * (1 - victorycontrol) + brightness * victorycontrol;
  strip.setPixelColor(index, weightedr, weightedg, weightedb);
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
  mydata.state = 0;
}

<<<<<<< HEAD
//
//void readNextCommand()
//{
//  if(Serial.available() < 5) return;  
//  Serial.println("reading");
//
//  if(Serial.read() != 255)
//  {
//    while(Serial.available() > 0)
//    {
//      if(Serial.peek() == 255) return;
//      Serial.read(); 
//    }
//  }
//  //else Serial.read();
//  //char low = Serial.read();
//
//  //char high = Serial.read();
//  //TIME = high * 256 + low;
//  mydata.state = Serial.read();
//  nitrogen = Serial.read();
//  ph = Serial.read();
//  temperature = Serial.read();
//  //  Serial.print("low :");
//  //  Serial.println(low);
//  //  Serial.print("High: ");
//  //  Serial.println(high);
//  Serial.print("State :");
//  Serial.println(state);
//  Serial.print("Nitro: ");
//  Serial.println(nitrogen);
//  Serial.print("ph: ");
//  Serial.println(ph);
//  Serial.print("temp");
//  Serial.println(temperature);
//
//}
//
//void human_input()
//{
//  if (Serial.available() < 4) return; 
//  state = Serial.read() - '0';
//  nitrogen = (Serial.read() - '0') * 25;
//  ph = (Serial.read() - '0') * 25;
//  temperature = (Serial.read() - '0') * 25; 
//}

led leds[70] = {{4, 16 Nitrogen}, }
=======
>>>>>>> d8b5b1590a9964026b905e736921ca9698741d11
void init_leds()
{
  for(int i = 0; i < LIGHT_COUNT; ++i)
  {
    leds[i].id = i;
    switch(i){
    case 0: 
      leds[i].x = 4;
      leds[i].y = 16;
      leds[i].type = NITROGEN;
      break;
    case 1: 
      leds[i].x = 6;
      leds[i].y = 10;
      leds[i].type = PLANKTON;
      break;
    case 2: 
      leds[i].x = 4;
      leds[i].y = 4;
      leds[i].type = PH;
      break;
    case 3: 
      leds[i].x = 10;
      leds[i].y = 8;
      leds[i].type = TEMPERATURE;
      break;
    case 4: 
      leds[i].x = 16;
      leds[i].y = 7;
      leds[i].type = PH;

      break;
    case 5: 
      leds[i].x = 23;
      leds[i].y = 5;
      leds[i].type = TEMPERATURE;
      break;
    case 6: 
      leds[i].x = 21;
      leds[i].y = 10;
      leds[i].type = PLANKTON;
      break;
    case 7: 
      leds[i].x = 23;
      leds[i].y = 17;
      leds[i].type = NITROGEN;
      break;
    case 8: 
      leds[i].x = 22;
      leds[i].y = 23;
      leds[i].type = PH;
      break; 
    case 9: 
      leds[i].x = 18;
      leds[i].y = 29;
      leds[i].type = TEMPERATURE;
      break;
    case 10: 
      leds[i].x = 23;
      leds[i].y = 39;
      leds[i].type = PH;
      break;
    case 11: 
      leds[i].x = 19;
      leds[i].y = 36;
      leds[i].type = PLANKTON;
      break;
    case 12: 
      leds[i].x = 15;
      leds[i].y = 34;
      leds[i].type = NITROGEN;
      break;
    case 13: 
      leds[i].x = 14;
      leds[i].y = 31;
      leds[i].type = TEMPERATURE;
      break;
    case 14: 
      leds[i].x = 10;
      leds[i].y = 33;
      leds[i].type = PLANKTON;
      break;
    case 15: 
      leds[i].x = 12;
      leds[i].y = 38;
      leds[i].type = PH;
      break;
    case 16: 
      leds[i].x = 15;
      leds[i].y = 45;
      leds[i].type = NITROGEN;
      break;
    case 17: 
      leds[i].x = 10;
      leds[i].y = 43;
      leds[i].type = PLANKTON;
      break;
    case 18: 
      leds[i].x = 4;
      leds[i].y = 39;
      leds[i].type = TEMPERATURE;
      break;
    case 19: 
      leds[i].x = 5;
      leds[i].y = 32;
      leds[i].type = NITROGEN;
      break;
    case 20: 
      leds[i].x = 11;
      leds[i].y = 25;
      leds[i].type = NITROGEN;
      break;
    case 21: 
      leds[i].x = 16;
      leds[i].y = 20;
      leds[i].type = PLANKTON;
      break;
    case 22: 
      leds[i].x = 13;
      leds[i].y = 17;
      leds[i].type = TEMPERATURE;
      break;
    case 23: 
      leds[i].x = 5;
      leds[i].y = 23;
      leds[i].type = PH;
      break;
    case 24: 
      leds[i].x = 22;
      leds[i].y = 10;
      leds[i].type = PH;
      break;
    case 25: 
      leds[i].x = 20;
      leds[i].y = 14;
      leds[i].type = PLANKTON;
      break;
    case 26: 
      leds[i].x = 21;
      leds[i].y = 18;
      leds[i].type = TEMPERATURE;
      break;
    case 27: 
      leds[i].x = 22;
      leds[i].y = 23;
      leds[i].type = PH;
      break;
    case 28: 
      leds[i].x = 19;
      leds[i].y = 27;
      leds[i].type = PLANKTON;
      break;
    case 29: 
      leds[i].x = 19;
      leds[i].y = 32;
      leds[i].type = NITROGEN;
      break;
    case 30: 
      leds[i].x = 14;
      leds[i].y = 35;
      leds[i].type = PH;
      break;
    case 31: 
      leds[i].x = 19;
      leds[i].y = 39;
      leds[i].type = TEMPERATURE;
      break;
    case 32: 
      leds[i].x = 15;
      leds[i].y = 42;
      leds[i].type = PLANKTON;
      break;
    case 33: 
      leds[i].x = 11;
      leds[i].y = 44;
      leds[i].type = NITROGEN;
      break;
    case 34: 
      leds[i].x = 5;
      leds[i].y = 38;
      leds[i].type = TEMPERATURE;
      break;
    case 35: 
      leds[i].x = 6;
      leds[i].y = 33;
      leds[i].type = PLANKTON;
      break;
    case 36: 
      leds[i].x = 4;
      leds[i].y = 27;
      leds[i].type = PH;
      break;
    case 37: 
      leds[i].x = 11;
      leds[i].y = 28;
      leds[i].type = NITROGEN;
      break;
    case 38: 
      leds[i].x = 14;
      leds[i].y = 24;
      leds[i].type = TEMPERATURE;
      break;
    case 39: 
      leds[i].x = 13;
      leds[i].y = 19;
      leds[i].type = NITROGEN;
      break;
    case 40: 
      leds[i].x = 14;
      leds[i].y = 13;
      leds[i].type = NITROGEN;
      break;
    case 41: 
      leds[i].x = 15;
      leds[i].y = 7;
      leds[i].type = TEMPERATURE;
      break;
    case 42: 
      leds[i].x = 8;
      leds[i].y = 6;
      leds[i].type = PLANKTON;
      break;
    case 43: 
      leds[i].x = 8;
      leds[i].y = 3;
      leds[i].type = PH;
      break;
    case 44: 
      leds[i].x = 3;
      leds[i].y = 8;
      leds[i].type = NITROGEN;
      break;
    case 45: 
      leds[i].x = 3;
      leds[i].y = 16;
      leds[i].type = TEMPERATURE;
      break;
    case 46: 
      leds[i].x = 7;
      leds[i].y = 19;
      leds[i].type = PLANKTON;
      break;
    case 47: 
      leds[i].x = 9;
      leds[i].y = 14;
      leds[i].type = PH;
      break;
    case 48: 
      leds[i].x = 23;
      leds[i].y = 26;
      leds[i].type = PH;
      break;
    case 49: 
      leds[i].x = 21;
      leds[i].y = 32;
      leds[i].type = NITROGEN;
      break;
    case 50: 
      leds[i].x = 18;
      leds[i].y = 37;
      leds[i].type = PH;
      break;
    case 51: 
      leds[i].x = 12;
      leds[i].y = 40;
      leds[i].type = PLANKTON;
      break;
    case 52: 
      leds[i].x = 13;
      leds[i].y = 44;
      leds[i].type = TEMPERATURE;
      break;
    case 53: 
      leds[i].x = 7;
      leds[i].y = 38;
      leds[i].type = NITROGEN;
      break;
    case 54: 
      leds[i].x = 12;
      leds[i].y = 32;
      leds[i].type = TEMPERATURE;
      break;
    case 55: 
      leds[i].x = 6;
      leds[i].y = 30;
      leds[i].type = PLANKTON;
      break;
    case 56: 
      leds[i].x = 3;
      leds[i].y = 26;
      leds[i].type = PH;
      break;
    case 57: 
      leds[i].x = 9;
      leds[i].y = 25;
      leds[i].type = NITROGEN;
      break;
    case 58: 
      leds[i].x = 12;
      leds[i].y = 19;
      leds[i].type = PLANKTON;
      break;
    case 59: 
      leds[i].x = 6;
      leds[i].y = 19;
      leds[i].type = TEMPERATURE;
      break;
    case 60: 
      leds[i].x = 4;
      leds[i].y = 14;
      leds[i].type = NITROGEN;
      break;
    case 61: 
      leds[i].x = 6;
      leds[i].y = 10;
      leds[i].type = PLANKTON;
      break;
    case 62: 
      leds[i].x = 4;
      leds[i].y = 6;
      leds[i].type = TEMPERATURE;
      break;
    case 63: 
      leds[i].x = 11;
      leds[i].y = 7;
      leds[i].type = PH;
      break;
    case 64: 
      leds[i].x = 11;
      leds[i].y = 14;
      leds[i].type = NITROGEN;
      break;
    case 65: 
      leds[i].x = 17;
      leds[i].y = 21;
      leds[i].type = TEMPERATURE;
      break;
    case 66: 
      leds[i].x = 18;
      leds[i].y = 28;
      leds[i].type = PLANKTON;
      break;
    case 67: 
      leds[i].x = 17;
      leds[i].y = 16;
      leds[i].type = PH;
      break;
    case 68: 
      leds[i].x = 17;
      leds[i].y = 5;
      leds[i].type = PLANKTON;
      break;
    case 69: 
      leds[i].x = 22;
      leds[i].y = 4;
      leds[i].type = NITROGEN;
      break;
    case 70: 
      leds[i].x = 22;
      leds[i].y = 14;
      leds[i].type = PH;
      break;
    default: 
      break;
    }    

  }

}








