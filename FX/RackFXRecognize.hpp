#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class Recognize
    {
    public:
        Recognize (DspFloatType * efxoutl_, DspFloatType * efxoutr_, DspFloatType trig);
        ~Recognize ();

        void schmittFloat (DspFloatType *indatal, DspFloatType *indatar);
        void sethpf(int value);
        void setlpf(int value);
        int note;

        signed short int *schmittBuffer;
        signed short int *schmittPointer;
        const char **notes;
        DspFloatType trigfact;
        DspFloatType lafreq;
        DspFloatType nfreq, afreq, freq;
        DspFloatType *efxoutl;
        DspFloatType *efxoutr;



    private:

        void displayFrequency (DspFloatType freq);
        void schmittInit (int size);
        void schmittS16LE (signed short int *indata);
        void schmittFree ();

        int ultima;
        int blockSize;

        AnalogFilter *lpfl, *lpfr, *hpfl, *hpfr;

        class Sustainer *Sus;

    };
}