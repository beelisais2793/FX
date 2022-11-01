/*
  ==============================================================================

    ERTapDelayLine.h
    Tap Delay used for Early Reflection Generation as proposed by Moorers design
    In Moorers 1979 paper About This Reverberation Business he took a model of
    real room impulse responses for Early Reflection Generation from impulses
    recorded in the Boston Symphony Hall.
    
    These values are used here in a multi-tap setup to provide the summed
    output of the 18 taps (the first tap will be the input impulse, added at 
    the reverb generation stage) to provide an early reflection generator.
    
    Created: 16 Nov 2014 5:15:53pm
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
//  ER TAP DELAY LINE CLASS (for use with Moorer Reverb)
//  see .cpp file for comments
//////////////////////////////////////////////////////////

namespace Delays::ERTapDelayLine
{
    class ERTapDelayLine{

    public:
        //constructor / destructor
        ERTapDelayLine(const int sr = 44100, const DspFloatType d_ms = 0.0f, const DspFloatType d_max = 0.0f);
        ~ERTapDelayLine();
        
        //getters
        DspFloatType getDelayTimeMS();
        DspFloatType getMaxDelayTimeMS();
        
        //setters
        void setDelayTimeMS(const int sr, const DspFloatType d_ms);
        void setDelay(DspFloatType d_ms);
        
        //business functions
        DspFloatType readDelay(DspFloatType in);
        void writeDelay(DspFloatType in);
        DspFloatType next(const DspFloatType in);
        void resetBuffer();
        void resetDelay();

    private:
        int writePos, readPosA, MAX_DELAY_SAMPLES;
        DspFloatType delay_ms, delay_samples, fraction, MAX_DELAY_MS, accumulatedTaps;
        DspFloatType *buffer;
        
        //Early Reflection TAP delay values as presented in Moorer's 1979 paper
        //values extracted from impulse measurements in Boston Symphony Hall
        //These are the delay times for each of the 18 tap readIndex
        //The first tap is unit gain for the input signal, not shown here
        const DspFloatType ER_TAPS[18] = {
            0.0043, 0.0215, 0.0225, 0.0268, 0.0270, 0.0298, 0.0458, 0.0485, 0.0572, 0.0587, 0.0595, 0.0612,
            0.0707, 0.0708, 0.0726, 0.0741, 0.0753, 0.0797
        };
        //Early reflection GAIN values to apply to each of the above taps
        const DspFloatType ER_GAINS[18] = {
            0.841, 0.504, 0.491, 0.379, 0.380, 0.346, 0.289, 0.272, 0.192, 0.193, 0.217, 0.181, 0.180,
            0.181, 0.176, 0.142, 0.167, 0.134
        };
        
    };



    //////////////////////////////////////////////////////////
    //  ER TAP DELAY LINE CLASS (for use with Moorer Reverb)
    //////////////////////////////////////////////////////////

    //-------------------------------------------------------------------------
    // Constructor :
    // Predefined sample rate = 44100, delay = 0, max delay
    // set read and write index for delay line
    // set max delay set from decay factor in reverb
    // create a max delay buffer
    //-------------------------------------------------------------------------
    ERTapDelayLine::ERTapDelayLine(const int sr, const DspFloatType d_ms, const DspFloatType d_max){
        //assert(d_ms <= d_ms_max);//check bound on delay time
        
        buffer = NULL;
        readPosA = writePos = 0;
        
        delay_samples = accumulatedTaps = 0.0f;
        delay_ms = d_ms;
        
        MAX_DELAY_SAMPLES = ceil(tapNumSamplesFromMSf(sr, d_max));
        MAX_DELAY_MS = MAX_DELAY_SAMPLES * 1000.0f / sr; //make sure DspFloatType version is set with to integer-rounded buffer size
        
        DspFloatType delay_samplesf = tapNumSamplesFromMSf(sr, d_ms);
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
    ERTapDelayLine::~ERTapDelayLine(){
        delete[] buffer;
    }

    //getters
    //-------------------------------------------------------------------------
    // getDelayTimeMS :
    // return the delay in milliseconds
    //-------------------------------------------------------------------------
    DspFloatType ERTapDelayLine::getDelayTimeMS(){return delay_ms;}

    //--------------------------------------------------------------------------------
    //  getMaxDelayTimeMS :
    //  return the Max delay time in Miliiseconds
    //--------------------------------------------------------------------------------
    DspFloatType ERTapDelayLine::getMaxDelayTimeMS(){return MAX_DELAY_MS;}

    //setters
    //--------------------------------------------------------------------------------
    //  Setter function that determines read position index
    //  read position is determined by subtracting the number of samples to delay
    //  from the write position index
    //
    //  readIndex = writeIndex - number of sample delay
    //
    //--------------------------------------------------------------------------------
    void ERTapDelayLine::setDelayTimeMS(const int sr, const DspFloatType d_ms){
        assert(d_ms <= MAX_DELAY_MS);//check bound on delay time
        
        DspFloatType delay_samplesf = tapNumSamplesFromMSf(sr, d_ms);
        this->delay_samples = floor(delay_samplesf);
        
        //storing fractional delay time, output will be interpolated
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
    void ERTapDelayLine::setDelay(DspFloatType d){
        //receiving the delay value through here in milliseconds 0 to 2000
        delay_ms = d;
        setDelayTimeMS(44100,delay_ms);
    };

    //----------------------------------------------------
    //----------------------------------------------------
    //  readDelay : this is early reflections delay line
    //
    //  each read of the delay line gives each of the 
    //  19 taps scaled by gains for a 19 Impulse FIR 
    //  as laid out in Moorerspaper. Values are 
    //  all hardcoded and initialised in the .h file
    //----------------------------------------------------
    //----------------------------------------------------
    DspFloatType ERTapDelayLine::readDelay(DspFloatType in){

        DspFloatType interpVal = 0.0f;
        DspFloatType accumulatedTaps = 0.0f;
        //first tap is the input sample at gain of 1
        accumulatedTaps = in;       //I have changed this to in value

    //output of delay at readPos
        DspFloatType yn = 0.0f;
        
        //loop through the amount of ER_TAP Values accumulate the delay*gain
        //each time through the loop
        for(int i=0; i < (sizeof(ER_TAPS)/sizeof(ER_TAPS[0])); i++ ){
            
            //set the delay samples for this specific TAP, which aligns the read index
            setDelay(ER_TAPS[i]);
            yn = buffer[readPosA];
    
            //read location at n-1, one behind yn
            int readPos_minus1 = readPosA - 1;
            if(readPos_minus1 < 0)
                readPos_minus1 = MAX_DELAY_SAMPLES - 1;   //MAX_DELAY_SAMPLES - 1 is the last location of the buffer
                
            //get y(n-1)
            DspFloatType yn_minus1 = buffer[readPos_minus1];
            
            //perform linear interpolation of : (0,yn) and (1,yn_minus1) by the ammount of fractional delay(fraction)
            interpVal = linInterp(0, 1, yn, yn_minus1, fraction);
            
            //This is the accumulation of tap delay values with the respective gain
            accumulatedTaps +=  interpVal * ER_GAINS[i];
        }
        
        //return the accumulated value for tap delays
        return accumulatedTaps;

    }

    //--------------------------------------------------------------------------------
    //  writeDelay :
    //
    //  The following function writes into the delay buffer at write index
    //---------------------------------------------------------------------------------
    void ERTapDelayLine::writeDelay(DspFloatType in){
        
        //write the input sample into the write position of the buffer
        buffer[writePos] = in;
        
        //increment indexes and wrap if bounds exceeded
        writePos++;
        if(writePos >= MAX_DELAY_SAMPLES)
            writePos = 0;
            
        readPosA++;
        if(readPosA >= MAX_DELAY_SAMPLES)
            readPosA = 0;
    }

    //business function
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
    DspFloatType ERTapDelayLine::next(const DspFloatType in){
            
        //input
        DspFloatType xn = in;

        //if delay value is zero just pass input out
        DspFloatType yn;
        yn = readDelay(in);

        writeDelay(xn);
        
        return yn;
            
    }

    //-------------------------------------------------------------------------
    // resetBuffer :
    // delete contents of buffer and instantiate a new buffer
    //-------------------------------------------------------------------------
    void ERTapDelayLine::resetBuffer(){
        
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
    void ERTapDelayLine::resetDelay(){
    
        if(buffer)
            memset(buffer, 0, MAX_DELAY_SAMPLES*sizeof(DspFloatType));
        
        readPosA = writePos = 0;
        
        return;
        
    }
}