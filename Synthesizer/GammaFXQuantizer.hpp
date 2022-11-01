#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"


namespace Gamma::FX
{
    struct Quantizer : public FunctionProcessorPlugin<gam::Quantizer<double>>
    {
        Quantizer() : FunctionProcessorPlugin<gam::Quantizer<double>>()
        {

        }
        enum {
            PORT_FREQ,
            PORT_PERIOD,
            PORT_STEP,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FREQ: this->freq(v); break;
                case PORT_PERDIO: this->period(v); break;
                case PORT_STEP: this->step(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*(*this)(I);
        }
    };    
}