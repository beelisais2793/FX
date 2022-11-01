#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma::Filters
{

    struct Integrator : public FilterProcessorPlugin<gam::Integrator>
    {
        Integrator() : FilterProcessorPlugin<gam::Integrator>()
        {

        }
        enum {
            PORT_ZERO,
            PORT_LEAK,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_LEAK: this->leak(v); break;
                case PORT_ZERO: this->zero(); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*(*this)(I);
        }
    };
}