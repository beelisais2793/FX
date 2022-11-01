#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Util
{
    struct SampleHold : public FunctionProcessorPlugin<daisysp::SampleHold>
    {
        SampleHold() : FunctionProcessorPlugin<daisysp::SampleHold>()
        {
            
        }
        bool trigger = false;
        Mode mode    = MODE_SAMPLE_HOLD;
        enum {
            PORT_TRIGGER,
            PORT_MODE,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_TRIGGER: trigger = (bool)v; break;
                case PORT_MODE: mode = (Mode)v; break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return A*this->Process(trigger,I,mode);
        }
    };
}