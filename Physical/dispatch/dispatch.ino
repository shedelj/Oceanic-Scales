#include "EasyTransfer.h"
#define DEBUG

enum states{
 RESTING,
 GAME,
 SLEEP,
 PULLDOWN,
 CALIBRATE
};

struct SEND_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  
  uint8_t state;
  uint8_t nitrogen;
  uint8_t ph;
  uint8_t temperature;

};

EasyTransfer ET; 

boolean winning = false;

SEND_DATA_STRUCTURE mydata;

void setup()
{
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(7, INPUT); 
  pinMode(40, OUTPUT);
  ET.begin(details(mydata), &Serial2);
  randomSeed(analogRead(0));
  
  mydata.temperature = 80;
  mydata.ph = 150;
  mydata.nitrogen = 40;

}

void loop()
{
  mydata.state = 1;
  //human_input(); //only while testing, in deployment will use rotary encoders.
  getEncoderData();  
#ifdef DEBUG
   Serial.println(mydata.state);
#endif
  ET.sendData();
  //writeState();
  //int balance = abs(mydata.temperature - 127) + abs(mydata.ph - 127)  + abs(mydata.nitrogen -127);
  int balance = abs(mydata.temperature - 127);//simplified for zero1

  //if (mydata.state != GAME) Serial.write(0);
  
  if (balance < 30)
  {
      Serial.write(1);

     if (!winning)
     {

       winning = true;
       digitalWrite(40, HIGH);
       delay(20);
       digitalWrite(40, LOW);
     }
     
  }
  else
  {
     Serial.write(2);

     if(winning)
    {
      winning = false;
      digitalWrite(40, HIGH);
      delay(20);
      digitalWrite(40, LOW);
    } 
  }
  
  
  
#ifdef DEBUG  
  for(int i = 0; i < 255; ++i){
    Serial.write(mydata.state);
    Serial.write(i);
    }
#endif
  
  delay(10);
}

int diff1 = 0;
int diff2 = 0;
int diff3 = 0;
int prevValue1 = 0;
int prevValue2 = 0;
int prevValue3 = 0;

//boolean winning = false;

void getEncoderData()
{
  int sensorValue1 = pulseIn(49, HIGH);
  int sensorValue2 = pulseIn(51, HIGH);
  int sensorValue3 = pulseIn(53, HIGH);

#ifdef DEBUG
  Serial.println(sensorValue3);
  Serial.println(temperature);
  Serial.println(sensorValue);
#endif

  diff1 = diff1 + (sensorValue1 - prevValue1) ;

  if (abs(diff1) > 600) {
      diff1 = 0;
  }

  diff2 = diff2 + (sensorValue2 - prevValue2) ;

  if (abs(diff2) > 600) {
      diff2 = 0;
  }

  diff3 = diff3 + (sensorValue3 - prevValue3) ;

  if (abs(diff3) > 600) {
      diff3 = 0;
  }

  float weighted1 = diff1 / 8;
  float weighted2 = diff2 / 8;
  float weighted3 = diff3 / 8;
  
  if ((mydata.temperature + weighted1) < 0) {
      mydata.temperature = 0;
      #ifdef DEBUG
      Serial.println("temperature reset ");
      #endif
  }
  else if ((mydata.temperature + weighted1) > 255) {
      mydata.temperature = 255;
      #ifdef DEBUG
      Serial.println("temperature reset ");
      #endif
  }
  else mydata.temperature += weighted1;
  
  if ((mydata.nitrogen + weighted2) < 0) {
      mydata.nitrogen = 0;
      #ifdef DEBUG
      Serial.println("nitrogen reset");
      #endif
  }
  else if ((mydata.nitrogen + weighted2) > 255) {
      mydata.nitrogen = 255;
      #ifdef DEBUG
      Serial.println("temperature reset ");
      #endif
  }
  else mydata.nitrogen += weighted2;

  if ((mydata.ph + weighted3) < 0) {
      mydata.ph = 0;
      #ifdef DEBUG
      Serial.println("ph reset");
      #endif
  }
  else if ((mydata.ph + weighted3) > 255) {
      mydata.ph = 255;
      #ifdef DEBUG
      Serial.println("temperature reset ");
      #endif
  }
  else mydata.ph += weighted3;

  diff1 = diff1 % 8; 
  diff2 = diff2 % 8; 
  diff3 = diff3 % 8; 

  prevValue1 = sensorValue1;
  prevValue2 = sensorValue2;
  prevValue3 = sensorValue3;

#ifdef DEBUG
    Serial.print("nitrogen = ");
    Serial.println(mydata.nitrogen);
    Serial.print("ph = ");
    Serial.println(mydata.ph);
    Serial.print("temperature = ");
    Serial.println(mydata.temperature);
#endif

}

//sends 6 byte message
void writeState()
{

}

void human_input()
{
  while (Serial.available() > 0)
  {
    mydata.state = Serial.read() - '0';
    mydata.nitrogen = (char) ((float) (Serial.read() -'0' / 10) * 255);
    mydata.ph = (char) ((float) (Serial.read() -'0' / 10) * 255);
    mydata.temperature = (char) ((float) (Serial.read() -'0' / 10) * 255);
  }
}


