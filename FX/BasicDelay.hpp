/*
  ==============================================================================

    BasicDelay.h
    Created: 6 Oct 2019 3:21:22pm
    Author:  Francesco Fucci

  ==============================================================================
*/

#pragma once

#include <vector>
#include <cmath>
#include "SoundObject.hpp"

namespace Delays
{
    struct BasicDelay : public StereoFXProcessor
    {
        
        BasicDelay(DspFloatType sr, int num_channels, DspFloatType delay, DspFloatType feedback, DspFloatType mix)
        : StereoFXProcessor(),sampleRate(sr), numChannels(num_channels),delayTime(delay/1000.0f),feedbackAmount(feedback),mixAmount(mix)
        {
            //size_t delayBufferLen = delayTime * sampleRate;
            //if(delayBufferLen < 1) delayBufferLen=1;
            size_t delayBufferLen = sr;
            delayBuffer[0].resize(delayBufferLen);
            delayBuffer[1].resize(delayBufferLen);
            writePosition[0] = 0;
            writePosition[1] = 0;  	
        }
    
        void setDelayTime(DspFloatType time) 
        {
            delayTime = time/1000.0f;
            size_t delayBufferLen = delayTime * sampleRate;
            if(delayBufferLen < 1) delayBufferLen=1;
            delayBuffer[0].resize(delayBufferLen);
            delayBuffer[1].resize(delayBufferLen);
        }
            
        void processBlock(int channel, size_t n, DspFloatType *buffer) noexcept;
        
        void ProcessBlock(size_t num, DspFloatType ** in, DspFloatType ** out)
        {
            memcpy(out[0],in[0],num*sizeof(DspFloatType));
            memcpy(out[1],in[1],num*sizeof(DspFloatType));
            processBlock(0,n,out[0]);
            processBlock(1,n,out[1]);
        }
        
        inline void updatePosition(int channel, int size) noexcept{
            writePosition[channel] += size;
            if( writePosition[channel] >= delayBuffer[channel].size()){
                writePosition[channel] -= delayBuffer[channel].size();
            }
        }
        enum {
            PORT_DELAYTIME,            
        };
        void setPort(int port, DspFloatType v) {
            if(port == PORT_DELAYTIME) setDelayTime(v);
            else printf("No port %d\n",port);

        }
        size_t numChannels;
        DspFloatType feedbackAmount,delayTime,mixAmount;
        DspFloatType sampleRate;
        std::vector<DspFloatType> delayBuffer[2];
        size_t writePosition[2];
        const DspFloatType delta = 0.3f;    
    };

    
    inline void BasicDelay::processBlock(int channel, size_t n, DspFloatType *buffer) noexcept{
        //const auto* bufferPointer = buffer.getReadPointer(channel);
        //const auto* delayPointer = mDelayBuffer.getReadPointer(channel);

        const int bufferLength 	 = n;
        const int delayBufferLength = delayBuffer[channel].size();
        
        //delaySmoothed[channel].setTargetValue(*timeAmount);
        //feedbackSmoothed[channel].setTargetValue(*feedbackAmount);
        
        auto* dryBuffer 	     = buffer;
        auto* delayWritePointer  = delayBuffer[channel].data();
        
        //DspFloatType delayTime = delaySmoothed.getNextValue();
        //DspFloatType feedbackCurrentAmount = feedbackSmoothed.getNextValue();
        
        //const DspFloatType nextValue = feedbackSmoothed.getNextValue();
        //String feedbackAm(nextValue);
        //Logger::outputDebugString(feedbackAm);
        int localWritePosition = writePosition[channel];
        
        for(auto sample = 0; sample < bufferLength; ++sample){
            const DspFloatType in = dryBuffer[numChannels*sample + channel];
            const DspFloatType fullPosition = delayBufferLength + localWritePosition -(sampleRate*delayTime);
            DspFloatType readPosition = fmodf(fullPosition, delayBufferLength);

            DspFloatType out = 0.0f;

            int approxReadPosition = floorf(readPosition);
            int approxReadPosition_1 = (approxReadPosition + 1);
            
            if(approxReadPosition_1 >= delayBufferLength){
                approxReadPosition_1 -= delayBufferLength;
            }
            
            // Calculate the fractional delay line (approxReadPosition + 1) % delayBufferLength
            // If the read position equals the write position then the fractional delay is zero 
            if(approxReadPosition != localWritePosition){            
                DspFloatType fraction = readPosition - static_cast<DspFloatType>(approxReadPosition);
                DspFloatType delayP1 = delayWritePointer[approxReadPosition];
                DspFloatType delayP2 = delayWritePointer[approxReadPosition_1];
                out = delayP1 + fraction*(delayP2- delayP1);
                //Write back to the input buffer
                dryBuffer[numChannels*sample+channel] = in + (out - in) * mixAmount;
                //delayWritePointer[localWritePosition] = in + out*feedbackAmount;
                delayWritePointer[localWritePosition] = in + out*(feedbackAmount);
            }
            
            if(++localWritePosition >= delayBufferLength){
                localWritePosition -= delayBufferLength;
            }
            
            /**readPosition += 1 - delta;
            
            if(readPosition >= delayBufferLength){
                readPosition -= delayBufferLength;
            }**/
        }
        updatePosition(channel,n);
    }
}
