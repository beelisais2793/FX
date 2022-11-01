#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma::Envelopes
{
    struct Gate : public GeneratorProcessorPlugin<gam::Gate>
    {
        Gate(double delay=0, double thresh=0.801) :
            GeneratorProcessorPlugin<gam::Decay>,
            gam::Gate(delay,thresh)
        {
            
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return (*this)();
        }
        enum {
            PORT_DELAY,
            PORT_DONE,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_DELAY: this->delay(v); break;
            }
        }
        double getPort(int port) {
            if(port == PORT_DONE) return this->done(); 
            return 0.0;
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return (*this)(I);
        }
    };
}