#pragma once
#include "StkHeaders.hpp
namespace Stk::FX
{
    
    struct DelayA : public MonoFXProcessorPlugin<stk::DelayL>
    {
        DelayA() : MonoFXProcessorPlugin<stk::DelayL>()
        {

        }
        enum {
            PORT_CLEAR,
            PORT_MAXDELAY,
            PORT_DELAY,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_CLEAR: this->clear(); break;
                case PORT_MAXDELAY: this->setMaximumDelay(v); break;
                case PORT_DELAY: this->setDelay(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return A*this->tick(I);
        }
        void ProcessBlock(size_t n, float * in, float * out) {
            for(size_t i = 0; i < n; i++) out[i] = Tick(in[i]);
        }
    };   
}