/*
  ==============================================================================
    AllPass.h
    Created: 15 Oct 2014 8:55:30pm
    Author:  Keith Hearne
    
    Based on model that Schroeder proposed in 1962 paper presenting his
    initial reverb designs, that uses a feedback delay line with feedforward
    line.
    
    A Basic All-pass IIR Filter class that sets delay and gain and allows
    access to each, while providing comb filter processing function
    
  ==============================================================================
*/

#pragma once

#include <algorithm>
#include <cmath>
#include <cassert>
#include <cstdlib>
#include <iostream>

namespace FX::KHDelays::Schroeder
{


//////////////////////////////////////////////////////////
//  DELAY LINE CLASS (for use with Schroeder Reverb)
//  see .cpp file for comments
//////////////////////////////////////////////////////////

class DelayLine{

public:
    //constructor / destructor
    DelayLine(const int sr = 44100, const DspFloatType d_ms = 0.0f, const DspFloatType d_max = 0.0f);
    ~DelayLine();
    
    //getters
    DspFloatType getDelayTimeMS();
    DspFloatType getMaxDelayTimeMS();
    
    //setters
    void setDelayTimeMS(const int sr, const DspFloatType d_ms);
    void setDelay(DspFloatType d_ms);
    
    //business functions
    DspFloatType readDelay();
    void writeDelay(DspFloatType in);
    DspFloatType next(const DspFloatType in);
    void resetBuffer();
    void resetDelay();

private:
    int writePos, readPosA, MAX_DELAY_SAMPLES;
    DspFloatType delay_ms, delay_samples, fraction, MAX_DELAY_MS;
    DspFloatType *buffer;
};

//////////////////////////////////////////////////////////
//  ALLPASS FILTER CLASS
//  see .cpp file for comments
//////////////////////////////////////////////////////////

class Allpass{

public:
    //constructor
    Allpass(const int sr, const DspFloatType d_ms, const DspFloatType d_ms_max, const DspFloatType g);
    ~Allpass();
    
    //getters
    DspFloatType getGain();
    DspFloatType getDelayTimeMS();
    
    //setters
    void setGain(const DspFloatType g);
    void setDelayTimeMS(const DspFloatType sr, const DspFloatType d_ms);
    void resetDelay();
    
    //business methods
    DspFloatType next(const DspFloatType in);
    
private:
    DspFloatType gain;
    DelayLine *delay;
    
};

//////////////////////////////////////////////////////////
//  COMB FILTER CLASS
//  see .cpp file for comments
//////////////////////////////////////////////////////////

class Comb{
    
public:
    //constructor / destructor
    Comb(const int sr, const DspFloatType d_ms, const DspFloatType d_ms_max, const DspFloatType g);
    ~Comb();
    
    //getters
    DspFloatType getGain();
    DspFloatType getDelayTimeMS();
    
    //setters
    void setGain(const DspFloatType g);
    void setDelayTimeMS(const DspFloatType sr, const DspFloatType d_ms);
    void resetDelay();
    
    //business methods
    DspFloatType next(const DspFloatType in);
    
private:
    DspFloatType gain;
    DelayLine *delay;    
    
};

//////////////////////////////////////////////////////////
//  SCHROEDER REVERB
//  see .cpp file for full comments
//////////////////////////////////////////////////////////

//predefined number of comb and allpass filters for array parsing

#define NUM_COMBS 4
#define NUM_ALLPASSES 2

class Schroeder{

public:
    //--------------------------------------------------------------
    //constructor setting initial values for comb delays and gains
    //comb delays must be mutually prime
    //
    //  Comb 1  : 29.7 msec delay
    //  Comb 2  : 37.1 msec delay
    //  Comb 3  : 41.1 msec delay
    //  Comb 4  : 43.7 msec delay
    //  APF 1   : 5.0 msec delay, gain 0.707
    //  APF 2   : 1.7 msec delay, gain 0.5
    //  sr      : defaulted to 44100KHz
    //  rt60    : defaulted to 3.0 seconds on initialisation
    //--------------------------------------------------------------
    Schroeder(const int sr = 44100, const DspFloatType rt60 = 3.0,
              const DspFloatType cDelay1 = 29.7, const DspFloatType cDelay2 = 37.1,
              const DspFloatType cDelay3 = 41.1, const DspFloatType cDelay4 = 43.7,
              const DspFloatType aDelay1 = 5.0, const DspFloatType aDelay2 = 1.7,
              const DspFloatType aGain1 = 0.707, const DspFloatType aGain2 = 0.5);
    ~Schroeder();
    
    //getters
    DspFloatType getDecayFactor();
    DspFloatType getCombDelay(const int id);
    DspFloatType getAllpassDelay(const int id);
    DspFloatType getAllpassGain(const int id);
    //DspFloatType getLowpassCutoff(const int id);
    bool getBypass();
    
    //setters
    void setDecayFactor(const DspFloatType df);
    void setCombDelay(const int id, const DspFloatType sr, const DspFloatType d_ms);
    void setAllpassGain(const int id, const DspFloatType g);
    void setAllpassDelay(const int id, const int sr, const DspFloatType d_ms);
    void setBypass(bool bp);
    void reset();
    
    //business methods
    DspFloatType next(const DspFloatType in);
    
    
    private:
    DspFloatType decayFactor, ALLPASS_GAIN_LIMIT = 0.707f;//to keep the allpasses from exploding
    bool bypass;
    Comb *combs[NUM_COMBS];
    Allpass *allpasses[NUM_ALLPASSES];
};

//////////////////////////////////////////////////////////
//  ALLPASS FILTER CLASS
//////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
// Constructor :
// Predefined sample rate = 44100, delay time, max delay time and gain
// creates a new DelayLine and sets the input gain
//-------------------------------------------------------------------------
Allpass::Allpass(const int sr, const DspFloatType d_ms, const DspFloatType d_ms_max, const DspFloatType g){
    gain = g;
    delay = new DelayLine(sr, d_ms, d_ms_max);
}

//-------------------------------------------------------------------------
// Destructor :
// deletes the delay
//-------------------------------------------------------------------------
Allpass::~Allpass(){
    delete delay;
}

//getters
//-------------------------------------------------------------------------
// getGain  :
// return the gain scalar value
//-------------------------------------------------------------------------
DspFloatType Allpass::getGain(){return gain;}

//-------------------------------------------------------------------------
// getDelayTimeMS  :
// return the delay time in milliseconds
//-------------------------------------------------------------------------
DspFloatType Allpass::getDelayTimeMS(){return delay->getDelayTimeMS();}

//setters
//-------------------------------------------------------------------------
// setGain  :
// set the scalar value for the gain of the filter
//-------------------------------------------------------------------------
void Allpass::setGain(const DspFloatType g){gain = g;}

//--------------------------------------------------------------------------------
//  setDelayTimeMS
//  Setter function sets delay time and from milliseconds
//  and passes it to the delayline which converts to discrete time samples
//
//--------------------------------------------------------------------------------
void Allpass::setDelayTimeMS(const DspFloatType sr, const DspFloatType d_ms){return delay->setDelayTimeMS(sr, d_ms);}

//-------------------------------------------------------------------------
// resetDelay  :
// calls the reset on the delayline which resets and fills the buffer
//-------------------------------------------------------------------------
void Allpass::resetDelay(){
    
    delay->resetDelay();
    
    return;
    
}

//------------------------------------------------------------------
//------------------------------------------------------------------
//  next    : Function to process next sample input in
//          : The all-pass filter process involves reading the delay 
//          : at the current readindex position (dL)
//          : scaling this delay value by the filter gain (gain*dL)
//          : and writing this value back into the delay line
//          : at the appropriate write position (fW)
//          : and outputing the -gain*fW + dL
//
//  in      :   input sample form the audio buffer
//  
//------------------------------------------------------------------
//------------------------------------------------------------------
DspFloatType Allpass::next(const DspFloatType in){
    //read delay value from buffer
    DspFloatType dL = delay->readDelay();
    // for the filter write or feedback input to buffer
    DspFloatType fW = in + (gain*dL);
    //form the output of all-pass  (delay + in*-gain)
    DspFloatType out = -gain*fW + dL;      
    // write feedback loop input back to delay buffer
    delay->writeDelay(fW);
    return out;
}

//////////////////////////////////////////////////////////
//  BASIC COMB FILTER CLASS
//////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
// Constructor :
// Predefined sample rate = 44100, delay time, max delay time and gain
// creates a new DelayLine and sets the input gain
//-------------------------------------------------------------------------
Comb::Comb(const int sr, const DspFloatType d_ms, const DspFloatType d_ms_max, const DspFloatType g){
    gain = g;
    delay = new DelayLine(sr, d_ms, d_ms_max);
}

//-------------------------------------------------------------------------
// Destructor :
// deletes the delay
//-------------------------------------------------------------------------
Comb::~Comb(){
    delete delay;
}

//getters
//-------------------------------------------------------------------------
// getGain  :
// return the gain scalar value
//-------------------------------------------------------------------------
DspFloatType Comb::getGain(){return gain;}

//-------------------------------------------------------------------------
// getDelayTimeMS  :
// return the delay time in milliseconds
//-------------------------------------------------------------------------
DspFloatType Comb::getDelayTimeMS(){return delay->getDelayTimeMS();}

//setters
//-------------------------------------------------------------------------
// setGain  :
// set the scalar value for the gain of the filter
//-------------------------------------------------------------------------
void Comb::setGain(const DspFloatType g){gain = g;}

//--------------------------------------------------------------------------------
//  setDelayTimeMS
//  Setter function sets delay time and from milliseconds
//  and passes it to the delayline which converts to discrete time samples
//
//--------------------------------------------------------------------------------
void Comb::setDelayTimeMS(const DspFloatType sr, const DspFloatType d_ms){return delay->setDelayTimeMS(sr, d_ms);}

//-------------------------------------------------------------------------
// resetDelay  :
// calls the reset on the delayline which resets and fills the buffer
//-------------------------------------------------------------------------
void Comb::resetDelay(){
 
    delay->resetDelay();
    
    return;
    
}

//------------------------------------------------------------------
//------------------------------------------------------------------
//  next    : Function to process next sample input in
//          : The comb filter process involves reading the delay 
//          : at the current readindex position (delay->readDelay)
//          : scaling this delay value by the combs gain (gain*dL)
//          : and writing this value back into the delay line
//          : at the appropriate write position 
//          : (delay->writeDelay(dLW))
//
//  in      :   input sample form the audio buffer
//  
//------------------------------------------------------------------
//------------------------------------------------------------------
DspFloatType Comb::next(const DspFloatType in){
    //read the delay value
    DspFloatType dL = delay->readDelay();
    
    //add the input sample plus the delay scaled by the gain
    DspFloatType dLW = in + gain*dL;
    
    //write this back into the elay buffer
    delay->writeDelay(dLW);
    
    //return the delay value
    return dL;
    
}

//helper functions
//-------------------------------------------------------------------------
// numSamplesFromMSf :
// Determine the number of samples from the number of milliseconds delay
// passed to function
//-------------------------------------------------------------------------
inline DspFloatType numSamplesFromMSf(const int sr, const DspFloatType d_ms){
    return sr * d_ms * .001;
}

//----------------------------------------------------------------------
//  Linear Interpolation Function
//  x1  :  weighting 1
//  x2  :  weighting 2
//  y1  :  output y(n) at read index in buffer
//  y2  :  output y(n-1) at read index minus 1 in buffer
//  x   :  fractional value between samples
//----------------------------------------------------------------------
inline DspFloatType linInterp(DspFloatType x1, DspFloatType x2, DspFloatType y1, DspFloatType y2, DspFloatType x){
   	DspFloatType denom = x2 - x1;
	if(denom == 0)
		return y1; // should not ever happen

	// calculate decimal position of x
	DspFloatType dx = (x - x1)/(x2 - x1);

	// use weighted sum method of interpolating
	DspFloatType result = dx*y2 + (1-dx)*y1;

	return result; 
}


//////////////////////////////////////////////////////////
//  DELAY LINE CLASS (for use with Schroeder Reverb)
//////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
// Constructor :
// Predefined sample rate = 44100, delay = 0, max delay
// set read and write index for delay line
// set max delay set from decay factor in reverb
// create a max delay buffer
//-------------------------------------------------------------------------
DelayLine::DelayLine(const int sr, const DspFloatType d_ms, const DspFloatType d_max){
    //assert(d_ms <= d_ms_max);//check bound on delay time
    
    //initialisation
    buffer = NULL;
    readPosA = writePos = 0;
    delay_samples = 0.0f;
    delay_ms = d_ms;
    
    MAX_DELAY_SAMPLES = ceil(numSamplesFromMSf(sr, d_max));
    MAX_DELAY_MS = MAX_DELAY_SAMPLES * 1000.0f / sr; //make sure DspFloatType version is set with to integer-rounded buffer size
    
    DspFloatType delay_samplesf = numSamplesFromMSf(sr, d_ms);
    delay_samples = floor(delay_samplesf);
    
    //storing fractional delay time, will be interpolated
    fraction = delay_samplesf - delay_samples;
    
    buffer = new DspFloatType[MAX_DELAY_SAMPLES];
    memset(buffer, 0, MAX_DELAY_SAMPLES*sizeof(DspFloatType));
    
}

//-------------------------------------------------------------------------
// Destructor :
// delete delay buffer
//-------------------------------------------------------------------------
DelayLine::~DelayLine(){
    delete[] buffer;
}

//getters
//-------------------------------------------------------------------------
// getDelayTimeMS :
// return the delay in milliseconds
//-------------------------------------------------------------------------
DspFloatType DelayLine::getDelayTimeMS(){return delay_ms;}

//--------------------------------------------------------------------------------
//  getMaxDelayTimeMS :
//  return the Max delay time in Miliiseconds
//--------------------------------------------------------------------------------
DspFloatType DelayLine::getMaxDelayTimeMS(){return MAX_DELAY_MS;}

//setters
//--------------------------------------------------------------------------------
//  Setter function that determines read position index
//  read position is determined by subtracting the number of samples to delay
//  from the write position index
//
//  readIndex = writeIndex - number of sample delay
//
//--------------------------------------------------------------------------------
void DelayLine::setDelayTimeMS(const int sr, const DspFloatType d_ms){
    assert(d_ms <= MAX_DELAY_MS);//check bound on delay time
    
    DspFloatType delay_samplesf = numSamplesFromMSf(sr, d_ms);
    delay_samples = floor(delay_samplesf);
    
    //storing fractional delay time, output will be interpolate
    fraction = delay_samplesf - delay_samples;
    readPosA = writePos - delay_samples;
    
    //wrap index if bounds exceeded
    if(readPosA < 0)
        readPosA += MAX_DELAY_SAMPLES;
                
}

//-------------------------------------------------------------------------
// setDelay :
// set the delay in milliseconds by the delay value passed to function
//-------------------------------------------------------------------------
void DelayLine::setDelay(DspFloatType d){
    //receiving the delay value through here in milliseconds 0 to 2000
    delay_ms = d;
    setDelayTimeMS(44100,delay_ms);
};


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  readDelay :
//
//  The following function reads from delay line at read index
//  It uses terms similar to the differential equation for delay
//
//      x(n)                : the input at sample n
//      y(n)                : the output at sample n after delay processing
//      buffer              : the circular buffer used
//      readPosA            : the read index of the delay buffer
//      writePos            : the write index of the delay buffer
//      MAX_DELAY_SAMPLES   : Max size of delay buffer
//  
//      y(n) = x(n) + x(n - D)              'delay with no feedback
//
//      MAX_DELAY_SAMPLES = sr * d_ms * .001;
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
DspFloatType DelayLine::readDelay(){
    
    //output of delay at readPos
    DspFloatType yn = buffer[readPosA];
    
    //read location at n-1, one behind yn
    int readPos_minus1 = readPosA - 1;
    if(readPos_minus1 < 0)
        readPos_minus1 = MAX_DELAY_SAMPLES - 1;   //MAX_DELAY_SAMPLES - 1 is the last location of the buffer
        
    //get y(n-1)
    DspFloatType yn_minus1 = buffer[readPos_minus1];
    
    //perform linear interpolation of : (0,yn) and (1,yn_minus1) by the ammount of fractional delay(fraction)
    return linInterp(0, 1, yn, yn_minus1, fraction);
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  writeDelay :
//
//  The following function writes into the delay buffer at write index
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
void DelayLine::writeDelay(DspFloatType in){
    
    //write the input into buffer at write position
    buffer[writePos] = in;
     
    //increment the write index, wrap index if bounds exceeded
    writePos++;
    if(writePos >= MAX_DELAY_SAMPLES)
        writePos = 0;
        
    readPosA++;
    if(readPosA >= MAX_DELAY_SAMPLES)
        readPosA = 0;
}

//business function
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  next    :
//  The following function is the workhorse of the delay line, but the filters
//  in the reverb will call the individual read/write function so as to have
//  more control over manipulating the inputs and outputs.
//
//      x(n)                : the input at sample n
//      y(n)                : the output at sample n after delay processing
//  
//      y(n) = x(n) + x(n - D)              'delay with no feedback
//
//  read the interpolated value from the delay line and write input back to buffer
//
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
DspFloatType DelayLine::next(const DspFloatType in){
        
    //input
    DspFloatType xn = in;

    //if delay value is zero just pass input out
    DspFloatType yn;
    if(delay_samples == 0)
        yn = xn;
    else
        yn = readDelay(); 

    writeDelay(xn);
    DspFloatType out = yn;
       
    
    return out;
        
}

//-------------------------------------------------------------------------
// resetBuffer :
// delete contents of buffer and instantiate a new buffer
//-------------------------------------------------------------------------
void DelayLine::resetBuffer(){
    
    if(buffer)
        delete [] buffer;
        
    buffer = new DspFloatType[MAX_DELAY_SAMPLES];
    
    resetDelay();
    setDelayTimeMS(44100,delay_ms);
    
    return;
    
}

//-------------------------------------------------------------------------
// resetDelay :
// reset the delay buffer by filling with 0's, reset all indexes and bypass
//-------------------------------------------------------------------------         
void DelayLine::resetDelay(){
 
    if(buffer)
        memset(buffer, 0, MAX_DELAY_SAMPLES*sizeof(DspFloatType));
    
    readPosA = writePos = 0;
    
    return;
    
}

//////////////////////////////////////////////////////////
//  SCHROEDER REVERB
//////////////////////////////////////////////////////////


//helper functions
//------------------------------------------------------------------
//------------------------------------------------------------------
//  calcCombGain : Function to calculate gain from decay/RT60
//
//  RT60    :   value from plugin decay parameter
//  d_ms    :   Delay value of the comb filter
//------------------------------------------------------------------
//------------------------------------------------------------------
inline DspFloatType calcCombGain(const DspFloatType d_ms, const DspFloatType rt60){
    return pow(10.0, ((-3.0 * d_ms) / (rt60 * 1000.0)));
}


//--------------------------------------------------------------
//--------------------------------------------------------------
//constructor setting initial values for comb delays and gains
//comb delays must be mutually prime
//
//  Comb 1  : 29.7 msec delay
//  Comb 2  : 37.1 msec delay
//  Comb 3  : 41.1 msec delay
//  Comb 4  : 43.7 msec delay
//  APF 1   : 5.0 msec delay, gain 0.707
//  APF 2   : 1.7 msec delay, gain 0.5
//  sr      : defaulted to 44100KHz
//  rt60    : defaulted to 3.0 seconds on initialisation
//--------------------------------------------------------------
//--------------------------------------------------------------
Schroeder::Schroeder(const int sr, const DspFloatType rt60,
          const DspFloatType cDelay1, const DspFloatType cDelay2, const DspFloatType cDelay3, const DspFloatType cDelay4,
          const DspFloatType aDelay1, const DspFloatType aDelay2, const DspFloatType aGain1, const DspFloatType aGain2){
    
    decayFactor = rt60;
    DspFloatType d_ms, d_ms_max = 100.0f, gain;
    bypass = false;
    
    //Comb Filter 1 setup
    d_ms = cDelay1;
    gain = calcCombGain(d_ms, decayFactor);
    combs[0] = new Comb(sr, d_ms, d_ms_max, gain);
    setCombDelay(0,sr,d_ms);
    
    //Comb Filter 2 setup
    d_ms = cDelay2;
    gain = calcCombGain(d_ms, decayFactor);
    combs[1] = new Comb(sr, d_ms, d_ms_max, gain);
    setCombDelay(1,sr,d_ms);
    
    //Comb Filter 3 setup
    d_ms = cDelay3;
    gain = calcCombGain(d_ms, decayFactor);
    combs[2] = new Comb(sr, d_ms, d_ms_max, gain);
    setCombDelay(2,sr,d_ms);
    
    //Comb Filter 4 setup
    d_ms = cDelay4;
    gain = calcCombGain(d_ms, decayFactor);
    combs[3] = new Comb(sr, d_ms, d_ms_max, gain);
    setCombDelay(3,sr,d_ms);

    d_ms_max = 20.0f;
    
    //All-pass filter setup
    allpasses[0] = new Allpass(sr, aDelay1, d_ms_max, aGain1);
    allpasses[1] = new Allpass(sr, aDelay2, d_ms_max, aGain2);


}

//-------------------------------------------------------------------------
// Destructor :
// delete all combs and allpasses
//-------------------------------------------------------------------------
Schroeder::~Schroeder(){
    for(int i = 0; i < NUM_COMBS; i++){
        delete combs[i];
    }
    for(int i = 0; i < NUM_ALLPASSES; i++){
        delete allpasses[i];
    }
}

//getters
//-------------------------------------------------------------------------
// getDecayFactor :
// return the decay factor used for determining RT60 and filter gain
//-------------------------------------------------------------------------
DspFloatType Schroeder::getDecayFactor(){return decayFactor;}

//-------------------------------------------------------------------------
// getCombDelay : comb id
// get the specified delay time in milliseconds of the indexed comb filter
//-------------------------------------------------------------------------
DspFloatType Schroeder::getCombDelay(const int id){return combs[id]->getDelayTimeMS();}

//-------------------------------------------------------------------------
// getAllpassDelay : allpass id
// get the specified delay time in milliseconds of the indexed allpass filter
//-------------------------------------------------------------------------
DspFloatType Schroeder::getAllpassDelay(const int id){return allpasses[id]->getDelayTimeMS();}

//-------------------------------------------------------------------------
// getAllpassGain : comb id
// get the specified gain scalar value of the indexed comb filter
//-------------------------------------------------------------------------
DspFloatType Schroeder::getAllpassGain(const int id){return allpasses[id]->getGain();}

//-------------------------------------------------------------------------
// getBypass : 
// return the status of the boolean for bypassing the plugin processing
//-------------------------------------------------------------------------
bool Schroeder::getBypass(){return bypass;}

//setters
//-------------------------------------------------------------------------
// setDecayFactor : decayfactor value in seconds
// decay time/desired RT60 is passed from UI to this function
// and the required comb filter gain values that will adhere to that RT60
// are calculated based on this factor
//-------------------------------------------------------------------------
void Schroeder::setDecayFactor(const DspFloatType df){
    decayFactor = df;
    
    //cycle through each comb and reset the comb gain value according to
    //the new decayFactor
    for(int i = 0; i < NUM_COMBS; i++){
        combs[i]->setGain(calcCombGain(combs[i]->getDelayTimeMS(), decayFactor));
    }
};

//-------------------------------------------------------------------------
// setCombDelay : id of comb, sample rate, delay time in milliseconds
// sets the gain and the delaytime in milliseconds of the Comb filters
// delay buffer when a value is changed through the UI
//-------------------------------------------------------------------------
void Schroeder::setCombDelay(const int id, const DspFloatType sr, const DspFloatType d_ms){
    combs[id]->setGain(calcCombGain(d_ms, decayFactor));
    combs[id]->setDelayTimeMS(sr, d_ms);
}

//-------------------------------------------------------------------------
// setAllpassGain : id of comb, gain
// sets the gain for the allpass filter, scaling by the GAIN_LIMIT so as
// not to blow the filter up due to the unstable nature of IIR filters
//-------------------------------------------------------------------------
void Schroeder::setAllpassGain(const int id, const DspFloatType g){allpasses[id]->setGain(g * ALLPASS_GAIN_LIMIT);}

//-------------------------------------------------------------------------
// setAllpassDelay : id of comb, sample rate, delay in milliseconds
// sets the delay time in milliseconds of the all pass delay line
//-------------------------------------------------------------------------
void Schroeder::setAllpassDelay(const int id, const int sr, const DspFloatType d_ms){allpasses[id]->setDelayTimeMS(sr, d_ms);}

//-------------------------------------------------------------------------
// setBypass : boolean bypass value
// sets a boolean which determines if processing should be bypassed in the
// worker next function
//-------------------------------------------------------------------------
void Schroeder::setBypass(bool bp){bypass = bp;}

//-------------------------------------------------------------------------
// reset : 
// resets the delay lines in the combs and allpass filters
//-------------------------------------------------------------------------
void Schroeder::reset(){
    for(int i = 0; i < NUM_COMBS; i++){
        combs[i]->resetDelay();
    }
    
    for(int i = 0; i < NUM_ALLPASSES; i++){
        allpasses[i]->resetDelay();
    }
    
}

//------------------------------------------------------------------
//------------------------------------------------------------------
//  next    : Function to process next sample input in
//          : each input sample is passed to each of the comb 
//          : filters in turn (scaling to prevent clipping)
//          : the output value is accumulated/summed
//          : the result is then passed in series through the 
//          : all-pass filters
//
//  in      :   input sample form the audio buffer
//  
//------------------------------------------------------------------
//------------------------------------------------------------------
DspFloatType Schroeder::next(const DspFloatType in){
    
    // if bypass is enabled on plugin then just pass back
    // the unprocessed input sample
    if(bypass)
        return in;
        
    DspFloatType out = 0.0f;
    
    //step through each of the 4 comb filters and pass a scaled input
    for(int i = 0; i < NUM_COMBS; i++){
        out += combs[i]->next(in * 0.50f); //scale down to avoid clipping
    }
    
    DspFloatType passOut = 0.0f;
    DspFloatType passOut2 = 0.0f;
    
    passOut = allpasses[0]->next(out);          //1 stage all-pass filtering
    passOut2 = allpasses[1]->next(passOut);     //2 stage all-pass filtering
  
    return passOut2;
    
}


}




