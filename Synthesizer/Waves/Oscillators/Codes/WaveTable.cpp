//
//  WaveTableOsc.cpp
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

#include "WaveTable.h"

using namespace SoundWave;

WaveTableOsc::WaveTableOsc(void) {
	
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


WaveTableOsc::~WaveTableOsc(void) {
    for (int idx = 0; idx < numWaveTableSlots; idx++) {
        float *temp = waveTables[idx].waveTable;
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
int WaveTableOsc::addWaveTable(int len, std::vector<float> waveTableIn, double topFreq) {
    if (numWaveTables < numWaveTableSlots) {
        float *waveTable = waveTables[numWaveTables].waveTable = new float[len];
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
float WaveTableOsc::getOutput() {
    // grab the appropriate wavetable
    int waveTableIdx = 0;
	//TODO: why is phaseInc compared to the topFreq?
    while ((phaseInc >= waveTables[waveTableIdx].topFreq) && (waveTableIdx < (numWaveTables - 1))) {
        ++waveTableIdx;
    }
    waveTable *waveTable = &waveTables[waveTableIdx];

#if !doLinearInterp
    // truncate
    return waveTable->waveTable[int(phasor * waveTable->waveTableLen)];
#else
    // linear interpolation
    double temp = phasor * waveTable->waveTableLen;
    int intPart = temp;
    double fracPart = temp - intPart;
    float samp0 = waveTable->waveTable[intPart];
    if (++intPart >= waveTable->waveTableLen)
        intPart = 0;
    float samp1 = waveTable->waveTable[intPart];
    
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
float WaveTableOsc::getOutputMinusOffset() {
    // grab the appropriate wavetable
    int waveTableIdx = 0;
    while ((this->phaseInc >= this->waveTables[waveTableIdx].topFreq) && (waveTableIdx < (this->numWaveTables - 1))) {
        ++waveTableIdx;
    }
    waveTable *waveTable = &this->waveTables[waveTableIdx];
    
#if !doLinearInterp
    // truncate
    double offsetPhasor = this->phasor + this->phaseOfs;
    if (offsetPhasor >= 1.0)
        offsetPhasor -= 1.0;
    return waveTable->waveTable[int(this->phasor * waveTable->waveTableLen)] - waveTable->waveTable[int(offsetPhasor * waveTable->waveTableLen)];
#else
    // linear
    double temp = this->phasor * waveTable->waveTableLen;
    int intPart = temp;
    double fracPart = temp - intPart;
    float samp0 = waveTable->waveTable[intPart];
    if (++intPart >= waveTable->waveTableLen)
        intPart = 0;
    float samp1 = waveTable->waveTable[intPart];
    float samp = samp0 + (samp1 - samp0) * fracPart;
    
    // and linear again for the offset part
    double offsetPhasor = this->phasor + this->phaseOfs;
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

