#pragma once

#include "DaisySP.hpp"

namespace DaisySP::PhysicalModels
{
    struct KarplusString : public GeneratorProcessorPlugin<daisysp::KarplusString>
    {
        KarplusString() : GeneratorProcessorPlugin<daisysp::KarplusString>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_FREQ,
            PORT_NONLINEAR,
            PORT_BRIGHT,
            PORT_DAMP
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FREQ: this->SetFreq(v); break;
                case PORT_NONLINEAR: this->SetNonLinearity(v); break;
                case PORT_BRIGHT: this->SetBrightness(v); break;
                case PORT_DAMP: this->SetDamping(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*this->Process(I);
        }
    };
}