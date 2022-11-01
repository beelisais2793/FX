#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Util
{
    struct Port : public FunctionProcessorPlugin<daisysp::Port>
    {
        Port(float htime) : FunctionProcessorPlugin<daisysp::Port>()
        {
            this->Init(sample_rate,htime);
        }
        enum {
            PORT_HTIME,            
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_HTIME: this->SetHTime(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*this->Process(I);
        }
    };
}