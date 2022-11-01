#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Dynamics
{   
    struct CrossFade : public Parameter2ProcessorPlugin<daisyp::CrossFade>
    {
        enum
        {
            CROSSFADE_LIN,
            CROSSFADE_CPOW,
            CROSSFADE_LOG,
            CROSSFADE_EXP,
            CROSSFADE_LAST,
        };
        CrossFade(int curve) : Parameter2ProcessorPlugin<daisyp::CrossFade>()
        {
            this->Init(curve);
        }
        enum {
            PORT_POS,
            PORT_CURVE,            
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_POS: this->SetPos(v); break;
                case PORT_CURVE: this->SetCurve((int)v); break;
            }
        }
        double Tick(double X, double Y)
            return A*this->Process(X,Y);
        }
    };
}