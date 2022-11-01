#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma::Filters
{
    struct BlockDC : public FilterProcessorPlugin<gam::BlockDC>
    {
        BlockDC() : FilterProcessorPlugin<gam::BlockDC>()
        {

        }
        enum {            
            PORT_WIDTH,
            PORT_ZERO,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_WIDTH: this->width(v); break;
                case PORT_ZERO: this->zero(); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*(*this)(I);
        }
    };
}