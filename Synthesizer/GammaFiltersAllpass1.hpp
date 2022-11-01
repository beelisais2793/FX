#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma::Filters
{
    struct AllPass1 : public FilterProcessorPlugin<gam::AllPass1>
    {
        AllPass1() : FilterProcessorPlugin<gam::AllPass1>()
        {

        }
        
        enum {
            PORT_CUTOFF,
            PORT_CUTOFFF,
            PORT_ZERO,
            PORT_HIGH,
            PORT_LOW,
            PORT_FREQ
        };
        int type = PORT_LOW;
        void setPort(int port, double v) {
            switch(port) {
                case PORT_CUTOFF: this->freq(v); break;
                case PORT_CUTOFFF: this->freqF(v); break;
                case PORT_ZERO: this->zero(v); break;
                case PORT_HIGH: type = PORT_HIGH; break;
                case PORT_LOW: type = PORT_LOW; break;
            }
        }
        double getPort(int port) {
            if(port == PORT_FREQ) return this->freq();
            return 0.0;
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            if(type == PORT_LOW) return A*this->low(I);
            return A*this->high(I);
        }
    };
}