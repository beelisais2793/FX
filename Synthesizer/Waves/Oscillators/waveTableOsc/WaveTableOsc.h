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

#ifndef Test_WaveTableOsc_h
#define Test_WaveTableOsc_h

#define doLinearInterp 1
#include <vector>

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
    void setFrequency(double inc);
    void setPhaseOffset(double offset);
    void updatePhase(void);
    float getOutput(void);
    float getOutputMinusOffset(void);
    int addWaveTable(int len, std::vector<float> waveTableIn, double topFreq);
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

#endif
