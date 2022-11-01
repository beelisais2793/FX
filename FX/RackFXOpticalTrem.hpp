#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class Opticaltrem
    {

    public:

        Opticaltrem (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~Opticaltrem ();

        void out (DspFloatType * smpsl, DspFloatType * smpsr);
        void setpanning(int value);
        void setpreset (int npreset);
        void changepar (int npar, int value);
        int getpar (int npar);
        void cleanup ();

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;


    private:
        int Pdepth;
        int Ppanning;
        int Pinvert;  //Invert the opto and resistor relationship
    
        DspFloatType Ra, Rb, R1, Rp, b, dTC, dRCl, dRCr, minTC, alphal, alphar, stepl, stepr, oldstepl, oldstepr, fdepth;
        DspFloatType lstep,rstep;
        DspFloatType cperiod;
        DspFloatType gl, oldgl;
        DspFloatType gr, oldgr;
        DspFloatType rpanning, lpanning;
        EffectLFO lfo;

        class FPreset *Fpre;

    };
}