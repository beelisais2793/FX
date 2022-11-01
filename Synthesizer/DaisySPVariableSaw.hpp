#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Synthesis
{
    struct VariableSaw : public GeneratorProcessorPlugin<daisysp::VariableSaw>
    {
        VariableSaw() : GeneratorProcessorPlugin<daisysp::VariableSaw>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_FREQ,
            PORT_PW,
            PORT_WAVESHAPE,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FREQ: this->SetFreq(v); break;
                case PORT_PW: this->SetPW(v); break;
                case PORT_WAVESHAPE: this->SetWaveshape(v); break;
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return A*this->Process();
        }
    };
}