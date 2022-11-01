#pragma once

#include "StkHeaders.hpp"

namespace Stk::Envelopes
{
    struct Asymp : public GeneratorProcessorPlugin<Stk::Asymp>
    {
        ASymp() : : GeneratorProcessorPlugin<Stk::Asymp>
        {
            
        }
        enum {
            PORT_TAU,
            PORT_TIME,
            PORT_T60,
            PORT_VALUE,            
        };
        void setPort(int port, double value) {
            switch(port) {
                case PORT_TAU: this->setTau(value); break;
                case PORT_TIME: this->setTime(value); break;
                case PORT_T60: this->setT60(value); break;
                case PORT_VALUE: this->setValue(value); break;
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return this->tick();
        }
    };
}