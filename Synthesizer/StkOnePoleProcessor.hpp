#pragma once

#include "StkHeaders.hpp"

namespace Stk 
{
    // todo: ports
    struct OnePole : public FilterProcessorPlugin<stk::OnePole>
    {
        OnePole() : FilterProcessorPlugin<stk::OnePole>()
        {

        }
        enum {
            PORT_B0,
            PORT_A1,
            PORT_POLE,
            PORT_CUTOFF,
            PORT_HIGHPASS,
        };
        void setCutoff(double f) {
            double p = exp(-2*M_PI*f/sampleRate);
            this->setPole(p);            
        }
        void setHighpassCutoff(double f) {
            double p = -exp(-2*M_PI*f/sampleRate);
            this->setPole(p);            
        }
        void setPort(int port, double v) {
            case PORT_B0: this->setB0(v); break;
            case PORT_A1: this->setA1(v); break;
            case PORT_POLE: this->setPole(v); break;
            case PORT_CUTOFF: setCutoff(v); break;
            case PORT_HIGHPASS: setHighpassCutoff(v); break;
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return this->tick(I);
        }
    };
}