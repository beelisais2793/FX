/*
  ==============================================================================

    DelayLine.cpp
    Created: 19 Feb 2018 4:40:29pm
    Author:  Nikolaj Schwab Andersson

  ==============================================================================
*/
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "OnePoleFilter.hpp"

class DelayLine{
    public:
    
    void initDelay(float _delayInSeconds, float _fs){
        //delayInSeconds = _delayInSeconds;
        fs = _fs;
        setDelayTime(_delayInSeconds);
        filter.setSampleRate(fs);
        
        for (int i = 0; i < maxDelayTime; i++)
            delay[i] = 0.0f;
    }
    
    
    void setDelayTime(float _delayInSeconds){
        delayInSeconds = _delayInSeconds;
        delayInSamples = ceil(fs*delayInSeconds);
        
        if (delayInSamples > maxDelayTime)
            delayInSamples = maxDelayTime;
        
        frac = fs*delayInSeconds - floor(fs*delayInSeconds);
    }
    
    void setFeedback(float _feedback){
        feedback = _feedback;
    }
    
    void setFrequency(float freq){
        delayInSamples = ceil(fs/freq);
        frac = fs/freq - floor(fs/freq);

    }
    void setDelayLengthInSamples(float lengthInSamples) // it can also be fractional samples
    {
        delayInSamples = ceil(lengthInSamples);
        
        if (delayInSamples > maxDelayTime)
            delayInSamples = maxDelayTime;

        frac = lengthInSamples - floor(lengthInSamples);
    }
    
    void tick(float input){
        
        int writePos = (pos + 1) % (delayInSamples + 1);

        int readPos = pos - delayInSamples;
        
        if (readPos < 0)
            readPos += delayInSamples + 1;
        
        int nextReadPos = (readPos + 1) % (delayInSamples + 1);
        
        float out = frac*delay[nextReadPos] + (1-frac)*delay[readPos];
        
        //delay[pos] += 
        delay[writePos] = input + feedback*out;

        pos = writePos;        
    }
    
    float getOutput(){
        int readPos = pos - delayInSamples;
        
        if (readPos < 0)
            readPos += delayInSamples + 1;
        
        int nextReadPos = (readPos + 1) % (delayInSamples + 1);
        
        float out = frac*delay[nextReadPos] + (1-frac)*delay[readPos];
        
        //delay[pos] += feedback*out;
        
        return out;
    }
    
    float getLPOutput(){          

        float out = getOutput();
        return filter.getLowpass(out);
    }
    private:
    static const int maxDelayTime = 88200;
    float delayInSeconds;
    int delayInSamples;
    float frac; 
    int pos;
    
    OnePoleFilter filter;
    
    float feedback = 0.0;
    float fs = 44100; 
    float delay[maxDelayTime] = {};
    
};
