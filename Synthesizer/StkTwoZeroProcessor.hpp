#pragma once

#include "StkHeaders.hpp"

namespace Stk 
{
    // todo: ports
    struct TwoZero : public FilterProcessorPlugin<stk::TwoZero>
    {
        TwoZero() : FilterProcessorPlugin<stk::TwoZero>()
        {

        }
        enum {
            PORT_B0,
            PORT_B1,            
            PORT_B2,
            PORT_NOTCH,
            PORT_LASTOUT,       
        };        
        void setPort(int port, double v) {
            case PORT_B0: this->setB0(v); break;
            case PORT_B1: this->setB1(v); break;
            case PORT_B2: this->setB2(v); break;                        
            default: printf("No port %d\n",port);
        }
        void setPort2(int port, double a, double b) {
            if(port == PORT_NOTCH) this->setNotch(a,b,true);
            else printf("No port %d\n",port);
        }
        double getPort(int port) {
            if(port == PORT_LASTOUT) return this->lastOut();
            printf("No port %d\n",port);
            return 0.0;
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return this->tick(I);
        }
    };
}