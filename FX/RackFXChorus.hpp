#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{

    class Chorus 
    {

    public:
        Chorus ();
        ~Chorus ();
        void out (DspFloatType * smpsl, DspFloatType * smpsr);

        void changepar (int npar, int value);
        int getpar (int npar);
        void cleanup ();

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;
        DspFloatType outvolume;		//this is the volume of effect and is public because need it in system effect. The out volume of s

        void setpreset (int dgui, int npreset);
    private:
        //Parametrii Chorus
        EffectLFO lfo;		//lfo-ul chorus
        int Pvolume;
        int Ppanning;
        int Pdepth;		//the depth of the Chorus(ms)
        int Pdelay;		//the delay (ms)
        int Pfb;		//feedback
        int Plrcross;	//feedback
        int Pflangemode;	//how the LFO is scaled, to result chorus or flange
        int Poutsub;	//if I wish to substract the output instead of the adding it

        //Control Parametrii
        void setvolume (int Pvolume);
        void setpanning (int Ppanning);
        void setdepth (int Pdepth);
        void setdelay (int Pdelay);
        void setfb (int Pfb);
        void setlrcross (int Plrcross);

        //Valorile interne
        int maxdelay;
        int dlk, drk, dlhi, dlhi2;
        int awesome_mode;

        DspFloatType depth, delay, fb, lrcross, panning, oldr, oldl;
        DspFloatType dl1, dl2, dr1, dr2, lfol, lfor;
        DspFloatType *delayl, *delayr;
        DspFloatType getdelay (DspFloatType xlfo);
        DspFloatType dllo, mdel;

        class FPreset *Fpre;
        class delayline *ldelay, *rdelay;

        int Ppreset;
    };
}