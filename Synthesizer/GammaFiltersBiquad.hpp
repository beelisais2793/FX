#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma::Filters
{
    struct Biquad : public FilterProcessorPlugin<gam::Biquad>
    {
        Biquad() : FilterProcessorPlugin<gam::Biquad>()
        {

        }
        enum {
            PORT_CUTOFF,
            PORT_RES,
            PORT_LEVEL,
            PORT_TYPE,
            PORT_ZERO,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_CUTOFF: this->freq(v); break;
                case PORT_RES: this->res(v); break;
                case PORT_LEVEL: this->level(v); break;
                case PORT_TYPE: this->type((gam::FilterType)v); break;
                case PORT_ZERO: this->zero();
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*(*this)(I);
        }
    };
}