#include <Audio.h>
#include "trombone.h"

trombone  tbone;
AudioOutputI2S out;
AudioControlSGTL5000 audioShield;
AudioConnection patchCord0(tbone,0,out,0);
AudioConnection patchCord1(tbone,0,out,1);

//set scale by distance from C3, where C3=1, C#3=2, D=3 etc.
// for now, set scale to C lydian
int scale[8] = {1,3,5,7,8,10,12, 13};

//set how wide the slide can bend the quantised pitches
// max=100, a full semitone
int vibWidth = 15.0;

//set slideyness of trombone 0-1
// you don't need much!
float slidey = 0.05;

//Sensor Pin, A0
int slidePin = 14; 
int gatePin = 15;
int reading = 0;

//gate
int gate = 1;

//PM Brass Mpdel parameters for b2-c4
// These are the in-tune values, which are used in setup to 
// create interpolated lookup tables to allow for vibrato/
// glissano etc.
float tubeLengths[26] = {
  3.680, 
  3.500, 3.280, 3.085, 2.935, 2.760, 2.600,  
  2.460, 2.310, 2.075, 1.930, 1.790, 1.660,   
  1.540, 1.420, 1.310, 1.210, 1.120, 1.030,   
  0.950, 0.880, 0.820, 2.310, 2.170, 2.060,
  1.930
  };
float tube[2500];
float lipTensions[26] = {
  0.62,
  0.63, 0.63, 0.63, 0.64, 0.64, 0.64,
  0.64, 0.64, 0.60, 0.59, 0.58, 0.57, 
  0.56, 0.55, 0.54, 0.53, 0.52, 0.51, 
  0.50, 0.49, 0.48, 0.84, 0.85, 0.86,
  0.87 
  };
float lip[2500];
float pressures[26] = {
  0.05,
  0.05, 0.05, 0.05, 0.05, 0.05, 0.05,
  0.05, 0.05, 0.05, 0.05, 0.05, 0.05,
  0.05, 0.05, 0.05, 0.05, 0.05, 0.05,
  0.06, 0.06, 0.06, 0.07, 0.07, 0.07,
  0.07
  };
float prss[2500];

//SCALE DEGREE: takes an analog read (0-1023)
// and returns the degree of a diatonic scale,
// (0-7). 
int scaleDegree(int anlgVal){
  //each note is triggered within 99values, 
  //then gets a dead zone of 55 values
  int degree = anlgVal/128;
  if(anlgVal%128 < 98){
    return degree;
    }
  else{
    return -1; //in dead zone
    }
  }
  
//VIBRATO: returns the distance from a scale
// degree 'centre point' as +/- 'vibWidth', where 0 is in tune,
// returns 0 in deadzones
float vibrato(int anlgVal){
  float tuning = (anlgVal%128)-30.0;
  tuning = (tuning/30.0)*vibWidth;
  if(tuning<=30){
      return tuning;
    }
  else{
      return 0;
    }
  }

//TROMBONE: take analog read value, 
// calculate SCALE DEGREE and VIBRATO and
// set arguments for the brass physical model
void trombone(int anlgVal, int gate){
  int note = scaleDegree(anlgVal);
  float vib = vibrato(anlgVal);
  if(note!=-1){
      //use these values to get an index on lookup tables
      note = (scale[note]*100)+int(vib);
      vib = (vib/vibWidth)*0.2;
      //then set tbone parameters
      tbone.setParamValue("length", tube[note]);
      tbone.setParamValue("lipsTension", lip[note]);
      tbone.setParamValue("pressure", prss[note]);
      tbone.setParamValue("vib", vib);
    }
  tbone.setParamValue("toneGate", gate);
  }


void setup() {  
  AudioMemory(200);
  audioShield.enable();
  audioShield.volume(0.01);
  tbone.setParamValue("autoVibHz", 3);
  tbone.setParamValue("autoVibWidth", 0.01);
  tbone.setParamValue("vol", 1);
  tbone.setParamValue("smoo", 0.05);

  //create lookup table, 100 values between 1/2 tone
  for(int i=0; i<25; i++){
  //difference between one 1/2tone and the next then /100
  float tubeDiff = (tubeLengths[i+1]-tubeLengths[i])*0.01;
  float lipDiff = (lipTensions[i+1]-lipTensions[i])*0.01;
  float prssDiff = (pressures[i+1]-pressures[i])*0.01;
  for(int j=0; j<100; j++){
    //interpolate between two semitones...
    float tubeBtwn = tubeDiff*j; 
    float lipBtwn = lipDiff*j; 
    float prssBtwn = prssDiff*j; 
    //add to array
    tube[(i*100)+j]=tubeLengths[i]+tubeBtwn;
    lip[(i*100)+j]=lipTensions[i]+lipBtwn;
    prss[(i*100)+j]=pressures[i]+prssBtwn;
    }
  }
}

void loop() {
  reading = analogRead(sensPin);
  gate = digitalRead(gatePin);
  trombone(reading, gate);
}
