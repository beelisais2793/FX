#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Synthesis
{
    struct FM2 : public GeneratorProcessorPlugin<daisysp::FM2>
    {
        FM2() : GeneratorProcessorPlugin<daisysp::FM2>()
        {

        }
        enum {
            PORT_FREQ,
            PORT_RATIO,
            PORT_INDEX,
            PORT_RESET,
        };
        void setPort(int port, double v)
        {
            switch(port) {
                case PORT_FREQ: this->SetFrequency(v); break;
                case PORT_RATIO: this->SetRatio(v); break;
                case PORT_INDEX: this->SetIndex(v); break;
                case PORT_RESET: this->Reset();
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return A*this->Process(I);
        }        
    };
}