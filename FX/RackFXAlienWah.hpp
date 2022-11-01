#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{


    class Alienwah
    {
    public:
        Alienwah (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~Alienwah ();
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
        //Parametrii Alienwah
        EffectLFO lfo;		//lfo-ul Alienwah
        int Pvolume;
        int Ppanning;
        int Pdepth;		//the depth of the Alienwah
        int Pfb;		//feedback
        int Plrcross;	//feedback
        int Pdelay;
        int Pphase;

        int oldk;
        int oldpdelay;

        //Control Parametrii
        void setvolume (int Pvolume);
        void setpanning (int Ppanning);
        void setdepth (int Pdepth);
        void setfb (int Pfb);
        void setlrcross (int Plrcross);
        void setdelay (int Pdelay);
        void setphase (int Pphase);

        //Valorile interne
        DspFloatType panning, fb, depth, lrcross, phase;
        struct COMPLEXTYPE oldl[MAX_ALIENWAH_DELAY], oldr[MAX_ALIENWAH_DELAY];
        COMPLEXTYPE oldclfol, oldclfor;
        class FPreset *Fpre;

    };

}