#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class Harmonizer
    {

    public:
        Harmonizer (DspFloatType *efxoutl_, DspFloatType *efxoutr_, long int Quality, int DS, int uq, int dq);
        ~Harmonizer ();
        void out (DspFloatType *smpsl, DspFloatType *smpsr);
        void setpreset (int npreset);
        void changepar (int npar, int value);
        int getpar (int npar);
        void cleanup ();
        void applyfilters (DspFloatType * efxoutl);
        void adjust(int DS);


        int Ppreset;
        int Pinterval;
        int PMIDI;
        int PSELECT;
        int mira;
        int DS_state;
        int nPERIOD;
        int nSAMPLE_RATE;
        long window;

        long int hq;

        double u_up;
        double u_down;
        DspFloatType nfSAMPLE_RATE;


        DspFloatType *efxoutl;
        DspFloatType *efxoutr;
        DspFloatType *outi;
        DspFloatType *outo;
        DspFloatType *templ, *tempr;

        DspFloatType outvolume;



    private:

        int Pvolume;
        int Pgain;
        int Ppan;
        int Pnote;
        int Ptype;

        int fPfreq;
        int fPgain;
        int fPq;

        DspFloatType panning;
        DspFloatType gain;
        DspFloatType interval;

        void setvolume (int Pvolume);
        void setpanning (int Ppan);
        void setinterval (int Pinterval);
        void setgain (int Pgain);
        void setMIDI (int PMIDI);
        void fsetfreq (int value);
        void fsetgain (int value);
        void fsetq (int value);


        AnalogFilter *pl;

        class Resample *U_Resample;
        class Resample *D_Resample;

        PitchShifter *PS;
        class FPreset *Fpre;

    };
}