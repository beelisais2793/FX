#pragma once

#include "StkHeaders.hpp"

namespace Stk::Envelopes
{
    struct Envelope : public GeneratorProcessorPlugin<Stk::Envelope>
    {
        Envelope() : : GeneratorProcessorPlugin<Stk::Envelope>
        {
            
        }
        enum {
            PORT_RATE,
            PORT_TIME,
            PORT_TARGET,
            PORT_VALUE,
        };
        void setPort(int port, double value) {
            switch(port) {
                case PORT_RATE: this->setRate(value); break;
                case PORT_TIME: this->setTime(value); break;
                case PORT_TARGET: this->setTarget(value); break;
                case PORT_VALUE: this->setValue(value); break;
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return this->tick();
        }
    };
}