#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Util
{
    struct DCBlock : public FilterProcessorPlugin<daisysp::DCBlock>
    {
        DCBlock() : FilterProcessorPlugin<daisysp::DCBlock>()
        {
            this->Init(sampleRate);
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return this->Process(I);
        }
        void ProcessBlock(size_t n, float * in, float * out) {
            for(size_t i = 0; i < n; i++) out[i] = Tick(in[i]);
        }
    };
}