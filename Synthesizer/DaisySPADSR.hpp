#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Control
{
    struct ADSR : public GeneratorProcessorPlugin<daisyp::ADSR>
    {
        bool gate = false;
        ADSR() : GeneratorProcessorPlugin<daisyp::ADSR>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_RETRIGGER,            
            PORT_ATTACKTIME,
            PORT_DECAYTIME,
            PORT_RELEASETIME,
            PORT_SUSTAINLEVEL,
            PORT_GATE,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_RETRIGGER: this->Retrigger((bool)v); break;
                case PORT_ATTACKTIME: this->SetAttackTime(v); break;
                case PORT_DECAYTIME: this->SetDecayTime(v); break;
                case PORT_RELEASETIME: this->SetReleaseTime(v); break;
                case PORT_SUSTAINLEVEL: this->SetSustainlevel(v); break;
                case PORT_GATE: gate = (bool)v; break;
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return this->Process(gate);
        }
    };

}