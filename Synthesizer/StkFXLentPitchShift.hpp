#pragma once
#include "StkHeaders.hpp
namespace Stk::FX
{
    struct LentPitchShift : public MonoFXProcessorPlugin<stk::LentPitchShift>
    {
        LentPitchShift() : MonoFXProcessorPlugin<stk::LentPitchShift>()
        {

        }
        enum {
            PORT_CLEAR,
            PORT_SHIFT
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_CLEAR: this->clear(); break;
                case PORT_SHIFT: this->setShift(v); break;
            }
        }

        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return this->tick(I);
        }
        void ProcessBlock(size_t n, float * in, float * out) {
            for(size_t i = 0; i < n; i++) out[i] = Tick(in[i]);
        }
    };
}