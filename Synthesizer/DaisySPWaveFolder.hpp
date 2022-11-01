#pragma once

#include "DaisySP.hpp"

namespace DaisySP::FX
{
    struct WaveFolder : public FunctionProcessorPlugin<daisysp::Wavefolder>
    {
        WaveFolder() : FunctionProcessorPlugin<daisysp::Wavefolder>()
        {
            this->Init();
        }
        enum {
            PORT_GAIN,
            PORT_OFFSET,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_GAIN: this->SetGain(v); break;
                case PORT_OFFSET: this->SetOffset(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*this->Process(I);
        }
    };
}