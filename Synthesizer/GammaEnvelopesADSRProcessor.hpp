#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma::Envelopes
{
    struct ADSR : public GeneratorProcessorPlugin<gam::ADSR>
    {
        ADSR(double att, double dec, double sus, double rel, double amp=1, double crv=-4) :
            GeneratorProcessorPlugin<gam::ADSR>()
            gam::ADSR(att,dec,rel,amp,crv)
        {
            
        }
        enum {
            PORT_ATTACK,
            PORT_DECAY,
            PORT_SUSTAIN,
            PORT_RELEASE,
            PORT_AMP,            
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_ATTACK: this->attack(v); break;
                case PORT_DECAY: this->decay(v); break;
                case PORT_SUSTAIN: this->sustain(v); break;
                case PORT_RELEASE: this->release(v); break;
                case PORT_AMP: this->amp(v);
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return A*(*this)();
        }
    };
}