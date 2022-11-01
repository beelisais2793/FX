#pragma once

#include "DaisySP.hpp"

namespace DaisySP::FX
{
    struct AutoWah : public MonoFXProcessorPlugin<daisyp::AutoWah>
    {
        AutoWah() : MonoFXProcessorPlugin<daisyp::AutoWah>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_WAH,
            PORT_DRYWET,
            PORT_LEVEL
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_WAH: this->SetWah(v); break;
                case PORT_DRYWET: this->SetDryWet(v); break;
                case PORT_LEVEL: this->SetLevel(v); break;
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