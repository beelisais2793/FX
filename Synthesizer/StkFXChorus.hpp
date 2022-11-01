#pragma once

#include "StkHeaders.hpp"

namespace Stk::FX
{
    struct Chorus : public MonoFXProcessorPlugin<stk::Chorus>
    {
        Chorus() : MonoFXProcessorPlugin<stk::Chorus>()
        {

        }
        enum {
            PORT_CLEAR,
            PORT_MODDEPTH,
            PORT_MODFREQ,
            PORT_LASTOUT,
        };
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return A*this->tick(I);            
        }        
        void ProcessBlock(size_t n, float * in, float * out) {
            for(size_t i = 0; i < n; i++) out[i] = Tick(in[i]);
        }
    };
    struct StereoChorus : public MonoFXProcessorPlugin<stk::Chorus>
    {
        StereoChorus() : StereoFXProcessorPlugin<stk::Chorus>()
        {

        }
        enum {
            PORT_CLEAR,
            PORT_MODDEPTH,
            PORT_MODFREQ,
            PORT_LASTOUT,
        };   
        double Tick(double iL, double iR, double &L, double &R, double A=1, double X=1, double Y=1)
        {
            L = A*this->tick(iL,0);
            R = A*this->tick(iR,1);
            return 0.5*(L+R);
        }
        void ProcessSample(size_t n, float ** in, float ** out) {
            for(size_t i = 0; i < n; i++) {
                double L = out[0][i];
                double R = out[1][i];
                Tick(in[0][i],in[1][i],L,R);
                out[0][i] = L;
                out[1][i] = R;
            }
        }
    };
}