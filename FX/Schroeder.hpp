/*
  ==============================================================================

    Schroeder.h
    Created: 15 Oct 2014 9:10:31pm
    Author:  Keith Hearne

    The Schroeder Reverberator as proposed by M.R. Schroeder in his paper
    in 1962, Natural-sounding artificial reverberation.
    consisting of 4 parallel comb filters feeding into a series
    of 2 All-pass filters. This version sticks as rigidly as possible to 
    the designs laid out in the paper with no embellishments.
    
  ==============================================================================
*/

#pragma once

#include "DelayLine.hpp"
#include "Comb.hpp"
#include "AllPass.hpp"
#include "Helper.h"

//////////////////////////////////////////////////////////
//  SCHROEDER REVERB
//  see .cpp file for full comments
//////////////////////////////////////////////////////////

//predefined number of comb and allpass filters for array parsing

#define NUM_COMBS 4
#define NUM_ALLPASSES 2

namespace FX::Reverb
{
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
        //  sr      : defaulted to 44100KHz
        //  rt60    : defaulted to 3.0 seconds on initialisation
        //--------------------------------------------------------------
        Schroeder(const int sr = 44100, const DspFloatType rt60 = 3.0,
                const DspFloatType cDelay1 = 29.7, const DspFloatType cDelay2 = 37.1,
                const DspFloatType cDelay3 = 41.1, const DspFloatType cDelay4 = 43.7,
                const DspFloatType aDelay1 = 5.0, const DspFloatType aDelay2 = 1.7,
                const DspFloatType aGain1 = 0.707, const DspFloatType aGain2 = 0.5,
                const DspFloatType lpCutoff = 0.5);
        ~Schroeder();
        
        //getters
        DspFloatType getDecayFactor();
        DspFloatType getCombDelay(const int id);
        DspFloatType getAllpassDelay(const int id);
        DspFloatType getAllpassGain(const int id);
    //    DspFloatType getLowpassCutoff(const int id);
        bool getBypass();
        
        //setters
        void setDecayFactor(const DspFloatType df);
        void setCombDelay(const int id, const DspFloatType sr, const DspFloatType d_ms);
        void setAllpassGain(const int id, const DspFloatType g);
        void setAllpassDelay(const int id, const int sr, const DspFloatType d_ms);
        void setBypass(bool bp);
    //    void setLowpassCutoff(const int id, const int sr, const DspFloatType cf_hz);
        void reset();
        
        //business methods
        DspFloatType next(const DspFloatType in);
        
        
        private:
        DspFloatType decayFactor, ALLPASS_GAIN_LIMIT = 0.707f;//to keep the allpasses from exploding
        bool bypass;
        Comb *combs[NUM_COMBS];
        Allpass *allpasses[NUM_ALLPASSES];
    };



    //////////////////////////////////////////////////////////
    //  SCHROEDER REVERB
    //////////////////////////////////////////////////////////


    #define NUM_COMBS 4
    #define NUM_ALLPASSES 2


    //constructor / destructor
    Schroeder::Schroeder(const int sr, const DspFloatType rt60,
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
        
        //lowpasses[0] = new Lowpass(sr, lCutoff1);
        //std::cout << "Schroeder constructor called" << std::endl;

    }
    Schroeder::~Schroeder(){
        //std::cout << "Schroeder destructor called" << std::endl;
        for(int i = 0; i < NUM_COMBS; i++){
            delete combs[i];
        }
        for(int i = 0; i < NUM_ALLPASSES; i++){
            delete allpasses[i];
        }
    }

    //getters
    DspFloatType Schroeder::getDecayFactor(){return decayFactor;}
    DspFloatType Schroeder::getCombDelay(const int id){return combs[id]->getDelayTimeMS();}
    DspFloatType Schroeder::getAllpassDelay(const int id){return allpasses[id]->getDelayTimeMS();}
    DspFloatType Schroeder::getAllpassGain(const int id){return allpasses[id]->getGain();}
    //DspFloatType Schroeder::getLowpassCutoff(const int id){return lowpasses[id]->getCutoff();}
    bool Schroeder::getBypass(){return bypass;}

    //setters
    void Schroeder::setDecayFactor(const DspFloatType df){
        decayFactor = df;
        for(int i = 0; i < NUM_COMBS; i++){
            combs[i]->setGain(calcCombGain(combs[i]->getDelayTimeMS(), decayFactor));
        }
    };

    void Schroeder::setCombDelay(const int id, const DspFloatType sr, const DspFloatType d_ms){
        combs[id]->setGain(calcCombGain(d_ms, decayFactor));
        combs[id]->setDelayTimeMS(sr, d_ms);
    }

    void Schroeder::setAllpassGain(const int id, const DspFloatType g){allpasses[id]->setGain(g * ALLPASS_GAIN_LIMIT);}
    void Schroeder::setAllpassDelay(const int id, const int sr, const DspFloatType d_ms){allpasses[id]->setDelayTimeMS(sr, d_ms);}
    //void Schroeder::setLowpassCutoff(const int id, const int sr, const DspFloatType cf_hz){lowpasses[id]->setCutoff(sr, cf_hz);}
    void Schroeder::setBypass(bool bp){bypass = bp;}

    //business methods
    DspFloatType Schroeder::next(const DspFloatType in){
        
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
}