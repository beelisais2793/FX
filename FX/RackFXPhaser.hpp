#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class Phaser
    {
    public:
        Phaser (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~Phaser ();
        void out (DspFloatType * smpsl, DspFloatType * smpsr);
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
        void setdepth (int Pdepth);
        void setfb (int Pfb);
        void setlrcross (int Plrcross);
        void setstages (int Pstages);
        void setphase (int Pphase);


        //Parametrii Phaser
        int Pvolume;
        int Ppanning;
        int Pdepth;		//the depth of the Phaser
        int Pfb;		//feedback
        int Plrcross;	//feedback
        int Pstages;
        int Poutsub;	//if I wish to substract the output instead of the adding it
        int Pphase;

        //Control Parametrii

        //Valorile interne
        DspFloatType panning, fb, depth, lrcross, fbl, fbr, phase;
        DspFloatType *oldl, *oldr;
        DspFloatType oldlgain, oldrgain;

        EffectLFO lfo;		//lfo-ul Phaser
        class FPreset *Fpre;

    };
}