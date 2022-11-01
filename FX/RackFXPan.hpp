#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class Pan
    {

    public:
        Pan (DspFloatType *efxoutl_, DspFloatType *efxoutr_);
        ~Pan ();
        void out (DspFloatType *smpsl, DspFloatType *smpsr);
        void setpreset (int npreset);
        void changepar (int npar, int value);
        int getpar (int npar);
        void cleanup ();


        int Ppreset;
        DspFloatType outvolume;

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;



    private:

        void setvolume (int Pvolume);
        void setpanning (int Ppanning);
        void setextra (int Pdepth);

        int Pvolume;
        int Ppanning;
        int Pextra;
        int PAutoPan;
        int PextraON;


        DspFloatType dvalue,cdvalue,sdvalue;
        DspFloatType panning, mul;
        DspFloatType lfol, lfor;
        DspFloatType ll, lr;

        EffectLFO lfo;
        class FPreset *Fpre;

    };
}