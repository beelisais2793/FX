#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class Vibe
    {

    public:

        Vibe (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~Vibe ();

        void out (DspFloatType * smpsl, DspFloatType * smpsr);
        void setvolume(int value);
        void setpanning(int value);
        void setpreset (int npreset);
        void changepar (int npar, int value);
        int getpar (int npar);
        void cleanup ();

        DspFloatType outvolume;
        DspFloatType *efxoutl;
        DspFloatType *efxoutr;

    private:
        int Pwidth;
        int Pfb;
        int Plrcross;
        int Pdepth;
        int Ppanning;
        int Pvolume;
        int Pstereo;

        DspFloatType fwidth;
        DspFloatType fdepth;
        DspFloatType rpanning, lpanning;
        DspFloatType flrcross, fcross;
        DspFloatType fb;
        EffectLFO lfo;

        DspFloatType Ra, Rb, b, dTC, dRCl, dRCr, lampTC, ilampTC, minTC, alphal, alphar, stepl, stepr, oldstepl, oldstepr;
        DspFloatType fbr, fbl;
        DspFloatType dalphal, dalphar;
        DspFloatType lstep,rstep;
        DspFloatType cperiod;
        DspFloatType gl, oldgl;
        DspFloatType gr, oldgr;

        class fparams
        {
        public:
            DspFloatType x1;
            DspFloatType y1;
            //filter coefficients
            DspFloatType n0;
            DspFloatType n1;
            DspFloatType d0;
            DspFloatType d1;
        } vc[8], vcvo[8], ecvc[8], vevo[8], bootstrap[8];

        DspFloatType vibefilter(DspFloatType data, fparams *ftype, int stage);
        void init_vibes();
        void modulate(DspFloatType ldrl, DspFloatType ldrr);
        DspFloatType bjt_shape(DspFloatType data);

        DspFloatType R1;
        DspFloatType Rv;
        DspFloatType C2;
        DspFloatType C1[8];
        DspFloatType beta;  //transistor forward gain.
        DspFloatType gain, k;
        DspFloatType oldcvolt[8] ;
        DspFloatType en1[8], en0[8], ed1[8], ed0[8];
        DspFloatType cn1[8], cn0[8], cd1[8], cd0[8];
        DspFloatType ecn1[8], ecn0[8], ecd1[8], ecd0[8];
        DspFloatType on1[8], on0[8], od1[8], od0[8];

        class FPreset *Fpre;


    };

}