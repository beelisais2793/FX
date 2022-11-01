//
//  Psola.h
//  PitchTune
//
//  Created by Xinyuan Lai on 12/2/13.
//
//

#pragma once
#include <vector>


class Psola
{
public:
    Psola(float* auIn, long auLength, double sampleRate);
    Psola(float* auIn, long auLength, double sampleRate, int blockSize, int hopSize);
    ~Psola();
    void halfAutoCorr(int strtPtr);
    int getZeroCrsIndex();
    void medianFilter(int* array,int arrayLen, int filterLen);
    void medianFilter(float* array,int arrayLen, int filterLen);
    void getPitches();
    void freq2MidiCents();
    void getPitchMarks();
    void analyse();
    std::vector<int>& getPitchBorders();
    std::vector<int>& getMidiNums();
    std::vector<int>& getCentDiviations();
    void hannGen(int winLen);
    float* resynthesise(std::vector<int> pitchShifts);
    
    float* audioOut;
private:
    float* audioIn;
    long nSamples;
    long nBlocks;
    static double SAMPLE_RATE;
    static int BLOCK_SIZE;
    static int HOP_SIZE;
    static float FREQUENCY_UPPER_LIMIT;
    static int MEDIAN_FILTER_LENGTH;
    float* acf;
    float* f0;
    int* p0;
    int* midi;
    int* cents;
    float* blockRMS;
    std::vector<int> pitchMarks;
    std::vector<int> intrMarkIntvls;
    std::vector<int> pitchBorders;
    std::vector<int> midiNums;
    std::vector<int> centDeviations;
    float* hanning;
    
};

//
//  Psola.cpp
//  PitchTune
//
//  Created by Xinyuan Lai on 12/2/13.
//
//

#include "Psola.h"
#include <cmath>
#include <iostream>

double  Psola::SAMPLE_RATE = 44100.0;
int     Psola::BLOCK_SIZE = 2048;
int     Psola::HOP_SIZE = 1024;
float   Psola::FREQUENCY_UPPER_LIMIT = 300;
int     Psola::MEDIAN_FILTER_LENGTH = 6;

Psola::Psola(float* auIn, long auLength, double sampleRate)
{
    nSamples = auLength;
    audioIn = new float[auLength];
    for (long i = 0; i<auLength; i++) {
        audioIn[i] = auIn[i];
    }
    audioOut = new float[auLength];
    for (long i = 0; i<auLength; i++) {
        audioOut[i] = 0;
    }
    SAMPLE_RATE = sampleRate;
    
    nBlocks = floor( (nSamples-BLOCK_SIZE)/HOP_SIZE ) + 1;
    
    acf      = new float[BLOCK_SIZE];
    f0       = new float[nBlocks];
    p0       = new int[nBlocks];
    midi     = new int[nBlocks];
    cents    = new int[nBlocks];
    blockRMS = new float[nBlocks];
    hanning  = new float[BLOCK_SIZE];
    
}


Psola::Psola(float* auIn, long auLength, double sampleRate, int blockSize, int hopSize)
{
    nSamples = auLength;
    audioIn = new float[auLength];
    for (long i =0; i<auLength; i++) {
        audioIn[i] = auIn[i];
    }
    audioOut = new float[auLength];
    for (long i = 0; i<auLength; i++) {
        audioOut[i] = 0;
    }
    SAMPLE_RATE = sampleRate;
    
    BLOCK_SIZE = blockSize;
    HOP_SIZE   = hopSize;
    nBlocks = floor( (nSamples-BLOCK_SIZE)/HOP_SIZE ) + 1;
    
    acf      = new float[BLOCK_SIZE];
    f0       = new float[nBlocks];
    p0       = new int[nBlocks];
    midi     = new int[nBlocks];
    cents    = new int[nBlocks];
    blockRMS = new float[nBlocks];
    hanning  = new float[BLOCK_SIZE];
    
}

inline
Psola::~Psola()
{

    if (acf) {
        delete [] acf;
    }
    if (f0) {
        delete [] f0;
    }
    if (p0) {
        delete [] p0;
    }
    if (blockRMS) {
        delete [] blockRMS;
    }
    if (hanning) {
        delete [] hanning;
    }
    if (audioIn) {
        delete [] audioIn;
    }
    if (audioOut) {
        delete [] audioOut;
    }
}

inline
void Psola::halfAutoCorr(int strtPtr)
{
    for (int i=0; i<BLOCK_SIZE; i++) {
        acf[i] = 0;
    }
    
    for (int i=0; i<BLOCK_SIZE; i++) {
        for (int j = 0; j<BLOCK_SIZE - i; j++) {
            acf[i] += audioIn[strtPtr+j]*audioIn[strtPtr+i+j];
        }
    }
    
}


inline
int Psola::getZeroCrsIndex()
{
    int zeroIndex = 0;
    
    for (int i = 0; i<BLOCK_SIZE-1; i++) {
        if (acf[i] == 0) {
            zeroIndex = i;
            break;
        }
        else if (acf[i]*acf[i+1]<0) {
            zeroIndex = i;
            break;
        }
    }
    
    return zeroIndex;
}


inline
void Psola::medianFilter(int* array,int arrayLen, int filterLen)
{
    int* filterWin = new int[filterLen];
    
    for (int n =0; n<filterLen/2; n++) {
        array[n] = 0;
    }
    
    for (int n=filterLen/2; n<arrayLen-filterLen/2; n++) {
        
        //median filter window
        for (int i=0; i<filterLen; i++) {
            filterWin[i] = array[n-filterLen/2+i];
        }
        
        //sort filterWin
        for(int i = 0; i < filterLen; i++)
        {
            for(int j = 0; j < filterLen - i - 1; j++)
            {
                if(filterWin[j] > filterWin[j + 1]) {
                    int temp = filterWin[j];
                    filterWin[j] = filterWin[j + 1];
                    filterWin[j + 1] = temp;
                }
            }
        }
        
        //set median as filtered value
        if (filterLen%2 == 1) {
            array[n] = filterWin[filterLen/2];
        }
        else {
            array[n] = (filterWin[filterLen/2-1]+filterWin[filterLen/2]) / 2.0;
        }
    }
    
    delete [] filterWin;
}


inline
void Psola::medianFilter(float* array,int arrayLen, int filterLen)
{
    float* filterWin = new float[filterLen];
    
    for (int n =0; n<filterLen/2; n++) {
        array[n] = 0;
    }
    
    for (int n=filterLen/2; n<arrayLen-filterLen/2; n++) {
        
        //median filter window
        for (int i=0; i<filterLen; i++) {
            filterWin[i] = array[n-filterLen/2+i];
        }
        
        //sort filterWin
        for(int i = 0; i < filterLen; i++)
        {
            for(int j = 0; j < filterLen - i - 1; j++)
            {
                if(filterWin[j] > filterWin[j + 1]) {
                    int temp = filterWin[j];
                    filterWin[j] = filterWin[j + 1];
                    filterWin[j + 1] = temp;
                }
            }
        }
        
        //set median as filtered value
        if (filterLen%2 == 1) {
            array[n] = filterWin[filterLen/2];
        }
        else {
            array[n] = (filterWin[filterLen/2-1]+filterWin[filterLen/2]) / 2.0;
        }
    }
    
    delete [] filterWin;
}


inline
void Psola::getPitches()
{
    int minLag = (int)floor(SAMPLE_RATE/FREQUENCY_UPPER_LIMIT);
    float rmsMax = 0;
    
    for (int n = 0; n<nBlocks; n++) {
        
        //compute RMS for each block
        blockRMS[n] = 0;
        for (int i = 0; i<BLOCK_SIZE; i++) {
            blockRMS[n] += audioIn[n*HOP_SIZE+i]*audioIn[n*HOP_SIZE+i];
        }
        blockRMS[n] = sqrt(blockRMS[n]/BLOCK_SIZE);
        if (rmsMax<blockRMS[n]) {
            rmsMax=blockRMS[n];
        }
        
        //compute autocorrelation
        halfAutoCorr(n*HOP_SIZE);
        
        int zeroIndex = getZeroCrsIndex();
        
        //Pick the larger value between minimum lag and zero crossing
        int initLag = (minLag>zeroIndex)?minLag:zeroIndex;
        
        //find index of acf maximum after initial lag
        int maxInd = initLag;
        for (int i = initLag; i<BLOCK_SIZE; i++) {
            if (acf[i]>acf[maxInd]) {
                maxInd = i;
            }
        }
        
        //get the fundamental freq from max acf
        f0[n] = SAMPLE_RATE/(float)maxInd;
    }
    
    medianFilter(f0, nBlocks, MEDIAN_FILTER_LENGTH);
    
    for (int n = 0; n<nBlocks; n++) {
        if (blockRMS[n]<0.05*rmsMax) {
            f0[n] = 0;
        }
    }
    
}


inline
void Psola::freq2MidiCents()
{
    for (int n=0; n<nBlocks; n++) {
        if (f0[n]!=0) {
            midi[n] = 69 + round( 12.0*log2(f0[n]/440) );
            float midiFreq = 440*pow(2.0, ((float)midi[n]-69.0)/12.0 );
            cents[n] = 1200*log2(f0[n]/midiFreq);
        }
        else {
            midi[n]  = 0;
            cents[n] = 0;
        }

    }
    
    medianFilter(midi, nBlocks, 7);
    
    for (int n=1; n<nBlocks; n++) {
        if (midi[n]!=midi[n-1]) {
            pitchBorders.push_back(n*HOP_SIZE);
            midiNums.push_back(midi[n-1]);
            centDeviations.push_back(cents[n-1]);
        }
    }
    pitchBorders.push_back(nBlocks*HOP_SIZE+BLOCK_SIZE-1);
    midiNums.push_back(midi[nBlocks]);
    centDeviations.push_back(cents[nBlocks]);
}


inline
void Psola::getPitchMarks()
{
    int localMark = 0;
    for (int n=0; n<nBlocks; n++) {
        
        //fill in 0 frequencies
        if (f0[n] == 0) {
            if (n == 0) {
                f0[n] = 150;
            }
            else {
                f0[n] = f0[n-1];
            }
        }
        
        //get period
        p0[n] = round( SAMPLE_RATE/f0[n]);
        
        //find first pitch mark
        if (n == 0) {
            for (int i=0; i < p0[n]; i++) {
                if ( std::abs(audioIn[localMark]) < std::abs(audioIn[i])) {
                    localMark = i;
                }
            }
            pitchMarks.push_back(localMark);
        }
        
        //append new pitch marks after previous pitch mark based on period
        while (localMark + p0[n] <= BLOCK_SIZE + n*HOP_SIZE) {
            localMark += p0[n];
            pitchMarks.push_back(localMark);
            intrMarkIntvls.push_back(p0[n]);
            std::cout<<"pitch mark:"<<localMark<<std::endl;
        }
        
    }
}


inline
void Psola::analyse()
{
    getPitches();
    getPitchMarks();
    freq2MidiCents();
}


inline
std::vector<int>& Psola::getPitchBorders()
{
    return pitchBorders;
}


inline
std::vector<int>& Psola::getMidiNums()
{
    return midiNums;
}

inline
std::vector<int>& Psola::getCentDiviations()
{
    return centDeviations;
}


inline
void Psola::hannGen(int winLen)
{
    for (int i=0; i<winLen; i++) {
        hanning[i] = 0.5 * (1-cos( 2*M_PI*i/(winLen-1) ));
    }
}


inline
float* Psola::resynthesise(std::vector<int> pitchShifts)
{
    memset(audioOut, 0, sizeof(float) * nSamples);
    
    int nthSegment = 0;
    float shiftRatio = pow(2.0, pitchShifts.front()/1200.0);
    
    if (pitchMarks.front()<intrMarkIntvls.front()) {
        pitchMarks.erase(pitchMarks.begin());
    }
    
    if (pitchMarks.back()+ intrMarkIntvls.back() > nSamples ) {
        pitchMarks.pop_back();
    }
    else {
        intrMarkIntvls.push_back(intrMarkIntvls.back());
    }
    
    int outPtr = intrMarkIntvls.front();
    int selectedMark = 0;
    
    while (outPtr<nSamples) {
        
        //update pitch shifting ratio when coming to a new segment
        while ( outPtr >= pitchBorders.at(nthSegment) ) {
            shiftRatio = pow(2.0, pitchShifts.at(++nthSegment)/1200.0);
        }
        
        //find nreaest pitch mark to use
        for (int i = 0; i<pitchMarks.size(); i++) {
            if ( abs(pitchMarks.at(selectedMark)-outPtr) > abs(pitchMarks.at(i)-outPtr) ) {
                selectedMark = i;
            }
        }
        
        //add a grain from input to output
        hannGen(2*intrMarkIntvls.at(selectedMark)+1);
//        float ttt = audioOut[0];
        for (int i = -1*intrMarkIntvls.at(selectedMark); i<=intrMarkIntvls.at(selectedMark); i++) {
            int indexAudioIn = pitchMarks.at(selectedMark)+i;
            int indexHanning = intrMarkIntvls.at(selectedMark)+i;
            float audioInSample = audioIn[indexAudioIn];
            float hanningSample = hanning[indexHanning];
            int outIndex = outPtr + i;
            audioOut[outIndex] += audioInSample * hanningSample;
//            audioOut[outPtr+i] += audioIn[pitchMarks.at(selectedMark)+i] * hanning[intrMarkIntvls.at(selectedMark)+i];
        }
        
        //update output pointer position
        outPtr += (int)round(intrMarkIntvls.at(selectedMark)/shiftRatio);
        if (outPtr > 54000) {
            std::cout << "out pointer: "<<outPtr <<"    outPtr-nSamples: "<< outPtr-nSamples <<std::endl;
        }
    }
    
    return audioOut;
}
