#pragma once

#include "StkHeaders.hpp"

namespace Stk 
{
    // todo: ports
    struct TwoPole : public FilterProcessorPlugin<stk::TwoPole>
    {
        TwoPole() : FilterProcessorPlugin<stk::TwoPole>()
        {

        }
        enum {
            PORT_B0,
            PORT_B1,
            PORT_A1,            
            PORT_A2,
            PORT_RESONANCE,            
        };        
        void setPort(int port, double v) {
            case PORT_B0: this->setB0(v); break;
            case PORT_B1: this->setB1(v); break;
            case PORT_A1: this->setA1(v); break;            
            case PORT_A2: this->setA2(v); break;                        
            default: printf("No port %d\n",port);
        }
        void setPort2(int port, double a, double b) {
            if(port == PORT_RESONANCE) this->setResonance(a,b,true);
            else printf("No port %d\n",port);
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return this->tick(I);
        }
    };
}