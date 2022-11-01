#pragma once

#include "DaisySP.hpp"

namespace DaisySP::FX
{
    struct SampleRateReducer : public FunctionProcessorPlugin<daisyp::SampleRateReducer>
    {
        SampleRateReducer() : FunctionProcessorPlugin<daisyp::SampleRateReducer>()
        {
            this->Init();
        }
        enum {
            PORT_FREQ,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FREQ: this->SetFreq(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return A*this->Process(I);
        }
    };
}