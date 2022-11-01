#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class Vocoder
    {
    public:
        Vocoder (DspFloatType * efxoutl_, DspFloatType * efxoutr_, DspFloatType *auxresampled_,int bands, int DS, int uq, int dq);
        ~Vocoder ();
        void out (DspFloatType * smpsl, DspFloatType * smpr);
        void setpreset (int npreset);
        void changepar (int npar, int value);
        int getpar (int npar);
        void cleanup ();
        void adjust(int DS);

        int Ppreset;
        DspFloatType outvolume;
        DspFloatType vulevel;

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;
        DspFloatType *auxresampled;

    private:

        void setvolume (int Pvolume);
        void setpanning (int Ppanning);
        void init_filters();
        void adjustq(DspFloatType q);
        void   setbands(int numbands, DspFloatType startfreq, DspFloatType endfreq);
        int VOC_BANDS;
        //Parametrii
        int Pvolume;	//This is master wet/dry mix like other FX...but I am finding it is not useful
        int Ppanning;	//Panning
        int Plrcross;	// L/R Mixing  // This is a mono effect, so lrcross and panning are pointless
        int Plevel;		//This should only adjust the level of the IR effect, and not wet/dry mix
        int Pinput;
        int Pband;
        int Pmuffle;
        int Pqq;
        int Pring;

        int DS_state;
        int nPERIOD;
        int nSAMPLE_RATE;

        DspFloatType ncSAMPLE_RATE;
        DspFloatType nfSAMPLE_RATE;

        double u_up;
        double u_down;


        DspFloatType ringworm;
        DspFloatType lpanning, rpanning, input,level;
        DspFloatType alpha,beta,prls,gate;
        DspFloatType compeak, compg, compenv, oldcompenv, calpha, cbeta, cthresh, cratio, cpthresh;
        DspFloatType *tmpl, *tmpr;
        DspFloatType *tsmpsl, *tsmpsr;
        DspFloatType *tmpaux;
        struct fbank {
            DspFloatType sfreq, sq,speak,gain,oldgain;
            AnalogFilter *l, *r, *aux;

        } *filterbank;

        AnalogFilter *vhp, *vlp;

        Resample *U_Resample;
        Resample *D_Resample;
        Resample *A_Resample;

        class FPreset *Fpre;

    };
}