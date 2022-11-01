#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma::Filters
{
    struct Hilbert : public FilterProcessorPlugin<gam::Hilbert>
    {
        Hilbert() :FilterProcessorPlugin<gam::Hilbert>()
        {

        }
        enum {
            PORT_ZERO
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_ZERO: this->zero(); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*(*this)(I);
        }
    };
}