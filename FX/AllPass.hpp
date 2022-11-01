
/*
  ==============================================================================

    AllPass.h
    Created: 15 Oct 2014 8:55:30pm
    Author:  Keith Hearne
    
    Based on model that Schroeder proposed in 1962 paper presenting his
    initial reverb designs, that uses a feedback delay line with feedforward
    line.


  ==============================================================================
*/


#pragma once

#include <cstring>
#include <iostream>
#include "DelayLine.hpp"

namespace Delay
{
    //////////////////////////////////////////////////////////
    //    ALLPASS FILTER CLASS
    //  see .cpp file for comments
    //////////////////////////////////////////////////////////

    class Allpass : public FunctionProcessor
    {

    public:
        //constructor
        Allpass(const int sr, const DspFloatType d_ms, const DspFloatType d_ms_max, const DspFloatType g);
        ~Allpass();
        
        //getters
        DspFloatType getGain();
        DspFloatType getDelayTimeMS();
        
        //setters
        void setGain(const DspFloatType g);
        void setDelayTimeMS(const DspFloatType sr, const DspFloatType d_ms);
        
        enum {
            PORT_GAIN,
            PORT_DELAYTIMEMS
        };
        void printPorts() {
            printf("\nsetPort\n");
            printf("PORT_GAIN=0\n");
            printf("PORT_DELAYTIMEMS=1\n");
            printf("\ngetPort\n");
            printf("PORT_GAIN=0\n");
            printf("PORT_DELAYTIMEMS=1\n");
        }
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_GAIN: setGain(v); break;
                case PORT_DELAYTIMEMS: setDelayTimeMS(sampleRate,v); break;
                default: printf("No port %d\n",port);
            }
        }
        DspFloatType getPort(int port) {
            switch(port) {
                case PORT_GAIN: return getGain();
                case PORT_DELAYTIMEMS: return getDelayTimeMS();                
                default: printf("No port %d\n",port);
            }
            return 0;
        }
        //business methods
        DspFloatType next(const DspFloatType in);
        
        DspFloatType Tick(DspFloatType in, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            DspFloatType G = getGain();
            DspFloatType ms = getDelayTimeMS();
            setGain(G * X);
            setDelayTimeMS(ms * Y);
            DspFloatType out = next(in);
            setGain(G);
            setDelayTimeMS(sampleRate,ms);
            return A*out;
        }
    private:
        DspFloatType gain;
        DelayLine *delay;
        
    };




    //////////////////////////////////////////////////////////
    //  ALLPASS FILTER CLASS
    //////////////////////////////////////////////////////////

    //constructor
    //-------------------------------------------------------------------------
    // Constructor :
    // Predefined sample rate = 44100, delay time, max delay time and gain
    // creates a new DelayLine and sets the input gain
    //-------------------------------------------------------------------------
    Allpass::Allpass(const int sr, const DspFloatType d_ms, const DspFloatType d_ms_max, const DspFloatType g)
    : FilterObject()
    {
        gain = g;
        delay = new DelayLine(sr, d_ms, d_ms_max);
    }

    //-------------------------------------------------------------------------
    // Destructor :
    // deletes the delay
    //-------------------------------------------------------------------------
    Allpass::~Allpass(){
        delete delay;
    }

    //getters
    //-------------------------------------------------------------------------
    // getGain  :
    // return the gain scalar value
    //-------------------------------------------------------------------------
    DspFloatType Allpass::getGain(){return gain;}

    //-------------------------------------------------------------------------
    // getDelayTimeMS  :
    // return the delay time in milliseconds
    //-------------------------------------------------------------------------
    DspFloatType Allpass::getDelayTimeMS(){return delay->getDelayTimeMS();}

    //setters
    //-------------------------------------------------------------------------
    // setGain  :
    // set the scalar value for the gain of the filter
    //-------------------------------------------------------------------------
    void Allpass::setGain(const DspFloatType g){gain = g;}

    //--------------------------------------------------------------------------------
    //  setDelayTimeMS
    //  Setter function sets delay time and from milliseconds
    //  and passes it to the delayline which converts to discrete time samples
    //
    //--------------------------------------------------------------------------------
    void Allpass::setDelayTimeMS(const DspFloatType sr, const DspFloatType d_ms){return delay->setDelayTimeMS(sr, d_ms);}

    //------------------------------------------------------------------
    //------------------------------------------------------------------
    //  next    : Function to process next sample input in
    //          : The all-pass filter process involves reading the delay 
    //          : at the current readindex position (delay->readDelay)
    //          : scaling this delay value by the combs gain (gain*dL)
    //          : and writing this value back into the delay line
    //          : at the appropriate write position 
    //          : (delay->writeDelay(dLW))
    //
    //  in      :   input sample form the audio buffer
    //  
    //------------------------------------------------------------------
    //------------------------------------------------------------------
    DspFloatType Allpass::next(const DspFloatType in){
        //read delay value from buffer
        DspFloatType dL = delay->readDelay();
        // for the filter write or feedback input to buffer
        DspFloatType fW = in + (gain*dL);
        //form the output of all-pass  (delay + in*-gain)
        DspFloatType out = -gain*fW + dL;  
        // write feedback loop input back to delay buffer
        delay->writeDelay(fW);
        return out;
    }
}
