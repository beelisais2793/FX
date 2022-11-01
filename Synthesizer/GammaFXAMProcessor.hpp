#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma::FX
{
    struct AM : public FunctionProcessorPlugin<gam::AM<double>>
    {
        AM() : FunctionProcessorPlugin<gam::AM<double>>()
        {

        }
        enum {
            PORT_DEPTH
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_DEPTH: this->depth(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*(*this)(I);
        }
    };
}