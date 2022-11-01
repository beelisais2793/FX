#pragma once

#define doLinearInterp 1


#include <cmath>
#include <vector>
#include <algorithm>

//TODO: use vectors instead of an array
typedef struct {
    double topFreq;
    int waveTableLen;
    double *waveTable;
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
    double getOutput(void);
    double getOutputMinusOffset(void);
    int   addWaveTable(int len, std::vector<double> waveTableIn, double topFreq);

    void SetPhase(double f) { phasor = f; }

    double Tick()
    {
        double out = getOutput();
        updatePhase();
        return out;
    }
    double Tick(double fm)
    {
        double temp = phasor;
        phasor += fm;
        while(phasor < 0) phasor += 1.0f;
        while(phasor > 1) phasor -= 1.0f;
        double out = getOutput();
        updatePhase();
        phasor = temp;
        return out;
    }
    void Process(size_t n, double * input, double * output) {
        for(size_t i = 0; i < n; i++) output[i] = input[i]*Tick();
    }
    void Process(double * samples, size_t n) {
        for(size_t i = 0; i < n; i++) samples[i] = Tick();
    }
    void Process(size_t n, double * input, double * fm, double * output) {
        for(size_t i = 0; i < n; i++) output[i] = input[i]*Tick(fm[i]);
    }
    void Process(double * samples, double * fm, size_t n) {
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


// added by macho charlie
inline void MakeSineTable(std::vector<double> & in, size_t n, double fn, double sr) {
    in.resize(n);        
    double inc   = 1.0f/n;
    for(size_t i = 0; i < n; i++)
    {
        in[i] = std::sin(2.0*M_PI*(fn/sr+(double)i*inc));
        
    }        
}


inline void MakeSawTable(std::vector<double> & in, size_t n, double fn, double sr) {
    in.resize(n);
    size_t max_harmonics = (sr/2)/fn;
    if(max_harmonics <= 1 ) {            
        MakeSineTable(in,n,fn,sr);
        return;
    }
    double inc   = 1.0f/(double)n;        
    for(size_t i = 0; i < n; i++)
    {                   
        double t= i*inc;
        in[i] = 0.0;
        for(size_t k = 1; k < max_harmonics; k++) {                       
            double x = std::pow(-1,k)*std::sin(2*M_PI*k*t)/k;
            in[i] += x;
        }                       
    }
    //adjust_table(in);
}

inline void MakeReverseSawTable(std::vector<double> & in, size_t n, double fn, double sr) {
    in.resize(n);
    size_t max_harmonics = (sr/2)/fn;        
    if(max_harmonics <= 1 ) {            
        MakeSineTable(in,n,fn,sr);
        return;
    }
    double inc   = 1.0f/(double)n;        
    for(size_t i = 0; i < n; i++)
    {                   
        double t= i*inc;
        in[i] = 0.0;
        for(size_t k = 1; k < max_harmonics; k++) {                       
            double x = std::pow(-1,k)*std::sin(2*M_PI*k*t)/k;
            in[i] += x;
        }                                   
    }        
    std::reverse(in.begin(),in.end());
}

inline void MakeSquareTable(std::vector<double> & in, size_t n, double fn, double sr) {
    in.resize(n);                
    size_t max_harmonics = (sr/2)/fn;                        
    if(max_harmonics <= 1 ) {            
        MakeSineTable(in,n,fn,sr);
        return;
    }
    double inc   = 1.0f/(double)n;        
    for(size_t i = 0; i < n; i++)
    {                   
        double t= i*inc;
        in[i] = 0.0;
        for(size_t k = 1; k < max_harmonics; k += 2) {                                                       
            double x = std::sin(2*M_PI*k*t)/k;
            in[i] += x;
        }                                           
        in[i] = 4/M_PI*in[i];
        
    }
    //adjust_table(in);        
}

inline void MakePulseTable(std::vector<double> & in, size_t n, double fn, double sr, double d) {
    in.resize(n);
    size_t max_harmonics = (sr/2)/fn;
    if(max_harmonics <= 1 ) {            
        MakeSineTable(in,n,fn,sr);
        return;
    }
    double inc   = 1.0f/(double)n;        
    for(size_t i = 0; i < n; i++)
    {                   
        double t= i*inc;
        in[i] = 0.0;
        for(size_t k = 1; k < max_harmonics; k++) {                       
            double x = (std::sin(M_PI*k*d)/(M_PI*k*d))*std::cos(2*M_PI*k*t);
            in[i] += x;
        }            
        in[i] = d*(1 + 2*in[i]);
    }
    //adjust_table(in);
}
inline void MakeTriangleTable(std::vector<double> & in, size_t n, double fn, double sr) {
    in.resize(n);        
    size_t max_harmonics = (sr/2)/fn;                        
    if(max_harmonics <= 1 ) {            
        MakeSineTable(in,n,fn,sr);
        return;
    }
    double inc   = 1.0f/(double)n;        
    for(size_t i = 0; i < n; i++)
    {                   
        double t= i*inc;
        in[i] = 0.0;
        for(size_t k = 0; k < max_harmonics; k++) {                                                                                       
            double n = 2*k+1;
            double x = std::sin(2*M_PI*n*t)*std::pow(-1,k)/(n*n);
            in[i] += x;
        }                                           
        in[i] = (8.0/(M_PI*M_PI))*in[i];
        
    }        
}


inline WaveTableOsc::WaveTableOsc(void) {
	
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


inline WaveTableOsc::~WaveTableOsc(void) {
    for (int idx = 0; idx < numWaveTableSlots; idx++) {
        double *temp = waveTables[idx].waveTable;
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
inline int WaveTableOsc::addWaveTable(int len, std::vector<double> waveTableIn, double topFreq) {
    if (numWaveTables < numWaveTableSlots) {
        double *waveTable = waveTables[numWaveTables].waveTable = new double[len];
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
inline double WaveTableOsc::getOutput() {
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
    double samp0 = waveTable->waveTable[intPart];
    if (++intPart >= waveTable->waveTableLen)
        intPart = 0;
    double samp1 = waveTable->waveTable[intPart];
    
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
inline double WaveTableOsc::getOutputMinusOffset() {
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
    double samp0 = waveTable->waveTable[intPart];
    if (++intPart >= waveTable->waveTableLen)
        intPart = 0;
    double samp1 = waveTable->waveTable[intPart];
    double samp = samp0 + (samp1 - samp0) * fracPart;
    
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

