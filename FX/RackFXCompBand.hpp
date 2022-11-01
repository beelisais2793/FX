#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{

    class CompBand
    {
    public:
        CompBand (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~CompBand ();
        void out (DspFloatType * smpsl, DspFloatType * smpr);
        void setpreset (int npreset);
        void changepar (int npar, int value);
        int getpar (int npar);
        void cleanup ();

        int Ppreset;
        DspFloatType outvolume;
        DspFloatType level;

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
        void setlevel(int value);
        void setratio (int ch, int value);
        void setthres (int ch, int value);
        void setCross1 (int value);
        void setCross2 (int value);
        void setCross3 (int value);


        //Parametrii
        int Pvolume;	//Volumul or E/R
        int Plevel;
        int PLratio;
        int PMLratio;
        int PMHratio;
        int PHratio;
        int PLthres;
        int PMLthres;
        int PMHthres;
        int PHthres;


        int Cross1;
        int Cross2;
        int Cross3;

        //Parametrii reali

        AnalogFilter *lpf1l, *lpf1r, *hpf1l, *hpf1r;
        AnalogFilter *lpf2l, *lpf2r, *hpf2l, *hpf2r;
        AnalogFilter *lpf3l, *lpf3r, *hpf3l, *hpf3r;

        Compressor *CL,*CML,*CMH,*CH;

        class FPreset *Fpre;

    };
}