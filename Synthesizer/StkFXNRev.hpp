#pragma once
#include "StkHeaders.hpp
namespace Stk::FX
{
    struct NRev : public MonoFXProcessorPlugin<stk::NRev>
    {
        NRev() : MonoFXProcessorPlugin<stk::NRev>()
        {

        }
        enum {
            PORT_CLEAR,
            PORT_T60,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_CLEAR: this->clear(); break;
                case PORT_T60: this->setT60(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return this->tick(I);
        }
    };
    struct StereoNRev : public StereoFXProcessorPlugin<stk::NRev>
    {
        StereoNRev() : StereoFXProcessorPlugin<stk::NRev>()
        {

        }
        enum {
            PORT_CLEAR,
            PORT_T60,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_CLEAR: this->clear(); break;
                case PORT_T60: this->setT60(v); break;
            }
        }
        double Tick(double iL, double iR, double &L, double &R, double A=1, double X=1, double Y=1)
        {
            L = A*this->tick(I,0);
            R = A*this->tick(I,1);
            return 0.5*(L+R);
        }
        void ProcessBlock(size_t n, float ** in, float ** out) {
            for(size_t i = 0; i < n; i++) {
                double L,R;
                Tick(in[0][i],in[1][i],L,R);
                out[0][i] = L;
                out[1][i] = R;
            }
        }
    };
}