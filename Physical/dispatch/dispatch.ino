#include <EasyTransfer.h>


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
//  int nitrogen;
//  int ph;
//  int temperature;
  uint8_t nitrogen;
  uint8_t ph;
  uint8_t temperature;

};

EasyTransfer ET; 

boolean winning = false;

SEND_DATA_STRUCTURE mydata;
//
//
//uint8_t state = 0;
//uint8_t nitrogen = 0;
//uint8_t ph = 0;
//
//uint8_t temperature = 0;

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
  //Serial.println(mydata.state);
  ET.sendData();
  //writeState();
  int balance = abs(mydata.temperature - 127) + abs(mydata.ph - 127)  + abs(mydata.nitrogen -127);
  //if (mydata.state != GAME) Serial.write(0);
  
  
  //balance max = 381
  //balance mid = 190
  //if balance is less than 25 you win
//  
//  if (balance > 30)
//  { 
//    Serial.write(1);
//  }
//  else
//  {
//    Serial.write(2);
//  }
//  if (balance > 30) {
//  } else if (balance < 30) {
//  }
  //else Serial.write(3);
  //else Serial.write(0);
  //Serial.write(1);
//  
//  if (balance <= 29){
//    Serial.write(1); 
//  }
//  else if ( balance > 29 && balance > 31) Serial.write(2);
//  else Serial.write(3);

  //for(int i = 0; i < 
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
  
  
  
  
//  for(int i = 0; i < 255; ++i)
//  //Serial.write(mydata.state);
//  Serial.write(i);
  
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

  //Serial.println(sensorValue3);
//  Serial.println(temperature);
  //Serial.println(sensorValue);
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
  
//  if(weighted1 < 0 && mydata.temperature + weighted1 > mydata.temperature) weighted1 = 0;
//  if(weighted1 > 0 && mydata.temperature + weighted1 < mydata.temperature) weighted1 = 255;
//  if(weighted2 < 0 && mydata.temperature + weighted2 > mydata.temperature) weighted2 = 0;
//  if(weighted2 > 0 && mydata.temperature + weighted2 < mydata.temperature) weighted2 = 255;
//  if(weighted3 < 0 && mydata.temperature + weighted3 > mydata.temperature) weighted3 = 0;
//  if(weighted3 > 0 && mydata.temperature + weighted3 < mydata.temperature) weighted3 = 255;



//  if( mydata.temperature + weighted2 > mydata.temperature) weighted2 = 0;
//  if(weighted2 > 0 && mydata.temperature + weighted2 < mydata.temperature) weighted2 = 0;
//  if(weighted3 < 0 && mydata.temperature + weighted3 > mydata.temperature) weighted3 = 0;
//  if(weighted3 > 0 && mydata.temperature + weighted3 < mydata.temperature) weighted3 = 0;
//  uint8_t potentialdata1 = mydata.temperature + weighted1;
//  uint8_t potentialdata2 = mydata.nitrogen + weighted2;
//  uint8_t potentialdata3 = mydata.ph + weighted3;
//  
//  if (abs(potentialdata1 - mydata.temperature < 200)) mydata.temperature = potentialdata1;
//  if (abs(potentialdata2 - mydata.temperature < 200)) mydata.nitrogen = potentialdata2;
//  if (abs(potentialdata3 - mydata.temperature < 200)) mydata.ph = potentialdata3;


//  if (mydata.temperature > 10 && mydata.temperature < 245) mydata.temperature += weighted1;
//  if (mydata.nitrogen > 10 && mydata.nitrogen < 245) mydata.nitrogen += weighted2;
//  if (mydata.ph > 10 && mydata.ph < 245) mydata.ph += weighted3;
  
//  
  //mydata.temperature += weighted1;
  if ((mydata.temperature + weighted1) < 0) {
      mydata.temperature = 0;
      //Serial.println("temperature reset ");
  }
  else if ((mydata.temperature + weighted1) > 255) {
      mydata.temperature = 255;
      //Serial.println("temperature reset ");
  }
  else mydata.temperature += weighted1;
  
  if ((mydata.nitrogen + weighted2) < 0) {
      mydata.nitrogen = 0;
      //Serial.println("nitrogen reset");
  }
  else if ((mydata.nitrogen + weighted2) > 255) {
      mydata.nitrogen = 255;
      //Serial.println("temperature reset ");
  }
  else mydata.nitrogen += weighted2;
  //mydata.nitrogen += weighted2;
  if ((mydata.ph + weighted3) < 0) {
      mydata.ph = 0;
      //Serial.println("ph reset");
  }
  else if ((mydata.ph + weighted3) > 255) {
      mydata.ph = 255;
      //Serial.println("temperature reset ");
  }

  else mydata.ph += weighted3;
  
  //mydata.ph += weighted3;
  

//  if(mydata.temperature + weighted1 > 255) weighted1 = 255;
//  if(mydata.temperature + weighted1 < 0) weighted1 = 0;
//  if(mydata.nitrogen + weighted2 > 255) weighted2 = 255;
//  if(mydata.nitrogen + weighted2 < 0) weighted2 = 0;
//  if(mydata.ph + weighted3 > 255) weighted3 = 255;
//  if(mydata.ph + weighted3 < 0) weighted3 = 0;
//  if(mydata.temperature > 255) weighted1 = 255;
//  if(mydata.temperature < 0) weighted1 = 0;
//  if(mydata.nitrogen > 255) weighted2 = 255;
//  if(mydata.nitrogen < 0) weighted2 = 0;
//  if(mydata.ph > 255) weighted3 = 255;
//  if(mydata.ph < 0) weighted3 = 0;

//  if(mydata.temperature > 255) mydata.temperature = 255;
//  if(mydata.temperature < 0) mydata.temperature = 0;
//  if(mydata.nitrogen > 255) mydata.nitrogen = 255;
//  if(mydata.nitrogen < 0) mydata.nitrogen = 0;
//  if(mydata.ph > 255) mydata.ph = 255;
//  if(mydata.ph < 0) mydata.ph = 0;


  diff1 = diff1 % 8; 
  diff2 = diff2 % 8; 
  diff3 = diff3 % 8; 

  prevValue1 = sensorValue1;
  prevValue2 = sensorValue2;
  prevValue3 = sensorValue3;
  
//    Serial.print("nitrogen = ");
//    Serial.println(mydata.nitrogen);
//    Serial.print("ph = ");
//    Serial.println(mydata.ph);
//    Serial.print("temperature = ");
//    Serial.println(mydata.temperature);


}

//sends 6 byte message
void writeState()
{
//  int TIME = (int) (millis() % 32000);
//  uint8_t low = lowByte(TIME);
//  uint8_t high = highByte(TIME);
//  Serial1.write(255);
//  if (low == 255) low = 254; 
//  if (high == 255) high = 254; 
//  if (state == 255) state = 254; 
//  if (nitrogen == 255) nitrogen = 254; 
//  if (ph == 255) ph = 254; 
//  if (temperature == 255) temperature = 254; 
//  if (state == 255) state = 254; 
////  Serial1.write(low);
////  Serial1.write(high);
//  Serial1.write(state);
//  Serial1.write(nitrogen);
//  Serial1.write(ph);
//  Serial1.write(temperature);
//  Serial.println(state);
//  /*
//  Serial.print("low = ");
//  Serial.println(low, DEC);
//  //Serial.println(
//  Serial.print("high = ");
//  Serial.println(high, DEC);
//  Serial.print("TIME = ");
//  Serial.println(TIME, DEC);
//  Serial.print("TIME SHOULD BE ");
//  Serial.println(high * 256 + low, DEC);
//  Serial.println();
//  */
   // Serial.write(mydata.state);
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


