// 21khz - https://github.com/jakehuryn/21kHz-rack-plugins/blob/master/src/TachyonEntangler.cpp
// PolygonalOscillator

#pragma once

// PolyBLEP
// PolyBLEP/4 (21khz)

namespace Analog::PolyBLEP4
{
    #include <array>

    using std::array;


    // four point, fourth-order b-spline polyblep, from:
    // Välimäki, Pekonen, Nam. "Perceptually informed synthesis of bandlimited
    // classical waveforms using integrated polynomial interpolation"
    inline void polyblep4(array<DspFloatType, 4> &buffer, DspFloatType d, DspFloatType u) {
        if (d > 1.0f) {
            d = 1.0f;
        }
        else if (d < 0.0f) {
            d = 0.0f;
        }
        
        DspFloatType d2 = d * d;
        DspFloatType d3 = d2 * d;
        DspFloatType d4 = d3 * d;
        DspFloatType dd3 = 0.16667 * (d + d3);
        DspFloatType cd2 = 0.041667 + 0.25 * d2;
        DspFloatType d4_1 = 0.041667 * d4;
        
        buffer[3] += u * (d4_1);
        buffer[2] += u * (cd2 + dd3 - 0.125 * d4);
        buffer[1] += u * (-0.5 + 0.66667 * d - 0.33333 * d3 + 0.125 * d4);
        buffer[0] += u * (-cd2 + dd3 - d4_1);
    }


    // four point, fourth-order b-spline polyblamp, from:
    // Esqueda, Välimäki, Bilbao. "Rounding Corners with BLAMP".
    inline void polyblamp4(array<DspFloatType, 4> &buffer, DspFloatType d, DspFloatType u) {
        if (d > 1.0f) {
            d = 1.0f;
        }
        else if (d < 0.0f) {
            d = 0.0f;
        }
        
        DspFloatType d2 = d * d;
        DspFloatType d3 = d2 * d;
        DspFloatType d4 = d3 * d;
        DspFloatType d5 = d4 * d;
        DspFloatType d5_1 = 0.0083333 * d5;
        DspFloatType d5_2 = 0.025 * d5;
        
        buffer[3] += u * (d5_1);
        buffer[2] += u * (0.0083333 + 0.083333 * (d2 + d3) + 0.041667 * (d + d4) - d5_2);
        buffer[1] += u * (0.23333 - 0.5 * d + 0.33333 * d2 - 0.083333 * d4 + d5_2);
        buffer[0] += u * (0.0083333 + 0.041667 * (d4 - d) + 0.083333 * (d2 - d3) - d5_1);
    }


    // fast sine calculation. modified from the Reaktor 6 core library.
    // takes a [0, 1] range and folds it to a triangle on a [0, 0.5] range.
    inline DspFloatType sin_01(DspFloatType t) {
        if (t > 1.0f) {
            t = 1.0f;
        }
        else if (t > 0.5) {
            t = 1.0f - t;
        }
        else if (t < 0.0f) {
            t = 0.0f;
        }
        t = 2.0f * t - 0.5f;
        DspFloatType t2 = t * t;
        t = (((-0.540347 * t2 + 2.53566) * t2 - 5.16651) * t2 + 3.14159) * t;
        return t;
    }

    /*
    struct PolyBLEP4
    {
        DspFloatType phase,sqr,incr;
        DspFloatType inc;

        array<DspFloatType, 4> sawBufferA;        
        array<DspFloatType, 4> sqrBufferA;        
        array<DspFloatType, 3> oldPhasesA;        
        array<DspFloatType, 3> oldIncrsA;
        DspFloatType oldDecr = 0;

        PolyBLEP4() {
            inc = 440.0f/sampleRate;
            phase = sqr = incr = 0;
        }
        void setFrequency(DspFloatType f) {
            inc = f/sampleRate;
        }

        DspFloatType advancePhase(DspFloatType &phase, DspFloatType &square, DspFloatType incr, DspFloatType rand, int &discont) {
            phase += incr;
            DspFloatType decr = 1.0f;
            if (phase >= 0.0f && phase < 1.0f) {
                discont = 0;
            }
            else if (phase >= 1.0f && incr >= 0.0f) {
                discont = 1;
                decr = 1.0f - 2.0f * rand * (random::uniform() - 0.5f);
                phase -= decr;
                square *= -1.0f;
                if (phase >= 1.0f) {
                    phase = 1.0f;
                }
            }
            else if (phase < 0.0f && incr < 0.0f) {
                discont = -1;
                decr = 1.0f - 2.0f * rand * (random::uniform() - 0.5f);
                phase += decr;
                square *= -1.0f;
                if (phase <= -1.0f) {
                    phase = -1.0f;
                }
            }
            return decr;
        }

        DspFloatType Tick(DspFloatType I=1, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {

            for (int i = 0; i <= 2; ++i) {
                sawBufferA[i] = sawBufferA[i + 1];
                sawBufferB[i] = sawBufferB[i + 1];
                sqrBufferA[i] = sqrBufferA[i + 1];
                sqrBufferB[i] = sqrBufferB[i + 1];
            }
            for (int i = 0; i <= 1; ++i) {
                oldPhasesA[i] = oldPhasesA[i + 1];
                oldPhasesB[i] = oldPhasesB[i + 1];
                oldIncrsA[i] = oldIncrsA[i + 1];
                oldIncrsB[i] = oldIncrsB[i + 1];
            }
            int discontA = 0;            
            DspFloatType decr = advance_phase(phase,sqr,incr,0,discontA);            
            incr += inc;            
            oldPhasesA[3] = phase;
            oldIncrsA[2]  = incr;

            if(discontA) {
                polyblep4(phase[0],1.0f - oldPhasesA[1] / oldIncrsA[1]);
                polyblamp4(sqr[0]);
            }
            oldDecr = decr;
        }
    }
    */
}