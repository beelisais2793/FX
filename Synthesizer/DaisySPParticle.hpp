#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Noise
{
    struct Particle : public GeneratorProcessorPlugin<daisysp::Particle>
    {
        Particle (): GeneratorProcessorPlugin<daisysp::Particle>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_FREQ,
            PORT_RES,
            PORT_RANDOMFREQ,
            PORT_DENSITY,
            PORT_GAIN,
            PORT_SPREAD,
            PORT_SYNC,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FREQ: this->SetFreq(v); break;
                case PORT_RES: this->SetResonance(v); break;
                case PORT_RANDOMFREQ: this->SetRandomFreq(v); break;
                case PORT_DENSITY: this->SetDensity(v); break;
                case PORT_GAIN: this->SetGain(v); break;
                case PORT_SPREAD: this->SetSpread(v); break;
                case PORT_SYNC: this->SetSync(v); break;
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return A*this->Process();
        }
    };
}