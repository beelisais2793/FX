#pragma once

#include "DaisySP.hpp"

namespace DaisySP::FX
{
    struct Chorus : public MonoFXProcessorPlugin<daisyp::Chorus>
    {
        Chorus() : MonoFXProcessorPlugin<daisyp::Chorus>()
        {
            this->Init();
        }
        enum {
            PORT_LFODEPTH,
            PORT_LFOFREQ,
            PORT_DELAY,
            PORT_DELAYMS,
            PORT_FEEDBACK,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_LFODEPTH: this->SetLfoDepth(v); break;
                case PORT_LFOFREQ: this->SetLfoFreq(v); break;
                case PORT_DELAY: this->SetDelay(v); break;
                case PORT_DELAYMS: this->SetDelayMs(v); break;
                case PORT_FEEDBACK: this->SetFeedback(v); break;
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