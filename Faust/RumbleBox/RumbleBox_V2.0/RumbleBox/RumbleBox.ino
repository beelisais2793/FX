//BUG: need to smooth vol/MASTER_VOL to avoid artefacts at low volumes

//__________________________________________________________________LIBRARIES
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "HX711.h"
#include "rumbleBox.h"
//__________________________________________________________________MIX LEVELS
const float MASTER_VOL=1;
const float SET_SYNTH_VOL = 1;
const float  SET_WAV_VOL = 0.05;
const float WOB_VOL = 1;
const float AMIN_VOL = 1;
const float TECHNO_VOL = 1;
const float COMP_DB = -25;
//__________________________________________________________________LOAD CELL
const float LOADCELL_SENSITIVITY = 2; // old sketch sensitivity == 1
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 4;
int rangeMin=125;
int rangeMax=225;
int threshCount=0;
int thresh=3;
HX711 scale;
//__________________________________________________________________SET UP TEENSY AUDIO
AudioPlaySdWav           playWav1;
rumbleBox                rumbleBox ;
AudioMixer4              mixer1; 
AudioMixer4              mixer2; 
AudioMixer4              mixer3;   
AudioOutputI2S           out;
//LEFT CHANNEL: Synth to sub only!
AudioConnection          patchCord1(rumbleBox, 0, mixer2, 0);
AudioConnection          patchCord5(mixer2, 0, out,0);
//MIX FOR R-CHN: Mix synth and audio file
AudioConnection          patchCord2(playWav1,  0, mixer1, 0);
AudioConnection          patchCord3(rumbleBox, 1, mixer1, 1);
AudioConnection          patchCord4(mixer1, 0, mixer3, 0);
//RIGHT CHANNEL: Mixed synth&wav file signal
AudioConnection          patchCord6(mixer3, 0, out, 1);
AudioControlSGTL5000     audioShield; 

//__________________________________________________________________WAV PLAYER STUFF
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14
int wavPlaying = 0;
int wavBroken = 0;
//__________________________________________________________________VOLUME
const int VOL_PIN = 15;
float vol=0.0;
float fade=0.0;
//__________________________________________________________________SETUP
void setup() {
  Serial.begin(9600);
  //START AUDIO
  AudioMemory(200);
  audioShield.enable();
  audioShield.volume(1);
//  audioShield.autoVolumeEnable();
//  audioShield.autoVolumeControl(2,1,0,-10,2.5,3.5);
  //SET UP SD WAV PLAYER
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  delay(10);
  if (!(SD.begin(SDCARD_CS_PIN))) {
//    while (1) {
      Serial.println("Unable to access the SD card");
      wavBroken=1;
//      delay(500);
//    }
  }
  //SET UP MIX
  //wav
  mixer1.gain(0, SET_WAV_VOL);
  mixer1.gain(1, SET_SYNTH_VOL);
  mixer2.gain(0, MASTER_VOL);
  mixer3.gain(0, MASTER_VOL);
  //SET UP SYNTH
  rumbleBox.setParamValue("wobWob", 0);//subby rumbles
  rumbleBox.setParamValue("aMinVol", 0);//low a min drones
  rumbleBox.setParamValue("aMinEno", 0);//low a min drone speed
  rumbleBox.setParamValue("techno", 0);//techno vol
  rumbleBox.setParamValue("setPip", 0);//techno pitches
   rumbleBox.setParamValue("pipVol", 0.01);//techno pitches
   rumbleBox.setParamValue("pipDist", 0.01);//techno pitches
   rumbleBox.setParamValue("pipLP", 800);//techno pitches
  rumbleBox.setParamValue("mvol", 3);//master volume
  rumbleBox.setParamValue("mcomp", -30);//master volume
  rumbleBox.setParamValue("rscal", 1);//master volume
  rumbleBox.setParamValue("kikSpd", 0.7);//master volume
  rumbleBox.setParamValue("kickVol", 0.5);//master volume
  //SET UP LOAD CELL
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
}
//__________________________________________________________________GET VOL
void getVol(){
  //get & set volume
//  vol=1;
  vol = analogRead(VOL_PIN)/1023.0;
//  audioShield.volume(vol);
  mixer2.gain(0, MASTER_VOL*vol);
  mixer3.gain(0, MASTER_VOL*vol);
}
//__________________________________________________________________RANGE ADJUSTER
void adjustRange(int curRd){
  if(curRd<(rangeMin-5)){
      threshCount--;
      if(threshCount<(thresh*-1)){
        rangeMin=rangeMin-1;
        rangeMax=rangeMin+99;
      }
    }
  else if(curRd>(rangeMax+5)){
      threshCount++;
      if(threshCount>thresh){
        rangeMax=rangeMax+1;
        rangeMin=rangeMax-99;
      }
  }
  else{
      threshCount=0;
  }
}
//__________________________________________________________________CONTROL SYNTH
int notes[] = {33, 36, 38, 40, 43, 45, 48, 50, 52, 55, 57, 60, 62, 64, 67, 69, 72, 74, 76, 79, 81, 84, 86, 88, 91, 93, 96, 98, 100, 103};
void setSynth(float rd){
  //SET WOBS-----
  float wob = 0.1;
  if(rd<0.1){
      wob=map( constrain(rd, 0.0, 0.1),  0.0, 0.1, 0.1, 1.0);
    }
  if(rd>=0.1){
      wob= 1 - (map( constrain(rd, 0.1, 0.5),  0.1, 0.5, 0.0, 1.0));
    }
  
  //SET AMIN-----
  float aMin=0;
  float aMinSpd=0;
  float pipVol=0;
  if(rd<0.45){
    aMin = map( constrain(rd, 0.0, 0.45), 0.0, 0.45, 0.0, 1.0);
    aMinSpd = map( constrain(rd, 0.0, 0.45), 0.0, 0.45, 0, 0.4);
  }
  if (rd>=0.45){
    aMin = 1 - (map( constrain(rd, 0.45, 1), 0.45, 1, 0.0, 0.9));
    aMinSpd = (map( constrain(rd, 0.45, 0.9), 0.45, 0.9, 0.2, 5));
    pipVol = 1 - map( constrain(rd, 0.45, 1), 0.45, 1, 0.85, 0.92);
    
  }
  if (rd>=0.9){
    aMinSpd = (map( constrain(rd, 0.9, 1.0), 0.9, 1.0, 5, 25));
  }
  //Set Techno
  float techno = map( constrain(rd, 0.4, 1.0), 0.4, 1.0, 0.0,  1);
  int noteAdd = map( constrain(rd, 0.4, 1), 0.4, 1, 0, 22); 
  int note = random(8)+noteAdd;
  note = notes[note];

  rumbleBox.setParamValue("wobWob", wob*WOB_VOL);//subby rumbles
  rumbleBox.setParamValue("aMinVol", aMin*AMIN_VOL);//low a min drones
  rumbleBox.setParamValue("aMinEno", aMinSpd);//low a min drone speed
  rumbleBox.setParamValue("techno", techno*TECHNO_VOL);//techno vol
  rumbleBox.setParamValue("setPip", note);//techno pitches
  rumbleBox.setParamValue("pipVol", pipVol);//techno pitches

//  Serial.print(wob);
//  Serial.print("  ");
//  Serial.print(aMin);
//  Serial.print("  ");
//  Serial.print(aMinSpd);
//  Serial.print("  ");
//  Serial.print(techno);
//  Serial.print("  ");
//  Serial.println(rd);
}
//__________________________________________________________________GET LOAD
void readLoadCell(){
  //if scale ready get loadcell value
  if (scale.is_ready()) {
    float reading = scale.read();
    reading=reading*0.0001*LOADCELL_SENSITIVITY;
//    Serial.println(reading);
    adjustRange(reading);
    //Apply Range Tracking
    reading = map(reading, rangeMin, rangeMax, 0.0, 200.0);
    reading = constrain(reading, 0.0, 200.0);
    reading = reading/200.0;
//    Serial.println(reading);
    //Use value to control Synth parameters...
    setSynth(reading);
  } 
}
//__________________________________________________________________FADES
void setFades(){
  int pos=0;
  pos=playWav1.positionMillis();
//  Serial.println(pos);
  if(pos<5){fade=0.0; rumbleBox.setParamValue("mvol", 3*fade);}
  if(pos<60000){
    rumbleBox.setParamValue("mvol", 3*fade);//master volume
    }
  if( (pos>=60000) && (pos<=70000)){
    fade = map(constrain(pos, 60000.0,70000.0), 60000.0,70000.0,0.0,1.0);
    rumbleBox.setParamValue("mvol", 3*fade);//master volume
//    Serial.println(fade);
    }
}
//__________________________________________________________________PLAY FILE
void playAudio(const char *filename)
{
  Serial.print("Playing file: ");
  Serial.println(filename);
  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playWav1.play(filename);
  // A brief delay for the library read WAV info
  delay(250);
  // Simply wait for the file to finish playing.
  if(playWav1.isPlaying()){
    while (playWav1.isPlaying()) {
      //The rest of the code will execute here while the file plays
      readLoadCell();
      getVol();
      setFades();
      delay(10);
    }
  } else {
    wavBroken=1;
  }
}


//__________________________________________________________________LOOP
void loop() {
  //play audio file
  if(wavBroken==0){
    playAudio("SDTEST1.WAV");  // filenames are always uppercase 8.3 format  
  } else {
    readLoadCell();
    getVol();
  }
}
