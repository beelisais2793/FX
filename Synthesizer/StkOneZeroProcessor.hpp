#pragma once

#include "StkHeaders.hpp"

namespace Stk 
{
    // todo: ports
    struct OneZero : public FilterProcessorPlugin<stk::OneZero>
    {
        OneZero() : FilterProcessorPlugin<stk::OneZero>()
        {

        }
        enum {
            PORT_B0,
            PORT_B1,
            PORT_ZERO,
            PORT_CUTOFF,
            PORT_HIGHPASS,
        };
        void setCutoff(double f) {
            double p = exp(-2*M_PI*f/sampleRate);            
            this->setZero(p);
        }
        void setHighpassCutoff(double f) {
            double p = -exp(-2*M_PI*f/sampleRate);
            this->setZero(p);
        }
        void setPort(int port, double v) {
            case PORT_B0: this->setB0(v); break;
            case PORT_B1: this->setB1(v); break;
            case PORT_ZERO: this->setZero(v); break;
            case PORT_CUTOFF: setCutoff(v); break;
            case PORT_HIGHPASS: setHighpassCutoff(v); break;
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return this->tick(I);
        }
    };
}