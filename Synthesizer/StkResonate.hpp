#pragma once
#include "StkHeaders.hpp"
namespace Stk::Generators
{
    struct Resonate : public GeneratorProcessorPlugin<stk::Resonate>
    {
        Resonate() : GeneratorProcessorPlugin<stk::Resonatee>()
        {

        }
        enum {
            PORT_RESONANCE,
            PORT_NOTCH,
            PORT_EQUALGAINZEROS,
            PORT_KEYON,
            PORT_KEYOFF,
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_CC,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_EQUALGAINZEROS: this->setEqualGainZeros(); break;
                case PORT_KEYON: this->keyOn(); break;
                case PORT_KEYOFF: this->keyOff(); break;
                case PORT_NOTEOFF: this->noteOff(v); break;                
                default: printf("No port %d\n", port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_RESONANCE: this->setResonance(a,b); break;
                case PORT_NOTCH: this->setNotch(a,b); break;
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                default: printf("No port %d\n", port);
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick(I);
        }
    };
}