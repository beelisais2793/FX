#pragma once
#include "StkHeaders.hpp
namespace Stk::FX
{
    // todo: ports
    struct FreeVerb : public StereoFXProcessorPlugin<stk::FreeVerb>
    {
        FreeVerb() : StereoFXProcessorPlugin<stk::Freeverb>()
        {

        }
        enum {
            PORT_MIX,
            PORT_ROOMSIZE,
            PORT_DAMPING,
            PORT_WIDTH,
            PORT_MODE,
            PORT_CLEAR
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_MIX: this->setEffectMix(v); break;
                case PORT_ROOMSIZE: this->setRoomSize(v); break;
                case PORT_DAMPING: this->setDamping(v); break;
                case PORT_WIDTH: this->setWidth(v); break;
                case PORT_MODE: this->setMode((bool)v); break;
                case PORT_CLEAR: this->clear(); break;
            }
        }
        double Tick(double iL, double iR, double &L, double &R, double A=1, double X=1, double Y=1)
        {
            L = this->tick(iL,iR,0);
            R = this->tick(iL,iR,1);
            return (L+R)*0.5;
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