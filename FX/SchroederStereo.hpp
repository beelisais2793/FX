/*
  ==============================================================================

    Schroeder.h
    Created: 15 Oct 2014 9:10:31pm
    Author:  Keith Hearne
    
    The Schroeder Reverberator as proposed by M.R. Schroeder in his paper
    in 1962, Natural-sounding artificial reverberation.
    consisting of 4 parallel comb filters feeding into a series
    of 2 All-pass filters. 
    
    This stereo version includes a built in low pass filter for accomodation
    of some of the high frequency roll off but nothing more and it is 
    not adjustable. This will be made visible and adjustable in subsequent
    Moorer version.

  ==============================================================================
*/

#pragma once

#include "DelayLine.hpp"
#include "Comb.hpp"
#include "AllPass.hpp"
#include "LowPass.hpp"

//////////////////////////////////////////////////////////
//  SCHROEDER REVERB
//////////////////////////////////////////////////////////

//predefined number of comb and allpass filters for array parsing
//plus one all pass filter
#define NUM_COMBS 4
#define NUM_ALLPASSES 2
#define NUM_LOWPASSES 1

class Schroeder{

public:
    //--------------------------------------------------------------
    //constructor setting initial values for comb delays and gains
    //comb delays must be mutually prime
    //
    //  Comb 1  : 29.7 msec delay
    //  Comb 2  : 37.1 msec delay
    //  Comb 3  : 41.1 msec delay
    //  Comb 4  : 43.7 msec delay
    //  APF 1   : 5.0 msec delay, gain 0.707
    //  APF 2   : 1.7 msec delay, gain 0.5
    //--------------------------------------------------------------
    Schroeder(const int sr = 44100, const DspFloatType rt60 = 3.0,
              const DspFloatType cDelay1 = 29.7, const DspFloatType cDelay2 = 37.1, const DspFloatType cDelay3 = 41.1, const DspFloatType cDelay4 = 43.7,
              const DspFloatType aDelay1 = 5.0, const DspFloatType aDelay2 = 1.7, const DspFloatType aGain1 = 0.07, const DspFloatType aGain2 = 0.5, const DspFloatType lCutoff1 = 2300.0f);    //const DspFloatType lCutoff1 = 2300.0f
    ~Schroeder();
    
    //getters
    DspFloatType getDecayFactor();
    DspFloatType getCombDelay(const int id);
    DspFloatType getAllpassDelay(const int id);
    DspFloatType getAllpassGain(const int id);
    DspFloatType getLowpassCutoff(const int id);
    bool getBypass();
    DspFloatType getCombOutput(const DspFloatType in, const int id);
    DspFloatType getAllpassOutput(const DspFloatType in, const int id);    
    
    //setters
    void setDecayFactor(const DspFloatType df);
    void setCombDelay(const int id, const DspFloatType sr, const DspFloatType d_ms);
    void setAllpassGain(const int id, const DspFloatType g);
    void setAllpassDelay(const int id, const int sr, const DspFloatType d_ms);
    void setLowpassCutoff(const int id, const int sr, const DspFloatType cf_hz);
    void setBypass(bool bp);
    
    //business methods
    DspFloatType next(const DspFloatType in);
    
    
    private:
    DspFloatType decayFactor, ALLPASS_GAIN_LIMIT = 0.707f;//to keep the allpasses from exploding
    bool bypass;
    Comb *combs[NUM_COMBS];
    Allpass *allpasses[NUM_ALLPASSES];
    Lowpass *lowpasses[NUM_LOWPASSES];
};



//////////////////////////////////////////////////////////
//  SCHROEDER REVERB
//////////////////////////////////////////////////////////


#define NUM_COMBS 4
#define NUM_ALLPASSES 2

//helper functions
//------------------------------------------------------------------
//------------------------------------------------------------------
//  calcCombGain : Function to calculate gain from decay/RT60
//
//  RT60    :   value from plugin decay parameter
//  d_ms    :   Delay value of the comb filter
//------------------------------------------------------------------
//------------------------------------------------------------------
inline DspFloatType calcCombGain(const DspFloatType d_ms, const DspFloatType rt60){
    return pow(10.0, ((-3.0 * d_ms) / (rt60 * 1000.0)));

}


//--------------------------------------------------------------
//--------------------------------------------------------------
//constructor setting initial values for comb delays and gains
//comb delays must be mutually prime
//
//  Comb 1  : 29.7 msec delay
//  Comb 2  : 37.1 msec delay
//  Comb 3  : 41.1 msec delay
//  Comb 4  : 43.7 msec delay
//  APF 1   : 5.0 msec delay, gain 0.707
//  APF 2   : 1.7 msec delay, gain 0.5
//  sr      : defaulted to 44100KHz
//  rt60    : defaulted to 3.0 seconds on initialisation
//
//  The 2nd channel will be declared in the plugin with
//  time values specified in the constructur to override
//  the above
//
//--------------------------------------------------------------
//--------------------------------------------------------------
Schroeder::Schroeder(const int sr, const DspFloatType rt60,
          const DspFloatType cDelay1, const DspFloatType cDelay2, const DspFloatType cDelay3, const DspFloatType cDelay4,
          const DspFloatType aDelay1, const DspFloatType aDelay2, const DspFloatType aGain1, const DspFloatType aGain2, const DspFloatType lCutoff1){   //const DspFloatType lCutoff1
    
    decayFactor = rt60;
    DspFloatType d_ms, d_ms_max = 100.0f, gain;
    bypass = false;
    
    //Comb Filter 1 setup
    d_ms = cDelay1;
    gain = calcCombGain(d_ms, decayFactor);
    combs[0] = new Comb(sr, d_ms, d_ms_max, gain, lCutoff1);
    setCombDelay(0,sr,d_ms);
    
    //Comb Filter 2 setup
    d_ms = cDelay2;
    gain = calcCombGain(d_ms, decayFactor);
    combs[1] = new Comb(sr, d_ms, d_ms_max, gain, lCutoff1);
    setCombDelay(1,sr,d_ms);
    
    //Comb Filter 3 setup
    d_ms = cDelay3;
    gain = calcCombGain(d_ms, decayFactor);
    combs[2] = new Comb(sr, d_ms, d_ms_max, gain, lCutoff1);
    setCombDelay(2,sr,d_ms);
    
    //Comb Filter 4 setup
    d_ms = cDelay4;
    gain = calcCombGain(d_ms, decayFactor);
    combs[3] = new Comb(sr, d_ms, d_ms_max, gain, lCutoff1);
    setCombDelay(3,sr,d_ms);
    
    d_ms_max = 20.0f;

    //All-pass filter setup
    allpasses[0] = new Allpass(sr, aDelay1, d_ms_max, aGain1);
    allpasses[1] = new Allpass(sr, aDelay2, d_ms_max, aGain2);
    
    //Low pass Filter initialisation
    lowpasses[0] = new Lowpass(sr, lCutoff1);

}

//-------------------------------------------------------------------------
// Destructor :
// delete all combs and allpasses
//-------------------------------------------------------------------------
Schroeder::~Schroeder(){
    //std::cout << "Schroeder destructor called" << std::endl;
    for(int i = 0; i < NUM_COMBS; i++){
        delete combs[i];
    }
    for(int i = 0; i < NUM_ALLPASSES; i++){
        delete allpasses[i];
    }
    
    delete lowpasses[0];
}

//getters
//-------------------------------------------------------------------------
// getDecayFactor :
// return the decay factor used for determining RT60 and filter gain
//-------------------------------------------------------------------------
DspFloatType Schroeder::getDecayFactor(){return decayFactor;}

//-------------------------------------------------------------------------
// getCombDelay : comb id
// get the specified delay time in milliseconds of the indexed comb filter
//-------------------------------------------------------------------------
DspFloatType Schroeder::getCombDelay(const int id){return combs[id]->getDelayTimeMS();}

//-------------------------------------------------------------------------
// getAllpassDelay : allpass id
// get the specified delay time in milliseconds of the indexed allpass filter
//-------------------------------------------------------------------------
DspFloatType Schroeder::getAllpassDelay(const int id){return allpasses[id]->getDelayTimeMS();}

//-------------------------------------------------------------------------
// getAllpassGain : comb id
// get the specified gain scalar value of the indexed comb filter
//-------------------------------------------------------------------------
DspFloatType Schroeder::getAllpassGain(const int id){return allpasses[id]->getGain();}

//-------------------------------------------------------------------------
// getLowpassCutoff : 
// return the cutoff frequency for the low pass filter
//-------------------------------------------------------------------------
DspFloatType Schroeder::getLowpassCutoff(const int id){return lowpasses[id]->getCutoff();}

//-------------------------------------------------------------------------
// getBypass : 
// return the status of the boolean for bypassing the plugin processing
//-------------------------------------------------------------------------
bool Schroeder::getBypass(){return bypass;}

//-------------------------------------------------------------------------
// getCombOutput : input sample, comb id 
// return the output of the comb filter at sample passed in
// this function was added to be able to tap into the output from each
// comb individually. 
// This will be used when incorporating a summing matrix for the output
// to have control over how each individual filter is summed
//-------------------------------------------------------------------------
DspFloatType Schroeder::getCombOutput(DspFloatType in, int id){
    return combs[id]->next(in);
}

//-------------------------------------------------------------------------
// getAllpassOutput : input sample, comb id 
// return the output of the allpass filter at sample passed in
// this function was added to be able to tap into the output from each
// allpass individually. 
// This will be used when incorporating a summing matrix for the output
// to have control over how each individual filter is summed
//-------------------------------------------------------------------------
DspFloatType Schroeder::getAllpassOutput(DspFloatType in, int id){
    return allpasses[id]->next(in);
}

//setters
//-------------------------------------------------------------------------
// setDecayFactor : decayfactor value in seconds
// decay time/desired RT60 is passed from UI to this function
// and the required comb filter gain values that will adhere to that RT60
// are calculated based on this factor
//-------------------------------------------------------------------------
void Schroeder::setDecayFactor(const DspFloatType df){
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
void Schroeder::setCombDelay(const int id, const DspFloatType sr, const DspFloatType d_ms){
    combs[id]->setGain(calcCombGain(d_ms, decayFactor));
    combs[id]->setDelayTimeMS(sr, d_ms);
}

//-------------------------------------------------------------------------
// setAllpassGain : id of comb, gain
// sets the gain for the allpass filter, scaling by the GAIN_LIMIT so as
// not to blow the filter up due to the unstable nature of IIR filters
//-------------------------------------------------------------------------
void Schroeder::setAllpassGain(const int id, const DspFloatType g){allpasses[id]->setGain(g * ALLPASS_GAIN_LIMIT);}

//-------------------------------------------------------------------------
// setAllpassDelay : id of comb, sample rate, delay in milliseconds
// sets the delay time in milliseconds of the all pass delay line
//-------------------------------------------------------------------------
void Schroeder::setAllpassDelay(const int id, const int sr, const DspFloatType d_ms){allpasses[id]->setDelayTimeMS(sr, d_ms);}

//-------------------------------------------------------------------------
// setLowpassCutoff : id lowpass, sample rate, cutoff frequency
// sets the cutoff frequency for the one-pole low pass filter
//-------------------------------------------------------------------------
void Schroeder::setLowpassCutoff(const int id, const int sr, const DspFloatType cf_hz){lowpasses[id]->setCutoff(sr, cf_hz);}

//-------------------------------------------------------------------------
// setBypass : boolean bypass value
// sets a boolean which determines if processing should be bypassed in the
// worker next function
//-------------------------------------------------------------------------
void Schroeder::setBypass(bool bp){bypass = bp;}

//business methods
//------------------------------------------------------------------
//------------------------------------------------------------------
//  next    : Function to process next sample input in
//          : each input sample is passed to each of the comb 
//          : filters in turn (scaling to prevent clipping)
//          : the output value is accumulated/summed
//          : the result is then passed in series through the 
//          : all-pass filters
//
//  in      :   input sample form the audio buffer
//  
//------------------------------------------------------------------
//------------------------------------------------------------------
DspFloatType Schroeder::next(const DspFloatType in){

    // if bypass is enabled on plugin then just pass back
    // the unprocessed input sample
    if(bypass)
        return in;
        
    DspFloatType out = 0.0f;

    //step through each of the 4 comb filters and pass a scaled input
    for(int i = 0; i < NUM_COMBS; i++){
        out += combs[i]->next(in * 0.125f); //scale down to avoid clipping
    }
    
    DspFloatType passOut = 0.0f;
    DspFloatType passOut2 = 0.0f;
    
    passOut = allpasses[0]->next(out);          //1 stage all-pass
    passOut2 = allpasses[1]->next(passOut);     //2 stage all-pass
  
    //scale back up (not all the way) at output and filter hi freq
    return lowpasses[0]->next(passOut2 * NUM_COMBS); 
    
}


