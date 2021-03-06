int lapse_size = 279;
int nitrate_lapse[279] = {
33,31,31,32,33,49,50,60,46,43,40,34,37,40,41,43,38,39,39,38,34,31,31,30,28,29,30,28,31,31,15,15,15,26,27,28,29,25,22,27,29,29,29,29,28,28,26,26,28,40,59,51,41,41,37,35,15,15,15,15,15,15,15,15,15,15,15,15,15,59,62,15,220,15,15,15,255,204,187,15,224,213,180,207,15,15,15,15,15,15,15,15,15,15,17,16,13,13,13,15,14,15,15,95,86,76,71,72,75,68,49,53,42,39,51,50,51,49,118,105,128,104,136,121,160,157,140,151,157,139,137,103,111,15,115,88,97,120,121,126,91,59,52,48,53,58,53,47,50,44,43,65,60,95,15,138,15,167,157,143,146,117,98,75,64,61,64,65,61,59,57,46,45,43,33,36,39,34,31,32,34,33,26,35,27,25,25,22,18,18,18,24,21,20,19,17,39,31,19,20,16,14,17,15,15,18,18,17,16,17,19,19,25,31,24,20,20,18,18,20,20,18,16,16,14,14,17,18,21,24,26,24,29,23,17,13,12,12,15,0,15,15,15,15,15,15,15,13,22,22,18,16,16,19,24,27,28,33,30,29,29,27,26,27,24,18,23,21,24,24,21,18,20,25,28,23,20,20,19,};
int temperature_lapse[279]= {
234,215,213,205,208,189,185,190,191,187,188,174,168,167,168,174,180,183,178,179,166,155,155,166,169,167,168,169,165,175,176,176,172,134,136,140,154,159,150,138,134,129,122,115,113,104,105,118,129,142,152,158,151,158,154,149,147,137,130,127,125,122,122,118,117,119,133,132,136,142,137,134,136,121,121,124,126,130,132,131,141,145,142,144,0,119,108,110,119,122,122,133,148,155,161,161,164,163,159,156,150,153,141,140,138,142,147,156,162,159,160,159,155,155,161,165,164,157,176,163,172,164,162,162,170,178,171,173,177,182,190,185,182,184,174,183,201,202,212,226,232,211,206,193,193,192,195,189,184,182,192,197,181,170,177,209,206,200,196,199,213,203,214,203,197,212,229,234,214,183,200,204,210,215,200,196,209,227,217,217,213,211,192,177,171,170,184,191,191,207,211,204,209,213,209,194,207,199,201,190,208,206,215,218,227,234,244,228,226,229,235,235,217,188,191,193,179,172,201,207,213,226,225,224,221,209,210,200,193,204,222,228,228,216,222,230,225,220,209,222,228,227,227,235,234,232,240,237,225,232,232,236,232,224,215,222,210,209,226,225,238,245,252,255,249,250,252,252,246,241,226,220,217,207,226,246,245,240,237,};
int ph_lapse[279] = {
20,19,16,18,15,136,95,95,95,95,94,19,133,208,172,3,90,14,15,14,12,12,10,10,10,9,10,3,40,43,12,25,10,84,18,19,87,31,20,9,10,9,14,14,16,15,14,20,41,13,13,123,13,8,9,11,17,20,26,40,31,33,3,3,3,3,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,2,19,25,37,30,28,25,25,19,16,13,11,8,9,9,12,18,28,31,24,22,21,19,17,13,11,8,7,7,8,9,14,19,16,20,18,15,11,11,9,7,6,4,3,3,3,5,36,28,35,37,27,30,45,40,25,23,17,21,41,51,56,58,66,116,84,70,151,52,33,46,43,43,45,52,77,76,45,53,94,77,89,45,37,28,30,35,133,38,43,51,45,43,48,59,38,39,63,25,26,24,24,29,37,62,115,91,98,125,152,72,39,37,17,15,12,21,23,47,65,97,39,39,48,59,68,75,72,41,22,25,33,38,48,67,108,92,114,192,155,133,128,135,89,73,63,57,53,44,48,65,74,85,76,60,64,69,79,135,86,64,72,67,3,60,255,194,64,59,76,97,88,94,81,60,53,64,62,50,58,74,74,103,103,90,68,62,71,77,59,67,90,93,108,101,};

#include <Adafruit_WS2801.h>
#include "SPI.h"

//pins for controlling the light strip
#define DATA_PIN (2)
#define CLOCK_PIN (3)

#define LIGHT_COUNT (129)

//how much time after disinteraction until visualization starts in milliseconds
#define START_VISUALIZE_TIME (80000)

//pins for interacting with the swinging elements
#define PULSE_PIN 30
#define REDWHITE_PIN 31
#define SYNC_PIN (32)

//height of the wall in mm according to paper map that Gene has (sorry)
#define WINDOW_Y (800)

//The maximum value for the "balance" variable.  128 * number of walls
#define MAX_BALANCE (256)

Adafruit_WS2801 strip = Adafruit_WS2801(LIGHT_COUNT, DATA_PIN, CLOCK_PIN);

/* 
Arduino Mega SPI:
52 - CLK
51 - MOSI
*/

//the information for the led/element itself

struct led
{
  int x;
  int y;
  char type;
  uint8_t id;
  boolean alive;
};

//the information for each chem.
//there are three instances of these: temperature, nitrogen, and ph

struct chem
{
  float value;
  int prev;
  int diff;
  float velocity;
};

//different states of the machine.  Currently mostly unused

enum states{
  GAME,
  CALIBRATE
};

//flavors possible for each element

enum types{
  PLANKTON,
  TEMPERATURE,
  PH,
  NITROGEN
};

//array of literal leds
led leds [LIGHT_COUNT];


//how long Oceanic Scales has been in "visualize mode"
long visualize_time = -1;

//symbolizes how balanced the system is.  0 is the most balanced.
int balance = 0;

//state starts at game by default.  If the elements are mismatched, I recommend "CALIBRATE"
int state = GAME;

//used in the calculation required to kill off elements in the event of a crash
int living_count = LIGHT_COUNT;

//how much time has passed since the user interacted with the system.
unsigned long disinteraction_time = 0;

//used in calculations
unsigned long disinteraction_oldtime = 0;

//how long into disinteraction until the system starts resetting itself
unsigned long disinteraction_limit = 15000;  //milliseconds


chem temperature;
chem nitrogen;
chem ph;

void init_leds();

void setup() {
  Serial.begin(9600);

  //initialize LEDS
  init_leds();

  Serial.println("Ready"); 

  pinMode(30, OUTPUT);
  pinMode(31, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(53, OUTPUT);

  strip.begin();
  strip.show();
    
  temperature.value = 127;
  temperature.prev = 0;
  temperature.diff = 0;
  temperature.velocity = 0;
  
  ph.value = 127;
  ph.prev = 0;
  ph.diff = 0;
  ph.velocity = 0;



  nitrogen.value = 127;
  
  //SPI.begin();
}


//---------------------------------
// Main Body
//---------------------------------

void loop() {

 
//  balance = abs(temperature.value - 127) + abs(nitrogen.value - 127)  + abs(ph.value -127);
  balance = abs(temperature.value - 127)  + abs(ph.value -127);
  
  //interacting with swinging forms
  if (balance < MAX_BALANCE / 2){
      digitalWrite(REDWHITE_PIN, HIGH);
      digitalWrite(SYNC_PIN, HIGH);
  }
  else {
      digitalWrite(REDWHITE_PIN, LOW);
      digitalWrite(SYNC_PIN, LOW); 
  }
  
  //checks rotary encoders, updates chems
  updateChem(0);
  updateChem(1);

  //checks if the visualization should start and starts it.  
  if (disinteraction_time > START_VISUALIZE_TIME)
  {
     visualize() ;
  }
  
  switch(state)
  { 
    case GAME     :
      //Serial.println("GAME!"); 
      game();
      break;
    case CALIBRATE: 
      Serial.println("calibrate!");
      calibrate();
      break;
  }
  
  strip.show();
}

//traverses the data arrays over a period of time
void visualize()
{
   if (visualize_time == -1) visualize_time = millis();
   int index = (int) (((millis() - visualize_time) / 250) % lapse_size);

   int temperature_target = temperature_lapse[index];
   //int nitrogen_target = nitrogen_lapse[index];
   int ph_target = ph_lapse[index];
   
   temperature.velocity = 0;
   ph.velocity = 0;

   temperature.value +=   (temperature_target - temperature.value) / 5;
   ph.value += (ph_target - ph.value) / 5;
   
}


boolean timelapse = false;
float control = 0; 
long prevtime = 0;

float twinklecontroltemp = 0;
float twinklecontrolph = 0;
float twinklecontrolnitrate = 0;
void game()
{


  //this is all to figure out who lives and who dies
  int rand = random(100);
  double e = 2.71828;
  double living_ratio = double(LIGHT_COUNT - living_count) / double(LIGHT_COUNT);
  double living_modifier = 1 + pow(e, living_ratio * 2) / 5.0; 
  double dead_modifier =  1 +  pow(e, (1 - living_ratio) * 2) / 5.0;

  //float shift = (float) balance / MAX_BALANCE / 2;

  for(int i = 0; i < LIGHT_COUNT; i += 1)
  

    //this is all to figure out who lives and who dies, no one dies or gets resurrected during visualization
  {
    if(disinteraction_time <= START_VISUALIZE_TIME){
      
      if(leds[i].alive){ 
          if(shall_change(i, rand, living_modifier, balance)){
           
           leds[i].alive = false;
           living_count--;
           strip.setPixelColor(i, 0, 0, 0); 
          }
      
      }
      else{
         if(shall_change(i, rand, dead_modifier, MAX_BALANCE - balance)){
           leds[i].alive = true;
            living_count++;
  
         }
         
      }
      
    }
    char type = leds[i].type;
    uint8_t brightness = 0;
    uint8_t pv;
    float r;
    float b;

    //if the led is dead, then we don't worry about it any more
    if(!leds[i].alive) continue;

    //else we calculate its brightness according to the rules for that element, and set the color according
    //to its color scheme
    switch(type)
    {
     case PLANKTON:    
       brightness = get_brightness_plankton(i);
       strip.setPixelColor(i, brightness, brightness, brightness);
       break;

     case TEMPERATURE: 
       brightness = get_brightness_chem(i);       
       strip.setPixelColor(i, brightness, 10, 255 - brightness);
       break;

     case PH:
       brightness = get_brightness_chem(i);    
       strip.setPixelColor(i, 10,255 - brightness, brightness);
       break;

     default:
       break;
      
    }
    
  }

   long time = millis();

   //controls the sin wave
   control += (time - prevtime) / 1650.0;

   prevtime = time;

   //dont allow control to exceed 1.  When it must be set back to 0, send a pulse to swinging forms.
   if (control >= 1) 
   {
     control -= 1;
    digitalWrite(PULSE_PIN, HIGH);
    
   }
   else if(control >= .15){
    digitalWrite(PULSE_PIN, LOW); 
   }

   //variables for controlling the "twinkling" animation for each wall.  
   //Each wall has its own version of this so that they can "panic" independently

   twinklecontroltemp = twinklecontroltemp + .0845;
   if (abs(temperature.value - 127) > 110) twinklecontroltemp = twinklecontroltemp + ((temperature.value * 3) / 10000.0) * 2 ;

   if (twinklecontroltemp >= 1)
   {
     twinklecontroltemp -=1;
   }

   twinklecontrolph += .0845;
   if (abs(ph.value - 127) > 110) twinklecontrolph = twinklecontrolph + ((ph.value * 3) / 10000.0) * 2;

   if (twinklecontrolph >= 1)
   {
     twinklecontrolph -=1;
   }

   
}

//computes whether a given led should die or not on a pass. 
boolean shall_change(int i, int rando, double living_modifier, int b)
{
   int rand = micros() % 101;
     if(b * rand > 25200){  //MUST CHANGETHIS MANUALLY FOR THIRD WALL

       return true;
     }
   
   return false;
}

//interaction rules for plankton
uint8_t get_brightness_plankton(int id)
{
  //float shift = (float) balance / (127 * 3);
  float shift = (float) balance / (MAX_BALANCE );
  float weighted_average = ((float) twinkle_value(id) * shift) + ((float) pulse_value(id) * (1- shift));

  return (uint8_t) weighted_average;
}

//interaction rules for chemical elements
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
   //int distance = leds[id].y * (double) ( 255.0 / WINDOW_Y) - relevant_value ;
   int distance =  (255 - relevant_value) -leds[id].y * (double) ( 255.0 / WINDOW_Y) ;
   
   int raw_brightness = 0;
   
   if(distance < -100)
   {
      raw_brightness =  255; 
   }
   else if (distance > 100)
   {
      raw_brightness = 0; 
   }
   else
   {
      raw_brightness =  (255 / 200.0) * (200 -  (distance + 100)); 
   }
   
    return raw_brightness;

}

//interaction rules for the sin wave animation
uint8_t pulse_value(int id)
{
   float t = control * TWO_PI  ;
   float d = ((float) leds[id].y / (float) (WINDOW_Y * 2) ) * TWO_PI;
  //brightness level based on the difference of the (x) of sin(x) and d
  float level = cos(t - d) + 1;
  return level * 127;
}

//interaction rules for the twinkling animation
uint8_t twinkle_value(int id)
{
   double twinklecontrol = 0;
   if (id < 64) twinklecontrol = twinklecontroltemp;
   else twinklecontrol = twinklecontrolph;
   //Serial.println(twinklecontrol);
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

//every character you feed the arduino in serial will light up the next consecutive LED in the strip.  
//I'm not sure if this is possible to use with the pi, since there's no arduino IDE.  
//It should be theoretically possible but it's probably a bigger hassle than using a laptop.  
void calibrate(){
  //wipe();
  for(int i = 0; i < LIGHT_COUNT; )
  {
    if(Serial.read() != -1){
      //wipe();
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

//So, remember, the rotary encoder affects the rate of change or "velocity" as I call it, of each 
//chemical level.  
void updateChem(int select){
  int pin;
  chem* c;
  if (select == 0)
  {
    pin = 49;
    c = &temperature;
  }
  if (select == 1){
    pin = 48;
    c = &ph;
  }


  int sensorValue = pulseIn(pin, HIGH);
  //ls
  Serial.println(sensorValue);
  /*
  if(select == 0){
    Serial.println(sensorValue);
  }
  */

  c->diff = c->diff + (sensorValue - c->prev) ;

  //this is to handle the rotary encoder "rollover" behavior
  if (abs(c->diff) > 600) {
      c->diff = 0;
  }

  float weighted = c->diff / 25.0;
  
  unsigned long time = millis();

  if(abs(weighted) > 1){
    disinteraction_time = 0; 
  }
  else{
    if(select == 0) disinteraction_time += time - disinteraction_oldtime;
  }
  disinteraction_oldtime = time;

  float max_velocity = 2.0;

  //if the user is interacting with it, set the velocity based on rotary encoder input
  if (disinteraction_time < disinteraction_limit){
    if ((c->velocity - (weighted / 25.0)) < -max_velocity) {
        c->velocity = -max_velocity;
    }
    else if ((c->velocity - (weighted / 25.0)) > max_velocity) {
        c->velocity = max_velocity;
    }
    else{
      c->velocity -= (weighted /50.0) ;
    }
  }
  //else set the value based on how to get it to go to neutral.
  else{

    int b = 127;
    if(c->value >= b){
      c->velocity = -.2;
    }
    else{
      c->velocity = .2; 
    }
  }

  //add velocity to the value
  c->value += c->velocity;
  if(c->value > 255)
  {

    c->value = 255;
    //c->velocity = 0; 
  }
  if(c->value < 0)
  {
    c->value = 0; 
    //c->velocity = 0;
  }
  
  c->diff = c->diff / 8; 
  
  c->prev = sensorValue;
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
      break;
    case 1: 
      //leds[i].x = 241;
      leds[i].y = 79;
      leds[i].type = TEMPERATURE;
      break;
    case 2: 
      //leds[i].x = ;
      leds[i].y = 123;
      leds[i].type = PLANKTON;
      break;
    case 3: 
      //leds[i].x = 21;
      leds[i].y = 140;
      leds[i].type = TEMPERATURE;
      break;
    case 4: 
      //leds[i].x = 25;
      leds[i].y = 95;
      leds[i].type = PLANKTON;
      break; 
    case 5: 
      //leds[i].x = 27;
      leds[i].y = 69;
      leds[i].type = PLANKTON;
      break;
    case 6: 
      //leds[i].x = 23;
      leds[i].y = 31;
      leds[i].type = TEMPERATURE;
      break;
    case 7: 
      //leds[i].x = 18;
      leds[i].y = 60;
      leds[i].type = TEMPERATURE;
      break;
    case 8: 
      //leds[i].x = 15;
      leds[i].y = 34;
      leds[i].type = PLANKTON;
      break;
    case 9: 
      //leds[i].x = 4;
      leds[i].y = 20;
      leds[i].type = TEMPERATURE;
      break;
    case 10: 
      //leds[i].x = 11;
      leds[i].y = 83;
      leds[i].type = TEMPERATURE;
      break;
    case 11: 
      //leds[i].x = 13;
      leds[i].y = 55;
      leds[i].type = PLANKTON;
      break;
    case 12: 
      //leds[i].x = 22;
      leds[i].y = 89;
      leds[i].type = TEMPERATURE;
      break;
    case 13: 
      //leds[i].x = 21;
      leds[i].y = 140;
      leds[i].type = TEMPERATURE;
      break;
    case 14: 
      //leds[i].x = 19;
      leds[i].y = 123;
      leds[i].type = PLANKTON;
      break;
    case 15: 
      //leds[i].x = 14;
      leds[i].y = 132;
      leds[i].type = TEMPERATURE;
      break;
    case 16: 
      //leds[i].x = 15;
      leds[i].y = 166;
      leds[i].type = PLANKTON;
      break;
    case 17: 
      //leds[i].x = 5;
      leds[i].y = 179;
      leds[i].type = TEMPERATURE;
      break;
    case 18: 
      //leds[i].x = 4;
      leds[i].y = 204;
      leds[i].type = TEMPERATURE;
      break;
    case 19: 
      //leds[i].x = 14;
      leds[i].y = 235;
      leds[i].type = PLANKTON;
      break;
    case 20: 
      //leds[i].x = 14;
      leds[i].y = 274;
      leds[i].type = TEMPERATURE;
      break;
    case 21: 
      //leds[i].x = 8;
      leds[i].y = 251;
      leds[i].type = TEMPERATURE;
      break;
    case 22: 
      //leds[i].x = 3;
      leds[i].y = 280;
      leds[i].type = PLANKTON;
      break;
    case 23: 
      //leds[i].x = 7;
      leds[i].y = 290;
      leds[i].type = TEMPERATURE;
      break;
    case 24: 
      //leds[i].x = 23;
      leds[i].y = 263;
      leds[i].type = PLANKTON;
      break;
    case 25: 
      //leds[i].x = 18;
      leds[i].y = 228;
      leds[i].type = TEMPERATURE;
      break;
    case 26: 
      //leds[i].x = 13;
      leds[i].y = 235;
      leds[i].type = PLANKTON;
      break;
    case 27: 
      //leds[i].x = 12;
      leds[i].y = 200;
      leds[i].type = PLANKTON;
      break;
    case 28: 
      //leds[i].x = 3;
      leds[i].y = 198;
      leds[i].type = TEMPERATURE;
      break;
    case 29: 
      //leds[i].x = 12;
      leds[i].y = 171;
      leds[i].type = PLANKTON;
      break;
    case 30: 
      //leds[i].x = 4;
      leds[i].y = 225;
      leds[i].type = TEMPERATURE;
      break;
    case 31: 
      //leds[i].x = 4;
      leds[i].y = 275;
      leds[i].type = TEMPERATURE;
      break;
    case 32: 
      //leds[i].x = 11;
      leds[i].y = 55;
      leds[i].type = PLANKTON;
      break;
    case 33: 
      //leds[i].x = 18;
      leds[i].y = 90;
      leds[i].type = TEMPERATURE;
      break;
    case 34: 
      //leds[i].x = 17;
      leds[i].y = 110;
      leds[i].type = TEMPERATURE;
      break;
    case 35: 
      //leds[i].x = 22;
      leds[i].y = 157;
      leds[i].type = TEMPERATURE;
      break;
    case 36: 
      //leds[i].x = 22;
      leds[i].y = 170;
      leds[i].type = PLANKTON;
      break;
    case 37: 
      //leds[i].x = 22;
      leds[i].y = 131;
      leds[i].type = PLANKTON;
      break;
    case 38: 
      //leds[i].x = 22;
      leds[i].y = 93;
      leds[i].type = PLANKTON;
      break;
    case 39: 
      //leds[i].x = 22;
      leds[i].y = 52;
      leds[i].type = TEMPERATURE;
      break; 
    case 40: 
      //leds[i].x = 22;
      leds[i].y = 58;
      leds[i].type = TEMPERATURE;
      break;
    case 41: 
      //leds[i].x = 22;
      leds[i].y = 111;
      leds[i].type = PLANKTON;
      break;
    case 42: 
      //leds[i].x = 22;
      leds[i].y = 51;
      leds[i].type = PLANKTON;
      break;
    case 43: 
      //leds[i].x = 22;
      leds[i].y = 78;
      leds[i].type = TEMPERATURE;
      break;
    case 44: 
      //leds[i].x = 22;
      leds[i].y = 118;
      leds[i].type = TEMPERATURE;
      break;
    case 45: 
      //leds[i].x = 22;
      leds[i].y = 154;
      leds[i].type = PLANKTON;
      break;
    case 46: 
      //leds[i].x = 22;
      leds[i].y = 140;
      leds[i].type = TEMPERATURE;
      break;
    case 47: 
      //leds[i].x = 22;
      leds[i].y = 167;
      leds[i].type = TEMPERATURE;
      break;
    //case 96: THERE'S NOTHING HERE
    case 48: 
      //leds[i].x = 22;
      leds[i].y = 55;
      leds[i].type = TEMPERATURE;
      break;
    case 49: 
      //leds[i].x = 22;
      leds[i].y = 88;
      leds[i].type = TEMPERATURE;
      break;
    case 50: 
      //leds[i].x = 22;
      leds[i].y = 98;
      leds[i].type = PLANKTON;
      break;
    case 51: 
      //leds[i].x = 22;
      leds[i].y = 129;
      leds[i].type = TEMPERATURE;
      break;
    case 52: 
      //leds[i].x = 22;
      leds[i].y = 160;
      leds[i].type = PLANKTON;
      break;
    case 53: 
      //leds[i].x = 22;
      leds[i].y = 167;
      leds[i].type = TEMPERATURE;
      break;
    case 54: 
      //leds[i].x = 22;
      leds[i].y = 121;
      leds[i].type = PLANKTON;
      break;
    case 55: 
      //leds[i].x = 22;
      leds[i].y = 73;
      leds[i].type = PLANKTON;
      break;
    case 56: 
      //leds[i].x = 22;
      leds[i].y = 49;
      leds[i].type = TEMPERATURE;
      break;
    //case 96: THERE'S NOTHING HERE
    case 57: 
      //leds[i].x = 22;
      leds[i].y = 104;
      leds[i].type = TEMPERATURE;
      break;
    case 58: 
      //leds[i].x = 22;
      leds[i].y = 148;
      leds[i].type = PLANKTON;
      break;
    case 59: 
      //leds[i].x = 22;
      leds[i].y = 125;
      leds[i].type = TEMPERATURE;
      break;
    case 60: 
      //leds[i].x = 22;
      leds[i].y = 78;
      leds[i].type = TEMPERATURE;
      break;
    case 61: 
      //leds[i].x = 22;
      leds[i].y = 53;
      leds[i].type = PLANKTON;
      break;
    case 62: 
      //leds[i].x = 22;
      leds[i].y = 127;
      leds[i].type = PLANKTON;
      break;
    case 63: 
      //leds[i].x = 22;
      leds[i].y = 170;
      leds[i].type = TEMPERATURE;
      break;
    case 64:
      leds[i].y = 23;
      leds[i].type = PLANKTON; 
            break;

    case 65:
      leds[i].y = 23;
      leds[i].type = PH;
      break;
    case 66:
      leds[i].y = 43;
      leds[i].type = PLANKTON; 
      break;
    case 67:
      leds[i].y = 23;
      leds[i].type = PH;
      break;
    case 68:
      leds[i].y = 23;
      leds[i].type = PH;   
      break;
    case 69:
      leds[i].y = 40;
      leds[i].type = PH;
      break;
    case 70:
      leds[i].y = 28;
      leds[i].type = PH;  
      break;
    case 71:
      leds[i].y = 29;
      leds[i].type = PLANKTON;
      break;
    case 72:
      leds[i].y = 35;
      leds[i].type = PLANKTON;
      break;
    case 73:
      leds[i].y = 50;
      leds[i].type = PH;
      break;
    case 74:
      leds[i].y = 56;
      leds[i].type = PLANKTON;
      break;
    case 75:
      leds[i].y = 49;
      leds[i].type = PH;
      break;
    case 76:
      leds[i].y = 52;
      leds[i].type = PLANKTON;
      break;
    case 77:
      leds[i].y = 51;
      leds[i].type = PH;
      break;
    case 78:
      leds[i].y = 56;
      leds[i].type = PH;
      break;
    case 79:
      leds[i].y = 50;
      leds[i].type = PLANKTON;
      break;
    case 80:
      leds[i].y = 53;
      leds[i].type = PH;
      break;
    case 81:
      leds[i].y = 75;
      leds[i].type = PLANKTON;
      break;
    case 82:
      leds[i].y = 66;
      leds[i].type = PH;
      break;
    case 83:
      leds[i].y = 66;
      leds[i].type = PLANKTON;
      break;
    case 84:
      leds[i].y = 66;
      leds[i].type = PH;
      break;
    case 85:
      leds[i].y = 77;
      leds[i].type = PH;   
     break; 
    case 86:
      leds[i].y = 89;
      leds[i].type = PH;
      break;
    case 87:
      leds[i].y = 100;
      leds[i].type = PLANKTON;
      break;
    case 88:
      leds[i].y = 113;
      leds[i].type = PH;
      break;
    case 89:
      leds[i].y = 103;
      leds[i].type = PH;
      break;
    case 90:
      leds[i].y = 91;
      leds[i].type = PLANKTON;  
     break; 
    case 91:
      leds[i].y = 105;
      leds[i].type = PH;
      break;
    case 92:
      leds[i].y = 99;
      leds[i].type = PH;
      break;
    case 93:
      leds[i].y = 105;
      leds[i].type = PLANKTON;  
      break;
    case 94:
      leds[i].y = 105;
      leds[i].type = PH;
      break;
    case 95:
      leds[i].y = 100;
      leds[i].type = PLANKTON;
      break;
    case 96:
      leds[i].y = 131;
      leds[i].type = PH;
      break;
    case 97:
      leds[i].y = 135;
      leds[i].type = PH;
      break;
    case 98:
      leds[i].y = 149;
      leds[i].type = PLANKTON;
      break;
    case 99:
      leds[i].y = 133;
      leds[i].type = PH;      
      break;
    case 100:
      leds[i].y = 130;
      leds[i].type = PLANKTON;
      break;
    case 101:
      leds[i].y =   131;
      leds[i].type = PH;
      break;
    case 102:
      leds[i].y = 132;
      leds[i].type = PLANKTON; 
      break;
    case 103:
      leds[i].y = 146;
      leds[i].type = PH;
      break;
    case 104:
      leds[i].y = 148;
      leds[i].type = PLANKTON;  
      break;
    case 105:
      leds[i].y = 161;
      leds[i].type = PH;
      break;
    case 106:
      leds[i].y = 171;
      leds[i].type = PLANKTON;
      break;
    case 107:
      leds[i].y = 170;
      leds[i].type = PH;   
      break;
    case 108:
      leds[i].y = 162;
      leds[i].type = PLANKTON; 
      break;
    case 109:
      leds[i].y = 150;
      leds[i].type = PH;
      break;
    case 110:
      leds[i].y = 172;
      leds[i].type = PH;
      break;
    case 111:
      leds[i].y = 161;
      leds[i].type = PLANKTON;
      break;
    case 112:
      leds[i].y = 212;
      leds[i].type = PLANKTON;   
     break; 
    case 113:
      leds[i].y = 231;
      leds[i].type = PH;
      break;
    case 114:
      leds[i].y = 240;
      leds[i].type = PLANKTON;
      break;
    case 115:
      leds[i].y = 228;
      leds[i].type = PLANKTON;
      break;
    case 116:
      leds[i].y = 227;
      leds[i].type = PH;
      break;
    case 117:
      leds[i].y = 237;
      leds[i].type = PLANKTON;   
    break;  
    case 118:
      leds[i].y = 235;
      leds[i].type = PH;
      break;
    case 119:
      leds[i].y = 211;
      leds[i].type = PLANKTON;  
      break;
    case 120:
      leds[i].y = 191;
      leds[i].type = PH;
      break;
    case 121:
      leds[i].y = 198;
      leds[i].type = PH;
      break;
    case 122:
      leds[i].y = 191;
      leds[i].type = PLANKTON;
      break;
    case 123:
      leds[i].y = 196;
      leds[i].type = PH;
      break;
    case 124:
      leds[i].y = 192;
      leds[i].type = PH;
      break;
    case 125:
      leds[i].y = 208;
      leds[i].type = PH;
      break;
    case 126:
      leds[i].y = 190;
      leds[i].type = PLANKTON;   
  break;    
    default: 
      break;
    }    
    if (i < 64){
      if(i >= 32) {
        leds[i].y += 300;
      }
      if(i >= 48){
        leds[i].y += 150; 
      }
    }
    else{
       leds[i].y = int(leds[i].y * 3); 
    }
  }

}








