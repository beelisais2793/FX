// https://github.com/Squishy47/Schroeder-All-Pass-Filter/blob/master/SchroederAllPass.cpp
#pragma once

// USER HEADERS
#include <cstdio>
#include <cmath>
#include <vector>

namespace FX::Filters::SchroederAllpass
{
    class CircularBuffer{
        std::vector<DspFloatType> buffer;
        
        int bufferLength;
        int head = 0;
        int tail = 0;
        
    public:
        CircularBuffer(DspFloatType inValue);
        
        DspFloatType readCubic(DspFloatType numElementsToRead);

        DspFloatType readLinear(DspFloatType numElementsToRead);

        void write(DspFloatType inValue);
        
        DspFloatType cubicInterpolation(DspFloatType y0, DspFloatType y1, DspFloatType y2, DspFloatType y3, DspFloatType mu);
        
        DspFloatType getSample(DspFloatType inValue);
            
        void setBufferLength(DspFloatType inValue);
        
        int getBufferLength();
    };

    //delay 1.7 - 5 ms
    class SchroederAllPass{
        CircularBuffer CB{44100};
        
        int Fs;
        DspFloatType delayLength;
        DspFloatType g;
        
        DspFloatType delayedSample = 0;
        DspFloatType feedFordwardSample = 0;
    public:
        SchroederAllPass(DspFloatType inValue, DspFloatType inG);
            
        void process(DspFloatType* samples, int bufferSize);
        
        DspFloatType processSingleSample(DspFloatType sample);
        
        void setFeedback(DspFloatType inValue);
        
        DspFloatType getFeedback();
        
        void setDelayLength(DspFloatType inValue);
        
        DspFloatType getDelayLength();
        
        void setFs(int inValue);
    };

    CircularBuffer::CircularBuffer(DspFloatType inValue){
        setBufferLength(inValue);
    }

    DspFloatType CircularBuffer::readCubic(DspFloatType index){
        DspFloatType y0, y1, y2, y3, mu;

        y0 = floor(index - 1);
        y1 = floor(index);
        y2 = floor(index);
        y3 = floor(index + 1);
        mu = index - y1;
        
        return cubicInterpolation(getSample(y0), getSample(y1), getSample(y2), getSample(y3), mu);
    }

    DspFloatType CircularBuffer::readLinear(DspFloatType index){
        DspFloatType upper = floor(index) + 1;
        DspFloatType lower = floor(index);
        DspFloatType interpAmount = index - lower;
        
        return (getSample(upper) * interpAmount + (1.0 - interpAmount) * getSample(lower));
    }

    void CircularBuffer::write(DspFloatType inValue){
        head = (head += 4) % bufferLength;
        buffer[head] = inValue;
    }

    DspFloatType CircularBuffer::getSample(DspFloatType inValue){
        tail = head - inValue;
        
        if (tail > bufferLength - 1)
            tail -= bufferLength;
        else if (tail < 0)
            tail += bufferLength;
        
        return buffer[tail];
    }

    void CircularBuffer::setBufferLength(DspFloatType inValue){
        bufferLength = inValue;
        buffer.resize(inValue);
    }

    int CircularBuffer::getBufferLength(){
        return (int)buffer.size();
    }

    DspFloatType CircularBuffer::cubicInterpolation(DspFloatType y0, DspFloatType y1, DspFloatType y2, DspFloatType y3, DspFloatType mu){
        //Cubic interp (this algorithm) taken from: http://paulbourke.net/miscellaneous/interpolation/
        DspFloatType a0, a1, a2, a3, mu2;
        
        mu2 = mu * mu;
        a0 = -0.5 * y0 + 1.5 * y1 - 1.5 * y2 + 0.5 * y3;
        a1 = y0 - 2.5 * y1 + 2 * y2 - 0.5 * y3;
        a2 = -0.5 * y0 + 0.5 * y2;
        a3 = y1;
        
        return(a0 * mu * mu2 + a1 * mu2 + a2 * mu + a3);
    }

    SchroederAllPass::SchroederAllPass(DspFloatType inValue, DspFloatType inG){
        delayLength = inValue;
        g = inG;
    }

    void SchroederAllPass::process(DspFloatType* samples, int bufferSize){
        for(int i = 0; i < bufferSize; i++)
            samples[i] = processSingleSample(samples[i]);
    }

    DspFloatType SchroederAllPass::processSingleSample(DspFloatType sample){
        delayedSample = CB.readCubic(delayLength);
        
        CB.write(sample + (delayedSample * g));

        feedFordwardSample = sample * -g;

        return (delayedSample + feedFordwardSample);
    }

    void SchroederAllPass::setFeedback(DspFloatType inValue){
        g = inValue;
    }

    DspFloatType SchroederAllPass::getFeedback(){
        return g;
    }

    void SchroederAllPass::setDelayLength(DspFloatType inValue){
        delayLength = inValue;
        if (delayLength > CB.getBufferLength())
            CB.setBufferLength(delayLength);
    }

    DspFloatType SchroederAllPass::getDelayLength(){
        return delayLength;
    }

    void SchroederAllPass::setFs(int inValue){
        Fs = inValue;
    }

}