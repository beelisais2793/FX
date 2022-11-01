/*
  ==============================================================================

    Moorer.h
    Created: 15 Oct 2014 9:10:31pm
    Author:  Keith Hearne
    
    The Moorer Reverberator as proposed by James Moorer in his paper
    in 1979, About This Reverb Business.
    Adapted from Schroeders 1962 model, with addition of more comb
    filters, adjusting the number fo all-pass filters and including
    early reflection generation as well as low pass filtering in
    the comb feedback loops.
    
    Consists of FIR filter 19 tap delay for early reflection generation
    6 parallel low pass comb filters feeding into a single
    all-pass filter. There is also use of a delay to sync the early 
    reflections and late reflections arrival.

  ==============================================================================
*/

#pragma once

#include "DelayLine.hpp"
#include "Comb.hpp"
#include "AllPass.hpp"
#include "LowPass.hpp"
#include "ERTapDelayLine.hpp"
#include "Helper.h"

//////////////////////////////////////////////////////////
//  Moorer REVERB
//  see .cpp file for comments
//////////////////////////////////////////////////////////

//predefined values for filter numbers
#define NUM_COMBS 6
#define NUM_ALLPASSES 1
#define NUM_LOWPASSES 6

class Moorer{

public:
    //--------------------------------------------------------------
    //constructor setting initial values for comb delays and gains
    //comb delays must be mutually prime
    //
    //  Comb 1  : 50.0 msec delay
    //  Comb 2  : 56.0 msec delay
    //  Comb 3  : 61.0 msec delay
    //  Comb 4  : 68.0 msec delay
    //  Comb 5  : 72.0 msec delay
    //  Comb 6  : 78.0 msec delay
    //  APF 1   : 6.0 msec delay, gain 0.707
    //  LPF 1-6 : low pass filter values for each comb feedback loop
    //  SR      : 44100KHz
    //  RT60    : default of 3 seconds
    //  LD      : late delay, is a delay between onset of ER and reverb tails
    //--------------------------------------------------------------
    Moorer(const int sr = 44100, const DspFloatType rt60 = 3.0,
              const DspFloatType cDelay1 = 50.0, const DspFloatType cDelay2 = 56.0, 
              const DspFloatType cDelay3 = 61.0, const DspFloatType cDelay4 = 68.0,
              const DspFloatType cDelay5 = 72.0, const DspFloatType cDelay6 = 78.0,
              const DspFloatType aDelay1 = 6.0, const DspFloatType aGain1 = 0.707,
              const DspFloatType lCutoff1 = 4942.0f, const DspFloatType lCutoff2 = 4363.0f, 
              const DspFloatType lCutoff3 = 4312.0f, const DspFloatType lCutoff4 = 4574.0f, 
              const DspFloatType lCutoff5 = 3981.0f, const DspFloatType lCutoff6 = 4036.0f, DspFloatType ld = 10.0f);
    ~Moorer();
    
    //getters
    DspFloatType getDecayFactor();
    DspFloatType getCombDelay(const int id);
    DspFloatType getAllpassDelay(const int id);
    DspFloatType getAllpassGain(const int id);
    bool getBypass();
    
    //setters
    void setDecayFactor(const DspFloatType df);
    void setCombDelay(const int id, const DspFloatType sr, const DspFloatType d_ms);
    void setAllpassGain(const int id, const DspFloatType g);
    void setAllpassDelay(const int id, const int sr, const DspFloatType d_ms);
    void setBypass(bool bp);
    void setLPFreq(const DspFloatType lpf);    
    
    //business methods
    DspFloatType next(const DspFloatType in);
    
    
    private:
    DspFloatType decayFactor, ALLPASS_GAIN_LIMIT = 0.707f, lp_freq, lateDelay; //GAIN to keep the allpasses from exploding
    bool bypass;
    Comb *combs[NUM_COMBS];
    Allpass *allpasses[NUM_ALLPASSES];
    ERTapDelayLine *ergenerator[1];
    DelayLine *LRDelay; 

};



//////////////////////////////////////////////////////////
//  Moorer REVERB
//////////////////////////////////////////////////////////



//--------------------------------------------------------------
//constructor setting initial values for comb delays and gains
//comb delays must be mutually prime
//
//  Comb 1  : 50.0 msec delay
//  Comb 2  : 56.0 msec delay
//  Comb 3  : 61.0 msec delay
//  Comb 4  : 68.0 msec delay
//  Comb 5  : 72.0 msec delay
//  Comb 6  : 78.0 msec delay
//  APF 1   : 6.0 msec delay, gain 0.707
//  LPF 1-6 : low pass filter values for each comb feedback loop
//  SR      : 44100KHz
//  RT60    : default of 3 seconds
//  LD      : late delay set to 10ms, a slight delay between ER
//--------------------------------------------------------------
Moorer::Moorer(const int sr, const DspFloatType rt60,
          const DspFloatType cDelay1, const DspFloatType cDelay2, const DspFloatType cDelay3, const DspFloatType cDelay4, const DspFloatType cDelay5, const DspFloatType cDelay6,
          const DspFloatType aDelay1, const DspFloatType aGain1, 
          const DspFloatType lCutoff1, const DspFloatType lCutoff2, const DspFloatType lCutoff3,
          const DspFloatType lCutoff4, const DspFloatType lCutoff5, const DspFloatType lCutoff6, DspFloatType ld){
    
    decayFactor = rt60;
    DspFloatType d_ms, d_ms_max = 100.0f, gain;
    lateDelay = ld;
    bypass = false;
    
    //Comb 1 setup
    d_ms = cDelay1;
    gain = calcCombGain(d_ms, decayFactor);
    combs[0] = new Comb(sr, d_ms, d_ms_max, gain, lCutoff1);
    setCombDelay(0,sr,d_ms);
    
    //Comb 2 setup
    d_ms = cDelay2;
    gain = calcCombGain(d_ms, decayFactor);
    combs[1] = new Comb(sr, d_ms, d_ms_max, gain, lCutoff2);
    setCombDelay(1,sr,d_ms);
    
    //Comb 3 setup
    d_ms = cDelay3;
    gain = calcCombGain(d_ms, decayFactor);
    combs[2] = new Comb(sr, d_ms, d_ms_max, gain, lCutoff3);
    setCombDelay(2,sr,d_ms);
    
    //Comb 4 setup
    d_ms = cDelay4;
    gain = calcCombGain(d_ms, decayFactor);
    combs[3] = new Comb(sr, d_ms, d_ms_max, gain, lCutoff4);
    setCombDelay(3,sr,d_ms);
    
    //Comb 5 setup
    d_ms = cDelay5;
    gain = calcCombGain(d_ms, decayFactor);
    combs[4] = new Comb(sr, d_ms, d_ms_max, gain, lCutoff5);
    setCombDelay(4,sr,d_ms);
    
    //Comb 6 setup
    d_ms = cDelay6;
    gain = calcCombGain(d_ms, decayFactor);
    combs[5] = new Comb(sr, d_ms, d_ms_max, gain, lCutoff6);
    setCombDelay(5,sr,d_ms);
    
    d_ms_max = 20.0f;

    //all pass setup
    allpasses[0] = new Allpass(sr, aDelay1, d_ms_max, aGain1);

    //early reflection generator setup, 0 delay initialisation
    ergenerator[0] = new ERTapDelayLine(sr,0.0f, d_ms_max);
    
    //Late Reflections delay to ensure they arrive slightly
    //after the Early Reflections 
    LRDelay = new DelayLine(sr, 0.0f, d_ms_max);
    LRDelay->setDelay(lateDelay);

}

//-------------------------------------------------------------------------
// Destructor :
// delete all combs and allpasses
//-------------------------------------------------------------------------
Moorer::~Moorer(){
    for(int i = 0; i < NUM_COMBS; i++){
        delete combs[i];
    }
    for(int i = 0; i < NUM_ALLPASSES; i++){
        delete allpasses[i];
    }
    
    delete ergenerator[0];
    delete LRDelay;
}

//getters
//-------------------------------------------------------------------------
// getDecayFactor :
// return the decay factor used for determining RT60 and filter gain
//-------------------------------------------------------------------------
DspFloatType Moorer::getDecayFactor(){return decayFactor;}

//-------------------------------------------------------------------------
// getCombDelay : comb id
// get the specified delay time in milliseconds of the indexed comb filter
//-------------------------------------------------------------------------
DspFloatType Moorer::getCombDelay(const int id){return combs[id]->getDelayTimeMS();}

//-------------------------------------------------------------------------
// getAllpassDelay : allpass id
// get the specified delay time in milliseconds of the indexed allpass filter
//-------------------------------------------------------------------------
DspFloatType Moorer::getAllpassDelay(const int id){return allpasses[id]->getDelayTimeMS();}

//-------------------------------------------------------------------------
// getAllpassGain : comb id
// get the specified gain scalar value of the indexed comb filter
//-------------------------------------------------------------------------
DspFloatType Moorer::getAllpassGain(const int id){return allpasses[id]->getGain();}

//-------------------------------------------------------------------------
// getBypass : 
// return the status of the boolean for bypassing the plugin processing
//-------------------------------------------------------------------------
bool Moorer::getBypass(){return bypass;}

//setters
//-------------------------------------------------------------------------
// setDecayFactor : decayfactor value in seconds
// decay time/desired RT60 is passed from UI to this function
// and the required comb filter gain values that will adhere to that RT60
// are calculated based on this factor
//-------------------------------------------------------------------------
void Moorer::setDecayFactor(const DspFloatType df){
    decayFactor = df;
    
    //cycle through each comb and reset the comb gain value according to
    //the new decayFactor
    for(int i = 0; i < NUM_COMBS; i++){
        combs[i]->setGain(calcCombGain(combs[i]->getDelayTimeMS(), decayFactor));
    }
};

//-------------------------------------------------------------------------
// setCombDelay : id of comb, sample rate, delay time in milliseconds
// sets the gain and the delaytime in milliseconds of the Comb filters
// delay buffer when a value is changed through the UI
//-------------------------------------------------------------------------
void Moorer::setCombDelay(const int id, const DspFloatType sr, const DspFloatType d_ms){
    combs[id]->setGain(calcCombGain(d_ms, decayFactor));
    combs[id]->setDelayTimeMS(sr, d_ms);
}

//-------------------------------------------------------------------------
// setAllpassGain : id of comb, gain
// sets the gain for the allpass filter, scaling by the GAIN_LIMIT so as
// not to blow the filter up due to the unstable nature of IIR filters
//-------------------------------------------------------------------------
void Moorer::setAllpassGain(const int id, const DspFloatType g){allpasses[id]->setGain(g * ALLPASS_GAIN_LIMIT);}

//-------------------------------------------------------------------------
// setAllpassDelay : id of comb, sample rate, delay in milliseconds
// sets the delay time in milliseconds of the all pass delay line
//-------------------------------------------------------------------------
void Moorer::setAllpassDelay(const int id, const int sr, const DspFloatType d_ms){allpasses[id]->setDelayTimeMS(sr, d_ms);}

//-------------------------------------------------------------------------
// setBypass : boolean bypass value
// sets a boolean which determines if processing should be bypassed in the
// worker next function
//-------------------------------------------------------------------------
void Moorer::setBypass(bool bp){bypass = bp;}

//-------------------------------------------------------------------------
// setLPFreq : low pass filter frequency
// sets the frequency for the low pass filter cutoff frequency coefficient
//-------------------------------------------------------------------------
void Moorer::setLPFreq(const DspFloatType lpf){
    lp_freq = lpf;
    
    //cycle through each of the comb filters and set the frequency according
    //to the value passed in from the parameter control in UI
    for(int i = 0; i < NUM_COMBS; i++){
        combs[i]->setLPF(lp_freq);
    }
}

//business methods
//------------------------------------------------------------------
//------------------------------------------------------------------
//  next    : Function to process next sample input in
//          : each input sample is passed to the FIR filter
//          : for Early Reflection generation using a 19 tap
//          : multi-tap delay. The output is then passed to the 
//          : output stage, and also fed to the parallel comb
//          : filter configuration.
//          : The comb filters in turn (scaling to prevent clipping)
//          : process the input and pass each feedback loop through
//          : a low pass filter stage, the output value of the 6
//          : comb filters is accumulated/summed and the 
//          : result is then passed through a single all-pass filter
//          : the result is slightly delay and then summed with the
//          : output from the early ER generation
//
//  in      :   input sample form the audio buffer
//  
//------------------------------------------------------------------
//------------------------------------------------------------------
DspFloatType Moorer::next(const DspFloatType in){
    DspFloatType out = 0.0f;
    DspFloatType passOut = 0.0f;
    DspFloatType ers = 0.0f;
    DspFloatType tapOut = 0.0f;
    
    if(bypass)
        return in;
    

    //early reflections generator
    ers = ergenerator[0]->next(in);
           
    tapOut = ers;
    
    //the comb filters all receive the output from the FIR stage as 
    //there input, and are scalled to prevent clipping
    for(int i = 0; i < NUM_COMBS; i++){
        out += combs[i]->next(tapOut * 0.30f); //scale down to avoid clipping
    }
    
    //the output of the comb filter above is fed to the allpass stage
    passOut = allpasses[0]->next(out);
    
    DspFloatType lr_Shift = 0.0f;
    
    //Late Reflections generated delay to provide a slight delay in
    //the return of the late reflections reverberant tail
    lr_Shift = LRDelay->next(passOut);
    
    //return Early Reflections followed closely by Late Reflections
    return (tapOut*0.30f) + lr_Shift;
    
}


