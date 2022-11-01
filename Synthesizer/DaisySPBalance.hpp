#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Dynamics
{
    struct Balance : public FunctionProcessorPlugin<daisyp::Balance>
    {
        Balance() : FunctionProcessorPlugin<daisyp::Balance>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_CUTOFF,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_CUTOFF: this->SetCutoff(v); break;
            }
        }        
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*this->Process(I,X);
        }
        void ProcessBlock(size_t n, float * in, float * out) {
            for(size_t i = 0; i < n; i++) out[i] = Tick(in[i]);
        }s
    };
}