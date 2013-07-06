#include <stdio.h>
#include <string>
using namespace std;

struct led
{
  int x; //coordinate positions 
  int y; //on the board
  char* type; //phyto, ph, or nitrogen
  int id; //position on strip
};


char  PLANKTON[] = "PLANKTON";
char  TEMPERATURE[] = "TEMPERATURE";
char  PH[] = "PH";
char  NITROGEN[] = "NITROGEN";


led leds[70];

int main()
{
  for(int i = 0; i < 70; ++i)
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
  for (int i = 0; i < 70; i++){
  	printf("{%d,%d,%s,%d},\n", leds[i].x, leds[i].y, leds[i].type, leds[i].id);
  }
  return 1;
}