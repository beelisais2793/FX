#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma::Filters
{

    struct OnePole : public FilterProcessorPlugin<gam::OnePole>
    {
        OnePole() : FilterProcessorPlugin<gam::OnePole>()
        {

        }
        enum {
            PORT_FREQ,
            PORT_TYPE,
            PORT_LAG,
            PORT_SMOOTH,
            PORT_ZERO,
            PORT_RESET,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FREQ: this->freq(v); break;
                case PORT_TYPE: this->type((gam::FilterType)v); break;
                case PORT_LAG: this->lag(v); break;
                case PORT_SMOOTH: this->smooth(v); break;
                case PORT_ZERO: this->zero(); break;
                case PORT_RESET: this->reset(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*(*this)(I);
        }
    };
}