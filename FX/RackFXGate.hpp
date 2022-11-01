#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class Gate
    {

    public:

        Gate (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~Gate ();

        void out (DspFloatType * smps_l, DspFloatType * smps_r);

        void Gate_Change (int np, int value);
        void Gate_Change_Preset (int npreset);
        void cleanup ();
        int getpar (int npar);




        // Compressor

        int Pthreshold;		// attack time  (ms)
        int Pattack;			// release time (ms)
        int Ohold;
        int Pdecay;
        int Prange;
        int Plpf;
        int Phpf;
        int Phold;

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;


    private:

        void setlpf (int Plpf);
        void sethpf (int Phpf);


        int hold_count;
        int state;
        DspFloatType range;
        DspFloatType cut;
        DspFloatType t_level;
        DspFloatType a_rate;
        DspFloatType d_rate;
        DspFloatType env;
        DspFloatType gate;
        DspFloatType fs;
        DspFloatType hold;



        AnalogFilter *lpfl, *lpfr, *hpfl, *hpfr;
        class FPreset *Fpre;

    };

}