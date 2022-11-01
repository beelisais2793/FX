#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Util
{
    struct SmoothRandom : public GeneratorProcessorPlugin<daisysp::SmoothRandom>
    {
        SmoothRandom() : GeneratorProcessorPlugin<daisysp::SmoothRandom>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_FREQ,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FREQ: this->SetFreq(v); break;
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1)
        {
            return A*this->Process();
        }
    };
}