#pragma once

#include "DaisySP.hpp"

namespace DaisySP::FX
{
    struct BitCrush : public MonoFXProcessorPlugin<daisyp::BitCrush>
    {
        BitCrush() : MonoFXProcessorPlugin<daisyp::BitCrush>( )
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_DEPTH,
            PORT_RATE,
        };
        void setPort(int port, double v) {
            switch(port)
            {
                case PORT_DEPTH: this->SetBitDepth(v); break;
                case PORT_RATE: this->SetCrushRate(v); break;
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