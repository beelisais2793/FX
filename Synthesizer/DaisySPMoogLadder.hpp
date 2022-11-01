#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Filters
{
    struct MoogLadder : public FilterProcessorPlugin<daisysp::MoogLadder>
    {
        MoogLadder() : FilterProcessorPlugin<daisysp::MoogLadder>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_FREQ,
            PORT_RES,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FREQ:this->SetFreq(v); break;
                case PORT_RES: this->SetRes(v); break;
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