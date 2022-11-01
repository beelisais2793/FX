#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Util
{
    struct Jitter : public GeneratorProcessorPlugin<daisysp::Jitter>
    {
        Jitter() : GeneratorProcessorPlugin<daisysp::Jitter>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_CPSMIN,
            PORT_CPSMAX,
            PORT_AMP,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_CPSMIN: this->SetCpsMin(v); break;
                case PORT_CPSMAX: this->SetCpsMax(v); break;
                case PORT_AMP: this->SetAmp(v); break;
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1) {
            return A*this->Process();
        }
    };
}