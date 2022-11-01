#pragma once

#include "HammerFX.hpp"

namespace FX::HammerFX
{

    struct AutoWah
    {
        autowah_params ap;

        AutoWah() {
            autowah_init(&ap);
        }
        ~AutoWah() {
            autowah_destroy(&ap);
        }

        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            autowah_filter(&ap,n,in,out);
        }
    };
}