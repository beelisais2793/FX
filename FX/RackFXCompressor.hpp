#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class Compressor
    {

    public:

        Compressor (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~Compressor ();

        void out (DspFloatType * smps_l, DspFloatType * smps_r);

        void Compressor_Change (int np, int value);
        void Compressor_Change_Preset (int dgui,int npreset);
        int getpar (int npar);
        void cleanup ();

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;



        // Compressor

        int tatt;			// attack time  (ms)
        int trel;			// release time (ms)
        int tratio;
        int toutput;
        int tthreshold;
        int a_out;
        int stereo;
        int tknee;
        int peak;
        int clipping;
        int limit;

    private:

        DspFloatType rvolume;
        DspFloatType lvolume;
        DspFloatType rvolume_db;
        DspFloatType lvolume_db;
        DspFloatType thres_db;		// threshold
        DspFloatType knee;
        DspFloatType thres_mx;
        DspFloatType kpct;
        DspFloatType ratio;			// ratio
        DspFloatType kratio;			// ratio maximum for knee region
        DspFloatType eratio;			// dynamic ratio
        DspFloatType makeup;			// make-up gain
        DspFloatType makeuplin;

        DspFloatType outlevel;
        DspFloatType att, attr, attl;
        DspFloatType rel, relr, rell;
        DspFloatType relcnst, attconst;
        int ltimer, rtimer, hold;

        DspFloatType rgain;
        DspFloatType rgain_old;

        DspFloatType lgain;
        DspFloatType lgain_old;

        DspFloatType lgain_t;
        DspFloatType rgain_t;

        DspFloatType coeff_kratio;
        DspFloatType coeff_ratio;
        DspFloatType coeff_knee;
        DspFloatType coeff_kk;
        DspFloatType lpeak;
        DspFloatType rpeak;

        class FPreset *Fpre;

    };
}