#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Noise
{
    struct FractalNoise : public GeneratorProcessorPlugin<daisysp::FractalRandomGenerator>
    {
        FractalNoise() : GeneratorProcessorPlugin<daisysp::FractalRandomGenerator>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_FREQ,
            PORT_COLOR,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FREQ: this->SetFreq(v); break;
                case PORT_COLOR: this->SetColor(v); break;
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return A*this->Process();
        }
    };
}