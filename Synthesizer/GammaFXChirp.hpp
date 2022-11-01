#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"


namespace Gamma::FX
{
    struct Chirp : public FunctionProcessorPlugin<gam::Chirp<double>>
    {
        Chirp() : FunctionProcessorPlugin<gam::Chirp<double>>()
        {

        }
        enum {
            PORT_DECAY,
            PORT_FREQ,
            PORT_RESET,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_DECAY: this->decay(v); break;
                case PORT_RESET: this->reset(v); break;
            }
        }
        void setPort2(int port, double x, double y) {
            switch(port) {
                case PORT_FREQ: this->freq(x,y);
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return A*(*this)();
        }
    };
}