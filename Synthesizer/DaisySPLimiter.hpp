#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Dynamics
{   
    struct Limiter : public MonoFXProcessorPlugin<daisyp::Limiter>
    {
        double pre_gain = 1.0;
        Limiter() : MonoFXProcessorPlugin<daisyp::Limiter>()
        {
            this->Init();
        }
        enum {
            PORT_PREGAIN,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_PREGAIN: pre_gain = v; break;
            }
        }
        void ProcessBlock(size_t n, float * in, float * out) {
            memcpy(out,in,n*sizeof(float));
            this->ProcessBlock(out,n,pre_gain);
        }
    };
}