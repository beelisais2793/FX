#pragma once

#include "HammerFX.hpp"

namespace FX::HammerFX
{
    struct Chorus
    {
        chorus_params cp;

        Chorus() {
            chorus_create(&cp);
        }
        ~Chorus() {
            chorus_destroy(&cp);
        }
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            chorus_filter_mono(&cp,n,in,out);
        }
    };
}