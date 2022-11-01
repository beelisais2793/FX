#pragma once

#include "DaisySP.hpp"

namespace DaisySP::FX
{
    struct Phaser : public MonoFXProcessorPlugin<daisyp::Phaser>
    {
        Phaser() : MonoFXProcessorPlugin<daisyp::Phaser>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_LFODEPTH,
            PORT_LFOFREQ,
            PORT_FREQ,
            PORT_FEEDBACK,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_LFODEPTH: this->SetLfoDepth(v); break;
                case PORT_LFOFREQ: this->SetLfoFreq(v); break;
                case PORT_FREQ: this->SetFreq(v); break;
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