#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class SVFilter:public Filter_
    {
    public:
        SVFilter (unsigned char Ftype, DspFloatType Ffreq, DspFloatType Fq,
                unsigned char Fstages);
        ~SVFilter ();
        void filterout (DspFloatType * smp);
        void setfreq (DspFloatType frequency);
        void setfreq_and_q (DspFloatType frequency, DspFloatType q_);
        void setq (DspFloatType q_);

        void settype (int type_);
        void setgain (DspFloatType dBgain);
        void setstages (int stages_);
        void cleanup ();

    private:
        struct fstage {
            DspFloatType low, high, band, notch;
        } st[MAX_FILTER_STAGES + 1];

        struct parameters {
            DspFloatType f, q, q_sqrt;
        } par, ipar;


        void singlefilterout (DspFloatType * smp, fstage & x, parameters & par);
        void computefiltercoefs ();
        int type;			//The type of the filter (LPF1,HPF1,LPF2,HPF2...)
        int stages;			//how many times the filter is applied (0->1,1->2,etc.)
        int abovenq;			//this is 1 if the frequency is above the nyquist
        int oldabovenq;
        int needsinterpolation, firsttime;

        DspFloatType freq;		//Frequency given in Hz
        DspFloatType q;			//Q factor (resonance or Q factor)
        DspFloatType gain;		//the gain of the filter (if are shelf/peak) filters

    };
}