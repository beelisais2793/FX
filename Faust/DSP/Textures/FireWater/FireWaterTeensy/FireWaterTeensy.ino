
#include <Audio.h>
#include "firewater.h"

firewater firewater;

AudioOutputI2S out;
AudioControlSGTL5000 audioShield;
AudioConnection patchCord0(firewater,0,out,0);

int readPin = A0;
float reading = 0;
float prevReading = 0;
int count=0;
float delta = 0;
int lowest = 500;
int highest = 600;

void setup() {
  // put your setup code here, to run once:
   AudioMemory(200);
   audioShield.enable();
   audioShield.volume(1);
   firewater.setParamValue("vol", 1);
   firewater.setParamValue("volSmth", 0.2);
   firewater.setParamValue("fireWater", 1);
   firewater.setParamValue("smooth", 1);
   firewater.setParamValue("phut", 0.5);
   firewater.setParamValue("pop", 1);
   firewater.setParamValue("hiss", 0.02);
   firewater.setParamValue("woof", 0.8);
   firewater.setParamValue("water", 0.1);
    firewater.setParamValue("maxPop", 200);
    firewater.setParamValue("maxWater", 600);
     firewater.setParamValue("waterSmth", 0.5);
   Serial.begin(9600);
}

void loop() {
  
  reading=analogRead(readPin);
  reading = constrain(reading, 300, 950);
  reading = map(reading, 300,950,0,1000);
  Serial.println(reading);
  reading = 1-(reading/1000);
  Serial.println(reading);
  firewater.setParamValue("fireWater", reading);
  if(count==0){
    delta = abs(reading-prevReading)*10;
    delta = constrain(delta, 0.05, 1.05)-0.05;
    firewater.setParamValue("vol", 1+delta);
    prevReading=reading;
    }
  count=(count+1)%1000;
}
