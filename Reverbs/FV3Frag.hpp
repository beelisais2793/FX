#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_fragfft = fv3::fragfft_;
    using fv3_frag = fv3::frag_;
    #else
    using fv3_fragfft = fv3::fragfft_f;
    using fv3_frag = fv3::frag_f;    
    #endif

    struct FragFFT : public ClassProcessor<fv3_fragfft>
    {
        FragFFT() : ClassProcessor<fv3_fragfft>()
        {

        }
    };

    struct Frag : public ClassProcessor<fv3_frag>
    {
        Frag() : ClassProcessor<fv3_frag>()
        {

        }
    };
}

