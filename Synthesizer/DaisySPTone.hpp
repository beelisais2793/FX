#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Filters
{
    struct Tone : public FilterProcessorPlugin<daisysp::Tone>
    {
        Tone() : FilterProcessorPlugin<daisysp::Tone>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_FREQ,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FREQ: setFreq(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*this->Process(I);
        }
    };
}