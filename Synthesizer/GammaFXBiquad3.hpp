#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"


namespace Gamma::FX
{
    struct Biquad3 : public FunctionProcessorPlugin<gam::Biquad3<double>>
    {
        Biquad3() : FunctionProcessorPlugin<gam::Biquad3<double>>()
        {

        }
        enum {
            PORT_FREQ,
        };
        void setPort(int port, double v) {
            switch(port)
            {
                case PORT_FREQ: this->freq(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*(*this)(I);
        }
    };
}