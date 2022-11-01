#include <Audio.h>
#include "Oboe.h"

oboe oboe;
AudioOutputI2S out;
AudioControlSGTL5000 audioShield;
AudioConnection patchCord0(oboe,0,out,0);
AudioConnection patchCord1(oboe,0,out,1);


//Set scale for oboe (midi values)
// for now, set to C lydian
int scale[8] = {60,62,64,66,67,69,71, 72};

int fsrPin = A0; 
int reading = 0;
int gate=0;
int midiNote = 0;
int prevNote=0;

void setup() {
  AudioMemory(200);
  audioShield.enable();
  audioShield.volume(1);
  oboe.setParamValue("vol", 0.1);
  oboe.setParamValue("vibratoHz", 3.5);
  oboe.setParamValue("vibratoWidth", 0.3);
  Serial.begin(9600);
}


int triggerCount=0;
int triggerNote=0;

void loop() {
  reading = analogRead(fsrPin)/127;
  Serial.println(reading);
  midiNote = scale[reading];

//oboe array from touchpads
//oboe function takes care of notes/envs
  
  if(prevNote!=midiNote){
    gate=0;
    prevNote=midiNote;
    }
  else{gate=1;}

  if(triggerCount==0){
    triggerNote=(triggerNote+1)%2;
    }
  triggerCount=(triggerCount+1)%20000;
  
  oboe.setParamValue("fundFreq",midiNote);
  oboe.setParamValue("legatoTrigger",gate); 
  oboe.setParamValue("trigger",1);//triggerNote); 
  
  
}
