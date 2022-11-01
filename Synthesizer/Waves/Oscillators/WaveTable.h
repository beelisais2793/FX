//
//  WaveTableOsc.h
//
//  Created by Nigel Redmon on 5/15/12
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the wavetable oscillator and code,
//  read the series of articles by the author, starting here:
//  www.earlevel.com/main/2012/05/03/a-wavetable-oscillator—introduction/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code for your own purposes, free or commercial.
//

#pragma once

#include <cmath>

#define doLinearInterp 1
#include <vector>

namespace SoundWave {
    //TODO: use vectors instead of an array
    typedef struct {
        double topFreq;
        int waveTableLen;
        float *waveTable;
    } waveTable;

    const int numWaveTableSlots = 32;

    class WaveTableOsc {
    protected:
        double phasor;      // phase accumulator
        double phaseInc;    // phase increment
        double phaseOfs;    // phase offset for PWM
        

        // list of wavetables
        int numWaveTables;
        waveTable waveTables[numWaveTableSlots];

    public:
        WaveTableOsc(void);
        ~WaveTableOsc(void);

        void  setFrequency(double inc);
        void  setPhaseOffset(double offset);
        void  updatePhase(void);
        float getOutput(void);
        float getOutputMinusOffset(void);
        int   addWaveTable(int len, std::vector<float> waveTableIn, double topFreq);

        void SetPhase(float f) { phasor = f; }

        double Tick()
        {
            float out = getOutput();
            updatePhase();
            return out;
        }
        double Tick(double fm)
        {
            double temp = phasor;
            phasor += fm;
            while(phasor < 0) phasor += 1.0f;
            while(phasor > 1) phasor -= 1.0f;
            float out = getOutput();
            updatePhase();
            phasor = temp;
            return out;
        }
        void Process(size_t n, float * input, float * output) {
            for(size_t i = 0; i < n; i++) output[i] = input[i]*Tick();
        }
        void Process(float * samples, size_t n) {
            for(size_t i = 0; i < n; i++) samples[i] = Tick();
        }
        void Process(size_t n, float * input, float * fm, float * output) {
            for(size_t i = 0; i < n; i++) output[i] = input[i]*Tick(fm[i]);
        }
        void Process(float * samples, float * fm, size_t n) {
            for(size_t i = 0; i < n; i++) samples[i] = Tick(fm[i]);
        }

    };


    // note: if you don't keep this in the range of 0-1, you'll need to make changes elsewhere
    inline void WaveTableOsc::setFrequency(double inc) {
        phaseInc = inc;
    }

    // note: if you don't keep this in the range of 0-1, you'll need to make changes elsewhere
    inline void WaveTableOsc::setPhaseOffset(double offset) {
        phaseOfs = offset;
    }

    inline void WaveTableOsc::updatePhase() {
        phasor += phaseInc;

        if (phasor >= 1.0)
            phasor -= 1.0;
    }



    inline void MakeSine(std::vector<float> & in, size_t n, float fn, float sr, float phase=0) {
        in.resize(n);
        float dtime = 1/(float)n;
        float t = 0;
        for(size_t i = 0; i < n; i++)
        {
            in[i] = std::sin(2.0*M_PI*fn*t + phase);
            t += dtime;
        }
    }



    inline void MakeSaw(std::vector<float> & in, size_t n, float fn, float sr, float phase=0) {
        in.resize(n);
        size_t max_harmonics = (sr/2)/fn;
        //float dtime = (fn/sr)/n;
        float dtime = 1.0 /(float)n;
        float t = 0;
        for(size_t i = 0; i < n; i++)
        {
            in[i] = 0.0;
            for(size_t k = 1; k < max_harmonics; k++) {                
                float x = std::sin(2*M_PI*fn*k*t+phase)/(float)k;
                in[i] += x;
            }
            t += dtime;
            in[i] = 0.5 - (1.0/M_PI)*in[i];
        }
    }

    inline void MakeReverseSaw(std::vector<float> & in, size_t n, float fn, float sr, float phase=0) {
        in.resize(n);
        size_t max_harmonics = (sr/2)/fn;
        ///float dtime = (fn/sr)/n;
        float dtime = 1.0 /(float)n;
        float t = 0;
        for(size_t i = 1; i < n; i++)
        {
            in[i] = 0.0;
            for(size_t k = 1; k < max_harmonics; k++) {                
                float x = std::sin(2*M_PI*fn*k*t + phase)/k;
                in[i] += x;
            }
            t += dtime;
            in[i] = (2.0/M_PI)*in[i];
        }
    }

    inline void MakeSquare(std::vector<float> & in, size_t n, float fn, float sr, float phase=0) {
        in.resize(n);
        size_t max_harmonics = (sr/2)/fn;
        //float dtime = (fn/sr)/n;
        float dtime = 1.0 /(float)n;
        float t = 0;
        for(size_t i = 0; i < n; i++)
        {
            in[i] = 0.0;
            for(size_t k = 1; k < max_harmonics; k++) {
                float blip = 2*k-1;
                float x = std::sin(2*M_PI*blip*fn*t + phase)/blip;
                in[i] += x;
            }
            t += dtime;
            in[i] = (4.0/M_PI)*in[i];
        }
    }

    inline void MakeTriangle(std::vector<float> & in, size_t n, float fn, float sr, float phase=0.0) {
        in.resize(n);
        size_t max_harmonics = (sr/2)/fn;
        //float dtime = (fn/sr)/n;
        float dtime = 1.0 /(float)n;
        float t = 0;
        for(size_t i = 0; i < n; i++)
        {

            in[i] = 0.0;
            for(size_t k = 0; k < max_harmonics-1; k++) {
                float mode = 2*k+1;
                float x = std::pow(-1.0,k)*std::pow(mode,-2.0)*(std::sin(2*M_PI*fn*mode*t+phase));
                in[i] += x;
            }
            t += dtime;
            in[i] = (8.0/(M_PI*M_PI))*in[i];
        }
    }


}
