#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Synthesis
{
    struct VOSIM : public GeneratorProcessorPlugin<daisysp::VOSIM>
    {
        VOSIM() : GeneratorProcessorPlugin<daisysp::VOSIM>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_FREQ,
            PORT_FORM1FREQ,
            PORT_FORM2FREQ,
            PORT_SHAPE
        };
        void setPort(int port, double v) {
            switch(port)
            {
                case PORT_FREQ: this->SetFreq(v); break;
                case PORT_FORM1FREQ: this->SetForm1Freq(v); break;
                case PORT_FORM2FREQ: this->SetForm2Freq(v); break;
                case PORT_SHAPE: this->SetShape(v); break;
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return A*this->Process();
        }
    };
}