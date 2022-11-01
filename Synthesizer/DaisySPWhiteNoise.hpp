#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Noise
{
    struct WhiteNoise : public GeneratorProcessorPlugin<daisysp::WhiteNoise>
    {
        WhiteNoise() :GeneratorProcessorPlugin<daisysp::WhiteNoise>()
        {
            this->Init();
        }
        enum {
            PORT_AMP,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_AMP: this->SetAmp(v); break;
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return A*this->Process();
        }
    };
}
