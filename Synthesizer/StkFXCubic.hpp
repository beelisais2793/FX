#pragma once

#include "StkHeaders.hpp

namespace Stk 
{
    // todo: ports
    struct Cubic : public MonoFXProcessorPlugin<stk::Cubic>
    {
        Cubic() : MonoFXProcessorPlugin<stk::Cubic>()
        {

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