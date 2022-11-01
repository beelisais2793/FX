/*
  ==============================================================================

    CombFilter.h
    Created: 10 Jun 2020 5:11:52pm
    Author:  mw

  ==============================================================================
*/

#pragma once
#include <cstdint>
#include <memory>
#include <cmath>

#include "FX/RingBufferProcessor.hpp"

namespace Delays
{
    class CombFilter : public FilterProcessor
    {
    public:
        CombFilter(size_t max_length = 44100) :
            FilterProcessor(),
            buffer(RingBuffer(max_length))
        {
            buffer.resize(1000);
        }

        bool resize(size_t new_length)
        {
            return buffer.resize(new_length);
        }

        DspFloatType processSample(DspFloatType sample)
        {
            for (int i = 0; i < 8; ++i) {
                buffer.update(sample);
                sample = 0.5f * sample + 0.5f * buffer.get();
            }
            return sample;
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            size_t h = buffer.getHead();
            buffer.setHead(h + X*buffer.getCurrentLength());
            size_t t = buffer.getTail();
            buffer.setTail(t + Y*buffer.getCurrentLength());
            DspFloatType out = processSample(I);
            buffer.setHead(h);
            buffer.setTail(t);
            return A*out;
        }

    private:
        RingBuffer buffer;
    };
}