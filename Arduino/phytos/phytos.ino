
#include <Adafruit_WS2801.h>
#include "SPI.h"
#include "variables.h"



int balance = 0;
int state = 0;
int living_count = LIGHT_COUNT;
unsigned long disinteraction_time = 0;
unsigned long disinteraction_oldtime = 0;
unsigned long disinteraction_limit = 15000;  //milliseconds

int max_balance = 127;

int nitrate_lapse[365];
int temperature_lapse[365];
int ph_lapse[365];

int nitrate_lapse_size = 0;
int temperature_lapse_size = 0;
int ph_lapse_size = 0;
//int tempByte = 127; //default resting value

//I know, it's weird, but don't touch this declaration.
//https://code.google.com/p/arduino/issues/detail?id=973
//just ask me if you want to know


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



void setup() {
  init_vals();
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
   int index = (int) (((millis() - visualize_time) / 250) % temperature_lapse_size);
   int temperature_target = temperature_lapse[index];
   //int nitrogen_target = nitrogen_lapse[index];
   //int ph_target = ph_lapse[index];
   
   temperature.value +=   (temperature_target - temperature.value) / 5;
   
   
   //c.velocity = (127 - c.value) / 100.0;
}


boolean timelapse = false;
float control = 0; 
float twinklecontrol = 0;
void game()
{
  int rand = random(100);
  double e = 2.71828;
  double living_ratio = double(LIGHT_COUNT - living_count) / double(LIGHT_COUNT);
  double living_modifier = 1 + pow(e, living_ratio * 2) / 5.0; 
  double dead_modifier =  1 +  pow(e, (1 - living_ratio) * 2) / 5.0; 
  //Serial.println(temperature.value);
  //balance - this basically determines what state we go into
  float shift = (float) balance / 127;

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
       strip.setPixelColor(i, brightness, brightness, brightness);
       break;
     case TEMPERATURE: 
       brightness = get_brightness_chem(i);
       strip.setPixelColor(i, brightness, 10, 255 - brightness);
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

   control = control + .0095;
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

boolean shall_change(int i, int rando, double living_modifier, int b)
{
   int rand = micros() % 101;
   //if(((i + (rand * millis())) % LIGHT_COUNT) == 0){
     //Serial.println( 1 + (pow(5, (living_ratio - 0.5) * 4 ) / 25.0));
     //double random_calculated = ((i + (rand * millis())) % LIGHT_COUNT) / (double(LIGHT_COUNT));
     if(b * rand > 12600){
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
  int dead_index = dead_modifier * ((127 - balance) / 10.0);
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
  
  unsigned long time = millis();

  if(abs(weighted) > 1){
    disinteraction_time = 0; // This MIGHT break if the user turns the knob really slowly.  Make more robust
  }
  else{
    disinteraction_time += time - disinteraction_oldtime;
  }
  disinteraction_oldtime = time;

  //Serial.println(weighted);
  
  float max_velocity = 2.0;
  if (disinteraction_time < disinteraction_limit){
    
    if ((c->velocity - (weighted / 100.0)) < -max_velocity) {
        c->velocity = -max_velocity;
    }
    else if ((c->velocity - (weighted / 100.0)) > max_velocity) {
        c->velocity = max_velocity;
    }
    else{
      c->velocity -= (weighted /100.0) ;
    }
  }
  else{
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
     ;
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
    default: 
      break;
    }    

  if(i >= 32) {
    leds[i].y += 300;
  }
  if(i >= 48){
    leds[i].y += 150; 
  }
//NEXT TIME: OFFSET BLRUGH
  }

}








