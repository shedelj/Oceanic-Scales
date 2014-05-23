
#include <Adafruit_WS2801.h>
#include "SPI.h"

#define DATA_PIN (2)
#define CLOCK_PIN (3)

#define LIGHT_COUNT (129)
#define DEVICE_ID (1)
#define PARAMETER_MAX (255)
#define START_VISUALIZE_TIME (80000)

#define PULSE_PIN 30
#define REDWHITE_PIN 31
#define SYNC_PIN (32);


#define WINDOW_X (26)
#define WINDOW_Y (800)

Adafruit_WS2801 strip = Adafruit_WS2801(LIGHT_COUNT, DATA_PIN, CLOCK_PIN);

/* 
Arduino Mega SPI:
52 - CLK
51 - MOSI
*/

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
long visualize_time = -1;

int balance = 0;
int state = 0;
int living_count = LIGHT_COUNT;
unsigned long disinteraction_time = 0;
unsigned long disinteraction_oldtime = 0;
unsigned long disinteraction_limit = 15000;  //milliseconds

int max_balance = 256;

chem temperature;
chem nitrogen;
chem ph;



void setup() {
  Serial.begin(9600);
  state = GAME;
  //state = 54;
  //initialize LEDS
  init_leds();
  Serial.println("Ready"); 
  pinMode(30, OUTPUT);
  pinMode(31, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(53, OUTPUT);
  strip.begin();
  strip.show();
  
  //SPI.setBitOrder(LSBFIRST);
  //SPI.setDataMode(SPI_MODE0);
  
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

  //  Serial.print("state = ");
    //Serial.println(state);
  
//  balance = abs(temperature.value - 127) + abs(nitrogen.value - 127)  + abs(ph.value -127);
  //balance = abs(temperature.value - 127);
  balance = abs(temperature.value - 127)  + abs(ph.value -127);
  
  if (balance < max_balance / 2){
      digitalWrite(31, HIGH);
      digitalWrite(32, HIGH);
  }
  else {
      digitalWrite(31, LOW);
      digitalWrite(32, LOW); 
  }
  
  updateChem(0);
  updateChem(1);
  //updateChem(48, &nitrate);
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

void visualize()
{
   if (visualize_time == -1) visualize_time = millis();
   int index = (int) (((millis() - visualize_time) / 250) % lapse_size);

   int temperature_target = temperature_lapse[index];
   //int nitrogen_target = nitrogen_lapse[index];
   int ph_target = ph_lapse[index];
   
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

  int rand = random(100);
  double e = 2.71828;
  double living_ratio = double(LIGHT_COUNT - living_count) / double(LIGHT_COUNT);
  double living_modifier = 1 + pow(e, living_ratio * 2) / 5.0; 
  double dead_modifier =  1 +  pow(e, (1 - living_ratio) * 2) / 5.0; 
  //Serial.println(temperature.value);
  //balance - this basically determines what state we go into
  float shift = (float) balance / max_balance / 2;

  ///////// These values must be calculated here otherwise updating takes too long.
  //update_life();
  for(int i = 0; i < LIGHT_COUNT; i += 1)
  
  /////////
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
         //living_modifier = 1 + 4 * (pow(5, (1 - living_ratio - 2.5)));
         if(shall_change(i, rand, dead_modifier, max_balance - balance)){
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

    if(!leds[i].alive) continue;
    switch(type)
    {
     case PLANKTON:    
       brightness = get_brightness_plankton(i);
       //if (i < 64) brightness = brightness / (1 + abs(temperature.value - 127));
       strip.setPixelColor(i, brightness, brightness, brightness);
       //if (i == 0) Serial.println(brightness); 
       break;
     case TEMPERATURE: 
       brightness = get_brightness_chem(i);       
       //strip.setPixelColor(i, 10, brightness, 255- brightness);

       strip.setPixelColor(i, brightness, 10, 255 - brightness);
       break;
     case PH:
       brightness = get_brightness_chem(i);    
       if (i == 91) Serial.println(brightness);
       strip.setPixelColor(i, 10,255 - brightness, brightness);


       break;
       //uint8_t r = brightness;
       //uint8_t b = 255 - brightness
       //pv = pulse_value(i);
       //r = shift * brightness + ((1 - shift) * (pv / 256.0));
       //b = shift * (255 - brightness) + ((1- shift)* (pv / 256.0));

       //b = (255 - brightness) * (pv / 256.0);
       //b = 0;
       //if (i == 43) Serial.println((1 - shift) * pv);
       //strip.setPixelColor(i, r, 0, b);
       //float weighted_average = ((float) twinkle_value(id) * shift) + ((float) pulse_value(id) * (1- shift));
       break; 
     default:
       break;
      
    }
    
  }
   long time = millis();
   control += (time - prevtime) / 1650.0;
   prevtime = time;
   if (control >= 1) 
   {
     control -= 1;
    digitalWrite(30, HIGH);
    
   }
   else if(control >= .15){
    digitalWrite(30, LOW); 
   }
   //twinklecontroltemp = twinklecontroltemp + ((temperature.value * 3) / 10000.0);
   twinklecontroltemp = twinklecontroltemp + .0845;
   if (abs(temperature.value - 127) > 110) twinklecontroltemp = twinklecontroltemp + ((temperature.value * 3) / 10000.0) * 2 ;

   if (twinklecontroltemp >= 1)
   {
     twinklecontroltemp -=1;
   }
   //twinklecontrolph = twinklecontrolph + ((ph.value * 3) / 10000.0);
   twinklecontrolph += .0845;
   if (abs(ph.value - 127) > 110) twinklecontrolph = twinklecontrolph + ((ph.value * 3) / 10000.0) * 2;

   if (twinklecontrolph >= 1)
   {
     twinklecontrolph -=1;
   }

   //Serial.println(twinklecontrol);
   
}

boolean shall_change(int i, int rando, double living_modifier, int b)
{
   int rand = micros() % 101;
   //if(((i + (rand * millis())) % LIGHT_COUNT) == 0){
     //Serial.println( 1 + (pow(5, (living_ratio - 0.5) * 4 ) / 25.0));
     //double random_calculated = ((i + (rand * millis())) % LIGHT_COUNT) / (double(LIGHT_COUNT));
     //if(b * rand > 12600){
     if(b * rand > 25200){  //MUST CHANGETHIS MANUALLY FOR THIRD WALL

       //if( random_calculated * balance * rand * ( 1 ) > 11500){
       //Serial.println("Die.");
       return true;
     }
   
   return false;
}

void update_life(){
  int rand = random(100);
  double e = 2.71828;
  double living_ratio = double(LIGHT_COUNT - living_count) / double(LIGHT_COUNT);
  //double living_modifier  = 1 + 5 * (pow(5, (living_ratio - 2.5)));
  //double dead_modifier = 1 + 5 * (pow(5, (1 - living_ratio - 2.5)));
//  double living_modifier_gaussian = 1 + 5 * (pow(2.71828, pow(-10.0 * (living_ratio - .5), 2)))  ;
//  double dead_modifier_gaussian = 1 + 5 * (pow(2.71828, pow(-10.0 * (1 - living_ratio - .5), 2)))  ;
//  double living_modifier = 1 + pow(e, living_ratio * 2) / 5.0; 
//  double dead_modifier =  1 +  pow(e, (1 - living_ratio) * 2) / 5.0; 
  //Serial.println(living_modifier);
  double living_modifier = pow(2.71828, 5 * (living_ratio + .5)) / 100.0;
  //Serial.println(living_modifier);
  double dead_modifier = pow(2.71828, 5 * ((1 - living_ratio) + .5)) / 100.0;
  int living_index = (int) (living_modifier * (balance / 10.0));
  int dead_index = dead_modifier * ((max_balance - balance) / 10.0);
  //Serial.println(living_index);
  for(int i = 0; i < living_index; ++i){
     int chosen = millis() % 500;
     if (chosen < LIGHT_COUNT){

            leds[chosen].alive = false;
            living_count--;
            strip.setPixelColor(chosen, 0, 0, 0); 
         }
     }  
  
  for(int i = 0; i < dead_index; ++i){
     int chosen = millis() % 500;
     if (chosen < LIGHT_COUNT){
        if(!leds[chosen].alive){
           leds[chosen].alive = true;
           living_count++;
        }
     } 
  }
  
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
  //float shift = (float) balance / (127 * 3);
  float shift = (float) balance / (max_balance );
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
   
   
    //float shift = (float) balance / (127);
    //float weighted_average = ((float) raw_brightness * shift) + ((float) pulse_value(id) * (1- shift));
    //return (uint8_t) weighted_average;
    return raw_brightness;

//   if(distance > 25)
//   {
//      return 255; 
//   }
//   else if (distance < -25)
//   {
//      return 0; 
//   }
//   else
//   {
//      return (255 / 50.0) * (50 -  (distance + 25)); 
//   }
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
  //int sensorValue2 = pulseIn(51, HIGH);
  //int sensorValue3 = pulseIn(53, HIGH);


 
#ifdef DEBUG
  Serial.println(sensorValue);
#endif
 // Serial.println(sensorValue);

  c->diff = c->diff + (sensorValue - c->prev) ;
  //if (select == 0) Serial.println(c->diff);
  if (abs(c->diff) > 600) {
      c->diff = 0;
  }

  float weighted = c->diff / 25.0;
  
  unsigned long time = millis();

  if(abs(weighted) > 1){
    disinteraction_time = 0; // This MIGHT break if the user turns the knob really slowly.  Make more robust
  }
  else{
    if(select == 0) disinteraction_time += time - disinteraction_oldtime;
  }
  disinteraction_oldtime = time;

  //Serial.println(weighted);
  //Serial.println(disinteraction_time);
  float max_velocity = 2.0;
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
  else{
    //Serial.println("Hello");

    int b = 127;
    if(c->value >= b){
      c->velocity = -.2;
    }
    else{
      c->velocity = .2; 
    }
  }
  //Serial.println(c->velocity);
//  c->velocity = (raw_velocity - 127) / 100.0;
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








