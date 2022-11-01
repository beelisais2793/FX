#pragma once
#include "StkHeaders.hpp
namespace Stk::FX
{
    struct Echo : public MonoFXProcessorPlugin<stk::Echo>
    {
        Echo(unsigned long max = (unsigned long)sampleRate) : 
            MonoFXProcessorPlugin<stk::Echo>(),
            stk::Echo(max)
        {

        }
        enum {
            PORT_MAXDELAY,
            PORT_DELAY,            
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_MAXDELAY: this->setMaxDelay(v); break;
                case PORT_DELAY: this->setDelay(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return this->tick(I);
        }
    };
}