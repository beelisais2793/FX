#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma::Envelopes
{
    struct Decay : public GeneratorProcessorPlugin<gam::Decay>
    {
        Decay(double decay, double val=1) :
            GeneratorProcessorPlugin<gam::Decay>,
            gam::Decay(decay,val)
        {
            
        }
        enum {
            PORT_DECAY,
            PORT_RESET,
            PORT_FINISH,
            PORT_VALUE,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_DECAY: this->decay(v); break;
                case PORT_RESET: this->reset(v); break;
                case PORT_FINISH: this->finish(v); break;
                case PORT_VALUE: this->value(v); break;
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return (*this)();
        }
    };
}