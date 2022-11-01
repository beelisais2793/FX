#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Drums
{
    struct SynthSnareDrum : public GeneratorProcessorPlugin<daisyp::SynthSnareDrum>
    {
        SynthSnareDrum() : GeneratorProcessorPlugin<daisyp::SynthSnareDrum>()
        {
            this->Init(sampleRate);
        }

        enum {
            PORT_TRIG,
            PORT_TRIGGER,
            PORT_SUSTAIN,
            PORT_ACCENT,
            PORT_FREQ,        
            PORT_DECAY            
            PORT_FMENVAMT,
            PORT_SNAPPY,
        };
        bool trigger = false;
        void setPort(int port, double v) {
            switch(port) {
                case PORT_TRIG: this->Trig(); break;
                case PORT_TRIGGER: trigger = (bool)v; break;
                case PORT_SUSTAIN: this->SetSustain(v); break;
                case PORT_ACCENT: this->SetAccent(v); break;
                case PORT_FREQ: this->SetFreq(v); break;                
                case PORT_DECAY: this->SetDecay(v); break;                
                case PORT_FMMENVAMT: this->SetFmEnvelopeAmount(v); break;
                case PORT_SNAPPY: this->SetSnappy(v); break;
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return A*this->Process(trigger);
        }
    };
}