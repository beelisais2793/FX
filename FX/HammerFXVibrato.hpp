#pragma once

#include "HammerFX.hpp"

namespace FX::HammerFX
{
    struct Vibrato
    {
        vibrato_params cp;

        Vibrato() {
            vibrato_create(&cp);
        }
        
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            vibrato_filter(&cp,n,in,out);
        }
    };
}

