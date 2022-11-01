#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class DynamicFilter
    {
    public:
        DynamicFilter (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~DynamicFilter ();
        void out (DspFloatType * smpsl, DspFloatType * smpsr);

        void setpreset (int npreset);
        void changepar (int npar, int value);
        int getpar (int npar);
        void cleanup ();


        int Ppreset;
        DspFloatType *efxoutl;
        DspFloatType *efxoutr;
        DspFloatType outvolume;




    private:
        //Parametrii DynamicFilter
        EffectLFO lfo;		//lfo-ul DynamicFilter
        int Pvolume;
        int Ppanning;
        int Pdepth;		//the depth of the lfo of the DynamicFilter
        int Pampsns;	//how the filter varies according to the input amplitude
        int Pampsnsinv;	//if the filter freq is lowered if the input amplitude rises
        int Pampsmooth;	//how smooth the input amplitude changes the filter

        //Control Parametrii
        void setvolume (int Pvolume);
        void setpanning (int Ppanning);
        void setdepth (int Pdepth);
        void setampsns (int Pampsns);

        void reinitfilter ();

        //Valorile interne

        DspFloatType panning, depth, ampsns, ampsmooth;
        DspFloatType ms1, ms2, ms3, ms4;	//mean squares

        class Filter *filterl, *filterr;
        class FilterParams *filterpars;
        class FPreset *Fpre;

    };
}