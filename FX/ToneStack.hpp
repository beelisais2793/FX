
//---------------------------------------------------------------------------
//
//                                3 Band EQ :)
//
// EQ.H - Header file for 3 band EQ
//
// (c) Neil C / Etanza Systems / 2K6
//
// Shouts / Loves / Moans = etanza at lycos dot co dot uk
//
// This work is hereby placed in the public domain for all purposes, including
// use in commercial applications.
//
// The author assumes NO RESPONSIBILITY for any problems caused by the use of
// this software.
//
//----------------------------------------------------------------------------

#pragma once 
#include <cmath>
#include "Undenormal.hpp"

// ------------
//| Structures |
// ------------

struct ToneStack
{
    // Filter #1 (Low band)

    DspFloatType  lf;       // Frequency
    DspFloatType  f1p0;     // Poles ...
    DspFloatType  f1p1;
    DspFloatType  f1p2;
    DspFloatType  f1p3;

    // Filter #2 (High band)

    DspFloatType  hf;       // Frequency
    DspFloatType  f2p0;     // Poles ...
    DspFloatType  f2p1;
    DspFloatType  f2p2;
    DspFloatType  f2p3;

    // Sample history buffer

    DspFloatType  sdm1;     // Sample data minus 1
    DspFloatType  sdm2;     //                   2
    DspFloatType  sdm3;     //                   3

    // Gain Controls

    DspFloatType  lg;       // low  gain
    DspFloatType  mg;       // mid  gain
    DspFloatType  hg;       // high gain

    DspFloatType lowfreq,highfreq,mixfreq,input_gain,output_gain;
    ToneStack(DspFloatType lowfreq=880.0f,DspFloatType highfreq=5000.0f, DspFloatType mixfreq=44100.0f) {
        lg = 1.0;
        mg = 1.0;
        hg = 1.0;
        this->lowfreq = lowfreq;
        this->highfreq = highfreq;
        this->mixfreq = mixfreq;
        input_gain = output_gain = 1.0f;
        // Calculate filter cutoff frequencies

        lf = 2 * std::sin(M_PI * ((DspFloatType)lowfreq / (DspFloatType)mixfreq));
        hf = 2 * std::sin(M_PI * ((DspFloatType)highfreq / (DspFloatType)mixfreq));
    }
    DspFloatType Tick(DspFloatType sample, DspFloatType A = 1, DspFloatType X = 0, DspFloatType Y = 0)
    {
            DspFloatType  l,m,h;      // Low / Mid / High - Sample Values
            Undenormal denormal;

            
            DspFloatType tl = lowfreq;
            DspFloatType th = highfreq;

            lf = 2 * std::sin(M_PI * ((DspFloatType)(lowfreq + X * lowfreq)/ (DspFloatType)mixfreq));
            hf = 2 * std::sin(M_PI * ((DspFloatType)(highfreq + Y * highfreq)/ (DspFloatType)mixfreq));

            // Filter #1 (lowpass)
            sample *= input_gain;

            f1p0  += (lf * (sample - f1p0));
            f1p1  += (lf * (f1p0 - f1p1));
            f1p2  += (lf * (f1p1 - f1p2));
            f1p3  += (lf * (f1p2 - f1p3));

            l          = f1p3;

            // Filter #2 (highpass)
            f2p0  += (hf * (sample - f2p0));
            f2p1  += (hf * (f2p0 - f2p1));
            f2p2  += (hf * (f2p1 - f2p2));
            f2p3  += (hf * (f2p2 - f2p3));

            h          = sdm3 - f2p3;

            // Calculate midrange (signal - (low + high))
            m          = sdm3 - (h + l);

            // Scale, Combine and store

            l         *= lg;
            m         *= mg;
            h         *= hg;

            // Shuffle history buffer

            sdm3   = sdm2;
            sdm2   = sdm1;
            sdm1   = output_gain*sample;

            lf = tl;
            hf = th;

            // Return result
            return A*(l + m + h);
    }
};




