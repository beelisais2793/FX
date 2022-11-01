/*
  ==============================================================================
    BasicDelayLine.h
    New version of BasicDelayLine that also accepts an external feedback path
    as input, and which uses fractional delay
    
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
#include "SoundAlchemy.hpp"

namespace SoundAlchemy::Delays
{
    template<typename T>
    class BasicDelayLine
    {
    public:
        BasicDelayLine(const int sr = 44100, const T d_ms = 0.0f, const T feedback = 0.0f, const T mixLevel = 0.5f);
        ~BasicDelayLine();
        
        T getDelayTimeMS();
        T getMaxDelayTimeMS();
        T getFeedback();
        T getMix();
        bool getByPass();
        T getCurrentFeedbackOutput();
        
        void setDelayTimeMS(const int sr, const T d_ms);
        void setDelay(T d_ms);
        void setFeedback(T f_pct);
        void setMix(T m_pct);
        void setByPass(bool bp);
        void setCurrentFeedbackInput(T fb);
        void setUseExternalFeedback(bool b);
        
        T next(const T in);
        void resetBuffer();
        void resetDelay();

    private:
        int writePos, readPosA, MAX_DELAY_SAMPLES;
        T delay_ms, delay_samples, fraction, feedback, mixLevel, MAX_DELAY_MS, feedbackIn;
        bool delay_bypass, useExternalFeedback;
        T *buffer;
        
    };

    //-------------------------------------------------------------------------
    // numSamplesFromMSf :
    // Determine the number of samples from the number of milliseconds delay
    // passed to function
    //-------------------------------------------------------------------------
    template<typename T>
    inline T numSamplesFromMSf(const int sr, const T d_ms){
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
    template<typename T>
    inline T linInterp(T x1, T x2, T y1, T y2, T x){
        T denom = x2 - x1;
        if(denom == 0)
            return y1; // should not ever happen

        // calculate decimal position of x
        T dx = (x - x1)/(x2 - x1);

        // use weighted sum method of interpolating
        T result = dx*y2 + (1-dx)*y1;

        return result; 
    }


    //////////////////////////////////////////////////////////
    //  BASIC VARIABLE DELAY LINE CLASS (feedback optional)
    //////////////////////////////////////////////////////////

    //-------------------------------------------------------------------------
    // Constructor :
    // Predefined sample rate = 44100, feedback level & delay = 0, mixlevel=50%
    // set read and write index for delay line
    // set max delay to 2 seconds
    // create a max delay buffer
    //-------------------------------------------------------------------------

    template<typename T>
    BasicDelayLine<T>::BasicDelayLine(const int sr, const T d_ms, const T fb, const T mix){
        //assert(d_ms <= d_ms_max);//check bound on delay time
        
        buffer = NULL;
        readPosA = writePos = feedback = mixLevel = 0;
        delay_bypass = 0;
        
        //external feedback not enabled
        useExternalFeedback = false;
        
        //max delay of 2 seconds
        T d_ms_max = 2000.0f;
        delay_samples = 0.0f;
        delay_ms = d_ms;
    
        feedback = fb;
        mixLevel = mix;
        
        MAX_DELAY_SAMPLES = std::ceil(numSamplesFromMSf(sr, d_ms_max));
        MAX_DELAY_MS = MAX_DELAY_SAMPLES * 1000.0f / sr; //make sure T version is set with to integer-rounded buffer size
        
        T delay_samplesf = numSamplesFromMSf(sr, d_ms);
        delay_samples = std::floor(delay_samplesf);
        fraction = delay_samplesf - delay_samples;//storing fractional delay time, will be interpolated
        
        buffer = new T[MAX_DELAY_SAMPLES];
        memset(buffer, 0, MAX_DELAY_SAMPLES*sizeof(T));
        
    }

    //-------------------------------------------------------------------------
    // Destructor :
    // delete delay buffer
    //-------------------------------------------------------------------------

    template<typename T>
    BasicDelayLine<T>::~BasicDelayLine(){
        delete[] buffer;
    }

    //getters
    //-------------------------------------------------------------------------
    // getDelayTimeMS :
    // return the delay in milliseconds
    //-------------------------------------------------------------------------

    template<typename T>
    T BasicDelayLine<T>::getDelayTimeMS(){return delay_ms;}

    //-------------------------------------------------------------------------
    // getFeedback :
    // return the feedback value
    //-------------------------------------------------------------------------

    template<typename T>
    T BasicDelayLine<T>::getFeedback(){return feedback;}

    //-------------------------------------------------------------------------
    // getMix :
    // return the mix level, set to 50% by default
    //-------------------------------------------------------------------------

    template<typename T>
    T BasicDelayLine<T>::getMix(){return mixLevel;}

    //-------------------------------------------------------------------------
    // getByPass :
    // return the boolean value indicating if plugin is bypassed or not
    //-------------------------------------------------------------------------
    template<typename T>
    bool BasicDelayLine<T>::getByPass(){return delay_bypass;}

    //-------------------------------------------------------------------------
    // getCurrentFeedbackOutput :
    // return delay from the buffer * feedback value
    //-------------------------------------------------------------------------

    template<typename T>
    T BasicDelayLine<T>::getCurrentFeedbackOutput(){
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

    template<typename T>
    void BasicDelayLine<T>::setDelayTimeMS(const int sr, const T d_ms){
        assert(d_ms <= MAX_DELAY_MS);//check bound on delay time
        
        //function returns the number of samples from a delay in millsecond value
        //  number of samples = sr * d_ms * .001
        T delay_samplesf = numSamplesFromMSf(sr, d_ms);
        delay_samples = floor(delay_samplesf);
        
        //storing fractional delay time, output will be interpolated
        fraction = delay_samplesf - delay_samples;
        
        //the read index is determined from the write index minus the number of
        //samples to delay by
        readPosA = writePos - (int)delay_samples;
        if(readPosA < 0)
            readPosA += MAX_DELAY_SAMPLES;
                
    }

    //-------------------------------------------------------------------------
    // setDelay :
    // set the delay in milliseconds by the delay value passed to function
    //-------------------------------------------------------------------------

    template<typename T>
    void BasicDelayLine<T>::setDelay(T d){
        //receiving the delay value through here in milliseconds 0 to 2000
        delay_ms = d;
        setDelayTimeMS(44100,delay_ms);
    };

    //-------------------------------------------------------------------------
    // setFeedback :
    // set the feedback value passed to function -1 to 1
    //-------------------------------------------------------------------------

    template<typename T>
    void BasicDelayLine<T>::setFeedback(T f){
        //receiving the feedback here as a value between -100 and +100
        feedback = f;
    };

    //-------------------------------------------------------------------------
    // setMix :
    // set the mix level, value from plugin is 0-100, value in function 0-1
    //-------------------------------------------------------------------------

    template<typename T>
    void BasicDelayLine<T>::setMix(T m){
        // receiving the mix value through here as value between 0 and 100
        mixLevel = m;
    };

    //-------------------------------------------------------------------------
    // setByPass :
    // set the bypass value based on bypass value received from plugin
    //-------------------------------------------------------------------------

    template<typename T>
    void BasicDelayLine<T>::setByPass(bool bp){
        // receiving the mix value through here as value between 0 and 100
        delay_bypass = bp;
    };

    //-------------------------------------------------------------------------
    // setCurrentFeedbackInput :
    // set the feedback value being passed from the feedback parameter on UI
    //-------------------------------------------------------------------------

    template<typename T>
    void BasicDelayLine<T>::setCurrentFeedbackInput(T fb){
        //set the feedback sample
        feedbackIn = fb;
    };
        
    //-------------------------------------------------------------------------
    // setUseExternalFeedback :
    // set the whether feedback value should be read from an external input
    // such as the other feedback channel output instead of internal
    //-------------------------------------------------------------------------

    template<typename T>
    void BasicDelayLine<T>::setUseExternalFeedback(bool b){
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

    template<typename T>
    T BasicDelayLine<T>::next(const T in){
        
        if(delay_bypass)
            return in;
            
        //input
        T xn = in;
        
        //output of delay at readPos
        T yn = buffer[readPosA];
        
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
        T yn_minus1 = buffer[readPos_minus1];
        
        //perform linear interpolation of : (0,yn) and (1,yn_minus1) by the ammount of fractional delay(fraction)
        T interp = linInterp(0, 1, yn, yn_minus1, fraction);
        
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
        
        //create wet level and write to output buffer
        T out = mixLevel*yn + (1.0 - mixLevel)*xn;
        
        //wrap values if they exceed bounds
        writePos++;
        if(writePos >= MAX_DELAY_SAMPLES)
            writePos = 0;
            
        readPosA++;
        if(readPosA >= MAX_DELAY_SAMPLES)
            readPosA = 0;
            
        
        return out;
            
    }

    //-------------------------------------------------------------------------
    // resetBuffer :
    // delete contents of buffer and instantiate a new buffer
    //-------------------------------------------------------------------------

    template<typename T>
    void BasicDelayLine<T>::resetBuffer(){
        
        if(buffer)
            delete [] buffer;
            
        buffer = new T[MAX_DELAY_SAMPLES];
        
        resetDelay();
        setDelayTimeMS(44100,delay_ms);
        
        return;
        
    }
            
    //-------------------------------------------------------------------------
    // resetDelay :
    // reset the delay buffer by filling with 0's, reset all indexes and bypass
    //-------------------------------------------------------------------------

    template<typename T>
    void BasicDelayLine<T>::resetDelay(){
    
        if(buffer)
            memset(buffer, 0, MAX_DELAY_SAMPLES*sizeof(T));
        
        
        readPosA = writePos = 0;
        delay_bypass = 0;
        
        return;
        
    }
}