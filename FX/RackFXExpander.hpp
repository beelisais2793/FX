#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class Expander
    {

    public:

        Expander (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~Expander ();

        void out (DspFloatType * smps_l, DspFloatType * smps_r);

        void Expander_Change (int np, int value);
        void Expander_Change_Preset (int npreset);
        void cleanup ();
        int getpar (int npar);

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;



        // Compressor

        int Pthreshold;		// Threshold, -80 to 0dB
        int Pattack;			// attack time  (ms)  Set range from 10ms ... 2000ms
        int Pdecay;			// release time (ms)  Set range from 10ms ... 500ms
        int Pshape;			// Sharpness of transition from off to on.  Range is 0 ... 50
        int Plpf;
        int Phpf;
        int Plevel;
        int efollower;		// This is a mode allowing this object to be used as a "dynamics detector"
        // (envelope follower).  If efollower == 1, then efxoutl is a level to be used in place of an LFO
        // for filter modulation, etc. Then efxoutr is signal + envelope for things such as dynamic distortion.
        // Variable efollower is set to 0 by default in constructor.  Do not set this mode unless using this object
        // to control a parameter with signal dynamics.

    private:

        void setlpf (int Plpf);
        void sethpf (int Phpf);


        DspFloatType sgain;
        DspFloatType sfactor;
        DspFloatType tfactor;
        DspFloatType tlevel;
        DspFloatType a_rate;
        DspFloatType d_rate;
        DspFloatType env;
        DspFloatType oldgain;
        DspFloatType gain;
        DspFloatType fs;
        DspFloatType level;



        AnalogFilter *lpfl, *lpfr, *hpfl, *hpfr;

        class FPreset *Fpre;

    };
}