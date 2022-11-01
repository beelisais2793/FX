/*
  ==============================================================================

    CrossDelayLine.h
    Created: 9 Oct 2014 11:30:25pm
    Author:  Keith Hearne

    While this design is not absolutely essential to the iterative approach
    of the reverb plugin, the use of crossed feedback loops is employed in
    many filter designs such as all-passes and is used in many stereo effects.
    
    This crossed the feedback paths of the two delay lines left/right through
    the use of the external feedback Input
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
//  CROSS DELAY LINE CLASS (feedback optional)
//////////////////////////////////////////////////////////

namespace Delays
{
    class CrossDelayLine : public MonoFXProcessor
    {

    public:
        //constructor / destructor
        CrossDelayLine(const int sr = 44100, const DspFloatType d_ms = 0.0f, const DspFloatType feedback = 0.0f, const DspFloatType mixLevel = 0.5f, const bool crossFeedback = false);
        ~CrossDelayLine();
        
        //getters
        DspFloatType getDelayTimeMS();
        DspFloatType getMaxDelayTimeMS();
        DspFloatType getFeedback();
        DspFloatType getMix();
        bool getByPass();
        DspFloatType getCurrentFeedbackOutput();
        bool isCrossedFeedback();
        
        //setters
        void setDelayTimeMS(const int sr, const DspFloatType d_ms);
        void setDelay(DspFloatType d_ms);
        void setFeedback(DspFloatType f_pct);
        void setMix(DspFloatType m_pct);
        void setByPass(bool bp);
        void setCurrentFeedbackInput(DspFloatType fb);
        void setUseExternalFeedback(bool b);
        void setCrossedFeedback(bool b);
        
        //business functions
        DspFloatType next(const DspFloatType in);
        void resetBuffer();
        void resetDelay();

        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
        {
            // X = readPos modulation
            // Y = writePos modulation
            return A*next(I);
        }
        void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out)
        {
            for(size_t i = 0; i < n; i++) out[i] = Tick(in[I}]);
        }
    private:
        int writePos, readPosA, MAX_DELAY_SAMPLES;
        DspFloatType delay_ms, delay_samples, fraction, feedback, mixLevel, MAX_DELAY_MS, feedbackIn;
        bool delay_bypass, useExternalFeedback, crossFeedback;
        DspFloatType *buffer;
        
    };






    //////////////////////////////////////////////////////////
    //  CROSS  DELAY LINE CLASS (feedback optional)
    //////////////////////////////////////////////////////////

    //-------------------------------------------------------------------------
    // Constructor :
    // Predefined sample rate = 44100, feedback level & delay = 0, mixlevel=50%
    // cross parameter to determine if feedback paths should be crosed default
    // set read and write index for delay line
    // set max delay to 2 seconds
    // create a max delay buffer
    //-------------------------------------------------------------------------
    CrossDelayLine::CrossDelayLine(const int sr, const DspFloatType d_ms, const DspFloatType fb, const DspFloatType mix, const bool cross)
    : StereoFXProcessor()
    {
        //assert(d_ms <= d_ms_max);//check bound on delay time
        
        buffer = NULL;
        readPosA = writePos = feedback = mixLevel = 0;
        delay_bypass = 0;
        //do not use external feedback by default
        useExternalFeedback = false;
        
        //default max delay of 2 seconds
        DspFloatType d_ms_max = 2000.0f;
        delay_samples = 0.0f;
        delay_ms = d_ms;
    
        feedback = fb;
        mixLevel = mix;
        //crossed feedback boolean for crossing feedback paths
        crossFeedback = cross;
        
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
    CrossDelayLine::~CrossDelayLine(){
        delete[] buffer;
    }

    //getters
    //-------------------------------------------------------------------------
    // getDelayTimeMS :
    // return the delay in milliseconds
    //-------------------------------------------------------------------------
    DspFloatType CrossDelayLine::getDelayTimeMS(){return delay_ms;}

    //-------------------------------------------------------------------------
    // getFeedback :
    // return the delay value
    //-------------------------------------------------------------------------
    DspFloatType CrossDelayLine::getFeedback(){return feedback;}

    //-------------------------------------------------------------------------
    // getMix :
    // return the mix level, set to 50% by default
    //-------------------------------------------------------------------------
    DspFloatType CrossDelayLine::getMix(){return mixLevel;}
        
    //-------------------------------------------------------------------------
    // getByPass :
    // return the boolean value indicating if plugin is bypassed or not
    //-------------------------------------------------------------------------
    bool CrossDelayLine::getByPass(){return delay_bypass;}

    //-------------------------------------------------------------------------
    // getCurrentFeedbackOutput :
    // return the delay value * feedback value
    //-------------------------------------------------------------------------
    DspFloatType CrossDelayLine::getCurrentFeedbackOutput(){
            //current feedback is feedback*output
            return (feedback*buffer[readPosA]);
    };

    //-------------------------------------------------------------------------
    // isCrossedFeedbackOutput :
    // return a boolean to determine if feedback paths are currently crossed
    //-------------------------------------------------------------------------
    bool CrossDelayLine::isCrossedFeedback(){return crossFeedback;}

    //setters
    //--------------------------------------------------------------------------------
    //  Setter function that determines read position index
    //  read position is determined by subtracting the number of samples to delay
    //  from the write position index
    //
    //  readIndex = writeIndex - number of sample delay
    //
    //--------------------------------------------------------------------------------
    void CrossDelayLine::setDelayTimeMS(const int sr, const DspFloatType d_ms){
        assert(d_ms <= MAX_DELAY_MS);//check bound on delay time
        
        //d_ms = d_ms;
        DspFloatType delay_samplesf = numSamplesFromMSf(sr, d_ms);
        delay_samples = floor(delay_samplesf);
        
        //storing fractional delay time, output will be interpolated
        fraction = delay_samplesf - delay_samples;
        
        //determine read index and wrap if bounds exceeded
        readPosA = writePos - (int)delay_samples;
        if(readPosA < 0)
            readPosA += MAX_DELAY_SAMPLES;
            
            
    }

    //-------------------------------------------------------------------------
    // setDelay :
    // set the delay in milliseconds by the delay value passed to function
    //-------------------------------------------------------------------------
    void CrossDelayLine::setDelay(DspFloatType d){
        //receiving the delay value through here in milliseconds 0 to 2000
        delay_ms = d;
        setDelayTimeMS(44100,delay_ms);
    };

    //-------------------------------------------------------------------------
    // setFeedback :
    // set the feedback value passed to function, -1 to 1
    //-------------------------------------------------------------------------
    void CrossDelayLine::setFeedback(DspFloatType f){
        //receiving the feedback here as a value between -100 and +100
        feedback = f/100;
    };

    //-------------------------------------------------------------------------
    // setMix :
    // set the mix level, value from plugin is 0-100, value in function 0-1
    //-------------------------------------------------------------------------
    void CrossDelayLine::setMix(DspFloatType m){
        // receiving the mix value through here as value between 0 and 100
        mixLevel = m/100;
    };

    //-------------------------------------------------------------------------
    // setByPass :
    // set the bypass value based on bypass value received from plugin
    //-------------------------------------------------------------------------
    void CrossDelayLine::setByPass(bool bp){
        // receiving the mix value through here as value between 0 and 100
        delay_bypass = bp;
    };

    //-------------------------------------------------------------------------
    // setCurrentFeedbackInput :
    // used when crossing feedback lines to provide a feedback input signal
    //-------------------------------------------------------------------------
    void CrossDelayLine::setCurrentFeedbackInput(DspFloatType fb){
        //set the feedback sample
        feedbackIn = fb;
    };
        
    //-------------------------------------------------------------------------
    // setUseExternalFeedback :
    // used when crossing feedback lines to tell delay to take its feedback
    // signal externally
    //-------------------------------------------------------------------------
    void CrossDelayLine::setUseExternalFeedback(bool b){
        //enable or disable an external feedback source
        useExternalFeedback = b;
    };

    //-------------------------------------------------------------------------
    // setCrossedFeedback :
    // set the use of the external feedback based on crossFeedback value
    //-------------------------------------------------------------------------
    void CrossDelayLine::setCrossedFeedback(bool b){
        //enable or disable crossed-feedback delay lines
        crossFeedback = b;
        if(crossFeedback)
            setUseExternalFeedback(true);
        else
            setUseExternalFeedback(false);
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
    DspFloatType CrossDelayLine::next(const DspFloatType in){
        
        if(delay_bypass)
            return in;
            
        //input
        DspFloatType xn = in;
        
        //output of delay at readPos
        DspFloatType yn = buffer[readPosA];
        
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
        //if enabled then you write the input plus the 
        //feedbaclIn signal to the delay line instead
        if(!useExternalFeedback)
            buffer[writePos] = xn + feedback*yn;
        else
            buffer[writePos] = xn + feedbackIn;
        
        //create wet level and write to output buffer
        DspFloatType out = mixLevel*yn + (1.0 - mixLevel)*xn;
        
        //wrap if bounds exceeded
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
    void CrossDelayLine::resetBuffer(){
        
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
    void CrossDelayLine::resetDelay(){
    
        if(buffer)
            memset(buffer, 0, MAX_DELAY_SAMPLES*sizeof(DspFloatType));
        
        
        readPosA = writePos = 0;
        delay_bypass = 0;
        
        return;
        
    }
}