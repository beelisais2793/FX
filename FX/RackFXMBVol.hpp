#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class MBVvol
    {
    public:
        MBVvol (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~MBVvol ();
        void out (DspFloatType * smpsl, DspFloatType * smpr);
        void setpreset (int npreset);
        void changepar (int npar, int value);
        int getpar (int npar);
        void cleanup ();

        int Ppreset;
        DspFloatType outvolume;

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;
        DspFloatType *lowl;
        DspFloatType *lowr;
        DspFloatType *midll;
        DspFloatType *midlr;
        DspFloatType *midhl;
        DspFloatType *midhr;
        DspFloatType *highl;
        DspFloatType *highr;


    private:

        void setvolume (int Pvolume);
        void setpanning (int Ppanning);
        void setCross1 (int value);
        void setCross2 (int value);
        void setCross3 (int value);
        void setCombi (int value);


        //Parametrii
        int Pvolume;	//Volumul or E/R
        int Pcombi;
        int Cross1;
        int Cross2;
        int Cross3;

        //Parametrii reali

        DspFloatType coeff;
        DspFloatType lfo1l,lfo1r,lfo2l,lfo2r;
        DspFloatType v1l,v1r,v2l,v2r;
        DspFloatType d1,d2,d3,d4;
        DspFloatType volL,volML,volMH,volH;
        DspFloatType volLr,volMLr,volMHr,volHr;
        AnalogFilter *lpf1l, *lpf1r, *hpf1l, *hpf1r;
        AnalogFilter *lpf2l, *lpf2r, *hpf2l, *hpf2r;
        AnalogFilter *lpf3l, *lpf3r, *hpf3l, *hpf3r;

        EffectLFO lfo1,lfo2;

        class FPreset *Fpre;

    };

}