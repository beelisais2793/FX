#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Control
{
    struct Phasor : public GeneratorProcessorPlugin<daisyp::Phasor>
    {
        Phasor(double freq, double phase=0.0) : GeneratorProcessorPlugin<daisyp::Phasor>()
        {
            this->Init(sampleRate,freq,phase);
        }
        enum {
            PORT_FREQ,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FREQ: this->SetFreq(v); break;
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return A*this->Process();
        }
    };
}
