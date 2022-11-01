#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma::Envelopes
{
    struct AD : public GeneratorProcessorPlugin<gam::AD>
    {
        AD(double att, double dec, double amp=1, doubl crv=-4) 
        : GeneratorProcessorPlugin<gam::AD>,
            gam::AD(att,dec,amp,crv)
        {
            
        }
        enum {
            PORT_ATTACK,
            PORT_DECAY,
            PORT_AMP,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_ATTACK: this->attack(v); break;
                case PORT_DECAY: this->decay(v); break;
                case PORT_AMP: this->amp(v); break;
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return (*this)();
        }
    };
}