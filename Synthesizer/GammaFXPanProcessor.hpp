#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"


namespace Gamma::FX
{
    struct Pan : public StereoFunctionProcessorPlugin<gam::Pan<double>>
    {
        Pan() : StereoFunctionProcessorPlugin<gam::Pan<double>>()
        {

        }
        enum {
            PORT_PAN,
            PORT_POS,
            PORT_POSL,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_PAN: this->pan(v); break;
                case PORT_POSL: this->posL(v); break;
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_POS: this->pos(a,b);
            }
        }
        double Tick(double iL, double iR, double &L, double &R, double A=1, double X=1, double Y=1) {
            (*this)(iL,iR,L,R);
            L *= A;
            R *= A;
            return 0.5*(L+R);
        }
        void ProcessBlock(size_t n, float ** in, float ** out) {
            for(size_t i = 0; i < n; i++) 
            {
                double L = out[0][i];
                double R = out[1][i];
                Tick(in[0][i],in[1][i],L,R);
                out[0][i] = L;
                out[1][i] = R;
            }                
        }
    };
}