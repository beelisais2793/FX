#pragma once

#include <cmath>
#include <cstdio>
#include <iostream>

namespace FX::Mu45
{
    class Mu45LFO {
    public:
        Mu45LFO();                              // Constructor
        void setFreq(DspFloatType freq, DspFloatType fs);     // Set the frequency of the oscillator.
        DspFloatType tick();                           // Generate a sample of output and update the state
        void resetPhase();                      // Set the oscillator phase to 0
        
    private:
        static const int N = 1024;      // size of the wavetable
        DspFloatType table[N];                 // the wavetable
        DspFloatType phase_inc;                // amount to increment phase each tick
        DspFloatType phase;                    // current index into the wavetable
    };

    class Mu45FilterCalc
    {
    public:
        
        constexpr static DspFloatType myPI = 3.1415926535897932384626433832795;
        
        // Calculates the filter coefficients for a 1-pole low-pass filter.
        // coeffs = [b0 a1]
        // fc = -3dB freq
        // fs = sampling rate
        static void calcCoeffs1PoleLPF(DspFloatType* coeffs, DspFloatType fc, DspFloatType fs);
        
        // Calculates the filter coefficients for a 2nd-order peak filter.
        // coeffs = [b0, b1, b2, a1, a2]
        // fc     = center frequency in Hz
        // gainDB = the amount of peak gain or cut, in dB
        // Q      = filter Q, proportional to fc/bandwidth
        // fs     = sampling rate in Hz;
        static void calcCoeffsPeak(DspFloatType* coeffs, DspFloatType fc, DspFloatType gainDb, DspFloatType Q, DspFloatType fs);
        
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Calculates the filter coefficients for a 2nd-order low-shelf filter.
        // coeffs = [b0, b1, b2, a1, a2]
        // fc     = transition frequency in Hz
        // gainDb = the amount of peak gain or cut, in dB
        // fs     = sampling rate in Hz
        static void calcCoeffsLowShelf(DspFloatType* coeffs, DspFloatType fc, DspFloatType gainDb, DspFloatType fs);
        
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Calculates the filter coefficients for a 2nd-order high-shelf filter.
        // coeffs = [b0, b1, b2, a1, a2]
        // fc     = transition frequency in Hz
        // gainDb = the amount of peak gain or cut, in dB
        // fs     = sampling rate in Hz
        static void calcCoeffsHighShelf(DspFloatType* coeffs, DspFloatType fc, DspFloatType gainDb, DspFloatType fs);
        
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Calculates the filter coefficients for a 2nd-order low-pass filter.
        // coeffs = [b0, b1, b2, a1, a2]
        // fc     = transition frequency in Hz
        // Q      = filter Q, proportional to fc/bandwidth
        // fs     = sampling rate in Hz
        static void calcCoeffsLPF(DspFloatType* coeffs, DspFloatType fc, DspFloatType Q, DspFloatType fs);
        
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Calculates the filter coefficients for a 2nd-order high-pass filter.
        // coeffs = [b0, b1, b2, a1, a2]
        // fc     = transition frequency in Hz
        // Q      = filter Q, proportional to fc/bandwidth
        // fs     = sampling rate in Hz
        static void calcCoeffsHPF(DspFloatType* coeffs, DspFloatType fc, DspFloatType Q, DspFloatType fs);
        
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Calculates the filter coefficients for a 2nd-order band-pass filter.
        // coeffs = [b0, b1, b2, a1, a2]
        // fc     = transition frequency in Hz
        // Q      = filter Q, proportional to fc/bandwidth
        // fs     = sampling rate in Hz
        static void calcCoeffsBPF(DspFloatType* coeffs, DspFloatType fc, DspFloatType Q, DspFloatType fs);
        
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Calculates the filter coefficients for a 2nd-order all-pass filter.
        // coeffs = [b0, b1, b2, a1, a2]
        // fc     = frequencyin Hz where phase response crosses 180d (pi rad) in Hz
        // R      = The pole radius. This affects how steep the phase curve is at fc
        //          (R must be <= 1.0 for the filter to be stable!)
        // fs     = sampling rate in Hz
        static void calcCoeffsAPF(DspFloatType* coeffs, DspFloatType fc, DspFloatType R, DspFloatType fs);
        
    };


    // constructor
    Mu45LFO::Mu45LFO()
    {
        DspFloatType pi = 3.1415926535897932384626433832795;
        
        // initialize a table with a sine waveform
        for (int i = 0; i < N; i++) {
            table[i] = sin(2*pi*i/N);
        }
        
        // init other stuff
        setFreq(1.0, 44100);
        resetPhase();
    }

    // set the frequency of the LFO (freq) and how often it will be called (fs). Both values are in Hz.
    void Mu45LFO::setFreq(DspFloatType freq, DspFloatType fs)
    {
        phase_inc = N * freq / fs;
    }

    // reset the phase, so that the next output is at the beginning of the wavetable
    void Mu45LFO::resetPhase()
    {
        phase = 0.0;
    }

    // update the phase of the LFO and produce an output sample
    DspFloatType Mu45LFO::tick()
    {
        // calc output
        DspFloatType outSamp, frac;
        int idx1, idx2;         // the indeces to the two table entries we will interpolate between
        
        idx1 = floor(phase);    // the first index is the integer part of phase
        idx2 = idx1 + 1;        // the second index is the next table entry
        if (idx2 >= N) {        // unless that is greater than the table length...
            idx2 = 0;           // in which case wrap around
        }
        frac = phase - idx1;    // the fraction of the index, i.e. the amount to interpolate
        
        // linearly interpolate between adjacent table entries
        outSamp  = (1-frac)*table[idx1] + frac*table[idx2];
        
        // update phase
        phase += phase_inc;
        if (phase >= N) {    // wrap around
            phase -= N;
        }
        
        return outSamp;
    }

    // Calculates the filter coefficients for a 1-pole low-pass filter.
    // fc = -3dB freq
    // fs = sampling rate
    // coeffs = [b0 a1]

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    void Mu45FilterCalc::calcCoeffs1PoleLPF(DspFloatType* coeffs, DspFloatType fc, DspFloatType fs)
    {
        DspFloatType wc, b, b0, a1;
        
        // fc cannot be zero. let's limit it to 10Hz to fs/2
        if (fc < 10){
            fc = 10;
        }
        else if (fc > fs/2) {
            fc = fs/2;
        }
        
        wc = 2*myPI*fc/fs;    // the -3dB freq in rad/sec
        b = 4 -2*cos(wc);
        a1 =0.5*(sqrt(b*b - 4) - b);
        b0 = 1 + a1;
        
        coeffs[0] = b0;
        coeffs[1] = a1;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Calculates the filter coefficients for a 2nd-order peak filter.
    // coeffs = [b0, b1, b2, a1, a2]
    // fc     = center frequency in Hz
    // gainDB = the amount of peak gain or cut, in dB
    // Q      = filter Q, proportional to fc/bandwidth
    // fs     = sampling rate in Hz

    void Mu45FilterCalc::calcCoeffsPeak(DspFloatType* coeffs, DspFloatType fc, DspFloatType gainDb, DspFloatType Q, DspFloatType fs)
    {
        // Based on Zolzer, DASP, Chapter 5
        DspFloatType K, V, D;
        DspFloatType b0, b1, b2, a1, a2;
        
        // let's limit fc to 10Hz to fs/2
        if (fc < 10){
            fc = 10;
        }
        else if (fc > fs/2) {
            fc = fs/2;
        }
        
        K = tan(myPI*fc/fs);
        
        if (gainDb >= 0) {
            // boost or peak
            V = powf(10, gainDb/20);
            b0 = 1 + V*K/Q + K*K;
            b1 = 2*(K*K - 1);
            b2 = 1 - V*K/Q + K*K;
            D = 1 + K/Q + K*K;
            a1 = 2*(K*K - 1);
            a2 = 1 - K/Q + K*K;
        }
        else {
            // cut or notch
            V = pow(10, -gainDb/20);
            b0 = 1 + K/Q + K*K;
            b1 = 2*(K*K - 1);
            b2 = 1 - K/Q + K*K;
            D = 1 + V*K/Q + K*K;
            a1 = 2*(K*K - 1);
            a2 = 1 - V*K/Q + K*K;
        }
        // normalize by D
        b0 = b0 / D;
        b1 = b1 / D;
        b2 = b2 / D;
        a1 = a1 / D;
        a2 = a2 / D;
        
        coeffs[0] = b0;
        coeffs[1] = b1;
        coeffs[2] = b2;
        coeffs[3] = a1;
        coeffs[4] = a2;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Calculates the filter coefficients for a 2nd-order low-shelf filter.
    // coeffs = [b0, b1, b2, a1, a2]
    // fc     = transition frequency in Hz
    // gainDb = the amount of peak gain or cut, in dB
    // fs     = sampling rate in Hz

    void Mu45FilterCalc::calcCoeffsLowShelf(DspFloatType* coeffs, DspFloatType fc, DspFloatType gainDb, DspFloatType fs)
    {
        // Based on Zolzer, DASP, Chapter 5
        DspFloatType K, V, D;
        DspFloatType b0, b1, b2, a1, a2;
        
        // let's limit fc to 10Hz to fs/2
        if (fc < 10){
            fc = 10;
        }
        else if (fc > fs/2) {
            fc = fs/2;
        }
        
        K = tan(myPI*fc/fs);
        
        if (gainDb >= 0) {
            // boost low freqs
            V = pow(10, gainDb/20);
            b0 = 1 + sqrt(2*V)*K + V*K*K;
            b1 = 2*(V*K*K - 1);
            b2 = 1 - sqrt(2*V)*K + V*K*K;
            D = 1 + sqrt(2)*K + K*K;
            a1 = 2*(K*K - 1);
            a2 = 1 - sqrt(2)*K + K*K;
        }
        else {
            // cut low freqs
            V = pow(10, -gainDb/20);
            b0 = 1 + sqrt(2)*K + K*K;
            b1 = 2*(K*K - 1);
            b2 = 1 - sqrt(2)*K + K*K;
            D = 1 + sqrt(2*V)*K + V*K*K;
            a1 = 2*(V*K*K - 1);
            a2 = 1 - sqrt(2*V)*K + V*K*K;
        }
        // normalize by D
        b0 = b0 / D;
        b1 = b1 / D;
        b2 = b2 / D;
        a1 = a1 / D;
        a2 = a2 / D;
        
        coeffs[0] = b0;
        coeffs[1] = b1;
        coeffs[2] = b2;
        coeffs[3] = a1;
        coeffs[4] = a2;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Calculates the filter coefficients for a 2nd-order high-shelf filter.
    // coeffs = [b0, b1, b2, a1, a2]
    // fc     = transition frequency in Hz
    // gainDb = the amount of peak gain or cut, in dB
    // fs     = sampling rate in Hz

    void Mu45FilterCalc::calcCoeffsHighShelf(DspFloatType* coeffs, DspFloatType fc, DspFloatType gainDb, DspFloatType fs)
    {
        // Based on Zolzer, DASP, Chapter 5
        DspFloatType K, V, D1, D2;
        DspFloatType b0, b1, b2, a1, a2;
        
        // let's limit fc to 10Hz to fs/2
        if (fc < 10){
            fc = 10;
        }
        else if (fc > fs/2) {
            fc = fs/2;
        }
        
        K = tan(myPI*fc/fs);
        
        if (gainDb >= 0) {
            // boost high freqs
            V = pow(10, gainDb/20);
            b0 = V + sqrt(2*V)*K + K*K;
            b1 = 2*(K*K - V);
            b2 = V - sqrt(2*V)*K + K*K;
            D1 = 1 + sqrt(2)*K + K*K;
            a1 = 2*(K*K - 1);
            a2 = 1 - sqrt(2)*K + K*K;
            
            //normalize
            b0 = b0 / D1;
            b1 = b1 / D1;
            b2 = b2 / D1;
            a1 = a1 / D1;
            a2 = a2 / D1;
        }
        else {
            // cut high freqs
            V = pow(10, -gainDb/20);
            b0 = 1 + sqrt(2)*K + K*K;
            b1 = 2*(K*K - 1);
            b2 = 1 - sqrt(2)*K + K*K;
            D1 = V + sqrt(2*V)*K + K*K;
            D2 = 1 + sqrt(2/V)*K + K*K/V;
            a1 = 2*(K*K/V - 1);
            a2 = 1 - sqrt(2/V)*K + K*K/V;
            
            // normalize
            b0 = b0 / D1;
            b1 = b1 / D1;
            b2 = b2 / D1;
            a1 = a1 / D2;
            a2 = a2 / D2;
        }
        
        coeffs[0] = b0;
        coeffs[1] = b1;
        coeffs[2] = b2;
        coeffs[3] = a1;
        coeffs[4] = a2;
    }


    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Calculates the filter coefficients for a 2nd-order low-pass filter.
    // coeffs = [b0, b1, b2, a1, a2]
    // fc     = transition frequency in Hz
    // Q      = filter Q, proportional to fc/bandwidth
    // fs     = sampling rate in Hz

    void Mu45FilterCalc::calcCoeffsLPF(DspFloatType* coeffs, DspFloatType fc, DspFloatType Q, DspFloatType fs)
    {
        DspFloatType K, Ksq, D, b0, b1, b2, a1, a2;
        
        K = tan(myPI*fc/fs);
        Ksq = K*K;
        
        D = Ksq*Q + K +Q;
        b0 = Ksq*Q;
        b1 = 2*b0;
        a1 = 2*Q*(Ksq-1);
        a2 = Ksq*Q - K + Q;
        
        b0 = b0 / D;
        b1 = b1 / D;
        b2 = b0;
        a1 = a1 / D;
        a2 = a2 / D;
        
        coeffs[0] = b0;
        coeffs[1] = b1;
        coeffs[2] = b2;
        coeffs[3] = a1;
        coeffs[4] = a2;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Calculates the filter coefficients for a 2nd-order high-pass filter.
    // coeffs = [b0, b1, b2, a1, a2]
    // fc     = transition frequency in Hz
    // Q      = filter Q, proportional to fc/bandwidth
    // fs     = sampling rate in Hz

    void Mu45FilterCalc::calcCoeffsHPF(DspFloatType* coeffs, DspFloatType fc, DspFloatType Q, DspFloatType fs)
    {
        DspFloatType K, Ksq, D, b0, b1, b2, a1, a2;
        
        K = tan(myPI*fc/fs);
        Ksq = K*K;
        
        D = Ksq*Q + K +Q;
        b0 = Q;
        b1 = -2*b0;
        a1 = 2*Q*(Ksq-1);
        a2 = Ksq*Q - K + Q;
        
        b0 = b0 / D;
        b1 = b1 / D;
        b2 = b0;
        a1 = a1 / D;
        a2 = a2 / D;
        
        coeffs[0] = b0;
        coeffs[1] = b1;
        coeffs[2] = b2;
        coeffs[3] = a1;
        coeffs[4] = a2;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Calculates the filter coefficients for a 2nd-order band-pass filter.
    // coeffs = [b0, b1, b2, a1, a2]
    // fc     = transition frequency in Hz
    // Q      = filter Q, proportional to fc/bandwidth
    // fs     = sampling rate in Hz

    void Mu45FilterCalc::calcCoeffsBPF(DspFloatType* coeffs, DspFloatType fc, DspFloatType Q, DspFloatType fs)
    {
        DspFloatType K, Ksq, D, b0, b1, b2, a1, a2;
        
        K = tan(myPI*fc/fs);
        Ksq = K*K;
        
        D = Ksq*Q + K +Q;
        b0 = K;
        a1 = 2*Q*(Ksq-1);
        a2 = Ksq*Q - K + Q;
        
        b0 = b0 / D;
        b1 = 0;
        b2 = -b0;
        a1 = a1 / D;
        a2 = a2 / D;
        
        coeffs[0] = b0;
        coeffs[1] = b1;
        coeffs[2] = b2;
        coeffs[3] = a1;
        coeffs[4] = a2;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Calculates the filter coefficients for a 2nd-order all-pass filter.
    // coeffs = [b0, b1, b2, a1, a2]
    // fc     = frequencyin Hz where phase response crosses 180 degrees (or PI radians)
    // R      = The pole radius. This affects how steep the phase curve is at fc
    //          (R must be <= 1.0 for the filter to be stable!)
    // fs     = sampling rate in Hz
    void Mu45FilterCalc::calcCoeffsAPF(DspFloatType* coeffs, DspFloatType fc, DspFloatType R, DspFloatType fs)
    {
        DspFloatType theta, b0, b1, b2, a1, a2;
        theta = 2*Mu45FilterCalc::myPI*fc/fs;
        a1 = -2.0*R*cos(theta);
        a2 = R*R;
        b0 = a2;
        b1 =a1;
        b2 = 1.0;
        
        coeffs[0] = b0;
        coeffs[1] = b1;
        coeffs[2] = b2;
        coeffs[3] = a1;
        coeffs[4] = a2;
    }
}