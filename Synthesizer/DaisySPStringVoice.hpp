#pragma once

#include "DaisySP.hpp"

namespace DaisySP::PhysicalModels
{
    struct StringVoice : public GeneratorProcessorPlugin<daisysp::StringVoice>
    {
        bool trigger = false;
        StringVoice() : GeneratorProcessorPlugin<daisysp::StringVoice>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_SUSTAIN,
            PORT_TRIG,
            PORT_TRIGGER,
            PORT_FREQ,
            PORT_ACCENT,
            PORT_STRUCTURE,
            PORT_BRIGHTNESS,
            PORT_DAMPING,
        };
        void setPort(int port, double v) {
            switch(port) {
            case PORT_SUSTAIN: this->SetSustain(v); break;
            case PORT_TRIG: this->Trig(); break;
            case PORT_TRIGGER: trigger = (bool)v; break;
            case PORT_FREQ: this->SetFreq(v); break;
            case PORT_ACCEENT: this->SetAccent(v); break;
            case PORT_STRUCTURE: this->SetStructure(v); break;
            case PORT_BRIGHTNESS: this->SetBrightness(v); break;
            case PORT_DAMPING: this->SetDamping(v); break;
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return A*this->Process();
        }
    };
}