/*
  ==============================================================================

    Schroeder.h
    Created: 15 Oct 2014 9:10:31pm
    Author:  Keith Hearne

  ==============================================================================
*/

#pragma once

#include "DelayLine.hpp"
#include "Comb.hpp"
#include "AllPass.hpp"
#include "LowPass.hpp"
#include "Helper.h"

//////////////////////////////////////////////////////////
//  SCHROEDER REVERB
//////////////////////////////////////////////////////////

#define NUM_COMBS 4
#define NUM_ALLPASSES 2
#define NUM_LOWPASSES 1

class SchroederImproved{

public:
    //constructor / destructor
    SchroederImproved(const int sr = 44100, const DspFloatType rt60 = 3.0,
              const DspFloatType cDelay1 = 29.7, const DspFloatType cDelay2 = 37.1, const DspFloatType cDelay3 = 41.1, const DspFloatType cDelay4 = 43.7,
              const DspFloatType aDelay1 = 5.0, const DspFloatType aDelay2 = 1.7, const DspFloatType aGain1 = 0.707, const DspFloatType aGain2 = 0.5, const DspFloatType lCutoff1 = 2300.0f);    //const DspFloatType lCutoff1 = 2300.0f
    ~SchroederImproved();
    
    //getters
    DspFloatType getDecayFactor();
    DspFloatType getCombDelay(const int id);
    DspFloatType getAllpassDelay(const int id);
    DspFloatType getAllpassGain(const int id);
    DspFloatType getLowpassCutoff(const int id);
    bool getBypass();
    
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


//constructor / destructor
SchroederImproved::SchroederImproved(const int sr, const DspFloatType rt60,
          const DspFloatType cDelay1, const DspFloatType cDelay2, const DspFloatType cDelay3, const DspFloatType cDelay4,
          const DspFloatType aDelay1, const DspFloatType aDelay2, const DspFloatType aGain1, const DspFloatType aGain2, const DspFloatType lCutoff1){   //const DspFloatType lCutoff1
    
    decayFactor = rt60;
    DspFloatType d_ms, d_ms_max = 100.0f, gain;
    d_ms = cDelay1;
    bypass = false;
    
    gain = calcCombGain(d_ms, decayFactor);
    combs[0] = new Comb(sr, d_ms, d_ms_max, gain,lCutoff1);
    setCombDelay(0,sr,d_ms);
    d_ms = cDelay2;
    gain = calcCombGain(d_ms, decayFactor);
    combs[1] = new Comb(sr, d_ms, d_ms_max, gain,lCutoff1);
    setCombDelay(1,sr,d_ms);
    d_ms = cDelay3;
    gain = calcCombGain(d_ms, decayFactor);
    combs[2] = new Comb(sr, d_ms, d_ms_max, gain,lCutoff1);
    setCombDelay(2,sr,d_ms);
    d_ms = cDelay4;
    gain = calcCombGain(d_ms, decayFactor);
    combs[3] = new Comb(sr, d_ms, d_ms_max, gain,lCutoff1);
    setCombDelay(3,sr,d_ms);
    
    d_ms_max = 20.0f;
    //gain = calcAPGain(aDelay1, 96.83);
    allpasses[0] = new Allpass(sr, aDelay1, d_ms_max, aGain1);
    //gain = calcAPGain(aDelay2, 32.92);
    allpasses[1] = new Allpass(sr, aDelay2, d_ms_max, aGain2);
    
    lowpasses[0] = new Lowpass(sr, lCutoff1);
    //std::cout << "Schroeder constructor called" << std::endl;

}
SchroederImproved::~SchroederImproved(){
    //std::cout << "Schroeder destructor called" << std::endl;
    for(int i = 0; i < NUM_COMBS; i++){
        delete combs[i];
    }
    for(int i = 0; i < NUM_ALLPASSES; i++){
        delete allpasses[i];
    }
}

//getters
DspFloatType SchroederImproved::getDecayFactor(){return decayFactor;}
DspFloatType SchroederImproved::getCombDelay(const int id){return combs[id]->getDelayTimeMS();}
DspFloatType SchroederImproved::getAllpassDelay(const int id){return allpasses[id]->getDelayTimeMS();}
DspFloatType SchroederImproved::getAllpassGain(const int id){return allpasses[id]->getGain();}
DspFloatType SchroederImproved::getLowpassCutoff(const int id){return lowpasses[id]->getCutoff();}
bool SchroederImproved::getBypass(){return bypass;}

//setters
void SchroederImproved::setDecayFactor(const DspFloatType df){
    decayFactor = df;
    for(int i = 0; i < NUM_COMBS; i++){
        combs[i]->setGain(calcCombGain(combs[i]->getDelayTimeMS(), decayFactor));
    }
};

void SchroederImproved::setCombDelay(const int id, const DspFloatType sr, const DspFloatType d_ms){
    combs[id]->setGain(calcCombGain(d_ms, decayFactor));
    combs[id]->setDelayTimeMS(sr, d_ms);
}

void SchroederImproved::setAllpassGain(const int id, const DspFloatType g){allpasses[id]->setGain(g * ALLPASS_GAIN_LIMIT);}
void SchroederImproved::setAllpassDelay(const int id, const int sr, const DspFloatType d_ms){allpasses[id]->setDelayTimeMS(sr, d_ms);}
void SchroederImproved::setLowpassCutoff(const int id, const int sr, const DspFloatType cf_hz){lowpasses[id]->setCutoff(sr, cf_hz);}
void SchroederImproved::setBypass(bool bp){bypass = bp;}

//business methods
DspFloatType SchroederImproved::next(const DspFloatType in){
    
    if(bypass)
        return in;
        
    DspFloatType out = 0.0f;
    
    for(int i = 0; i < NUM_COMBS; i++){
        out += combs[i]->next(in * 0.125f); //scale down to avoid clipping
        //out+= combs[i]->next(in * 0.5);
    }
    
    DspFloatType passOut = 0.0f;
    DspFloatType passOut2 = 0.0f;
    
    passOut = allpasses[0]->next(out);
    passOut2 = allpasses[1]->next(passOut);
  
    //out = allpasses[1]->next((1.0f - allpasses[1]->getGain()) * allpasses[0]->next((1.0f - allpasses[0]->getGain()) * out));//still working out best way to avoid clipping
    
    //KH***return lowpasses[0]->next(passOut2 * NUM_COMBS); //scale back up (not all the way) at output
    return passOut2 * NUM_COMBS; 
    //return passOut2;
    
}


