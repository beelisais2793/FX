#pragma once

namespace DSPFX
{

    struct FxPanLaw
    {
        void linear_pan(DspFloatType control, DspFloatType * l_gain, DspFloatType * r_gain) {
            ::linear_panD(control,l_gain,r_gain);
        }
        void equal_power_3db_pan(DspFloatType control, DspFloatType * l_gain, DspFloatType * r_gain) 
        {
            ::equal_power_3db_panD(control,l_gain,r_gain);
        }
        void equal_power_6db_pan(DspFloatType control, DspFloatType * l_gain, DspFloatType * r_gain) 
        {
            ::equal_power_6db_panD(control,l_gain,r_gain);
        }
    };
}
