#pragma once

#include "HammerFX.hpp"

namespace FX::HammerFX
{
    struct Delay
    {
        delay_params cp;

        Delay() {
            delay_create(&cp);
        }
        ~Delay() {
            delay_destroy(&cp);
        }
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            delay_filter_mono(&cp,n,in,out);
        }
    };
}