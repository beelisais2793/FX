#pragma once

#include "DaisySP.hpp"

namespace DaisySP::FX
{
    struct Fold : public MonoFXProcessorPlugin<daisyp::Fold>
    {
        Fold() : MonoFXProcessorPlugin<daisyp::Fold>()
        {
            this->Init();
        }
        enum {
            PORT_INC,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_INC: this->SetIncrement(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*this->Process(I);
        }
        void ProcessBlock(size_t n, float * in, float * out) {
            for(size_t i = 0; i < n; i++) out[i] = Tick(in[i]);
        }
    };
}