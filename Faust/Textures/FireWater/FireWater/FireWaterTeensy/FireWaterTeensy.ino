#include <Audio.h>
#include "FireWater.h"

fireWater FireWater;

AudioOutputI2S out;
AudioControlSGTL5000 audioShield;
AudioConnection patchCord0(fireWater,0,out,0);

void setup() {
  // put your setup code here, to run once:
   AudioMemory(200);
   audioShield.enable();
   audioShield.volume(1);
   fireWater.setParamValue("vol", 0.1);
}

void loop() {
  // put your main code here, to run repeatedly:

}
