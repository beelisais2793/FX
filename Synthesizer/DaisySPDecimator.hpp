#pragma once

#include "DaisySP.hpp"

namespace DaisySP::FX
{
    struct Decimator : public MonoFXProcessorPlugin<daisyp::Decimator>
    {
        Decimator() :  MonoFXProcessorPlugin<daisyp::Decimator>()
        {
            this->Init();
        }
        enum {
            PORT_DOWNSAMPLE_FACTOR,
            PORT_BITCRUSH_FACTOR,
            PORT_BITS_TO_CRUSH,            
        };
        void setPort(int port, double value) {
            switch(port) {
                case PORT_DOWNSAMPLE_FACTOR: this->SetDownsampleFactor(v); break;
                case PORT_BITCRUSH_FACTOR: this->SetBitcrushFactor(v); break;
                case PORT_BITS_TO_CRUSH: this->SetBitsToCrush(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return this->Process(I);
        }
        void ProcessBlock(size_t n, float * in, float * out) {
            for(size_t i = 0; i < n; i++) out[i] = Tick(in[i]);
        }
    };
}