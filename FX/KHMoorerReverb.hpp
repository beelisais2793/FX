#pragma once

    #include <algorithm>
    #include <cmath>
    #include <cassert>
    #include <cstdlib>
    #include <iostream>

namespace FX::KHDelays::MoorerReverb
{
    /*
    ==============================================================================
        DelayLine.h
        New version of a basic DelayLine which uses fractional delay
        Operations for reading and writing from the delay have been
        separated out into new functions so that the can be read from
        and written to in the Comb and All pass filters seperately.
        
        Created: 15 Oct 2014 5:15:53pm
        Author:  Keith Hearne
    ==============================================================================
    */



    //////////////////////////////////////////////////////////
    //  DELAY LINE CLASS (for use with Moorer Reverb)
    //  see .cpp file for comments
    //////////////////////////////////////////////////////////

    class DelayLine{

    public:
        //constructor / destructor
        DelayLine(const int sr = 44100, const DspFloatType d_ms = 0.0f, const DspFloatType d_max = 0.0f);
        ~DelayLine();
        
        //getters
        DspFloatType getDelayTimeMS();
        DspFloatType getMaxDelayTimeMS();
        
        //setters
        void setDelayTimeMS(const int sr, const DspFloatType d_ms);
        void setDelay(DspFloatType d_ms);
        
        //business functions
        DspFloatType readDelay();
        void writeDelay(DspFloatType in);
        DspFloatType next(const DspFloatType in);
        void resetBuffer();
        void resetDelay();

    private:
        int writePos, readPosA, MAX_DELAY_SAMPLES;
        DspFloatType delay_ms, delay_samples, fraction, MAX_DELAY_MS;
        DspFloatType *buffer;
        
    };

    //helper functions
    //-------------------------------------------------------------------------
    // numSamplesFromMSf :
    // Determine the number of samples from the number of milliseconds delay
    // passed to function
    //-------------------------------------------------------------------------
    inline DspFloatType numSamplesFromMSf(const int sr, const DspFloatType d_ms){
        return sr * d_ms * .001;
    }

    //----------------------------------------------------------------------
    //  Linear Interpolation Function
    //  x1  :  weighting 1
    //  x2  :  weighting 2
    //  y1  :  output y(n) at read index in buffer
    //  y2  :  output y(n-1) at read index minus 1 in buffer
    //  x   :  fractional value between samples
    //----------------------------------------------------------------------
    inline DspFloatType linInterp(DspFloatType x1, DspFloatType x2, DspFloatType y1, DspFloatType y2, DspFloatType x){
        DspFloatType denom = x2 - x1;
        if(denom == 0)
            return y1; // should not ever happen

        // calculate decimal position of x
        DspFloatType dx = (x - x1)/(x2 - x1);

        // use weighted sum method of interpolating
        DspFloatType result = dx*y2 + (1-dx)*y1;

        return result; 
    }


    //////////////////////////////////////////////////////////
    //  DELAY LINE CLASS (for use with Schroeder Reverb)
    //////////////////////////////////////////////////////////

    //-------------------------------------------------------------------------
    // Constructor :
    // Predefined sample rate = 44100, delay = 0, max delay
    // set read and write index for delay line
    // set max delay set from decay factor in reverb
    // create a max delay buffer
    //-------------------------------------------------------------------------
    DelayLine::DelayLine(const int sr, const DspFloatType d_ms, const DspFloatType d_max){
        //assert(d_ms <= d_ms_max);//check bound on delay time
        
        //initialisation
        buffer = NULL;
        readPosA = writePos = 0;
        delay_samples = 0.0f;
        delay_ms = d_ms;
        
        MAX_DELAY_SAMPLES = ceil(numSamplesFromMSf(sr, d_max));
        MAX_DELAY_MS = MAX_DELAY_SAMPLES * 1000.0f / sr; //make sure DspFloatType version is set with to integer-rounded buffer size
        
        DspFloatType delay_samplesf = numSamplesFromMSf(sr, d_ms);
        delay_samples = floor(delay_samplesf);
        
        //storing fractional delay time, will be interpolated
        fraction = delay_samplesf - delay_samples;
        
        buffer = new DspFloatType[MAX_DELAY_SAMPLES];
        memset(buffer, 0, MAX_DELAY_SAMPLES*sizeof(DspFloatType));

    }

    //-------------------------------------------------------------------------
    // Destructor :
    // delete delay buffer
    //-------------------------------------------------------------------------
    DelayLine::~DelayLine(){
        delete[] buffer;
    }

    //getters
    //-------------------------------------------------------------------------
    // getDelayTimeMS :
    // return the delay in milliseconds
    //-------------------------------------------------------------------------
    DspFloatType DelayLine::getDelayTimeMS(){return delay_ms;}

    //--------------------------------------------------------------------------------
    //  getMaxDelayTimeMS :
    //  return the Max delay time in Miliiseconds
    //--------------------------------------------------------------------------------
    DspFloatType DelayLine::getMaxDelayTimeMS(){return MAX_DELAY_MS;}

    //setters
    //--------------------------------------------------------------------------------
    //  Setter function that determines read position index
    //  read position is determined by subtracting the number of samples to delay
    //  from the write position index
    //
    //  readIndex = writeIndex - number of sample delay
    //
    //--------------------------------------------------------------------------------
    void DelayLine::setDelayTimeMS(const int sr, const DspFloatType d_ms){
        assert(d_ms <= MAX_DELAY_MS);//check bound on delay time
        
        DspFloatType delay_samplesf = numSamplesFromMSf(sr, d_ms);
        delay_samples = floor(delay_samplesf);
        
        //storing fractional delay time, output will be interpolate
        fraction = delay_samplesf - delay_samples;
        
        readPosA = writePos - delay_samples; 
        
        //wrap index if bounds exceeded
        if(readPosA < 0)
            readPosA += MAX_DELAY_SAMPLES;
            
            
    }

    //-------------------------------------------------------------------------
    // setDelay :
    // set the delay in milliseconds by the delay value passed to function
    //-------------------------------------------------------------------------
    void DelayLine::setDelay(DspFloatType d){
        //receiving the delay value through here in milliseconds 0 to 2000
        delay_ms = d;
        setDelayTimeMS(44100,delay_ms);
    };

    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    //  readDelay :
    //
    //  The following function reads from delay line at read index
    //  It uses terms similar to the differential equation for delay
    //
    //      x(n)                : the input at sample n
    //      y(n)                : the output at sample n after delay processing
    //      buffer              : the circular buffer used
    //      readPosA            : the read index of the delay buffer
    //      writePos            : the write index of the delay buffer
    //      MAX_DELAY_SAMPLES   : Max size of delay buffer
    //  
    //      y(n) = x(n) + x(n - D)              'delay with no feedback
    //
    //      MAX_DELAY_SAMPLES = sr * d_ms * .001;
    //---------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------
    DspFloatType DelayLine::readDelay(){
        
        //output of delay at readPos
        DspFloatType yn = buffer[readPosA];
        
        //read location at n-1, one behind yn
        int readPos_minus1 = readPosA - 1;
        if(readPos_minus1 < 0)
            readPos_minus1 = MAX_DELAY_SAMPLES - 1;   //MAX_DELAY_SAMPLES - 1 is the last location of the buffer
            
        //get y(n-1)
        DspFloatType yn_minus1 = buffer[readPos_minus1];
        
        //perform linear interpolation of : (0,yn) and (1,yn_minus1) by the ammount of fractional delay(fraction)
        return linInterp(0, 1, yn, yn_minus1, fraction);
    }

    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    //  writeDelay :
    //
    //  The following function writes into the delay buffer at write index
    //---------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------
    void DelayLine::writeDelay(DspFloatType in){
        
        //write the input into buffer at write position
        buffer[writePos] = in;
        
        //increment the write index, wrap index if bounds exceeded
        writePos++;
        if(writePos >= MAX_DELAY_SAMPLES)
            writePos = 0;
            
        readPosA++;
        if(readPosA >= MAX_DELAY_SAMPLES)
            readPosA = 0;
    }

    //business function
    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    //  next    :
    //  The following function is the workhorse of the delay line, but the filters
    //  in the reverb will call the individual read/write function so as to have
    //  more control over manipulating the inputs and outputs.
    //
    //      x(n)                : the input at sample n
    //      y(n)                : the output at sample n after delay processing
    //  
    //      y(n) = x(n) + x(n - D)              'delay with no feedback
    //
    //  read the interpolated value from the delay line and write input back to buffer
    //
    //---------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------
    DspFloatType DelayLine::next(const DspFloatType in){
            
        //input
        DspFloatType xn = in;

        //if delay value is zero just pass input out
        DspFloatType yn;
        if(delay_samples == 0)
            yn = xn;
        else
            yn = readDelay(); 

        writeDelay(xn);
        
        return yn;
            
    }

    //-------------------------------------------------------------------------
    // resetBuffer :
    // delete contents of buffer and instantiate a new buffer
    //-------------------------------------------------------------------------
    void DelayLine::resetBuffer(){
        
        if(buffer)
            delete [] buffer;
            
        buffer = new DspFloatType[MAX_DELAY_SAMPLES];
        
        resetDelay();
        setDelayTimeMS(44100,delay_ms);
        
        return;
        
    }
    
    //-------------------------------------------------------------------------
    // resetDelay :
    // reset the delay buffer by filling with 0's, reset all indexes and bypass
    //------------------------------------------------------------------------- 
    void DelayLine::resetDelay(){
    
        if(buffer)
            memset(buffer, 0, MAX_DELAY_SAMPLES*sizeof(DspFloatType));
        
        readPosA = writePos = 0;
        
        return;
        
    }
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




    //////////////////////////////////////////////////////////
    //    ALLPASS FILTER CLASS
    //  see .cpp file for comments
    //////////////////////////////////////////////////////////

    class Allpass{

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
        
        //business methods
        DspFloatType next(const DspFloatType in);
        
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
    Allpass::Allpass(const int sr, const DspFloatType d_ms, const DspFloatType d_ms_max, const DspFloatType g){
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



    /*
    ==============================================================================
        ERTapDelayLine.h
        Tap Delay used for Early Reflection Generation as proposed by Moorers design
        In Moorers 1979 paper About This Reverberation Business he took a model of
        real room impulse responses for Early Reflection Generation from impulses
        recorded in the Boston Symphony Hall.
        
        These values are used here in a multi-tap setup to provide the summed
        output of the 18 taps (the first tap will be the input impulse, added at 
        the reverb generation stage) to provide an early reflection generator.
        
        Created: 16 Nov 2014 5:15:53pm
        Author:  Keith Hearne
    ==============================================================================
    */


    //////////////////////////////////////////////////////////
    //  ER TAP DELAY LINE CLASS (for use with Moorer Reverb)
    //  see .cpp file for comments
    //////////////////////////////////////////////////////////

    class ERTapDelayLine{

    public:
        //constructor / destructor
        ERTapDelayLine(const int sr = 44100, const DspFloatType d_ms = 0.0f, const DspFloatType d_max = 0.0f);
        ~ERTapDelayLine();
        
        //getters
        DspFloatType getDelayTimeMS();
        DspFloatType getMaxDelayTimeMS();
        
        //setters
        void setDelayTimeMS(const int sr, const DspFloatType d_ms);
        void setDelay(DspFloatType d_ms);
        
        //business functions
        DspFloatType readDelay(DspFloatType in);
        void writeDelay(DspFloatType in);
        DspFloatType next(const DspFloatType in);
        void resetBuffer();
        void resetDelay();

    private:
        int writePos, readPosA, MAX_DELAY_SAMPLES;
        DspFloatType delay_ms, delay_samples, fraction, MAX_DELAY_MS, accumulatedTaps;
        DspFloatType *buffer;
        
        //Early Reflection TAP delay values as presented in Moorer's 1979 paper
        //values extracted from impulse measurements in Boston Symphony Hall
        //These are the delay times for each of the 18 tap readIndex
        //The first tap is unit gain for the input signal, not shown here
        const DspFloatType ER_TAPS[18] = {
            0.0043, 0.0215, 0.0225, 0.0268, 0.0270, 0.0298, 0.0458, 0.0485, 0.0572, 0.0587, 0.0595, 0.0612,
            0.0707, 0.0708, 0.0726, 0.0741, 0.0753, 0.0797
        };
        //Early reflection GAIN values to apply to each of the above taps
        const DspFloatType ER_GAINS[18] = {
            0.841, 0.504, 0.491, 0.379, 0.380, 0.346, 0.289, 0.272, 0.192, 0.193, 0.217, 0.181, 0.180,
            0.181, 0.176, 0.142, 0.167, 0.134
        };
        
    };



    //helper functions
    //-------------------------------------------------------------------------
    // numSamplesFromMSf :
    // Determine the number of samples from the number of milliseconds delay
    // passed to function
    //-------------------------------------------------------------------------
    inline DspFloatType tapNumSamplesFromMSf(const int sr, const DspFloatType d_ms){
        return sr * d_ms;
    }

    //----------------------------------------------------------------------
    //  Linear Interpolation Function
    //  x1  :  weighting 1
    //  x2  :  weighting 2
    //  y1  :  output y(n) at read index in buffer
    //  y2  :  output y(n-1) at read index minus 1 in buffer
    //  x   :  fractional value between samples
    //----------------------------------------------------------------------
    inline DspFloatType linInterp(DspFloatType x1, DspFloatType x2, DspFloatType y1, DspFloatType y2, DspFloatType x){
        DspFloatType denom = x2 - x1;
        if(denom == 0)
            return y1; // should not ever happen

        // calculate decimal position of x
        DspFloatType dx = (x - x1)/(x2 - x1);

        // use weighted sum method of interpolating
        DspFloatType result = dx*y2 + (1-dx)*y1;

        return result; 
    }


    //////////////////////////////////////////////////////////
    //  ER TAP DELAY LINE CLASS (for use with Moorer Reverb)
    //////////////////////////////////////////////////////////

    //-------------------------------------------------------------------------
    // Constructor :
    // Predefined sample rate = 44100, delay = 0, max delay
    // set read and write index for delay line
    // set max delay set from decay factor in reverb
    // create a max delay buffer
    //-------------------------------------------------------------------------
    ERTapDelayLine::ERTapDelayLine(const int sr, const DspFloatType d_ms, const DspFloatType d_max){
        //assert(d_ms <= d_ms_max);//check bound on delay time
        
        buffer = NULL;
        readPosA = writePos = 0;
        
        delay_samples = accumulatedTaps = 0.0f;
        delay_ms = d_ms;
        
        MAX_DELAY_SAMPLES = ceil(tapNumSamplesFromMSf(sr, d_max));
        MAX_DELAY_MS = MAX_DELAY_SAMPLES * 1000.0f / sr; //make sure DspFloatType version is set with to integer-rounded buffer size
        
        DspFloatType delay_samplesf = tapNumSamplesFromMSf(sr, d_ms);
        delay_samples = floor(delay_samplesf);
        
        //storing fractional delay time, will be interpolated
        fraction = delay_samplesf - delay_samples;
        
        buffer = new DspFloatType[MAX_DELAY_SAMPLES];
        memset(buffer, 0, MAX_DELAY_SAMPLES*sizeof(DspFloatType));
    }

    //-------------------------------------------------------------------------
    // Destructor :
    // delete delay buffer
    //-------------------------------------------------------------------------
    ERTapDelayLine::~ERTapDelayLine(){
        delete[] buffer;
    }

    //getters
    //-------------------------------------------------------------------------
    // getDelayTimeMS :
    // return the delay in milliseconds
    //-------------------------------------------------------------------------
    DspFloatType ERTapDelayLine::getDelayTimeMS(){return delay_ms;}

    //--------------------------------------------------------------------------------
    //  getMaxDelayTimeMS :
    //  return the Max delay time in Miliiseconds
    //--------------------------------------------------------------------------------
    DspFloatType ERTapDelayLine::getMaxDelayTimeMS(){return MAX_DELAY_MS;}

    //setters
    //--------------------------------------------------------------------------------
    //  Setter function that determines read position index
    //  read position is determined by subtracting the number of samples to delay
    //  from the write position index
    //
    //  readIndex = writeIndex - number of sample delay
    //
    //--------------------------------------------------------------------------------
    void ERTapDelayLine::setDelayTimeMS(const int sr, const DspFloatType d_ms){
        assert(d_ms <= MAX_DELAY_MS);//check bound on delay time
        
        DspFloatType delay_samplesf = tapNumSamplesFromMSf(sr, d_ms);
        this->delay_samples = floor(delay_samplesf);
        
        //storing fractional delay time, output will be interpolated
        fraction = delay_samplesf - delay_samples;
        readPosA = writePos - delay_samples;
        
        //wrap index if bounds exceeded
        if(readPosA < 0)
            readPosA += MAX_DELAY_SAMPLES;
            
            
    }

    //-------------------------------------------------------------------------
    // setDelay :
    // set the delay in milliseconds by the delay value passed to function
    //-------------------------------------------------------------------------
    void ERTapDelayLine::setDelay(DspFloatType d){
        //receiving the delay value through here in milliseconds 0 to 2000
        delay_ms = d;
        setDelayTimeMS(44100,delay_ms);
    };

    //----------------------------------------------------
    //----------------------------------------------------
    //  readDelay : this is early reflections delay line
    //
    //  each read of the delay line gives each of the 
    //  19 taps scaled by gains for a 19 Impulse FIR 
    //  as laid out in Moorerspaper. Values are 
    //  all hardcoded and initialised in the .h file
    //----------------------------------------------------
    //----------------------------------------------------
    DspFloatType ERTapDelayLine::readDelay(DspFloatType in){

        DspFloatType interpVal = 0.0f;
        DspFloatType accumulatedTaps = 0.0f;
        //first tap is the input sample at gain of 1
        accumulatedTaps = in;       //I have changed this to in value

    //output of delay at readPos
        DspFloatType yn = 0.0f;
        
        //loop through the amount of ER_TAP Values accumulate the delay*gain
        //each time through the loop
        for(int i=0; i < (sizeof(ER_TAPS)/sizeof(ER_TAPS[0])); i++ ){
            
            //set the delay samples for this specific TAP, which aligns the read index
            setDelay(ER_TAPS[i]);
            yn = buffer[readPosA];
    
            //read location at n-1, one behind yn
            int readPos_minus1 = readPosA - 1;
            if(readPos_minus1 < 0)
                readPos_minus1 = MAX_DELAY_SAMPLES - 1;   //MAX_DELAY_SAMPLES - 1 is the last location of the buffer
                
            //get y(n-1)
            DspFloatType yn_minus1 = buffer[readPos_minus1];
            
            //perform linear interpolation of : (0,yn) and (1,yn_minus1) by the ammount of fractional delay(fraction)
            interpVal = linInterp(0, 1, yn, yn_minus1, fraction);
            
            //This is the accumulation of tap delay values with the respective gain
            accumulatedTaps +=  interpVal * ER_GAINS[i];
        }
        
        //return the accumulated value for tap delays
        return accumulatedTaps;

    }

    //--------------------------------------------------------------------------------
    //  writeDelay :
    //
    //  The following function writes into the delay buffer at write index
    //---------------------------------------------------------------------------------
    void ERTapDelayLine::writeDelay(DspFloatType in){
        
        //write the input sample into the write position of the buffer
        buffer[writePos] = in;
        
        //increment indexes and wrap if bounds exceeded
        writePos++;
        if(writePos >= MAX_DELAY_SAMPLES)
            writePos = 0;
            
        readPosA++;
        if(readPosA >= MAX_DELAY_SAMPLES)
            readPosA = 0;
    }

    //business function
    //--------------------------------------------------------------------------------
    //  next    :
    //  The following function is the workhorse of the delay line, but the filters
    //  in the reverb will call the individual read/write function so as to have
    //  more control over manipulating the inputs and outputs.
    //
    //      x(n)                : the input at sample n
    //      y(n)                : the output at sample n after delay processing
    //  
    //      y(n) = x(n) + x(n - D)              'delay with no feedback
    //
    //  read the interpolated value from the delay line and write input back to buffer
    //
    //---------------------------------------------------------------------------------
    DspFloatType ERTapDelayLine::next(const DspFloatType in){
            
        //input
        DspFloatType xn = in;

        //if delay value is zero just pass input out
        DspFloatType yn;
        yn = readDelay(in);

        writeDelay(xn);
        
        return yn;
            
    }

    //-------------------------------------------------------------------------
    // resetBuffer :
    // delete contents of buffer and instantiate a new buffer
    //-------------------------------------------------------------------------
    void ERTapDelayLine::resetBuffer(){
        
        if(buffer)
            delete [] buffer;
            
        buffer = new DspFloatType[MAX_DELAY_SAMPLES];
        
        resetDelay();
        setDelayTimeMS(44100,delay_ms);
        
        return;
        
    }
    
    //-------------------------------------------------------------------------
    // resetDelay :
    // reset the delay buffer by filling with 0's, reset all indexes and bypass
    //------------------------------------------------------------------------- 
    void ERTapDelayLine::resetDelay(){
    
        if(buffer)
            memset(buffer, 0, MAX_DELAY_SAMPLES*sizeof(DspFloatType));
        
        readPosA = writePos = 0;
        
        return;
        
    }


    //////////////////////////////////////////////////////////
    //  BASIC LOWPASS FILTER CLASS
    //  see .cpp file for comments
    //////////////////////////////////////////////////////////

    class Lowpass{

    public:
        //constructor
        Lowpass(const int sr, const DspFloatType cf_hz);
        
        //getters
        DspFloatType getCutoff();
        
        //setters
        void setCutoff(const int sr, const DspFloatType cf_hz);
        
        //business methods
        DspFloatType next(const DspFloatType in);
        
    private:
        DspFloatType cutoff, coef, prev;
        
    };


    //////////////////////////////////////////////////////////
    //  BASIC LOWPASS FILTER CLASS
    //////////////////////////////////////////////////////////

    //-------------------------------------------------------------------------
    // Constructor :
    // Predefined sample rate = 44100, default cutoff frequency passed here
    // from these values the coefficients for a and b are calculated 
    // a = 1 + b
    // b = sqrt(squared[2 - cos(2*PI*freq/sr)] - 1) -2 + cos(2*PI*freq/sr)
    //-------------------------------------------------------------------------
    Lowpass::Lowpass(const int sr, const DspFloatType cf_hz){
        cutoff = coef = prev = 0;
        cutoff = cf_hz;
        DspFloatType costh = 2.0 - cos(2.0 * M_PI * cutoff / sr);
        coef = sqrt(costh * costh - 1.0) - costh;
    }

    //getters
    //-------------------------------------------------------------------------
    // getCutoff :
    // return the value of the cutoff frequency
    //-------------------------------------------------------------------------
    DspFloatType Lowpass::getCutoff(){return cutoff;}

    //setters
    //-------------------------------------------------------------------------
    // setCutoff :
    // function to adjust and set the filter coefficients from the cutoff
    // frequency parameter
    //-------------------------------------------------------------------------
    void Lowpass::setCutoff(const int sr, const DspFloatType cf_hz){
        cutoff = cf_hz;
        DspFloatType costh = 2.0 - cos(2.0 * M_PI * cutoff / sr);
        coef = sqrt(costh * costh - 1.0) - costh;
    }

    //business methods
    //-------------------------------------------------------------------------
    // next :
    // the process function which takes the input discrete time sample and 
    // applies the coefficient one sample delay formula to it returning the
    // previous value (input * (1 + b) - (delayed sample * b)
    //-------------------------------------------------------------------------
    DspFloatType Lowpass::next(const DspFloatType in){
        prev = in * (1 + coef) - (prev * coef);
        return prev;
    }


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


}