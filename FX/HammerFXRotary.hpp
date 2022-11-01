#pragma once

#include "HammerFX.hpp"

namespace FX::HammerFX
{
    struct Rotary
    {
        rotary_params cp;

        Rotary() {
            rotary_create(&cp);
        }
        
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            rotary_filter(&cp,n,in,out);
        }
    };
}

