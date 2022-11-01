#pragma once

#include <algorithm>
#include <cmath>
#include <cassert>
#include <cstdlib>
#include <iostream>

namespace FX::KHDelays
{
    //////////////////////////////////////////////////////////
    //  BASIC DELAY LINE CLASS (feedback optional)
    //  see comments in .cpp file
    //////////////////////////////////////////////////////////

    class BasicDelayLine{

    public:
        BasicDelayLine(const int sr = 44100, const DspFloatType d_ms = 0.0f, const DspFloatType feedback = 0.0f, const DspFloatType mixLevel = 0.5f);
        ~BasicDelayLine();
        
        //getters
        DspFloatType getDelayTimeMS();
        DspFloatType getMaxDelayTimeMS();
        DspFloatType getFeedback();
        DspFloatType getMix();
        bool getByPass();
        
        //setters
        void setDelayTimeMS(const int sr, const DspFloatType d_ms);
        void setDelay(DspFloatType d_ms);
        void setFeedback(DspFloatType f_pct);
        void setMix(DspFloatType m_pct);
        void setByPass(bool bp);
        
        //business functions
        DspFloatType next(const DspFloatType in);
        void resetBuffer();
        void resetDelay();

    private:
        int writePos, readPosA, MAX_DELAY_SAMPLES;
        DspFloatType delay_ms, delay_samples, fraction, feedback, mixLevel, MAX_DELAY_MS;
        bool delay_bypass;
        DspFloatType *buffer;
        
    };

    //helper functions
    //-------------------------------------------------------------------------
    // numSamplesFromMSf :
    // Determine the number of samples from the number of milliseconds delay
    // passed to function
    //-------------------------------------------------------------------------
    inline DspFloatType numSamplesFromMSf(const int sr, const DspFloatType d_ms){
        return sr * d_ms * .001;
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
    BasicDelayLine::BasicDelayLine(const int sr, const DspFloatType d_ms, const DspFloatType fb, const DspFloatType mix){
        //assert(d_ms <= d_ms_max);//check bound on delay time
        
        buffer = NULL;
        readPosA = writePos = feedback = mixLevel = 0;
        delay_bypass = 0;
        
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
    BasicDelayLine::~BasicDelayLine(){
        delete[] buffer;
    }

    //getters
    //-------------------------------------------------------------------------
    // getDelayTimeMS :
    // return the delay in milliseconds
    //-------------------------------------------------------------------------
    DspFloatType BasicDelayLine::getDelayTimeMS(){return delay_ms;}

    //-------------------------------------------------------------------------
    // getFeedback :
    // return the feedback value
    //-------------------------------------------------------------------------
    DspFloatType BasicDelayLine::getFeedback(){return feedback;}

    //-------------------------------------------------------------------------
    // getMix :
    // return the mix level, set to 50% by default
    //-------------------------------------------------------------------------
    DspFloatType BasicDelayLine::getMix(){return mixLevel;}
    
    //-------------------------------------------------------------------------
    // getByPass :
    // return the boolean value indicating if plugin is bypassed or not
    //-------------------------------------------------------------------------
    bool BasicDelayLine::getByPass(){return delay_bypass;}


    //--------------------------------------------------------------------------------
    //  Setter function that determines read position index
    //  read position is determined by subtracting the number of samples to delay
    //  from the write position index
    //
    //  readIndex = writeIndex - number of sample delay
    //
    //--------------------------------------------------------------------------------
    void BasicDelayLine::setDelayTimeMS(const int sr, const DspFloatType d_ms){
        assert(d_ms <= MAX_DELAY_MS);//check bound on delay time
        
        //function returns the number of samples from a delay in millsecond value
        //  number of samples = sr * d_ms * .001
        DspFloatType delay_samplesf = numSamplesFromMSf(sr, d_ms);
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
    void BasicDelayLine::setDelay(DspFloatType d){
        //receiving the delay value through here in milliseconds 0 to 2000
        delay_ms = d;
        setDelayTimeMS(44100,delay_ms);
    };

    //-------------------------------------------------------------------------
    // setFeedback :
    // set the feedback value passed to function
    //-------------------------------------------------------------------------
    void BasicDelayLine::setFeedback(DspFloatType f){
        //receiving the feedback here as a value between -100 and +100
        feedback = f/100;
    };

    //-------------------------------------------------------------------------
    // setMix :
    // set the mix level, value from plugin is 0-100, value in function 0-1
    //-------------------------------------------------------------------------
    void BasicDelayLine::setMix(DspFloatType m){
        // receiving the mix value through here as value between 0 and 100
        mixLevel = m/100;
    };

    //-------------------------------------------------------------------------
    // setByPass :
    // set the bypass value based on bypass value received from plugin
    //-------------------------------------------------------------------------
    void BasicDelayLine::setByPass(bool bp){
        // receiving the bypass value as true or false
        delay_bypass = bp;
    };


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

    DspFloatType BasicDelayLine::next(const DspFloatType in){
        
        if(delay_bypass)
            return in;
            
        //input x(n) takes the sample passed into function
        DspFloatType xn = in;
        
        //output y(n) of delay at readPos
        //once read from the buffer the read index is incremented
        DspFloatType yn = buffer[readPosA++];
        
        //if delay value is zero just pass input out, y(n) = x(n)
        if(delay_samples == 0)
            yn = xn;

        //write the input to the delay
        //  y(n) = x(n - D) + fb*y(n - D)
        buffer[writePos] = xn + feedback*yn;
        
        //create wet level and write to output buffer
        //  y(n) = x(n) + x(n – D) - fb*x(n-D) + fb*y(n-D)
        DspFloatType out = mixLevel*yn + (1.0 - mixLevel)*xn;
        
        //wrap the circular buffer back to the start if exceeds bounds
        writePos++;
        if(writePos >= MAX_DELAY_SAMPLES)
            writePos = 0;
            
        if(readPosA >= MAX_DELAY_SAMPLES)
            readPosA = 0;
            
        // return the output y(n) with feedback + wet/dry
        return out;
            
    }

    //-------------------------------------------------------------------------
    // resetBuffer :
    // delete contents of buffer and instantiate a new buffer
    //-------------------------------------------------------------------------
    void BasicDelayLine::resetBuffer(){
        
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
    void BasicDelayLine::resetDelay(){
    
        if(buffer){
            memset(buffer, 0, MAX_DELAY_SAMPLES*sizeof(DspFloatType));
            //std::fill(buffer, buffer + MAX_DELAY_SAMPLES, 0.0f);
        }
        
        readPosA = writePos = 0;
        delay_bypass = 0;
        
        return;
        
    }


    struct KHDelay : public MonoFXProcessor
    {
        BasicDelayLine delay;
        

        KHDelay(const int sr, const DspFloatType d_ms, const DspFloatType fb, const DspFloatType mix) :
        MonoFXProcessor(),
        delay(sr,d_ms,fb,mix)
        {

        }

        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
        {
            DspFloatType r = delay.next(I);
            return r;
        }
    };
    struct KHDoubleFeedDelay : public MonoFXProcessor
    {
        BasicDelayLine delay;
        BasicDelayLine feed;
        DspFloatType mix = 0.5;

        KHDoubleFeedDelay(const int sr, const DspFloatType d_ms, const DspFloatType fb, const DspFloatType mix) :
        MonoFXProcessor(),
        delay(sr,d_ms,fb,mix),
        feed(sr,d_ms/2,fb.mix)
        {

        }

        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
        {
            DspFloatType x = feed.next(I);
            DspFloatType r = delay.next(mix*I + (1-mix)*x);
            return r;
        }
    };
    struct KHBipodDelay : public MonoFXProcessor
    {
        BasicDelayLine delay;
        BasicDelayLine feed;
        DspFloatType mix = 0.5;
        DspFloatType feedback = 0;
        DspFloatType last = 0;

        KHBipodDelay(const int sr, const DspFloatType d_ms, const DspFloatType fb, const DspFloatType mix) :
        MonoFXProcessor(),
        delay(sr,d_ms,fb,mix),
        feed(sr,d_ms/2,fb.mix)
        {

        }

        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
        {
            DspFloatType x = delayL.next(I + feedback*last);
            DspFloatType y = delayR.next(I + feedback*last);            
            if( I < 0) last=x;
            else last=y;
            return last;               
        }
    };
    struct KHDualFeedDelay : public MonoFXProcessor
    {
        KHDoubleFeedDelay delayL,delayR;
        DspFloatType mix = 0.5;
        
        KHDualFeedDelay(const int sr, const DspFloatType d_ms, const DspFloatType fb, const DspFloatType mix) :
        MonoFXProcessor(),
        delayL(sr,d_ms,fb,mix),
        delayR(sr,d_ms,fb,mix)
        {

        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
        {
            DspFloatType x = feed.next(I);
            DspFloatType r = delay.next(mix*I + (1-mix)*x);
            return r;            
        }
    }
    struct KHStereoDelay : public StereoFXProcessor
    {
        BasicDelayLine delayL;
        BasicDelayLine delayR;

        KHDelay(const int sr, const DspFloatType d_ms, const DspFloatType fb, const DspFloatType mix) :
        StereoFXProcessor(),
        delayL(sr,d_ms,fb,mix),
        delayR(sr,d_ms,fb,mix)
        {

        }

        DspFloatType Tick(DspFloatType IL, DspFloatType IR, DspFloatType & L, DspFloatType & R, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
        {
            L = delayL.next(IL);
            R = delayR.next(IR);
            return 0.5*(L+R);
        }
    };

    template<int N>
    struct KHMultiDelay : public MonoFXProcessor
    {
        BasicDelayLine *delay[N];

        KHMultiDelay(const int sr, const DspFloatType d_ms, const DspFloatType fb, const DspFloatType mix) :
        MonoFXProcessor(),        
        {
            for(size_t i = 0; i < N; i++)
                delay[i] = new BasicDelayLine(sr,d_ms,fb,mix);
        }

        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
        {
            DspFloatType r = I;
            for(size_t i = 0; i < N; i++) r += delay[i]->next(r);
            return r/(DspFloatType)N;
        }
    };
    struct KHMultiStereoDelay : public StereoFXProcessor
    {
        KHMultiDelay left,right;

        KHDelay(const int sr, const DspFloatType d_ms, const DspFloatType fb, const DspFloatType mix) :
        StereoFXProcessor(),
        left(sr,d_ms,fb,mix),
        right(sr,d_ms,fb,mix)
        {

        }

        DspFloatType Tick(DspFloatType IL, DspFloatType IR, DspFloatType & L, DspFloatType & R, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
        {
            L = left.next(IL);
            R = right.next(IR);
            return 0.5*(L+R);
        }
    };
}