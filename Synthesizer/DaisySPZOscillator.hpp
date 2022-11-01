#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Synthesis
{
    struct ZOscillator : public GeneratorProcessorPlugin<daisysp::ZOscillator>
    {
        ZOscillator() : GeneratorProcessorPlugin<daisysp::ZOscillator>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_FREQ,
            PORT_FORMANTFREQ,
            PORT_SHAPE,
            PORT_MODE,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FREQ: this->SetFreq(v); break;
                case PORT_FORMANTFREQ: this->SetFormantFreq(v); break;
                case PORT_SHAPE: this->SetShape(v); break;
                case PORT_MODE: this->SetMode(v); break;
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return A*this->Process();
        }
    };
}    
