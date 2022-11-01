#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma::Filters
{
    struct BlockNyquist : public FilterProcessorPlugin<gam::BlockNyq>
    {
        BlockNyquist() : FilterProcessorPlugin<gam::BlockNyq>()
        {

        }
        enum {
            PORT_WIDTH,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_WIDTH: this->width(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*(*this)(I);
        }
    };
}