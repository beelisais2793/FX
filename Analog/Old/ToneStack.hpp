
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

    double  lf;       // Frequency
    double  f1p0;     // Poles ...
    double  f1p1;
    double  f1p2;
    double  f1p3;

    // Filter #2 (High band)

    double  hf;       // Frequency
    double  f2p0;     // Poles ...
    double  f2p1;
    double  f2p2;
    double  f2p3;

    // Sample history buffer

    double  sdm1;     // Sample data minus 1
    double  sdm2;     //                   2
    double  sdm3;     //                   3

    // Gain Controls

    double  lg;       // low  gain
    double  mg;       // mid  gain
    double  hg;       // high gain

    float lowfreq,highfreq,mixfreq,input_gain,output_gain;
    ToneStack(float lowfreq=880.0f,float highfreq=5000.0f, float mixfreq=44100.0f) {
        lg = 1.0;
        mg = 1.0;
        hg = 1.0;
        this->lowfreq = lowfreq;
        this->highfreq = highfreq;
        this->mixfreq = mixfreq;
        input_gain = output_gain = 1.0f;
        // Calculate filter cutoff frequencies

        lf = 2 * std::sin(M_PI * ((double)lowfreq / (double)mixfreq));
        hf = 2 * std::sin(M_PI * ((double)highfreq / (double)mixfreq));
    }
    float Tick(float sample, float A = 1, float X = 0, float Y = 0)
    {
            double  l,m,h;      // Low / Mid / High - Sample Values
            Undenormal denormal;

            
            double tl = lowfreq;
            double th = highfreq;

            lf = 2 * std::sin(M_PI * ((double)(lowfreq + X * lowfreq)/ (double)mixfreq));
            hf = 2 * std::sin(M_PI * ((double)(highfreq + Y * highfreq)/ (double)mixfreq));

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




