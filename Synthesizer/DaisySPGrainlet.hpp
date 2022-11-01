#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Noise
{
    struct Grainlet : public GeneratorProcessorPlugin<daisysp::Grainlet>
    {
        Grainlet() : GeneratorProcessorPlugin<daisysp::Grainlet>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_FREQ,
            PORT_FORMANTFREQ,
            PORT_SHAPE,
            PORT_BLEED,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FREQ: this->SetFreq(v); break;
                case PORT_FORMANTFREQ: this->SetFormantFreq(v); break;
                case PORT_SHAPE: this->SetShape(v); break;
                case PORT_BLEED: this->SetBleed(v); break;
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return A*this->Process();
        }
    };
}