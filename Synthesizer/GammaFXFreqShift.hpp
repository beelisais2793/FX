#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"


namespace Gamma::FX
{
    struct FreqShift : public MonoFXProcessorPlugin<gam::FreqShift<double>>
    {
        FreqShift() : MonoFXProcessorPlugin<gam::FreqShift<double>>()
        {

        }
        enum {
            PORT_FREQ,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FREQ: this->freq(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*(*this)(I);
        }
        void ProcessBlock(size_t n, float * in, float * out) {
            for(size_t i = 0; i < n; i++) out[i] = Tick(in[i]);
        }
    };
}