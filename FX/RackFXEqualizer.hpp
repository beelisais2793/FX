#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    const size_t MAX_EQ_BANDS=16;

    class PluginEQ
    {
    public:
        PluginEQ (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~PluginEQ ();
        void out (DspFloatType * smpsl, DspFloatType * smpr);
        void setpreset (int npreset);
        void changepar (int npar, int value);
        int getpar (int npar);
        void cleanup ();
        DspFloatType getfreqresponse (DspFloatType freq);
        void setvolume (int Pvolume);

        int Ppreset;
        int Pvolume;	//Volumul

        DspFloatType outvolume;		//this is the volume of effect and is public because need it in system effect. The out volume of

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;


        struct {
            //parameters
            int Ptype, Pfreq, Pgain, Pq, Pstages;
            //internal values
            AnalogFilter *l, *r;
        } filter[MAX_EQ_BANDS];

    };
}