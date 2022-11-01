#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class FormantFilter:public Filter_
    {
    public:
        FormantFilter (class FilterParams * pars);
        ~FormantFilter ();
        void filterout (DspFloatType * smp);
        void setfreq (DspFloatType frequency);
        void setfreq_and_q (DspFloatType frequency, DspFloatType q_);
        void setq (DspFloatType q_);

        void cleanup ();
    private:

        void setpos (DspFloatType input);


        struct {
            DspFloatType freq, amp, q;	//frequency,amplitude,Q
        } formantpar[FF_MAX_VOWELS][FF_MAX_FORMANTS],
        currentformants[FF_MAX_FORMANTS];

        struct {
            unsigned char nvowel;
        } sequence[FF_MAX_SEQUENCE];

        int sequencesize, numformants, firsttime;

        DspFloatType oldformantamp[FF_MAX_FORMANTS];
        DspFloatType oldinput, slowinput;
        DspFloatType Qfactor, formantslowness, oldQfactor;
        DspFloatType vowelclearness, sequencestretch;

        DspFloatType *inbuffer, *tmpbuf;

        AnalogFilter * formant[FF_MAX_FORMANTS];

    };

}