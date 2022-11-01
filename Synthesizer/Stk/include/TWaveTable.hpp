//
//  TWaveTable.h
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

#define doLinearInterp 1
#include <vector>
#include <cmath>
#include "SoundAlchemy.hpp"

namespace SoundAlchemy::WaveTable {
        
    //TODO: use vectors instead of an array
    template<typename T>
    struct waveTable {
        T       topFreq;
        int     waveTableLen;
        T *     waveTable;
    };

    const int numWaveTableSlots = 32;

    template<typename T>
    class TWaveTable {
    protected:
        T phasor;      // phase accumulator
        T phaseInc;    // phase increment
        T phaseOfs;    // phase offset for PWM
        
        // list of wavetables
        int numWaveTables;
        waveTable<T> waveTables[numWaveTableSlots];

    
    public:
        TWaveTable(void);
        ~TWaveTable(void);
        void setFrequency(T inc);
        void setPhaseOffset(T offset);
        void updatePhase(void);
        T getOutput(void);
        T getOutputMinusOffset(void);
        int addWaveTable(int len, std::vector<T> waveTableIn, T topFreq);

        T Tick()
        {
            updatePhase();
            return getOutput();
        }   
        void Process(size_t n, T * input, T * output) {
            for(size_t i = 0; i < n; i++) output[i] = input[i]*Tick();
        }
        void Process(T * samples, size_t n) {
            for(size_t i = 0; i < n; i++) samples[i] = Tick();
        }

    };


    template<typename T>
    void MakeSine(std::vector<T> & in, size_t n, T fn, T sr, T phase=0) {
        in.resize(n);
        T dtime = (fn/sr)/n;
        T t = 0;
        for(size_t i = 0; i < n; i++) 
        {
            in[i] = std::sin(2.0*M_PI*fn*t + phase);
            t += dtime;
        }
    }

    template<typename T>
    void MakeSaw(std::vector<T> & in, size_t n, T fn, T sr, T phase=0) {
        in.resize(n);
        int max_harmonics = (sr/2)/fn;                
        T dtime = (fn/sr)/n;
        T t = 0;
        for(size_t i = 0; i < n; i++)
        {
            in[i] = 0.0;
            for(size_t k = 1; k < max_harmonics; k++) {
                T n = std::pow(-1.0,k);
                T x = std::sin(2*M_PI*fn*k*t+phase)/(T)k;                
                in[i] += n*x;
            }
            t += dtime;
            in[i] = 0.5 - (1.0/M_PI)*in[i];
        }
    }
    template<typename T>
    void MakeReverseSaw(std::vector<T> & in, size_t n, T fn, T sr, T phase=0) {
        in.resize(n);
        int max_harmonics = (sr/2)/fn;        
        T dtime = (fn/sr)/n;
        T t = 0;
        for(size_t i = 1; i < n; i++)
        {
            in[i] = 0.0;
            for(size_t k = 0; k < max_harmonics; k++) {
                T n = std::pow(-1.0,k);
                T x = std::sin(2*M_PI*fn*k*t + phase)/k;                
                in[i] += n*x;
            }
            t += dtime;
            in[i] = (2.0/M_PI)*in[i];
        }
    }
    template<typename T>
    void MakeSquare(std::vector<T> & in, size_t n, T fn, T sr, T phase=0) {
        in.resize(n);
        int max_harmonics = (sr/2)/fn;        
        T dtime = (fn/sr)/n;
        T t = 0;
        for(size_t i = 0; i < n; i++)
        {
            in[i] = 0.0;
            for(size_t k = 1; k < max_harmonics; k++) {                
                T blip = 2*k-1;
                T x = std::sin(2*M_PI*blip*fn*t + phase)/blip;                
                in[i] += x;
            }
            t += dtime;
            in[i] = (4.0/M_PI)*in[i];
        }
    }
    template<typename T>
    void MakeTriangle(std::vector<T> & in, size_t n, T fn, T sr, T phase=0.0) {
        in.resize(n);
        int max_harmonics = (sr/2)/fn;        
        T dtime = (fn/sr)/n;
        T t = 0;
        for(size_t i = 0; i < n; i++)
        {
            
            in[i] = 0.0;
            for(size_t k = 0; k < max_harmonics-1; k++) {                                             
                T mode = 2*k+1;   
                T x = std::pow(-1.0,k)*std::pow(mode,-2.0)*(std::sin(2*M_PI*fn*mode*t+phase));                
                in[i] += x;
            }
            t += dtime;
            in[i] = (8.0/(M_PI*M_PI))*in[i];
        }
    }
    // note: if you don't keep this in the range of 0-1, you'll need to make changes elsewhere
    template<typename T>
    inline void TWaveTable<T>::setFrequency(T inc) {
        phaseInc = inc;
    }

    // note: if you don't keep this in the range of 0-1, you'll need to make changes elsewhere
    template<typename T>
    inline void TWaveTable<T>::setPhaseOffset(T offset) {
        phaseOfs = offset;
    }

    template<typename T>
    inline void TWaveTable<T>::updatePhase() {
        phasor += phaseInc;
        
        if (phasor >= 1.0)
            phasor -= 1.0;
    }

    template<typename T>
    TWaveTable<T>::TWaveTable(void) {
        
        phasor = 0.0;		// phase accumulator
        phaseInc = 0.0;		// phase increment
        phaseOfs = 0.5;		// phase offset for PWM
        numWaveTables = 0;	//why is this 0?

        //initialize the array of waveTable structures (which could be replaced by vectors...)
        for (int idx = 0; idx < numWaveTableSlots; idx++) {
            waveTables[idx].topFreq = 0;
            waveTables[idx].waveTableLen = 0;
            waveTables[idx].waveTable = 0;
        }
    }


    template<typename T>
    TWaveTable<T>::~TWaveTable(void) {
        for (int idx = 0; idx < numWaveTableSlots; idx++) {
            T *temp = waveTables[idx].waveTable;
            if (temp != 0)
                delete [] temp;
        }
    }


    //
    // addWaveTable
    //
    // add wavetables in order of lowest frequency to highest
    // topFreq is the highest frequency supported by a wavetable
    // wavetables within an oscillator can be different lengths
    //
    // returns 0 upon success, or the number of wavetables if no more room is available
    //
    template<typename T>
    int TWaveTable<T>::addWaveTable(int len, std::vector<T> waveTableIn, T topFreq) {
        if (numWaveTables < numWaveTableSlots) {
            T *waveTable = waveTables[numWaveTables].waveTable = new T[len];
            waveTables[numWaveTables].waveTableLen = len;
            waveTables[numWaveTables].topFreq = topFreq;
            ++numWaveTables;
            
            // fill in wave
            for (long idx = 0; idx < len; idx++)
                waveTable[idx] = waveTableIn[idx];
            
            return 0;
        }
        return numWaveTables;
    }


    //
    // getOutput
    //
    // returns the current oscillator output
    //
    template<typename T>
    T TWaveTable<T>::getOutput() {
        // grab the appropriate wavetable
        int waveTableIdx = 0;
        //TODO: why is phaseInc compared to the topFreq?
        while ((phaseInc >= waveTables[waveTableIdx].topFreq) && (waveTableIdx < (numWaveTables - 1))) {
            ++waveTableIdx;
        }
        waveTable<T> *waveTable = &waveTables[waveTableIdx];

    #if !doLinearInterp
        // truncate
        return waveTable->waveTable[int(phasor * waveTable->waveTableLen)];
    #else
        // linear interpolation
        T temp = phasor * waveTable->waveTableLen;
        int intPart = temp;
        T fracPart = temp - intPart;
        T samp0 = waveTable->waveTable[intPart];
        if (++intPart >= waveTable->waveTableLen)
            intPart = 0;
        T samp1 = waveTable->waveTable[intPart];
        
        return samp0 + (samp1 - samp0) * fracPart;
    #endif
    }


    //
    // getOutputMinusOffset
    //
    // for variable pulse width: initialize to sawtooth,
    // set phaseOfs to duty cycle, use this for osc output
    //
    // returns the current oscillator output
    //
    template<typename T>
    T TWaveTable<T>::getOutputMinusOffset() {
        // grab the appropriate wavetable
        int waveTableIdx = 0;
        while ((this->phaseInc >= this->waveTables[waveTableIdx].topFreq) && (waveTableIdx < (this->numWaveTables - 1))) {
            ++waveTableIdx;
        }
        waveTable<T> *waveTable = &this->waveTables[waveTableIdx];
        
    #if !doLinearInterp
        // truncate
        T offsetPhasor = this->phasor + this->phaseOfs;
        if (offsetPhasor >= 1.0)
            offsetPhasor -= 1.0;
        return waveTable->waveTable[int(this->phasor * waveTable->waveTableLen)] - waveTable->waveTable[int(offsetPhasor * waveTable->waveTableLen)];
    #else
        // linear
        T temp = this->phasor * waveTable->waveTableLen;
        int intPart = temp;
        T fracPart = temp - intPart;
        T samp0 = waveTable->waveTable[intPart];
        if (++intPart >= waveTable->waveTableLen)
            intPart = 0;
        T samp1 = waveTable->waveTable[intPart];
        T samp = samp0 + (samp1 - samp0) * fracPart;
        
        // and linear again for the offset part
        T offsetPhasor = this->phasor + this->phaseOfs;
        if (offsetPhasor > 1.0)
            offsetPhasor -= 1.0;
        temp = offsetPhasor * waveTable->waveTableLen;
        intPart = temp;
        fracPart = temp - intPart;
        samp0 = waveTable->waveTable[intPart];
        if (++intPart >= waveTable->waveTableLen)
            intPart = 0;
        samp1 = waveTable->waveTable[intPart];
        
        return samp - (samp0 + (samp1 - samp0) * fracPart);
    #endif
    }

    template<typename T>
    struct LFO
    {
        TWaveTable<T> sine;       
        TWaveTable<T> saw;       
        TWaveTable<T> rsaw;       
        TWaveTable<T> square;       
        TWaveTable<T> triangle;
        T f;
        T sr;

        enum LfoType {
            SINE,
            SAW,
            REVERSE_SAW,
            SQUARE,
            TRIANGLE,
        } type;

        LFO(LfoType type=SINE,size_t n=256,T sr=44100, T freq = 1.0)
        {
            f = freq;
            this->sr = sr;
            this->type = type;
            std::vector<T> v(n);
            MakeSine(256,1,sr,v);
            sine.addWaveTable(256,v,sr/2);
            MakeSaw(256,1,sr,v);
            saw.addWaveTable(256,v,sr/2);
            MakeReverseSaw(256,1,sr,v);
            rsaw.addWaveTable(256,v,sr/2);
            MakeSquare(256,1,sr,v);
            square.addWaveTable(256,v,sr/2);       
            MakeTriangle(256,1,sr,v);
            triangle.addWaveTable(256,v,sr/2);
            sine.setFrequency(freq);
            saw.setFrequency(freq);
            rsaw.setFrequency(freq);
            square.setFrequency(freq);
            triangle.setFrequency(freq);
         }

         T tick() {             
             sine.updatePhase();             
             saw.updatePhase();
             square.updatePhase();
             rsaw.updatePhase();
             square.updatePhase();
             triangle.updatePhase();

             switch(type)
             {
                case SINE: return sine.getOutput();
                case SAW:  return saw.getOutput();
                case SQUARE: return square.getOutput();             
                case REVERSE_SAW: return rsaw.getOutput();
                case TRIANGLE: return triangle.getOutput();
             }
         }

         T get_sin() { return sine.getOutput(); }
         T get_saw() { return saw.getOutput(); }
         T get_rsaw() { return rsaw.getOutput(); }
         T get_square() { return square.getOutput(); }
         T get_triangle() { return triangle.getOutput(); }
    };
}