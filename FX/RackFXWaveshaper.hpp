#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class Waveshaper
    {
    public:
        Waveshaper ();
        ~Waveshaper ();
    //Waveshaping
        void waveshapesmps (int n, DspFloatType * smps, int type,
                            int drive, int eff);
        void cleanup();

        int period_coeff;

        double u_up;
        double u_down;

        DspFloatType dthresh;  //dynamic threshold in compression waveshaper
        DspFloatType dyno;
        DspFloatType dynodecay;  //dynamically control symmetry

        DspFloatType compg;  //used by compression distortion
        DspFloatType cratio;  //used by compression for hardness
        DspFloatType tmpgain;  // compression distortion temp variable
        DspFloatType ncSAMPLE_RATE;
        DspFloatType *temps;

        DspFloatType R, P, Vgbias, Vsupp, Ip, Vmin, Vg, Vfactor, Vdyno;  //Valve1 Modeling variables.
        DspFloatType mu, V2bias, Is, Vg2, vfact, ffact, Vlv2out, V2dyno; //Valve2 variables

        class Resample *U_Resample;
        class Resample *D_Resample;


    };
}