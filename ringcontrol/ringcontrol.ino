
#include <EasyTransfer.h>
#include <Adafruit_NeoPixel.h>

#define TEMP_RING_PIN (10)
#define PH_RING_PIN (12)
//create object
EasyTransfer ET; 

struct RECEIVE_DATA_STRUCTURE{
  int temp;
  int ph;
};

Adafruit_NeoPixel temp_ring = Adafruit_NeoPixel(60,TEMP_RING_PIN, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel ph_ring = Adafruit_NeoPixel(60,PH_RING_PIN, NEO_GRB + NEO_KHZ800);


//give a name to the group of data
RECEIVE_DATA_STRUCTURE ring_data;

void setup(){
  Serial1.begin(9600);
  //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc. 
  ET.begin(details(ring_data), &Serial1); 
  
  temp_ring.begin();
  temp_ring.show();
  temp_ring.setBrightness(20);
  
  ph_ring.begin();
  ph_ring.show();
  ph_ring.setBrightness(20);
  
}

void loop(){
  //check and see if a data packet has come in. 
  if(ET.receiveData()){
    
    int temp_leds_on = ring_data.temp / 4;
    int ph_leds_on = ring_data.ph /4;
    int iter_ph = 60;
    int iter_temp = 60;
    
    while (iter_temp > 0) {
       if (iter_temp <= temp_leds_on) {
         temp_ring.setPixelColor(iter_temp, (4 * temp_leds_on) , 0, 255 - (4 * temp_leds_on));
         temp_ring.show();
       }
       
       else { 
         temp_ring.setPixelColor(iter_temp, 0, 0, 0);
         //ph_ring.setPixelColor(iter, 0, 0, 0);
       }
       iter_temp--;
    }
    
    while (iter_ph > 0) {
       if (iter_ph <= ph_leds_on) {
         ph_ring.setPixelColor(iter_ph, 10 , 255, (4 * ph_leds_on));
         ph_ring.show();
       }
       else { 
         ph_ring.setPixelColor(iter_ph, 0, 0, 0);
       }
       iter_ph--;
    }
    
  }
  
  //you should make this delay shorter then your transmit delay or else messages could be lost
  //delay(250);
}
