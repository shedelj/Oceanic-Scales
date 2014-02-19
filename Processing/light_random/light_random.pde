

int LIGHT_COUNT = 192;

int windowX = 1200;
int windowY = 600;
int sqrlen = 20;

int BASE_VALUE = 255;

boolean interacted = false;
//int LIGHT_COUNT = 400;

//non-adjustable global variables.  Don't touch.

int balance = 0;
int state = 0;
int living_count = LIGHT_COUNT;
long disinteraction_time = 0;
 long disinteraction_oldtime = 0;
 long disinteraction_limit = 15000;  //milliseconds

 long pulldown_time = 0;
 long pulldown_oldtime = 0;
 long pulldown_limit = 45000;
int max_balance = 127;

int[] temperature_lapse = new int[365];
int[] nitrogen_lapse = new int[365];
int[] ph_lapse = new int[365];

long visualize_time = -1;

//int tempByte = 127; //default resting value

class led
{
  int x;
  int y;
  int type;
  int id;
  boolean alive;
};

class chem
{
  float value;
  int prev;
  int diff;
  float velocity;
  float resting_value;
  //int timelapsevals[365];
};

//I know, it's weird, but don't touch this declaration.
//https://code.google.com/p/arduino/issues/detail?id=973
//just ask me if you want to know


chem temperature = new chem();
chem nitrogen = new chem();
chem ph = new chem();



//state values
//0 - Rest
//1 - Game
//2 - Sleep
//3 - Calibrate
//4 - Pulldown
//5 - Display_Paramaters


led leds[] = new led[LIGHT_COUNT];

void setup() {
  size(windowX, windowY);

  colorMode(RGB);
  //set up the grid
  for(int i = 0; i < LIGHT_COUNT; ++i)
  {
    led newled = new led();
    newled.id = i;
    newled.type = i%2; 
    newled.alive = true;
    newled.x = (int) (Math.random() * ((windowX / 3) - sqrlen));// + sqrlen / 2;
    newled.y = (int) (Math.random() * ((windowY) - sqrlen));
    if(i > LIGHT_COUNT / 3){
            //println("Here.");
      newled.x += (windowX / 3);  
      if (newled.type == 1) newled.type = 2; 
    }
    if(i > (LIGHT_COUNT / 3) * 2){

       newled.x += (windowX / 3);
       //println(newled.x);
       if (newled.type == 2) newled.type = 3;
    }

    leds[i] = newled;
  }
  //initialize LEDS
  
  temperature.value = 127;
  temperature.resting_value = 127;
  temperature.prev = 0;
  temperature.diff = 0;
  temperature.velocity = 0;
  
  ph.value = 127;
  nitrogen.value = 127;
  temperature_lapse[0] = nitrogen_lapse[0] = ph_lapse[0] = 127;
  for(int i = 1; i < temperature_lapse.length; ++i){
      temperature_lapse[i] = temperature_lapse[i - 1] + (int) random(3) - 1;
      nitrogen_lapse[i] = nitrogen_lapse[i - 1] + (int) random(3) - 1;
      ph_lapse[i] = ph_lapse[i - 1] + (int) random(3) - 1;
  }
  
}


void draw() {
  background(0);
  balance = (int) (abs(temperature.value - 127) + abs(nitrogen.value - 127)  + abs(ph.value -127));
  //updateChem(49, &temperature);
  updateChem(temperature);
  updateChem(nitrogen);
  updateChem(ph);
  stroke(255, 255,255);
  line(windowX / 3, 0, windowX / 3, windowY);
  line(windowX * 2 / 3, 0, windowX * 2 / 3, windowY);
  stroke(0,0,0);
  if (disinteraction_time > 30000)
  {
     visualize() ;
  }
    game();
  
  

}

/*
int timelapsei = 0;

unsigned long timelapsetime = 0;
void lapse()
{
  unsigned long oldtime = timelapsetime;
  timelapsetime = millis();
  if (timelapsetime % 5000 > oldtime % 5000) 
  {
    temperature.resting_value = timelapsevalues[i];
  } 
}
*/
void visualize()
{
   if (visualize_time == -1) visualize_time = millis();
   int index = (int) (((millis() - visualize_time) / 250) % temperature_lapse.length);
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
  //println(temperature.value);
  int rand = (int) random(100);
  float e = 2.71828;
  float living_ratio = float(LIGHT_COUNT - living_count) / float(LIGHT_COUNT);
  float living_modifier = 1 + pow(e, living_ratio * 2) / 5.0; 
  float dead_modifier =  1 +  pow(e, (1 - living_ratio) * 2) / 5.0; 
  //Serial.println(temperature.value);
  //balance - this basically determines what state we go into
  float shift = (float) balance / 127;

  ///////// These values must be calculated here otherwise updating takes too long.
  update_life(0, rand, living_modifier, balance);
  for(int i = 0; i < LIGHT_COUNT; i += 1)
  /////////
  {
      if(!leds[i].alive) {
        continue;
      }

    int type = leds[i].type;
    int brightness = 0;
    int pv;
    float r;
    float b;

    //if(!leds[i].alive) continue;
    switch(type)
    {
     case 0:    //plankton
       brightness = get_brightness_plankton(i);
       //println(brightness);
       fill(brightness, brightness, brightness);
       //strip.setPixelColor(i, brightness, brightness, brightness);
       break;
     case 1: //temperature
       brightness = get_brightness_chem(i);
       //strip.setPixelColor(i, brightness, 10, 255 - brightness);
       fill(brightness, 10, 255 - brightness);
       //println(brightness);
       //int r = brightness;
       //int b = 255 - brightness
       //pv = pulse_value(i);
       //r = shift * brightness + ((1 - shift) * (pv / 256.0));
       //b = shift * (255 - brightness) + ((1- shift)* (pv / 256.0));

       //b = (255 - brightness) * (pv / 256.0);
       //b = 0;
       //if (i == 43) Serial.println((1 - shift) * pv);
       //strip.setPixelColor(i, r, 0, b);
       //float weighted_average = ((float) twinkle_value(id) * shift) + ((float) pulse_value(id) * (1- shift));
       break; 
     case 2: //nitrogen
       brightness = get_brightness_chem(i);
       fill(255 - brightness, brightness, 10);
       break;
     case 3: //ph
       brightness = get_brightness_chem(i);

       fill(10, 255 - brightness, brightness);
       break;
     default:
       break;
      
    }

    if (leds[i].type == 0) ellipse(leds[i].x + sqrlen / 2, leds[i].y, sqrlen, sqrlen);
    else rect(leds[i].x, leds[i].y, sqrlen, sqrlen);
    
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

boolean shall_change(int i, int rando, float living_modifier, int b)
{
   int rand = millis() % 101;
   //if(((i + (rand * millis())) % LIGHT_COUNT) == 0){
     //Serial.println( 1 + (pow(5, (living_ratio - 0.5) * 4 ) / 25.0));
     //float random_calculated = ((i + (rand * millis())) % LIGHT_COUNT) / (float(LIGHT_COUNT));
     if(b * rand > 37000){
       //if( random_calculated * balance * rand * ( 1 ) > 11500){
       //Serial.println("Die.");
       return true;
     }
   
   return false;
}

void update_life(int i, int rando, float modifer, int b){
  /*
  int rand = millis() % 101;
  if (!leds[i].alive) b = 255 - b;
  if (b * rand > 37000){
     leds[i].alive = !leds[i].alive; 
  }
  return leds[i].alive;
*/
  int count = b / 10;
  for(int it = 0; it < count; ++it){
    int rand = millis() % LIGHT_COUNT;
    if (rand < LIGHT_COUNT && leds[rand].alive){
      leds[rand].alive = false;
      living_count--;
    }
  }
  for(int it = 0; it < 10 - count; ++it){
    int rand = millis() % LIGHT_COUNT;
    if (rand < LIGHT_COUNT && !leds[rand].alive){
      leds[rand].alive = true;
      living_count++;
    }
  }

}
/*
void tint_plankton(id)
{
  uint32_t x = strip.getPixelColor(i);
  int oldb = x & 0xff;
  int oldg = (x >> 8) & 0xff;
  int oldr = (x >> 16) & 0xff;
  //int bleh = x >> 24;   // WHAT IS THIS VALUE??
  
  
}
*/
int get_brightness_plankton(int id)
{
  //float shift = (float) balance / (127 * 3);
  float relevant_value;
  if(id < LIGHT_COUNT / 3) relevant_value = temperature.value;
  else if(id < LIGHT_COUNT * 2 / 3) relevant_value = nitrogen.value;
  else relevant_value = ph.value;
  
  int distance =  (int) ((255 - relevant_value) - leds[id].y * (float) ( 255.0 / windowY)) ;
  if(abs(distance) > 200) {
     leds[id].alive = false; 
  }
  
  float shift = (float) balance / (127 );
  float weighted_average = ((float) twinkle_value(id) * shift) + ((float) pulse_value(id) * (1- shift));

  return (int) weighted_average;
}

int get_brightness_chem(int id)
{  
   float relevant_value;
   switch(leds[id].type)
   {
     case 2: relevant_value = nitrogen.value; //NITROGEN
                    break;
     case 3: relevant_value = ph.value; //PH
                    break;
     case 1: relevant_value = temperature.value; //TEMPERATURe
                       break;
    default: relevant_value = nitrogen.value;    
   }
   //int distance = leds[id].y * (float) ( 255.0 / WINDOW_Y) - relevant_value ;
   int distance =  (int) ((255 - relevant_value) -leds[id].y * (float) ( 255.0 / windowY)) ;
   
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
      raw_brightness =  (int)((255 / 200.0) * (200 -  (distance + 100))); 
   }
   
   
    //float shift = (float) balance / (127);
    //float weighted_average = ((float) raw_brightness * shift) + ((float) pulse_value(id) * (1- shift));
    //return (int) weighted_average;
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

int pulse_value(int id)
{
   float t = control * TWO_PI  ;
   float d = ((float) leds[id].y / (float) (windowY * 2) ) * TWO_PI;
  //brightness level based on the difference of the (x) of sin(x) and d
  float level = cos(t - d) + 1;
  return (int) (level * 127);
}


int twinkle_value(int id)
{
   switch(id % 5)
   {
     case 0: 
      return 127 + (int) (127 * sin(TWO_PI * twinklecontrol));
    case 1: 
      return 127 + (int) (127 * sin(TWO_PI * (twinklecontrol + .2)));
    case 2: 
      return 127 + (int) (127 * sin(TWO_PI * (twinklecontrol + .4)));
    case 3: 
      return 127 + (int) (127 * sin(TWO_PI * (twinklecontrol + .6)));
    case 4: 
      return 127 + (int) (127 * sin(TWO_PI * (twinklecontrol + .8)));
    default: return 0;
   }
   
 
}

void keyPressed(){
  if (key == 'q'){
    temperature.velocity += .04; 
    nitrogen.velocity -= .01;
  }
  if(key == 'e'){
    temperature.velocity -= .04;
    nitrogen.velocity -= .015; 
  }
    if(key == 'a'){
    nitrogen.velocity += .04;
    ph.velocity -= .015;
     
  }
    if(key == 'd'){
    nitrogen.velocity -= .04;
    ph.velocity -= .01; 
  }
    if(key == 'z'){
    ph.velocity += .04;
    nitrogen.velocity += .01; 
  }
    if(key == 'c'){
    ph.velocity -= .04; 
    temperature.velocity -= .01;
  }
  interacted = true;
}

void updateChem(chem c){
  //c.value += c.velocity;
  //println(c.velocity);
  long time = millis();
  
  if(interacted){
    disinteraction_time = 0; 
    interacted = false;
    visualize_time = -1;
  }
  else{
    disinteraction_time += time - disinteraction_oldtime;
  }
  disinteraction_oldtime = time;

  //Serial.println(weighted);
  
  float max_velocity = 2.0;
  if (disinteraction_time < disinteraction_limit){
  }
  else{
    int b = 127;
    
    c.velocity = (127 - c.value) / 100.0;
   /* if(c.value >= b){
      c.velocity = - c.value;
    }
    else{
      c.velocity += .005; 
    }*/
  }
  //Serial.println(c->velocity);
//  c->velocity = (raw_velocity - 127) / 100.0;
  c.value = c.value + c.velocity;
  if(c.value > 255)
  {
    c.value = 255;
    c.velocity = 0; 
  }
  if(c.value < 0)
  {
    c.value = 0; 
    c.velocity = 0;
  }
}

//int average_value(int v1, int v2)
//{
//  uint32_t x = strip.getPixelColor(i);
//  int oldb = x & 0xff;
//  int oldg = (x >> 8) & 0xff;
//  int oldr = (x >> 16) & 0xff;
//  int bleh = x >> 24;
//  
//  return oldg * (1 - victorycontrol) + brightness * victorycontrol;
//
//
//}

//wipe LEDs blank

//shows BLUE





