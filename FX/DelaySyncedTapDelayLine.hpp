/*
  ==============================================================================

    SyncedTapDelayLine.h
    New version of SyncedTapDelayLine that also accepts an external feedback path
    as input, and which uses fractional delay
    
    The multi-tap delay function is used in filter design as a Finite Impulse
    Response for the use of building an Early Reflections generator and a 
    similar function to this will be employed in the Moorer model later.
    
    Created: 14 Sep 2014 5:15:53pm
    Author:  Keith Hearne

  ==============================================================================
*/

#pragma once

#include <algorithm>
#include <cmath>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include "Helper.h"

//////////////////////////////////////////////////////////
//  SyncedTap DELAY LINE CLASS (feedback optional)
//  see .cpp file for commenting
//////////////////////////////////////////////////////////

class SyncedTapDelayLine{

public:
    //constructor / destructor
    SyncedTapDelayLine(const int sr = 44100, const DspFloatType d_ms = 0.0f, const DspFloatType feedback = 0.0f, const DspFloatType mixLevel = 0.5f);
    ~SyncedTapDelayLine();
    
    //getters
    DspFloatType getDelayTimeMS();
    DspFloatType getMaxDelayTimeMS();
    DspFloatType getDelay();
    DspFloatType getFeedback();
    DspFloatType getTap1Delay();
    DspFloatType getTap2Delay();
    DspFloatType getTap3Delay();
    DspFloatType getTap4Delay();
    DspFloatType getTap1Level();
    DspFloatType getTap2Level();
    DspFloatType getTap3Level();
    DspFloatType getTap4Level();
    DspFloatType getMix();
    bool getByPass();
    DspFloatType getCurrentFeedbackOutput();
    
    //setters
    void setDelayTimeMS(const int sr, const DspFloatType d_ms);
    void setDelay(DspFloatType d_ms);
    void setTap1Delay(DspFloatType td);
    void setTap2Delay(DspFloatType td);
    void setTap3Delay(DspFloatType td);
    void setTap4Delay(DspFloatType td);
    void setTap1Level(DspFloatType tl);
    void setTap2Level(DspFloatType tl);
    void setTap3Level(DspFloatType tl);
    void setTap4Level(DspFloatType tl);    
    void setFeedback(DspFloatType f_pct);
    void setMix(DspFloatType m_pct);
    void setByPass(bool bp);
    void setCurrentFeedbackInput(DspFloatType fb);
    void setUseExternalFeedback(bool b);
    void setDelaySync(DspFloatType quarter, DspFloatType eighth, DspFloatType sixtenth);
    
    //business functions
    DspFloatType next(const DspFloatType in);
    void resetBuffer();
    void resetDelay();

private:
    int writePos, readPosA, readTap1, readTap2, readTap3, readTap4, MAX_DELAY_SAMPLES;
    DspFloatType delay_ms, delay_samples, fraction, feedback, mixLevel, MAX_DELAY_MS, feedbackIn, 
        tap1delay, tap2delay, tap3delay, tap4delay, tap1level, tap2level, tap3level, tap4level;
    bool delay_bypass, useExternalFeedback;
    DspFloatType *buffer;
    
};


//////////////////////////////////////////////////////////
//  SyncedTap VARIABLE DELAY LINE CLASS (feedback optional)
//////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
// Constructor :
// Predefined sample rate = 44100, feedback level & delay = 0, mixlevel=50%
// set read and write index for delay line
// set read indexes for each of the 4 tap delays
// set the delay and gain level for each of the 4 tap delays
// set max delay to 2 seconds
// create a max delay buffer
//-------------------------------------------------------------------------
SyncedTapDelayLine::SyncedTapDelayLine(const int sr, const DspFloatType d_ms, const DspFloatType fb, const DspFloatType mix){
    //assert(d_ms <= d_ms_max);//check bound on delay time
    
    buffer = NULL;
    readPosA = readTap1 = readTap2 = readTap3 = readTap4 = writePos = feedback = mixLevel = 0;
    tap1delay = tap2delay = tap3delay = tap4delay = tap1level = tap2level = tap3level = tap4level = 0.0f;
    delay_bypass = 0;
    useExternalFeedback = false;
    
    //max delay of 2 seconds
    DspFloatType d_ms_max = 2000.0f;
    delay_samples = 0.0f;
    delay_ms = d_ms;
   
    feedback = fb;
    mixLevel = mix;
    
    MAX_DELAY_SAMPLES = ceil(numSamplesFromMSf(sr, d_ms_max));
    MAX_DELAY_MS = MAX_DELAY_SAMPLES * 1000.0f / sr; //make sure DspFloatType version is set with to integer-rounded buffer size
    
    //number of delay samples
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
SyncedTapDelayLine::~SyncedTapDelayLine(){
    delete[] buffer;
}

//getters
//-------------------------------------------------------------------------
// getDelayTimeMS :
// return the delay in milliseconds
//-------------------------------------------------------------------------
DspFloatType SyncedTapDelayLine::getDelayTimeMS(){return delay_ms;}

//-------------------------------------------------------------------------
// getFeedback :
// return the delay value
//-------------------------------------------------------------------------
DspFloatType SyncedTapDelayLine::getDelay(){return delay_ms;}

//-------------------------------------------------------------------------
// getFeedback :
// return the feedback value
//-------------------------------------------------------------------------
DspFloatType SyncedTapDelayLine::getFeedback(){return feedback;}

//-------------------------------------------------------------------------
// getMix :
// return the mix level, set to 50% by default
//-------------------------------------------------------------------------
DspFloatType SyncedTapDelayLine::getMix(){return mixLevel;}

//-------------------------------------------------------------------------
// getByPass :
// return the boolean value indicating if plugin is bypassed or not
//-------------------------------------------------------------------------
bool SyncedTapDelayLine::getByPass(){return delay_bypass;}

//-------------------------------------------------------------------------
// getCurrentFeedbackOutput :
// return the delay value * feedback value
//-------------------------------------------------------------------------
DspFloatType SyncedTapDelayLine::getCurrentFeedbackOutput(){
        //current feedback is feedback*output
        return (feedback*buffer[readPosA]);
};

//setters
//--------------------------------------------------------------------------------
//  Setter function that determines read position index
//  read position is determined by subtracting the number of samples to delay
//  from the write position index
//
//  readIndex = writeIndex - number of sample delay
//
//--------------------------------------------------------------------------------
void SyncedTapDelayLine::setDelayTimeMS(const int sr, const DspFloatType d_ms){
    assert(d_ms <= MAX_DELAY_MS);//check bound on delay time
    
    //d_ms = d_ms;
    DspFloatType delay_samplesf = numSamplesFromMSf(sr, d_ms);
    delay_samples = floor(delay_samplesf);
    fraction = delay_samplesf - delay_samples;//storing fractional delay time, output will be interpolated
    
    readPosA = writePos - (int)delay_samples;
    
    if(readPosA < 0)
        readPosA += MAX_DELAY_SAMPLES;       
        
}

//-------------------------------------------------------------------------
// setDelay :
// set the delay in milliseconds by the delay value passed to function
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setDelay(DspFloatType d){
    //receiving the delay value through here in milliseconds 0 to 2000
    delay_ms = d;
    setDelayTimeMS(44100,delay_ms);
};

//-------------------------------------------------------------------------
// setTap1Delay :
// set the delay and tap1 read index position in the delay buffer
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setTap1Delay(DspFloatType td){
    //receiving the delay value through here in milliseconds 0 to 2000
    tap1delay = td;
    DspFloatType tap_samples = numSamplesFromMSf(44100, tap1delay);
    readTap1 = writePos - (int)tap_samples;
    
    //wrap if out of bounds
    if(readTap1 < 0)
        readTap1 += MAX_DELAY_SAMPLES;       
        
};

//-------------------------------------------------------------------------
// setTap2Delay :
// set the delay and tap2 read index position in the delay buffer
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setTap2Delay(DspFloatType d){
    //receiving the delay value through here in milliseconds 0 to 2000
    tap2delay = d;
    DspFloatType tap_samples = numSamplesFromMSf(44100, tap2delay);
    readTap2 = writePos - (int)tap_samples;
    
    //wrap if out of bounds
    if(readTap2 < 0)
        readTap2 += MAX_DELAY_SAMPLES; 
};

//-------------------------------------------------------------------------
// setTap3Delay :
// set the delay and tap3 read index position in the delay buffer
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setTap3Delay(DspFloatType d){
    //receiving the delay value through here in milliseconds 0 to 2000
    tap3delay = d;
    DspFloatType tap_samples = numSamplesFromMSf(44100, tap3delay);
    readTap3 = writePos - (int)tap_samples;
    
    //wrap if out of bounds
    if(readTap3 < 0)
        readTap3 += MAX_DELAY_SAMPLES; 
};

//-------------------------------------------------------------------------
// setTap4Delay :
// set the delay and tap4 read index position in the delay buffer
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setTap4Delay(DspFloatType d){
    //receiving the delay value through here in milliseconds 0 to 2000
    tap4delay = d;
    DspFloatType tap_samples = numSamplesFromMSf(44100, tap4delay);
    readTap4 = writePos - (int)tap_samples;
    
    //wrap if out of bounds
    if(readTap4 < 0)
        readTap4 += MAX_DELAY_SAMPLES; 
};

//-------------------------------------------------------------------------
// setTap1Level :
// set the tap 1 level which will be a gain multiplier value
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setTap1Level(DspFloatType tl){
    // allow the max volume coefficient to be 0.5
    tap1level = tl;
};

//-------------------------------------------------------------------------
// setTap2Level :
// set the tap 2 level which will be a gain multiplier value
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setTap2Level(DspFloatType tl){
    tap2level = tl;
};

//-------------------------------------------------------------------------
// setTap3Level :
// set the tap 3 level which will be a gain multiplier value
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setTap3Level(DspFloatType tl){
    tap3level = tl;
};

//-------------------------------------------------------------------------
// setTap4Level :
// set the tap 4 level which will be a gain multiplier value
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setTap4Level(DspFloatType tl){
    tap4level = tl;
};

//-------------------------------------------------------------------------
// setFeedback :
// set the feedback value passed to function, -1 to 1
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setFeedback(DspFloatType f){
    //receiving the feedback here as a value between -100 and +100
    feedback = f/100;
};

//-------------------------------------------------------------------------
// setMix :
// set the mix level, value from plugin is 0-100, value in function 0-1
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setMix(DspFloatType m){
    // receiving the mix value through here as value between 0 and 100
    mixLevel = m/100;
};

//-------------------------------------------------------------------------
// setByPass :
// set the bypass value based on bypass value received from plugin
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setByPass(bool bp){
    // receiving the mix value through here as value between 0 and 100
    delay_bypass = bp;
};

//-------------------------------------------------------------------------
// setCurrentFeedbackInput :
// used when crossing feedback lines to provide a feedback input signal
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setCurrentFeedbackInput(DspFloatType fb){
    //set the feedback sample
    feedbackIn = fb;
};
    
//-------------------------------------------------------------------------
// setUseExternalFeedback :
// used when crossing feedback lines to tell delay to take its feedback
// signal externally
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setUseExternalFeedback(bool b){
    //enable or disable an external feedback source
    useExternalFeedback = b;
};


//business function
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//
//  The following function is the workhorse of the delay line
//  It uses terms similar to the differential equation for delay
//
//      x(n)                : the input at sample n
//      y(n)                : the output at sample n after delay processing
//      buffer              : the circular buffer used
//      readPosA            : the read index of the delay buffer
//      readTapX            : tap delay 1-4 read indexes set when delay values
//                          : for these are set
//      tapXOutput          : output for tap 1-4, delay position* tapXlevel
//      writePos            : the write index of the delay buffer
//      MAX_DELAY_SAMPLES   : Max size of delay buffer
//  
//      y(n) = x(n) + x(n - D)              'delay with no feedback
//
//      y(n) = x(n - D) + fb*y(n - D)       'delay with feedback
//
//      y(n) = x(n) + x(n – D) - fb*x(n-D) + fb*y(n-D)  'feedback with wet/dry mix
//
//      MAX_DELAY_SAMPLES = sr * d_ms * .001;
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
DspFloatType SyncedTapDelayLine::next(const DspFloatType in){
    
    if(delay_bypass)
        return in;
        
    //input
    DspFloatType xn = in;
    
    //output of delay at readPos
    DspFloatType yn = buffer[readPosA];
    
    //read the 4 individual taps from buffer and scale
    DspFloatType tap1Output = buffer[readTap1] * tap1level;
    DspFloatType tap2Output = buffer[readTap2] * tap2level;
    DspFloatType tap3Output = buffer[readTap3] * tap3level;
    DspFloatType tap4Output = buffer[readTap4] * tap4level;
    
    
    //if delay < 1 sample, interpolate between x(n) and x(n-1)
    if(readPosA == writePos && delay_samples < 1.00)
    {
            yn = xn;
    }
    
    //read location at n-1, one behind yn
    int readPos_minus1 = readPosA - 1;
    if(readPos_minus1 < 0)
        readPos_minus1 = MAX_DELAY_SAMPLES - 1;   //MAX_DELAY_SAMPLES - 1 is the last location of the buffer
        
    //get y(n-1)
    DspFloatType yn_minus1 = buffer[readPos_minus1];
    
    //perform linear interpolation of : (0,yn) and (1,yn_minus1) by the ammount of fractional delay(fraction)
    DspFloatType interp = linInterp(0, 1, yn, yn_minus1, fraction);
    
    //if delay value is zero just pass input out
    if(delay_samples == 0)
        yn = xn;
    else
        yn = interp;

    //write the input to the delay
    //check if External Feedback path is enabled
    if(!useExternalFeedback)
        buffer[writePos] = xn + feedback*yn;
    else
        buffer[writePos] = xn + feedbackIn;
    
    
    //add the tap delays that are not zero in length
    DspFloatType tappedOutput = yn;
    if(tap1delay){tappedOutput = tappedOutput + tap1Output;}
    if(tap2delay){tappedOutput = tappedOutput + tap2Output;}
    if(tap3delay){tappedOutput = tappedOutput + tap3Output;}
    if(tap4delay){tappedOutput = tappedOutput + tap4Output;}
    
    //create wet level and write to output buffer
    DspFloatType out = mixLevel*tappedOutput + (1.0 - mixLevel)*xn;
    
    //wrap indexes if out of bounds
    writePos++;
    if(writePos >= MAX_DELAY_SAMPLES)
        writePos = 0;
        
    readPosA++;
    if(readPosA >= MAX_DELAY_SAMPLES)
        readPosA = 0;
    
    readTap1++;
    readTap2++;
    readTap3++;
    readTap4++;
    
    if(readTap1 >= MAX_DELAY_SAMPLES)
        readTap1 = 0;

    if(readTap2 >= MAX_DELAY_SAMPLES)
        readTap2 = 0;

    if(readTap3 >= MAX_DELAY_SAMPLES)
        readTap3 = 0;

    if(readTap4 >= MAX_DELAY_SAMPLES)
        readTap4 = 0;        
    
    return out;
        
}

//-------------------------------------------------------------------------
// resetBuffer :
// delete contents of buffer and instantiate a new buffer
//-------------------------------------------------------------------------
void SyncedTapDelayLine::resetBuffer(){
    
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
void SyncedTapDelayLine::resetDelay(){
 
    if(buffer)
        memset(buffer, 0, MAX_DELAY_SAMPLES*sizeof(DspFloatType));
    
    
    readPosA = readTap1 = readTap2 = readTap3 = readTap4 = writePos = 0;
    delay_bypass = 0;
    
    return;
    
}
