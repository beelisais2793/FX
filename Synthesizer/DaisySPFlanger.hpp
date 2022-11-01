#pragma once

#include "DaisySP.hpp"

namespace DaisySP::FX
{
    struct Flanger : public MonoFXProcessorPlugin<daisyp::Flanger>
    {
        Flanger() : MonoFXProcessorPlugin<daisyp::Flanger>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_FEEDBACK,
            PORT_LFODEPTH,
            PORT_LFOFREQ,
            PORT_DELAY,
            PORT_DELAYMS
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FEEDBACK: this->SetFeedback(v); break;
                case PORT_LFODEPTH: this->SetLfoDepth(v); break;
                case PORT_LFOFREQ: this->SetLfoFreq(v); break;
                case PORT_DELAY: this->SetDelay(v); break;
                case PORT_DELAYMS: this->SetDelayMs(v); break;
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