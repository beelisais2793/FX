/*
  ==============================================================================

    Comb.h
    Created: 15 Oct 2014 8:55:11pm
    Author:  Keith Hearne
    
    A LowPass Comb Filter class that sets delay and gain while passing
    its output through a low pass filter to mimic the effect air has
    on the high frequency content lossand as refelctions in the feedback
    loop are processed 

  ==============================================================================
*/

#pragma once

#include <cstring>
#include "DelayLine.hpp"
#include "LowPass.hpp"

//////////////////////////////////////////////////////////
//  COMB FILTER CLASS
//  see .cpp file for comments
/////////////////////////////////////////////////////////

namespace Delays
{
    class Comb : public MonoFXProcessor
    {        
    public:
        //constructor / destructor
        Comb(const int sr, const DspFloatType d_ms, const DspFloatType d_ms_max, const DspFloatType g, const DspFloatType cutOff);
        ~Comb();
        
        //getters
        DspFloatType getGain();
        DspFloatType getDelayTimeMS();
        
        //setters
        void setGain(const DspFloatType g);
        void setDelayTimeMS(const DspFloatType sr, const DspFloatType d_ms);
        void setLPF(const DspFloatType cutoff);
        
        //business methods
        DspFloatType next(const DspFloatType in);

        DspFloatType Tick(DspFloatType In, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)  {
            return next(In);
        }
        void ProcessBlock(size_t num, DspFloatType * in, DspFloatType * out)
        {
            for(size_t i = 0; i < num; i++) out[i] = Tick(in);
        }
    private:
        DspFloatType gain;
        DelayLine *delay; 
        Lowpass *lpFilter;
        
    };



    //////////////////////////////////////////////////////////
    //  BASIC COMB FILTER CLASS
    //////////////////////////////////////////////////////////

    //-------------------------------------------------------------------------
    // Constructor :
    // Predefined sample rate = 44100, delay time, max delay time and gain
    // and lowpass cutoff frequency. creates a new DelayLine and sets 
    // the input gain and initiates the low pass filter
    //-------------------------------------------------------------------------
    Comb::Comb(const int sr, const DspFloatType d_ms, const DspFloatType d_ms_max, const DspFloatType g, const DspFloatType lp_freq)
    : MonoFXProcessor()
    {
        gain = g;
        delay = new DelayLine(sr, d_ms, d_ms_max);
        lpFilter = new Lowpass(44100, lp_freq);
    }

    //-------------------------------------------------------------------------
    // Destructor :
    // deletes the delay and filter
    //-------------------------------------------------------------------------
    Comb::~Comb(){
        delete delay;
        delete lpFilter;
    }

    //getters
    //-------------------------------------------------------------------------
    // getGain  :
    // return the gain scalar value
    //-------------------------------------------------------------------------
    DspFloatType Comb::getGain(){return gain;}

    //-------------------------------------------------------------------------
    // getDelayTimeMS  :
    // return the delay time in milliseconds
    //-------------------------------------------------------------------------
    DspFloatType Comb::getDelayTimeMS(){return delay->getDelayTimeMS();}

    //setters
    //-------------------------------------------------------------------------
    // setGain  :
    // set the scalar value for the gain of the filter
    //-------------------------------------------------------------------------
    void Comb::setGain(const DspFloatType g){gain = g;}

    //--------------------------------------------------------------------------------
    //  setDelayTimeMS
    //  Setter function sets delay time and from milliseconds
    //  and passes it to the delayline which converts to discrete time samples
    //
    //--------------------------------------------------------------------------------
    void Comb::setDelayTimeMS(const DspFloatType sr, const DspFloatType d_ms){return delay->setDelayTimeMS(sr, d_ms);}

    //--------------------------------------------------------------------------------
    //  setLPF
    //  Setter function sets the cutoff frequency of the low pass filter in the
    //  feedback loop
    //--------------------------------------------------------------------------------
    void Comb::setLPF(const DspFloatType cutoff_freq){ return lpFilter->setCutoff(44100, cutoff_freq);};

    //business methods
    //------------------------------------------------------------------
    //------------------------------------------------------------------
    //  next    : Function to process next sample input in
    //          : The comb filter process involves reading the delay 
    //          : at the current readindex position (delay->readDelay)
    //          : scaling this delay value by the combs gain (gain*dL)
    //          : passing this value through the low pass filter
    //          : and writing this value back into the delay line
    //          : at the appropriate write position 
    //          : (delay->writeDelay(dLW))
    //
    //  in      :   input sample form the audio buffer
    //  
    //------------------------------------------------------------------
    //------------------------------------------------------------------
    DspFloatType Comb::next(const DspFloatType in){
        //read delay
        DspFloatType dL = delay->readDelay();
        
        //attenuate with gain
        DspFloatType dlAttn = dL * gain;
        //pass through low pass filter
        DspFloatType lpOut = lpFilter->next(dlAttn);
        
        //combine output for feedback loop
        DspFloatType dLW = in + lpOut;
        //write feedback loop back to delay buffer
        delay->writeDelay(dLW);
        
        //return the initially read delay
        return dL;
        
    }
}