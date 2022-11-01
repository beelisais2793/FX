#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{

    #ifdef DSPFLOATDOUBLE
    using fv3_zrev = fv3::zrev_;        
    using fv3_zrev2 = fv3::zrev2_;        
    #else
    using fv3_zrev = fv3::zrev_f;        
    using fv3_zrev2 = fv3::zrev2_f;      
    #endif

    struct ZRev : public StereoFXProcessorPlugin<fv3_zrev>
    {
        ZRev() : StereoFXProcessorPlugin<fv3_zrev>()
        {

        }
    };

    struct ZRev2 : public StereoFXProcessorPlugin<fv3_zrev2>
    {
        ZRev2() : StereoFXProcessorPlugin<fv3_zrev2>()
        {

        }
    };
}    
