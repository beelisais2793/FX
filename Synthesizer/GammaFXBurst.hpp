#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"


namespace Gamma::FX
{
    struct Burst : public FunctionProcessorPlugin<gam::Burst>
    {
        Burst() : FunctionProcessorPlugin<gam::Burst>()
        {

        }
        enum {
            PORT_RESET,
        };
        void setPort(int port, double v) {
            if(port == PORT_RESET) this->reset();
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return A*(*this)();
        }
    };
}