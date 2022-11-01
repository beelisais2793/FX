
#pragma once
#include "StkHeaders.hpp
namespace Stk::FX
{
    struct PRCRev : public MonoFXProcessorPlugin<stk::PRCRev>
    {
        PRCRev() : MonoFXProcessorPlugin<stk::PRCRev>()
        {

        }
        enum {
            PORT_T60,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_T60: this->setT60(v); break;
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick(I);
        }
        void ProcessBlock(size_t n, float * input, float * output) {
            for(size_t i = 0; i < n; i++) output[i] = Tick(input[i]);
        }
    };

    struct StereoPRCRev : public StereoFXProcessorPlugin<stk::PRCRev>
    {
        StereoPRCRev() : StereoFXProcessorPlugin<stk::JCRev>()
        {

        }
        enum {
            PORT_T60,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_T60: this->setT60(v); break;
            }
        }
        double Tick(double iL=0, double iR=0, double &L, double &R, double A=1, double X=1, double Y=1)
        {
            L = A*this->tick(iL,0);
            R = A*this->tick(iR,1);
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