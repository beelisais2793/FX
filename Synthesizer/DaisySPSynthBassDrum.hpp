#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Drums
{
    struct SynthBassDrum : public GeneratorProcessorPlugin<daisyp::SynthBassDrum>
    {
        SynthBassDrum() : GeneratorProcessorPlugin<daisyp::SynthBassDrum>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_TRIG,
            PORT_TRIGGER,
            PORT_SUSTAIN,
            PORT_ACCENT,
            PORT_FREQ,
            PORT_TONE,
            PORT_DECAY
            PORT_DIRT,
            PORT_FMENVAMT,
            PORT_FMENVDCY,
        };
        bool trigger = false;
        void setPort(int port, double v) {
            switch(port) {
                case PORT_TRIG: this->Trig(); break;
                case PORT_TRIGGER: trigger = (bool)v; break;
                case PORT_SUSTAIN: this->SetSustain(v); break;
                case PORT_ACCENT: this->SetAccent(v); break;
                case PORT_FREQ: this->SetFreq(v); break;
                case PORT_TONE: this->SetTone(v); break;
                case PORT_DECAY: this->SetDecay(v); break;
                case PORT_DIRT: this->SetDirt(v); break;
                case PORT_FMMENVAMT: this->SetFmEnvelopeAmount(v); break;
                case PORT_FMENVDECY: this->SetFmEnvelopeDecay(v); break;
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return A*this->Process(trigger);
        }
    };
}