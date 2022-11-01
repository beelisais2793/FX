#pragma once

#include "stk/DelayL.h"
#include "stk/BiQuad.h"

namespace FX
{
    //////////////////////////////////////////////////////////////////////
    // ChorusDelay
    //////////////////////////////////////////////////////////////////////
    class FilterCalc
    {
    public:
        
        constexpr static DspFloatType myPI = 3.1415926535897932384626433832795; //NASA only needs 15 decimal places of Pi for their calculations
        
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
        
    };

    // Calculates the filter coefficients for a 1-pole low-pass filter.
    // fc = -3dB freq
    // fs = sampling rate
    // coeffs = [b0 a1]

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    void FilterCalc::calcCoeffs1PoleLPF(DspFloatType* coeffs, DspFloatType fc, DspFloatType fs)
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

    void FilterCalc::calcCoeffsPeak(DspFloatType* coeffs, DspFloatType fc, DspFloatType gainDb, DspFloatType Q, DspFloatType fs)
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

    void FilterCalc::calcCoeffsLowShelf(DspFloatType* coeffs, DspFloatType fc, DspFloatType gainDb, DspFloatType fs)
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

    void FilterCalc::calcCoeffsHighShelf(DspFloatType* coeffs, DspFloatType fc, DspFloatType gainDb, DspFloatType fs)
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

    void FilterCalc::calcCoeffsLPF(DspFloatType* coeffs, DspFloatType fc, DspFloatType Q, DspFloatType fs)
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

    void FilterCalc::calcCoeffsHPF(DspFloatType* coeffs, DspFloatType fc, DspFloatType Q, DspFloatType fs)
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

    void FilterCalc::calcCoeffsBPF(DspFloatType* coeffs, DspFloatType fc, DspFloatType Q, DspFloatType fs)
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



    class basicLFO {
    public:
        basicLFO();                             // Constructor
        void setFreq(DspFloatType freq, DspFloatType fs);     // Set the frequency of the oscillator.
        DspFloatType tick();                           // Generate a sample of output and update the state
        void resetPhase();                      // Set the oscillator phase to 0
        
    private:
        static const int N = 1024;      // size of the wavetable
        DspFloatType table[N];                 // the wavetable
        DspFloatType phase_inc;                // amount to increment phase each tick
        DspFloatType phase;                    // current index into the wavetable
    };

    // constructor
    basicLFO::basicLFO()
    {
        DspFloatType pi = 3.1415926535897932384626433832795;
        
        // initialize a table with a sine waveform
        for (int i = 0; i < N; i++) {
            table[i] = sin(2*pi*i/N);
        }
        
        // init other stuff
        setFreq(1.0, 48000); //will be overwritten by DAW's sampling rate when we use
        resetPhase();
    }

    // set the frequency of the LFO (freq) and how often it will be called (fs). Both values are in Hz.
    void basicLFO::setFreq(DspFloatType freq, DspFloatType fs)
    {
        phase_inc = N * freq / fs;
    }

    // reset the phase, so that the next output is at the beginning of the wavetable
    void basicLFO::resetPhase()
    {
        phase = 0.0;
    }

    // update the phase of the LFO and produce an output sample
    DspFloatType basicLFO::tick()
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

    struct ChorusDelay
    {
        // gain params
        DspFloatType oGainAParam = 0; // calculation of out gain parameter
        DspFloatType iGainAParam = 0; // calculation of gry gain parameter
        
        // delay params
        DspFloatType dryWetAParam = 0.5; // dry and wet knob
        DspFloatType dGain = 0.5; // variable to hold dry gain value
        DspFloatType wGain = 0.5; // variable to hold wet gain value
        DspFloatType fbAParam = 0.5; // delay.nextOut() * fbGain
        DspFloatType fbGL = 0.5, fbGR=0.5; // variables to hold left and right feedback
        
        DspFloatType delayTime = 250;
        DspFloatType depthAParam = 0.85;
        
        // special params
        DspFloatType shimmer = 0.5;
        DspFloatType shimmerAParam=0.5;
        DspFloatType shimmerBParam=0.5;
        DspFloatType chorusFilterGain=0.5;
        
        DspFloatType softClipThreshold = 2.f / 3.f;
        DspFloatType darkenParam=0.1;
        
        
        // chorus params
        DspFloatType chorusFbAParam = 0.95;
        DspFloatType chorusGainParam = 0;
        DspFloatType depthSamps;
        DspFloatType chorusDryWetParam = 0.9;    
        DspFloatType chorusDry = 0.5;
        DspFloatType chorusWet = 0.5;
        DspFloatType chorusSampsL, chorusSampsR;
        DspFloatType chorusMsecL, chorusMsecR;
        
        DspFloatType darkAParam = 0.5;
        DspFloatType ogainA = 0;
        DspFloatType igainA = 0;
            
        DspFloatType rate = 0.025f;
        // calc params
        DspFloatType fs; // need sample rate ro calculate delay    
            
        DspFloatType   delaySampsL, delaySampsR; // variable to store our delay in samples
        int     maxDelayInSamps; // max amount of memory to store delay
        
        // objects
        stk::DelayL delayL, delayR, chorusDelayL, chorusDelayR; // delay objects
        
        stk::BiQuad filterL, filterR,
                    lowFilterL, lowFilterR,
                    chFilterL, chFilterR,
                    mainFilterL, mainFilterR; // filter objects for high shelf
        
        basicLFO LFOL, LFOR; // LFO objects for chorus rate
        
        // interpolation stuff
        DspFloatType   curDelayL, prevDelayL,
                curDelayR, prevDelayR,
                curDepth, prevDepth,
                curChorusL, curChorusR,
                prevChorusL, prevChorusR;

        

        ChorusDelay()
        {
            fs = sampleRate;
            curDelayL = prevDelayL = curDelayR = prevDelayR =
            curDepth  = prevDepth  = curChorusL, curChorusR =
            prevChorusL = prevChorusR = 0;

            // initialize maximum delay based on max delay time desired
            maxDelayInSamps = fs * 2; // 2 seconds (or 2000 ms) of maximum delay
            delayL.setMaximumDelay(maxDelayInSamps); // initializing delay
            delayR.setMaximumDelay(maxDelayInSamps); // initializing delay
            chorusDelayL.setMaximumDelay(maxDelayInSamps);
            chorusDelayR.setMaximumDelay(maxDelayInSamps);

            delayL.setDelay(maxDelayInSamps);
            delayR.setDelay(maxDelayInSamps);
            chorusDelayL.setDelay(maxDelayInSamps);
            chorusDelayR.setDelay(maxDelayInSamps);

            // LFO stuff
            LFOL.resetPhase();
            LFOR.resetPhase();
            LFOL.setFreq(rate, fs);
            LFOR.setFreq(rate, fs);   
        }
        ~ChorusDelay()
        {

        }
        void calcAlgorithmParams()
        {
            fs = sampleRate; // samples per second
            
            DspFloatType leftDelayMsec  = delayTime;
            DspFloatType rightDelayMsec = delayTime;
            
            // convert delay from ms to samples
            delaySampsL = calcDelaySampsFromMsec(leftDelayMsec); // converted to samples
            delaySampsR = calcDelaySampsFromMsec(rightDelayMsec); // converted to samples
        
            // chorus ms from user delay ms
            chorusMsecL  = (leftDelayMsec * 1.5) + 10; // convert 0-2000 to 10-40 ms
            chorusMsecR  = (rightDelayMsec * 1.5) + 10; // convert 0-2000 to 10-40 ms
            chorusSampsL = calcDelaySampsFromMsec(chorusMsecL);
            chorusSampsR = calcDelaySampsFromMsec(chorusMsecR);
            
            // set current delay
            curDelayL  = delaySampsL;
            curDelayR  = delaySampsR;
            curChorusL = chorusSampsL;
            curChorusR = chorusSampsR;
            
            depthSamps = calcDelaySampsFromMsec(depthAParam * 10); // convert depth from 0-100% to 0-10 ms to samples
            
            curDepth   = depthSamps; // set current depth                
                            
            // delay mix        
            dGain = 1 - dryWetAParam; // if mix knob is 100%, dry = 0
            wGain = dryWetAParam; // if mix knob is 100%, wet = 1
            
            // chorus mix                
            chorusDry = 1 - chorusDryWetParam;
            chorusWet = chorusDryWetParam;
            
            // filter stuff
            DspFloatType shelfCoeffs[5];
            DspFloatType lowShelfCoeffs[5];
            DspFloatType chorusCoeffs[5];
            DspFloatType mainCoeffs[5];
            
            
            // user parameter conversions / thresholds for filters
            //shimmerAParam = (*shimmerUParam * 0.005); // 0 - 100% to 0 to +0.5 for gain dB
            shimmerBParam = ( shimmer * -6); // 0 - 100% to 0 to -6 for reduction dB
            if (fbAParam >= 1.0) { // 100% or greater feedback gets entirely attenuated
                shimmerAParam = ( shimmer * -0.5); // -0.5dB max
            }
            else if (fbAParam >= 0.92) { // no attenuation for fb 92% - 99.9%, no gain
                shimmerAParam = ( shimmer * 0.0);
            } else { // increase highs on fb if below 92%
                shimmerAParam = ( shimmer * 0.5); // 0 - 100% to 0 to +0.5 for gain dB
            }
            if (chorusFbAParam < 0.8) { // do nothing with filter until chorusFB > 80%
                chorusFilterGain = 0.0;
            } else {
                chorusFilterGain = ( chorusGainParam * -15 ); // 90 - 100% to 0 to -3 for reduction dB
            }
            
            darkenParam = darkAParam * -9; // 0 - 100% to 0 to -9 dB
            
            // set high shelf frequency center at 1.2kHz
            FilterCalc::calcCoeffsHighShelf(shelfCoeffs, 1200.0, shimmerAParam, fs);
            filterL.setCoefficients(shelfCoeffs[0],shelfCoeffs[1],shelfCoeffs[2],shelfCoeffs[3],shelfCoeffs[4]);
            filterR.setCoefficients(shelfCoeffs[0],shelfCoeffs[1],shelfCoeffs[2],shelfCoeffs[3],shelfCoeffs[4]);
            // set low shelf fc at 1.2kHz, user reduces low shelf gain while increasing high shelf gain
            FilterCalc::calcCoeffsLowShelf(lowShelfCoeffs, 1200.0, shimmerBParam, fs);
            lowFilterL.setCoefficients(lowShelfCoeffs[0],lowShelfCoeffs[1],lowShelfCoeffs[2],lowShelfCoeffs[3],lowShelfCoeffs[4]);
            lowFilterR.setCoefficients(lowShelfCoeffs[0],lowShelfCoeffs[1],lowShelfCoeffs[2],lowShelfCoeffs[3],lowShelfCoeffs[4]);
            // set filter for chorus feedback center at 900Hz, increase in feedback = decrease in gain
            FilterCalc::calcCoeffsLowShelf(chorusCoeffs, 900.0, chorusFilterGain, fs);
            chFilterL.setCoefficients(chorusCoeffs[0],chorusCoeffs[1],chorusCoeffs[2],chorusCoeffs[3],chorusCoeffs[4]);
            chFilterR.setCoefficients(chorusCoeffs[0],chorusCoeffs[1],chorusCoeffs[2],chorusCoeffs[3],chorusCoeffs[4]);
            // high self on overall output, allow dB decrease after 300Hz
            FilterCalc::calcCoeffsHighShelf(mainCoeffs, 300.0, darkenParam, fs);
            mainFilterL.setCoefficients(mainCoeffs[0],mainCoeffs[1],mainCoeffs[2],mainCoeffs[3],mainCoeffs[4]);
            mainFilterR.setCoefficients(mainCoeffs[0],mainCoeffs[1],mainCoeffs[2],mainCoeffs[3],mainCoeffs[4]);
            
            // input and output gains using dB
            oGainAParam = pow(10.0,(ogainA/20.0)); //convert user out dB gain to linear gain
            iGainAParam = pow(10.0,(igainA/20.0)); //convert user in dB gain to linear gain        
            
        }
        //linear interpolation
        DspFloatType linInterpolation(DspFloatType start, DspFloatType end, DspFloatType fract) {
            return start + ( fract * (end - start)); //a - fract * ( b - a )
        }

        DspFloatType calcDelaySampsFromMsec (DspFloatType msec) {
            return fs * (msec / 1000);
        }

        DspFloatType softClip(DspFloatType sample)
        {
            if (sample < -1.f) {
                return -softClipThreshold;
            }
            else if (sample > 1.f) {
                return softClipThreshold;
            }
            else {
                return sample - ((sample * sample * sample) / 3.f);
            }
        }

        void ProcessBlock (size_t numSamps, DspFloatType ** inputs, DspFloatType ** outputs)
        {
            Undenormal noDenormals;
            auto totalNumInputChannels  = 2;
            auto totalNumOutputChannels = 2;

            
            calcAlgorithmParams();

            
            DspFloatType delOutL, delOutR;
            DspFloatType tmpL, tmpR;
            DspFloatType tmpChorL, tmpChorR;
            DspFloatType tmp2L, tmp2R;
            DspFloatType* leftChannel;
            DspFloatType* rightChannel;
            DspFloatType fract;
            DspFloatType lfoAmpL, lfoAmpR;

            
            leftChannel = outputs[0];
            rightChannel = outputs[1];
            memcpy(leftChannel,inputs[0],numSamps*sizeof(DspFloatType));
            memcpy(rightChannel,inputs[1],numSamps*sizeof(DspFloatType));
            for (int i = 0; i < numSamps; ++i)
            {
                
                fract = ((DspFloatType) i / (DspFloatType) numSamps);
                
                if (curDelayL != prevDelayL) {
                    delaySampsL  = linInterpolation(prevDelayL, curDelayL, fract);
                    chorusSampsL = linInterpolation(prevChorusL, curChorusL, fract);
                }
                
                if (curDelayR != prevDelayR) {
                    delaySampsR  = linInterpolation(prevDelayR, curDelayR, fract);
                    chorusSampsR = linInterpolation(prevChorusR, curChorusR, fract);
                }
                
                if (curDepth != prevDepth) {
                    depthSamps = linInterpolation(prevDepth, curDepth, fract);
                }
                //CHORUS process
                lfoAmpL = 0.5 * (1 + LFOL.tick());
                lfoAmpR = 0.5 * (1 + LFOR.tick());
                
                tmpChorL = chorusSampsL + (depthSamps * lfoAmpL);
                tmpChorR = chorusSampsR + (depthSamps * lfoAmpR);
                chorusDelayL.setDelay(tmpChorL);
                chorusDelayR.setDelay(tmpChorR);
            
                tmp2L = (chorusDelayL.nextOut() * chorusFbAParam);
                tmp2L = chFilterL.tick(tmp2L);
                tmp2R = (chorusDelayR.nextOut() * chorusFbAParam);
                tmp2R = chFilterR.tick(tmp2R);
            
                //leftChannel[i] *= iGainAParam;
                // leftIn * dry + ((delayed leftIn + right fbck) * wet) * invert
                leftChannel[i] = (leftChannel[i] * chorusDry) +
                                ((chorusDelayL.tick(leftChannel[i] + tmp2R)) * chorusWet);
                //rightChannel[i] *= iGainAParam;
                // rightIn * dry + ((delayed rightIn + left fbck) * wet) * invert
                rightChannel[i] = (rightChannel[i] * chorusDry) +
                                ((chorusDelayR.tick(rightChannel[i] + tmp2L)) * chorusWet);
                
                // DELAY process
                delayL.setDelay(delaySampsL);
                delayR.setDelay(delaySampsR);
                // filtered feedback (shimmer)
                tmpL = lowFilterL.tick((delayL.nextOut() * fbAParam));
                tmpL = filterL.tick(tmpL);
                tmpR = lowFilterR.tick((delayR.nextOut() * fbAParam));
                tmpR = filterL.tick(tmpR);

                delOutL = delayL.tick(leftChannel[i] + tmpR); // dly in + R channel fb            
                leftChannel[i] = (leftChannel[i] * dGain) + (wGain * delOutL); // dry signal + wet signal
            
                delOutR = delayR.tick(rightChannel[i] + tmpL); // dly in + L channel fb
                rightChannel[i] = (rightChannel[i] * dGain) + (wGain * delOutR); // dry signal + wet signal
            
                // prevent signal overload
                if (chorusFbAParam >= 0.85) {
                    leftChannel[i] = softClip(leftChannel[i]);
                    rightChannel[i] = softClip(rightChannel[i]);
                }
                else if (fbAParam >= 0.99) {
                    leftChannel[i] = softClip(leftChannel[i]);
                    rightChannel[i] = softClip(rightChannel[i]);
                }
                else if (chorusFbAParam + fbAParam >= 1.0) {
                    leftChannel[i] = softClip(leftChannel[i]);
                    rightChannel[i] = softClip(rightChannel[i]);
                }
            
                // push entire signal through high shelf (darken)
                leftChannel[i] = mainFilterL.tick(leftChannel[i]);
                rightChannel[i] = mainFilterR.tick(rightChannel[i]);
                // allow dB inc/dec after entire line
                leftChannel[i] *= oGainAParam;
                rightChannel[i] *= oGainAParam;
            }
            // set previous parameters here
            prevDelayL = curDelayL;
            prevDelayR = curDelayR;
            prevDepth = curDepth;
            prevChorusL = curChorusL;
            prevChorusR = curChorusR;
        }
        void InplaceProcess(size_t n, DspFloatType ** buffer)
        {
            ProcessBlock(n,buffer,buffer);
        }
    };
}