#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Util
{
    struct DelayLine : public MonoFXProcessorPlugin<daisysp::DelayLine>
    {
        DelayLine(size_t delay) : MonoFXProcessorPlugin<daisysp::DelayLine>()
        {
            this->Init();
            this->SetDelay(delay);
        }
        enum {
            PORT_DELAY,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_DELAY: this->SetDelay(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=0) {
            double x = this->Read();
            this->Write( X*I + Y*x);
            return A*x;
        }
        void ProcessBlock(size_t n, float * in, float * out) {
            for(size_t i = 0; i < n; i++) out[i] = Tick(in[i]);
        }
    };
}