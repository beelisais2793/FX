#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"


namespace Gamma::FX
{
    struct MonoSynth : public GeneratorProcessorPlugin<gam::MonoSynth>
    {
        MonoSynth() : GeneratorProcessorPlugin<gam::MonoSynth>()
        {

        }
        enum {
            PORT_FREQ,
            PORT_RESET,
            PORT_CUTOFF,
            PORT_RES,
            PORT_DUR,
            PORT_CTF1,
            PORT_CTF2,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FREQ: this->freq(v); break;
                case PORT_RESET: this->reset(); break;
                case PORT_CUTOFF: this->filter.freq(v); break;
                case PORT_RES: this->filter.res(v); break;
                case PORT_DUR: this->env.decay(v); break;
                case PORT_CTF1: this->ctf1 = v; break;
                case PORT_CTF2: this->ctf2 = v; break;
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return A*(*this)();
        }
    };
}