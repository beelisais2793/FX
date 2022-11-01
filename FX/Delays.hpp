#pragma once

#include <algorithm>
#include <cmath>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <cstdint>
#include <memory>


namespace Delay::Delays
{
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
inline DspFloatType Lowpass::getCutoff(){return cutoff;}

//setters
//-------------------------------------------------------------------------
// setCutoff :
// function to adjust and set the filter coefficients from the cutoff
// frequency parameter
//-------------------------------------------------------------------------
inline void Lowpass::setCutoff(const int sr, const DspFloatType cf_hz){
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
inline 
DspFloatType Lowpass::next(const DspFloatType in){
    prev = in * (1 + coef) - (prev * coef);
    return prev;
}

//////////////////////////////////////////////////////////
//  BASIC DELAY LINE CLASS (feedback optional)
//  see comments in .cpp file
//////////////////////////////////////////////////////////
class BasicDelayLine : public FunctionProcessor
{
public:
    BasicDelayLine(const int sr = 44100, const DspFloatType d_ms = 0.0f, const DspFloatType feedback = 0.0f, const DspFloatType mixLevel = 0.5f);
    ~BasicDelayLine();
    
    //getters
    DspFloatType getDelayTimeMS();
    DspFloatType getMaxDelayTimeMS();
    DspFloatType getFeedback();
    DspFloatType getMix();
    bool getByPass();
    
    //setters
    void setDelayTimeMS(const int sr, const DspFloatType d_ms);
    void setDelay(DspFloatType d_ms);
    void setFeedback(DspFloatType f_pct);
    void setMix(DspFloatType m_pct);
    void setByPass(bool bp);
    
    //business functions
    DspFloatType next(const DspFloatType in);
    void resetBuffer();
    void resetDelay();

    DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
    {
        return A*next(I);
    }

private:
    int writePos, readPosA, MAX_DELAY_SAMPLES;
    DspFloatType delay_ms, delay_samples, fraction, feedback, mixLevel, MAX_DELAY_MS;
    bool delay_bypass;
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

//helper functions
//-------------------------------------------------------------------------
// numSamplesFromMSf :
// Determine the number of samples from the number of milliseconds delay
// passed to function
//-------------------------------------------------------------------------
inline DspFloatType tapNumSamplesFromMSf(const int sr, const DspFloatType d_ms){
    return sr * d_ms;
}

//////////////////////////////////////////////////////////
//  BASIC VARIABLE DELAY LINE CLASS (feedback optional)
//////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
// Constructor :
// Predefined sample rate = 44100, feedback level & delay = 0, mixlevel=50%
// set read and write index for delay line
// set max delay to 2 seconds
// create a max delay buffer
//-------------------------------------------------------------------------
inline
BasicDelayLine::BasicDelayLine(const int sr, const DspFloatType d_ms, const DspFloatType fb, const DspFloatType mix)
: FunctionProcessor()
{
    //assert(d_ms <= d_ms_max);//check bound on delay time
    
    buffer = NULL;
    readPosA = writePos = feedback = mixLevel = 0;
    delay_bypass = 0;
    
    //max delay of 2 seconds
    DspFloatType d_ms_max = 2000.0f;
    delay_samples = 0.0f;
    delay_ms = d_ms;
   
    feedback = fb;
    mixLevel = mix;
    
    MAX_DELAY_SAMPLES = ceil(numSamplesFromMSf(sr, d_ms_max));
    MAX_DELAY_MS = MAX_DELAY_SAMPLES * 1000.0f / sr; //make sure DspFloatType version is set with to integer-rounded buffer size
    
    //number of delay samples
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
inline
BasicDelayLine::~BasicDelayLine(){
    delete[] buffer;
}

//getters
//-------------------------------------------------------------------------
// getDelayTimeMS :
// return the delay in milliseconds
//-------------------------------------------------------------------------
inline
DspFloatType BasicDelayLine::getDelayTimeMS(){return delay_ms;}

//-------------------------------------------------------------------------
// getFeedback :
// return the feedback value
//-------------------------------------------------------------------------
inline
DspFloatType BasicDelayLine::getFeedback(){return feedback;}

//-------------------------------------------------------------------------
// getMix :
// return the mix level, set to 50% by default
//-------------------------------------------------------------------------
inline
DspFloatType BasicDelayLine::getMix(){return mixLevel;}
  
//-------------------------------------------------------------------------
// getByPass :
// return the boolean value indicating if plugin is bypassed or not
//-------------------------------------------------------------------------
inline
bool BasicDelayLine::getByPass(){return delay_bypass;}


//--------------------------------------------------------------------------------
//  Setter function that determines read position index
//  read position is determined by subtracting the number of samples to delay
//  from the write position index
//
//  readIndex = writeIndex - number of sample delay
//
//--------------------------------------------------------------------------------
inline
void BasicDelayLine::setDelayTimeMS(const int sr, const DspFloatType d_ms){
    assert(d_ms <= MAX_DELAY_MS);//check bound on delay time
    
    //function returns the number of samples from a delay in millsecond value
    //  number of samples = sr * d_ms * .001
    DspFloatType delay_samplesf = numSamplesFromMSf(sr, d_ms);
    delay_samples = floor(delay_samplesf);
    
    //storing fractional delay time, output will be interpolated
    fraction = delay_samplesf - delay_samples;
    
    //the read index is determined from the write index minus the number of
    //samples to delay by
    readPosA = writePos - (int)delay_samples;
    if(readPosA < 0)
        readPosA += MAX_DELAY_SAMPLES;
        
        
}

//-------------------------------------------------------------------------
// setDelay :
// set the delay in milliseconds by the delay value passed to function
//-------------------------------------------------------------------------
inline
void BasicDelayLine::setDelay(DspFloatType d){
    //receiving the delay value through here in milliseconds 0 to 2000
    delay_ms = d;
    setDelayTimeMS(44100,delay_ms);
};

//-------------------------------------------------------------------------
// setFeedback :
// set the feedback value passed to function
//-------------------------------------------------------------------------
inline
void BasicDelayLine::setFeedback(DspFloatType f){
    //receiving the feedback here as a value between -100 and +100
    feedback = f/100;
};

//-------------------------------------------------------------------------
// setMix :
// set the mix level, value from plugin is 0-100, value in function 0-1
//-------------------------------------------------------------------------
inline
void BasicDelayLine::setMix(DspFloatType m){
    // receiving the mix value through here as value between 0 and 100
    mixLevel = m/100;
};

//-------------------------------------------------------------------------
// setByPass :
// set the bypass value based on bypass value received from plugin
//-------------------------------------------------------------------------
inline
void BasicDelayLine::setByPass(bool bp){
    // receiving the bypass value as true or false
    delay_bypass = bp;
};


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//
//  The following function is the workhorse of the delay line
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
//      y(n) = x(n - D) + fb*y(n - D)       'delay with feedback
//
//      y(n) = x(n) + x(n – D) - fb*x(n-D) + fb*y(n-D)  'feedback with wet/dry mix
//
//      MAX_DELAY_SAMPLES = sr * d_ms * .001;
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
inline
DspFloatType BasicDelayLine::next(const DspFloatType in){
    
    if(delay_bypass)
        return in;
        
    //input x(n) takes the sample passed into function
    DspFloatType xn = in;
    
    //output y(n) of delay at readPos
    //once read from the buffer the read index is incremented
    DspFloatType yn = buffer[readPosA++];
    
    //if delay value is zero just pass input out, y(n) = x(n)
    if(delay_samples == 0)
        yn = xn;

    //write the input to the delay
    //  y(n) = x(n - D) + fb*y(n - D)
    buffer[writePos] = xn + feedback*yn;
    
    //create wet level and write to output buffer
    //  y(n) = x(n) + x(n – D) - fb*x(n-D) + fb*y(n-D)
    DspFloatType out = mixLevel*yn + (1.0 - mixLevel)*xn;
    
    //wrap the circular buffer back to the start if exceeds bounds
    writePos++;
    if(writePos >= MAX_DELAY_SAMPLES)
        writePos = 0;
        
    if(readPosA >= MAX_DELAY_SAMPLES)
        readPosA = 0;
        
    // return the output y(n) with feedback + wet/dry
    return out;
        
}

//-------------------------------------------------------------------------
// resetBuffer :
// delete contents of buffer and instantiate a new buffer
//-------------------------------------------------------------------------
inline
void BasicDelayLine::resetBuffer(){
    
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
inline
void BasicDelayLine::resetDelay(){
 
    if(buffer){
        memset(buffer, 0, MAX_DELAY_SAMPLES*sizeof(DspFloatType));
        //std::fill(buffer, buffer + MAX_DELAY_SAMPLES, 0.0f);
    }
    
    readPosA = writePos = 0;
    delay_bypass = 0;
    
    return;
    
}

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
inline
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
inline
DelayLine::~DelayLine(){
    delete[] buffer;
}

//getters
//-------------------------------------------------------------------------
// getDelayTimeMS :
// return the delay in milliseconds
//-------------------------------------------------------------------------
inline
DspFloatType DelayLine::getDelayTimeMS(){return delay_ms;}

//--------------------------------------------------------------------------------
//  getMaxDelayTimeMS :
//  return the Max delay time in Miliiseconds
//--------------------------------------------------------------------------------
inline
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
inline
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
inline
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
inline
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
inline
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
inline
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
inline
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
inline
void DelayLine::resetDelay(){
 
    if(buffer)
        memset(buffer, 0, MAX_DELAY_SAMPLES*sizeof(DspFloatType));
    
    readPosA = writePos = 0;
    
    return;
    
}

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
    
    void resetDelay() { delay->resetBuffer(); }
    //setters
    void setGain(const DspFloatType g);
    void setDelayTimeMS(const DspFloatType sr, const DspFloatType d_ms);
    
    //business methods
    DspFloatType next(const DspFloatType in);
    
private:
    DspFloatType gain;
    DelayLine *delay;
    
};

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


//////////////////////////////////////////////////////////
//  COMB FILTER CLASS
//  see .cpp file for comments
//////////////////////////////////////////////////////////

class Comb{
    
public:
    //constructor / destructor
    Comb(const int sr, const DspFloatType d_ms, const DspFloatType d_ms_max, const DspFloatType g, const DspFloatType cutOff=0);
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
    void resetDelay() { delay->resetBuffer(); }

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
inline Comb::Comb(const int sr, const DspFloatType d_ms, const DspFloatType d_ms_max, const DspFloatType g, const DspFloatType lp_freq){
    gain = g;
    delay = new DelayLine(sr, d_ms, d_ms_max);
    lpFilter = new Lowpass(44100, lp_freq);
}

//-------------------------------------------------------------------------
// Destructor :
// deletes the delay and filter
//-------------------------------------------------------------------------
inline Comb::~Comb(){
    delete delay;
    delete lpFilter;
}

//getters
//-------------------------------------------------------------------------
// getGain  :
// return the gain scalar value
//-------------------------------------------------------------------------
inline DspFloatType Comb::getGain(){return gain;}

//-------------------------------------------------------------------------
// getDelayTimeMS  :
// return the delay time in milliseconds
//-------------------------------------------------------------------------
inline DspFloatType Comb::getDelayTimeMS(){return delay->getDelayTimeMS();}

//setters
//-------------------------------------------------------------------------
// setGain  :
// set the scalar value for the gain of the filter
//-------------------------------------------------------------------------
inline void Comb::setGain(const DspFloatType g){gain = g;}

//--------------------------------------------------------------------------------
//  setDelayTimeMS
//  Setter function sets delay time and from milliseconds
//  and passes it to the delayline which converts to discrete time samples
//
//--------------------------------------------------------------------------------
inline void Comb::setDelayTimeMS(const DspFloatType sr, const DspFloatType d_ms){return delay->setDelayTimeMS(sr, d_ms);}

//--------------------------------------------------------------------------------
//  setLPF
//  Setter function sets the cutoff frequency of the low pass filter in the
//  feedback loop
//--------------------------------------------------------------------------------
inline void Comb::setLPF(const DspFloatType cutoff_freq){ return lpFilter->setCutoff(44100, cutoff_freq);};

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
inline DspFloatType Comb::next(const DspFloatType in){
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





class RingBuffer
{
public:
    RingBuffer(size_t max_length) :
        max_length(max_length),
        current_length(max_length),
        buffer(new DspFloatType[max_length]),
        head(max_length - 1u),
        tail(0)
    {
        for (size_t i = 0; i < max_length; ++i) {
            buffer[i] = 0.f;
        }
    }

    bool resize(size_t new_length)
    {
        if (new_length > max_length) {
            return false;
        }

        current_length = new_length;
        return true;
    }

    void update(DspFloatType sample)
    {
        buffer[head] = sample;
        head = (head + 1u) % current_length;
        tail = (tail + 1u) % current_length;
    }

    DspFloatType get(DspFloatType offset = 0u)
    {
        DspFloatType integerPart;
        DspFloatType fracPart = modf(offset, &integerPart);

        return (1.f - fracPart) * buffer[(tail + (uint8_t)offset) % current_length] +
            (fracPart)*buffer[(tail + (uint8_t)offset + 1u) % current_length];
    }

private:
    DspFloatType* buffer;
    size_t max_length;
    size_t current_length;
    size_t head;
    size_t tail;
};

class CombFilter
{
public:
    CombFilter(size_t max_length = 44100) :
        buffer(RingBuffer(max_length))
    {
        buffer.resize(1000);
    }

    bool resize(size_t new_length)
    {
        return buffer.resize(new_length);
    }

    DspFloatType processSample(DspFloatType sample)
    {
        for (int i = 0; i < 8; ++i) {
            buffer.update(sample);
            sample = 0.5f * sample + 0.5f * buffer.get();
        }
        return sample;
    }

private:
    RingBuffer buffer;
};



//////////////////////////////////////////////////////////
//  CROSS DELAY LINE CLASS (feedback optional)
//////////////////////////////////////////////////////////

class CrossDelayLine{

public:
    //constructor / destructor
    CrossDelayLine(const int sr = 44100, const DspFloatType d_ms = 0.0f, const DspFloatType feedback = 0.0f, const DspFloatType mixLevel = 0.5f, const bool crossFeedback = false);
    ~CrossDelayLine();
    
    //getters
    DspFloatType getDelayTimeMS();
    DspFloatType getMaxDelayTimeMS();
    DspFloatType getFeedback();
    DspFloatType getMix();
    bool getByPass();
    DspFloatType getCurrentFeedbackOutput();
    bool isCrossedFeedback();
    
    //setters
    void setDelayTimeMS(const int sr, const DspFloatType d_ms);
    void setDelay(DspFloatType d_ms);
    void setFeedback(DspFloatType f_pct);
    void setMix(DspFloatType m_pct);
    void setByPass(bool bp);
    void setCurrentFeedbackInput(DspFloatType fb);
    void setUseExternalFeedback(bool b);
    void setCrossedFeedback(bool b);
    
    //business functions
    DspFloatType next(const DspFloatType in);
    void resetBuffer();
    void resetDelay();

private:
    int writePos, readPosA, MAX_DELAY_SAMPLES;
    DspFloatType delay_ms, delay_samples, fraction, feedback, mixLevel, MAX_DELAY_MS, feedbackIn;
    bool delay_bypass, useExternalFeedback, crossFeedback;
    DspFloatType *buffer;
    
};


//////////////////////////////////////////////////////////
//  CROSS  DELAY LINE CLASS (feedback optional)
//////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
// Constructor :
// Predefined sample rate = 44100, feedback level & delay = 0, mixlevel=50%
// cross parameter to determine if feedback paths should be crosed default
// set read and write index for delay line
// set max delay to 2 seconds
// create a max delay buffer
//-------------------------------------------------------------------------
inline CrossDelayLine::CrossDelayLine(const int sr, const DspFloatType d_ms, const DspFloatType fb, const DspFloatType mix, const bool cross){
    //assert(d_ms <= d_ms_max);//check bound on delay time
    
    buffer = NULL;
    readPosA = writePos = feedback = mixLevel = 0;
    delay_bypass = 0;
    //do not use external feedback by default
    useExternalFeedback = false;
    
    //default max delay of 2 seconds
    DspFloatType d_ms_max = 2000.0f;
    delay_samples = 0.0f;
    delay_ms = d_ms;
   
    feedback = fb;
    mixLevel = mix;
    //crossed feedback boolean for crossing feedback paths
    crossFeedback = cross;
    
    MAX_DELAY_SAMPLES = ceil(numSamplesFromMSf(sr, d_ms_max));
    MAX_DELAY_MS = MAX_DELAY_SAMPLES * 1000.0f / sr; //make sure DspFloatType version is set with to integer-rounded buffer size
    
    //number of delay samples
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
inline CrossDelayLine::~CrossDelayLine(){
    delete[] buffer;
}

//getters
//-------------------------------------------------------------------------
// getDelayTimeMS :
// return the delay in milliseconds
//-------------------------------------------------------------------------
inline DspFloatType CrossDelayLine::getDelayTimeMS(){return delay_ms;}

//-------------------------------------------------------------------------
// getFeedback :
// return the delay value
//-------------------------------------------------------------------------
inline DspFloatType CrossDelayLine::getFeedback(){return feedback;}

//-------------------------------------------------------------------------
// getMix :
// return the mix level, set to 50% by default
//-------------------------------------------------------------------------
inline DspFloatType CrossDelayLine::getMix(){return mixLevel;}
    
//-------------------------------------------------------------------------
// getByPass :
// return the boolean value indicating if plugin is bypassed or not
//-------------------------------------------------------------------------
inline bool CrossDelayLine::getByPass(){return delay_bypass;}

//-------------------------------------------------------------------------
// getCurrentFeedbackOutput :
// return the delay value * feedback value
//-------------------------------------------------------------------------
inline DspFloatType CrossDelayLine::getCurrentFeedbackOutput(){
        //current feedback is feedback*output
        return (feedback*buffer[readPosA]);
};

//-------------------------------------------------------------------------
// isCrossedFeedbackOutput :
// return a boolean to determine if feedback paths are currently crossed
//-------------------------------------------------------------------------
inline bool CrossDelayLine::isCrossedFeedback(){return crossFeedback;}

//setters
//--------------------------------------------------------------------------------
//  Setter function that determines read position index
//  read position is determined by subtracting the number of samples to delay
//  from the write position index
//
//  readIndex = writeIndex - number of sample delay
//
//--------------------------------------------------------------------------------
inline void CrossDelayLine::setDelayTimeMS(const int sr, const DspFloatType d_ms){
    assert(d_ms <= MAX_DELAY_MS);//check bound on delay time
    
    //d_ms = d_ms;
    DspFloatType delay_samplesf = numSamplesFromMSf(sr, d_ms);
    delay_samples = floor(delay_samplesf);
    
    //storing fractional delay time, output will be interpolated
    fraction = delay_samplesf - delay_samples;
    
    //determine read index and wrap if bounds exceeded
    readPosA = writePos - (int)delay_samples;
    if(readPosA < 0)
        readPosA += MAX_DELAY_SAMPLES;
        
        
}

//-------------------------------------------------------------------------
// setDelay :
// set the delay in milliseconds by the delay value passed to function
//-------------------------------------------------------------------------
inline void CrossDelayLine::setDelay(DspFloatType d){
    //receiving the delay value through here in milliseconds 0 to 2000
    delay_ms = d;
    setDelayTimeMS(44100,delay_ms);
};

//-------------------------------------------------------------------------
// setFeedback :
// set the feedback value passed to function, -1 to 1
//-------------------------------------------------------------------------
inline void CrossDelayLine::setFeedback(DspFloatType f){
    //receiving the feedback here as a value between -100 and +100
    feedback = f/100;
};

//-------------------------------------------------------------------------
// setMix :
// set the mix level, value from plugin is 0-100, value in function 0-1
//-------------------------------------------------------------------------
inline void CrossDelayLine::setMix(DspFloatType m){
    // receiving the mix value through here as value between 0 and 100
    mixLevel = m/100;
};

//-------------------------------------------------------------------------
// setByPass :
// set the bypass value based on bypass value received from plugin
//-------------------------------------------------------------------------
inline void CrossDelayLine::setByPass(bool bp){
    // receiving the mix value through here as value between 0 and 100
    delay_bypass = bp;
};

//-------------------------------------------------------------------------
// setCurrentFeedbackInput :
// used when crossing feedback lines to provide a feedback input signal
//-------------------------------------------------------------------------
inline void CrossDelayLine::setCurrentFeedbackInput(DspFloatType fb){
    //set the feedback sample
    feedbackIn = fb;
};
    
//-------------------------------------------------------------------------
// setUseExternalFeedback :
// used when crossing feedback lines to tell delay to take its feedback
// signal externally
//-------------------------------------------------------------------------
inline void CrossDelayLine::setUseExternalFeedback(bool b){
    //enable or disable an external feedback source
    useExternalFeedback = b;
};

//-------------------------------------------------------------------------
// setCrossedFeedback :
// set the use of the external feedback based on crossFeedback value
//-------------------------------------------------------------------------
inline void CrossDelayLine::setCrossedFeedback(bool b){
    //enable or disable crossed-feedback delay lines
    crossFeedback = b;
    if(crossFeedback)
        setUseExternalFeedback(true);
    else
        setUseExternalFeedback(false);
};

//business function
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//
//  The following function is the workhorse of the delay line
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
//      y(n) = x(n - D) + fb*y(n - D)       'delay with feedback
//
//      y(n) = x(n) + x(n – D) - fb*x(n-D) + fb*y(n-D)  'feedback with wet/dry mix
//
//      MAX_DELAY_SAMPLES = sr * d_ms * .001;
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
inline DspFloatType CrossDelayLine::next(const DspFloatType in){
    
    if(delay_bypass)
        return in;
        
    //input
    DspFloatType xn = in;
    
    //output of delay at readPos
    DspFloatType yn = buffer[readPosA];
    
    //if delay < 1 sample, interpolate between x(n) and x(n-1)
    if(readPosA == writePos && delay_samples < 1.00)
    {
            yn = xn;
    }
    
    //read location at n-1, one behind yn
    int readPos_minus1 = readPosA - 1;
    if(readPos_minus1 < 0)
        readPos_minus1 = MAX_DELAY_SAMPLES - 1;   //MAX_DELAY_SAMPLES - 1 is the last location of the buffer
        
    //get y(n-1)
    DspFloatType yn_minus1 = buffer[readPos_minus1];
    
    //perform linear interpolation of : (0,yn) and (1,yn_minus1) by the ammount of fractional delay(fraction)
    DspFloatType interp = linInterp(0, 1, yn, yn_minus1, fraction);
    
    //if delay value is zero just pass input out
    if(delay_samples == 0)
        yn = xn;
    else
        yn = interp;

    //write the input to the delay
    //check if External Feedback path is enabled
    //if enabled then you write the input plus the 
    //feedbaclIn signal to the delay line instead
    if(!useExternalFeedback)
        buffer[writePos] = xn + feedback*yn;
    else
        buffer[writePos] = xn + feedbackIn;
    
    //create wet level and write to output buffer
    DspFloatType out = mixLevel*yn + (1.0 - mixLevel)*xn;
    
    //wrap if bounds exceeded
    writePos++;
    if(writePos >= MAX_DELAY_SAMPLES)
        writePos = 0;
        
    readPosA++;
    if(readPosA >= MAX_DELAY_SAMPLES)
        readPosA = 0;
        
    
    return out;
        
}

//-------------------------------------------------------------------------
// resetBuffer :
// delete contents of buffer and instantiate a new buffer
//-------------------------------------------------------------------------
inline void CrossDelayLine::resetBuffer(){
    
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
inline void CrossDelayLine::resetDelay(){
 
    if(buffer)
        memset(buffer, 0, MAX_DELAY_SAMPLES*sizeof(DspFloatType));
    
    
    readPosA = writePos = 0;
    delay_bypass = 0;
    
    return;
    
}




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
inline ERTapDelayLine::ERTapDelayLine(const int sr, const DspFloatType d_ms, const DspFloatType d_max){
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
inline ERTapDelayLine::~ERTapDelayLine(){
    delete[] buffer;
}

//getters
//-------------------------------------------------------------------------
// getDelayTimeMS :
// return the delay in milliseconds
//-------------------------------------------------------------------------
inline DspFloatType ERTapDelayLine::getDelayTimeMS(){return delay_ms;}

//--------------------------------------------------------------------------------
//  getMaxDelayTimeMS :
//  return the Max delay time in Miliiseconds
//--------------------------------------------------------------------------------
inline DspFloatType ERTapDelayLine::getMaxDelayTimeMS(){return MAX_DELAY_MS;}

//setters
//--------------------------------------------------------------------------------
//  Setter function that determines read position index
//  read position is determined by subtracting the number of samples to delay
//  from the write position index
//
//  readIndex = writeIndex - number of sample delay
//
//--------------------------------------------------------------------------------
inline void ERTapDelayLine::setDelayTimeMS(const int sr, const DspFloatType d_ms){
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
inline void ERTapDelayLine::setDelay(DspFloatType d){
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
inline DspFloatType ERTapDelayLine::readDelay(DspFloatType in){

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
inline void ERTapDelayLine::writeDelay(DspFloatType in){
    
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
inline DspFloatType ERTapDelayLine::next(const DspFloatType in){
        
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
inline void ERTapDelayLine::resetBuffer(){
    
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
inline void ERTapDelayLine::resetDelay(){
 
    if(buffer)
        memset(buffer, 0, MAX_DELAY_SAMPLES*sizeof(DspFloatType));
    
    readPosA = writePos = 0;
    
    return;
    
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


#undef NUM_COMBS 
#undef NUM_ALLPASSES 
#undef NUM_LOWPASSES 


//////////////////////////////////////////////////////////
//  StereoMoorer REVERB
//  see .cpp file for comments
//////////////////////////////////////////////////////////

//predefined values for filter numbers
#define NUM_COMBS 6
#define NUM_ALLPASSES 1
#define NUM_LOWPASSES 6

class StereoMoorer{

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
    //  LD      : Late Delay ration between onset of late tail and ER
    //--------------------------------------------------------------
    StereoMoorer(const int sr = 44100, const DspFloatType rt60 = 3.0,
              const DspFloatType cDelay1 = 50.0, const DspFloatType cDelay2 = 56.0, const DspFloatType cDelay3 = 61.0, const DspFloatType cDelay4 = 68.0,
              const DspFloatType cDelay5 = 72.0, const DspFloatType cDelay6 = 78.0,
              const DspFloatType aDelay1 = 6.0, const DspFloatType aGain1 = 0.707,
              const DspFloatType lCutoff1 = 4942.0f, const DspFloatType lCutoff2 = 4363.0f, const DspFloatType lCutoff3 = 4312.0f,
              const DspFloatType lCutoff4 = 4574.0f, const DspFloatType lCutoff5 = 3981.0f, const DspFloatType lCutoff6 = 4036.0f, DspFloatType ld = 10.0f);
    ~StereoMoorer();
    
    //getters
    DspFloatType getDecayFactor();
    DspFloatType getCombDelay(const int id);
    DspFloatType getAllpassDelay(const int id);
    DspFloatType getAllpassGain(const int id);
    bool getBypass();
    
    //getters for seperate stage outputs used when stereo summing
    DspFloatType getEROutput(const DspFloatType in);
    DspFloatType getCombOutput(const DspFloatType in, const int id);
    DspFloatType getAllpassOutput(const DspFloatType in, const int id);
    DspFloatType getLR(DspFloatType in);
    
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
    DspFloatType decayFactor, ALLPASS_GAIN_LIMIT = 0.707f, lp_freq, lateDelay; // GAIN LIMIT to keep the allpasses from exploding
    bool bypass;
    Comb *combs[NUM_COMBS];
    Allpass *allpasses[NUM_ALLPASSES];
    ERTapDelayLine *ergenerator[1];
    DelayLine *LRDelay; 

};


//////////////////////////////////////////////////////////
//  StereoMoorer REVERB
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
/*
inline DspFloatType calcCombGain(const DspFloatType d_ms, const DspFloatType rt60){
    return pow(10.0, ((-3.0 * d_ms) / (rt60 * 1000.0)));

}
*/

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
//  LD      : Late Delay ration between onset of late tail and ER
//--------------------------------------------------------------
StereoMoorer::StereoMoorer(const int sr, const DspFloatType rt60,
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
StereoMoorer::~StereoMoorer(){

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
DspFloatType StereoMoorer::getDecayFactor(){return decayFactor;}

//-------------------------------------------------------------------------
// getCombDelay : comb id
// get the specified delay time in milliseconds of the indexed comb filter
//-------------------------------------------------------------------------
DspFloatType StereoMoorer::getCombDelay(const int id){return combs[id]->getDelayTimeMS();}

//-------------------------------------------------------------------------
// getAllpassDelay : allpass id
// get the specified delay time in milliseconds of the indexed allpass filter
//-------------------------------------------------------------------------
DspFloatType StereoMoorer::getAllpassDelay(const int id){return allpasses[id]->getDelayTimeMS();}

//-------------------------------------------------------------------------
// getAllpassGain : comb id
// get the specified gain scalar value of the indexed comb filter
//-------------------------------------------------------------------------
DspFloatType StereoMoorer::getAllpassGain(const int id){return allpasses[id]->getGain();}

//-------------------------------------------------------------------------
// getBypass : 
// return the status of the boolean for bypassing the plugin processing
//-------------------------------------------------------------------------
bool StereoMoorer::getBypass(){return bypass;}

//-------------------------------------------------------------------------
// getEROutput : 
// return the output stage of the ER reflections which consists of a 
// 19 tap delay, the first tap is the input signal itself
// used to get individual outputs for summing in stereo
//-------------------------------------------------------------------------
DspFloatType StereoMoorer::getEROutput(const DspFloatType in){
    return ergenerator[0]->next(in);
}

//-------------------------------------------------------------------------
// getCombOutput : 
// return the output stage of the specific Comb Filter specified by the id
// used to get individual outputs for summing in stereo
//-------------------------------------------------------------------------
DspFloatType StereoMoorer::getCombOutput(DspFloatType in, int id){
    return combs[id]->next(in);
}

//-------------------------------------------------------------------------
// getEROutput : 
// return the output stage of the specific Allpass Filter specified by the id
// used to get individual outputs for summing in stereo
//-------------------------------------------------------------------------
DspFloatType StereoMoorer::getAllpassOutput(DspFloatType in, int id){
    return allpasses[id]->next(in);
}

//-------------------------------------------------------------------------
// getLR : 
// delay the late reflection part of the reverb tail and return output
//-------------------------------------------------------------------------
DspFloatType StereoMoorer::getLR(DspFloatType in){
    return LRDelay->next(in);
}

//setters
//-------------------------------------------------------------------------
// setDecayFactor : decayfactor value in seconds
// decay time/desired RT60 is passed from UI to this function
// and the required comb filter gain values that will adhere to that RT60
// are calculated based on this factor
//-------------------------------------------------------------------------
void StereoMoorer::setDecayFactor(const DspFloatType df){
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
void StereoMoorer::setCombDelay(const int id, const DspFloatType sr, const DspFloatType d_ms){
    combs[id]->setGain(calcCombGain(d_ms, decayFactor));
    combs[id]->setDelayTimeMS(sr, d_ms);
}

//-------------------------------------------------------------------------
// setAllpassGain : id of comb, gain
// sets the gain for the allpass filter, scaling by the GAIN_LIMIT so as
// not to blow the filter up due to the unstable nature of IIR filters
//-------------------------------------------------------------------------
void StereoMoorer::setAllpassGain(const int id, const DspFloatType g){allpasses[id]->setGain(g * ALLPASS_GAIN_LIMIT);}

//-------------------------------------------------------------------------
// setAllpassDelay : id of comb, sample rate, delay in milliseconds
// sets the delay time in milliseconds of the all pass delay line
//-------------------------------------------------------------------------
void StereoMoorer::setAllpassDelay(const int id, const int sr, const DspFloatType d_ms){allpasses[id]->setDelayTimeMS(sr, d_ms);}

//-------------------------------------------------------------------------
// setBypass : boolean bypass value
// sets a boolean which determines if processing should be bypassed in the
// worker next function
//-------------------------------------------------------------------------
void StereoMoorer::setBypass(bool bp){bypass = bp;}

//-------------------------------------------------------------------------
// setLPFreq : low pass filter frequency
// sets the frequency for the low pass filter cutoff frequency coefficient
//-------------------------------------------------------------------------
void StereoMoorer::setLPFreq(const DspFloatType lpf){
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
DspFloatType StereoMoorer::next(const DspFloatType in){
    DspFloatType out = 0.0f;
    DspFloatType passOut = 0.0f;
    DspFloatType ers = 0.0f;
    DspFloatType tapOut = 0.0f;
    
    if(bypass)
        return in;
    

    //early reflections generator
    ers = ergenerator[0]->next(in);
    //tapOut = ers + in;
    tapOut = ers;
    
    //the comb filters all receive the output from the FIR stage as 
    //there input, and are scalled to prevent clipping
    for(int i = 0; i < NUM_COMBS; i++){
        //out += combs[i]->next(in * 0.125f); //scale down to avoid clipping
        out += combs[i]->next(tapOut * 0.250f);
    }
    
    //the output of the comb filter above is fed to the allpass stage
    passOut = allpasses[0]->next(out);
    
    DspFloatType lr_Shift = 0.0f;
    
    //Late Reflections generated delay to provide a slight delay in
    //the return of the late reflections reverberant tail
    lr_Shift = LRDelay->next(passOut);

    //return Early Reflections followed closely by Late Reflections
    return (tapOut*0.25f) + lr_Shift;

    
}

#undef NUM_COMBS 
#undef NUM_ALLPASSES 
#undef NUM_LOWPASSES 



//////////////////////////////////////////////////////////
//  PINGPONG DELAY LINE CLASS (feedback optional)
//  see .cpp file for comments
//////////////////////////////////////////////////////////

class PPDelayLine{

public:
    //constructor / destructor
    PPDelayLine(const int sr = 44100, const DspFloatType d_ms = 0.0f, const DspFloatType feedback = 0.0f, const DspFloatType mixLevel = 0.5f, const bool crossFeedback = true);
    ~PPDelayLine();
    
    //getters
    DspFloatType getDelayTimeMS();
    DspFloatType getMaxDelayTimeMS();
    DspFloatType getFeedback();
    DspFloatType getMix();
    bool getByPass();
    DspFloatType getCurrentFeedbackOutput();
    bool isCrossedFeedback();
    
    //setters
    void setDelayTimeMS(const int sr, const DspFloatType d_ms);
    void setDelay(DspFloatType d_ms);
    void setFeedback(DspFloatType f_pct);
    void setMix(DspFloatType m_pct);
    void setByPass(bool bp);
    void setCurrentFeedbackInput(DspFloatType fb);
    void setUseExternalFeedback(bool b);
    /*void setCrossedFeedback(bool b);*/
    
    //business functions
    DspFloatType next(const DspFloatType in);
    void resetBuffer();
    void resetDelay();

private:
    int writePos, readPosA, MAX_DELAY_SAMPLES;
    DspFloatType delay_ms, delay_samples, fraction, feedback, mixLevel, MAX_DELAY_MS, feedbackIn;
    bool delay_bypass, useExternalFeedback, crossFeedback;
    DspFloatType *buffer;
    
};


//////////////////////////////////////////////////////////
//  PING PONG  DELAY LINE CLASS (feedback optional)
//////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
// Constructor :
// Predefined sample rate = 44100, feedback level & delay = 0, mixlevel=50%
// cross parameter to determine if feedback paths should be crosed default
// set read and write index for delay line
// set max delay to 2 seconds
// create a max delay buffer
//-------------------------------------------------------------------------
PPDelayLine::PPDelayLine(const int sr, const DspFloatType d_ms, const DspFloatType fb, const DspFloatType mix, const bool cross){
    //assert(d_ms <= d_ms_max);//check bound on delay time
    
    //initalise everything
    buffer = NULL;
    writePos = readPosA = MAX_DELAY_SAMPLES = 0;
    MAX_DELAY_MS = delay_ms = delay_samples = fraction = feedback = mixLevel = feedbackIn = 0.0f;
    delay_bypass = useExternalFeedback = crossFeedback = false;
    
    //by default use external crossed feedback paths
    useExternalFeedback = true;
    
    //set max 2 seconds
    DspFloatType d_ms_max = 2000.0f;
    delay_samples = 0.0f;
    delay_ms = d_ms;
   
    feedback = fb;
    mixLevel = mix;
    //crossFeedback = true;
    crossFeedback = cross;
    
    MAX_DELAY_SAMPLES = ceil(numSamplesFromMSf(sr, d_ms_max));
    MAX_DELAY_MS = MAX_DELAY_SAMPLES * 1000.0f / sr; //make sure DspFloatType version is set with to integer-rounded buffer size
    
    //number of delay samples
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
PPDelayLine::~PPDelayLine(){
    delete[] buffer;
}

//getters
//-------------------------------------------------------------------------
// getDelayTimeMS :
// return the delay in milliseconds
//-------------------------------------------------------------------------
DspFloatType PPDelayLine::getDelayTimeMS(){return delay_ms;}

//-------------------------------------------------------------------------
// getFeedback :
// return the delay value
//-------------------------------------------------------------------------
DspFloatType PPDelayLine::getFeedback(){return feedback;}

//-------------------------------------------------------------------------
// getMix :
// return the mix level, set to 50% by default
//-------------------------------------------------------------------------
DspFloatType PPDelayLine::getMix(){return mixLevel;}
    
//-------------------------------------------------------------------------
// getByPass :
// return the boolean value indicating if plugin is bypassed or not
//-------------------------------------------------------------------------
bool PPDelayLine::getByPass(){return delay_bypass;}

//-------------------------------------------------------------------------
// getCurrentFeedbackOutput :
// return the delay value * feedback value
//-------------------------------------------------------------------------
DspFloatType PPDelayLine::getCurrentFeedbackOutput(){
        //current feedback is feedback*output
        return (feedback*buffer[readPosA]);
};


//setters
//--------------------------------------------------------------------------------
//  Setter function that determines read position index
//  read position is determined by subtracting the number of samples to delay
//  from the write position index
//
//  readIndex = writeIndex - number of sample delay
//
//--------------------------------------------------------------------------------
void PPDelayLine::setDelayTimeMS(const int sr, const DspFloatType d_ms){
    assert(d_ms <= MAX_DELAY_MS);//check bound on delay time
    
    //d_ms = d_ms;
    DspFloatType delay_samplesf = numSamplesFromMSf(sr, d_ms);
    delay_samples = floor(delay_samplesf);
    
    //storing fractional delay time, output will be interpolated
    fraction = delay_samplesf - delay_samples;
    
    //wrap index if bounds exceeded
    readPosA = writePos - (int)delay_samples;
    if(readPosA < 0)
        readPosA += MAX_DELAY_SAMPLES;
        
        
}

//-------------------------------------------------------------------------
// setDelay :
// set the delay in milliseconds by the delay value passed to function
//-------------------------------------------------------------------------
void PPDelayLine::setDelay(DspFloatType d){
    //receiving the delay value through here in milliseconds 0 to 2000
    delay_ms = d;
    setDelayTimeMS(44100,delay_ms);
};

//-------------------------------------------------------------------------
// setFeedback :
// set the feedback value passed to function, -1 to 1
//-------------------------------------------------------------------------
void PPDelayLine::setFeedback(DspFloatType f){
    //receiving the feedback here as a value between -100 and +100
    feedback = f/100;
};

//-------------------------------------------------------------------------
// setMix :
// set the mix level, value from plugin is 0-100, value in function 0-1
//-------------------------------------------------------------------------
void PPDelayLine::setMix(DspFloatType m){
    // receiving the mix value through here as value between 0 and 100
    mixLevel = m/100;
};

//-------------------------------------------------------------------------
// setByPass :
// set the bypass value based on bypass value received from plugin
//-------------------------------------------------------------------------
void PPDelayLine::setByPass(bool bp){
    // receiving the mix value through here as value between 0 and 100
    delay_bypass = bp;
};

//-------------------------------------------------------------------------
// setCurrentFeedbackInput :
// used when crossing feedback lines to provide a feedback input signal
//-------------------------------------------------------------------------
void PPDelayLine::setCurrentFeedbackInput(DspFloatType fb){
    //set the feedback sample
    feedbackIn = fb;
};

//-------------------------------------------------------------------------
// setUseExternalFeedback :
// used when crossing feedback lines to tell delay to take its feedback
// signal externally
//-------------------------------------------------------------------------    
void PPDelayLine::setUseExternalFeedback(bool b){
    //enable or disable an external feedback source
    useExternalFeedback = b;
};


//business function
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//
//  The following function is the workhorse of the delay line
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
//      y(n) = x(n - D) + fb*y(n - D)       'delay with feedback
//
//      y(n) = x(n) + x(n – D) - fb*x(n-D) + fb*y(n-D)  'feedback with wet/dry mix
//
//      MAX_DELAY_SAMPLES = sr * d_ms * .001;
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
DspFloatType PPDelayLine::next(const DspFloatType in){
    
    if(delay_bypass)
        return in;
        
    //input
    DspFloatType xn = in;
    
    //output of delay at readPos
    DspFloatType yn = buffer[readPosA];
    
    //if delay < 1 sample, interpolate between x(n) and x(n-1)
    if(readPosA == writePos && delay_samples < 1.00)
    {
            yn = xn;
    }
    
    //read location at n-1, one behind yn
    int readPos_minus1 = readPosA - 1;
    if(readPos_minus1 < 0)
        readPos_minus1 = MAX_DELAY_SAMPLES - 1;   //MAX_DELAY_SAMPLES - 1 is the last location of the buffer
        
    //get y(n-1)
    DspFloatType yn_minus1 = buffer[readPos_minus1];
    
    //perform linear interpolation of : (0,yn) and (1,yn_minus1) by the ammount of fractional delay(fraction)
    DspFloatType interp = linInterp(0, 1, yn, yn_minus1, fraction);
    
    //if delay value is zero just pass input out
    if(delay_samples == 0)
        yn = xn;
    else
        yn = interp;

    //write the input to the delay
    //check if External Feedback path is enabled
    //if enabled then you write the input plus the 
    //feedbaclIn signal to the delay line instead
    if(!useExternalFeedback)
        buffer[writePos] = xn + feedback*yn;
    else
        buffer[writePos] = xn + feedbackIn;
    
    //create wet level and write to output buffer
    DspFloatType out = mixLevel*yn + (1.0 - mixLevel)*xn;
    
    //wrap bounds if exceeded
    writePos++;
    if(writePos >= MAX_DELAY_SAMPLES)
        writePos = 0;
        
    readPosA++;
    if(readPosA >= MAX_DELAY_SAMPLES)
        readPosA = 0;
        
    
    return out;
        
}

//-------------------------------------------------------------------------
// resetBuffer :
// delete contents of buffer and instantiate a new buffer
//-------------------------------------------------------------------------
void PPDelayLine::resetBuffer(){
    
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
void PPDelayLine::resetDelay(){
 
    if(buffer)
        memset(buffer, 0, MAX_DELAY_SAMPLES*sizeof(DspFloatType));
    
    
    readPosA = writePos = 0;
    delay_bypass = 0;
    
    return;
    
}


//////////////////////////////////////////////////////////
//  SCHROEDER REVERB
//  see .cpp file for full comments
//////////////////////////////////////////////////////////

//predefined number of comb and allpass filters for array parsing

#define NUM_COMBS 4
#define NUM_ALLPASSES 2

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
              const DspFloatType aGain1 = 0.707, const DspFloatType aGain2 = 0.5);
    ~Schroeder();
    
    //getters
    DspFloatType getDecayFactor();
    DspFloatType getCombDelay(const int id);
    DspFloatType getAllpassDelay(const int id);
    DspFloatType getAllpassGain(const int id);
    //DspFloatType getLowpassCutoff(const int id);
    bool getBypass();
    
    //setters
    void setDecayFactor(const DspFloatType df);
    void setCombDelay(const int id, const DspFloatType sr, const DspFloatType d_ms);
    void setAllpassGain(const int id, const DspFloatType g);
    void setAllpassDelay(const int id, const int sr, const DspFloatType d_ms);
    void setBypass(bool bp);
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
//--------------------------------------------------------------
//--------------------------------------------------------------
Schroeder::Schroeder(const int sr, const DspFloatType rt60,
          const DspFloatType cDelay1, const DspFloatType cDelay2, const DspFloatType cDelay3, const DspFloatType cDelay4,
          const DspFloatType aDelay1, const DspFloatType aDelay2, const DspFloatType aGain1, const DspFloatType aGain2){
    
    decayFactor = rt60;
    DspFloatType d_ms, d_ms_max = 100.0f, gain;
    bypass = false;
    
    //Comb Filter 1 setup
    d_ms = cDelay1;
    gain = calcCombGain(d_ms, decayFactor);
    combs[0] = new Comb(sr, d_ms, d_ms_max, gain);
    setCombDelay(0,sr,d_ms);
    
    //Comb Filter 2 setup
    d_ms = cDelay2;
    gain = calcCombGain(d_ms, decayFactor);
    combs[1] = new Comb(sr, d_ms, d_ms_max, gain);
    setCombDelay(1,sr,d_ms);
    
    //Comb Filter 3 setup
    d_ms = cDelay3;
    gain = calcCombGain(d_ms, decayFactor);
    combs[2] = new Comb(sr, d_ms, d_ms_max, gain);
    setCombDelay(2,sr,d_ms);
    
    //Comb Filter 4 setup
    d_ms = cDelay4;
    gain = calcCombGain(d_ms, decayFactor);
    combs[3] = new Comb(sr, d_ms, d_ms_max, gain);
    setCombDelay(3,sr,d_ms);

    d_ms_max = 20.0f;
    
    //All-pass filter setup
    allpasses[0] = new Allpass(sr, aDelay1, d_ms_max, aGain1);
    allpasses[1] = new Allpass(sr, aDelay2, d_ms_max, aGain2);


}

//-------------------------------------------------------------------------
// Destructor :
// delete all combs and allpasses
//-------------------------------------------------------------------------
Schroeder::~Schroeder(){
    for(int i = 0; i < NUM_COMBS; i++){
        delete combs[i];
    }
    for(int i = 0; i < NUM_ALLPASSES; i++){
        delete allpasses[i];
    }
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
// getBypass : 
// return the status of the boolean for bypassing the plugin processing
//-------------------------------------------------------------------------
bool Schroeder::getBypass(){return bypass;}

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
// setBypass : boolean bypass value
// sets a boolean which determines if processing should be bypassed in the
// worker next function
//-------------------------------------------------------------------------
void Schroeder::setBypass(bool bp){bypass = bp;}

//-------------------------------------------------------------------------
// reset : 
// resets the delay lines in the combs and allpass filters
//-------------------------------------------------------------------------
void Schroeder::reset(){
    for(int i = 0; i < NUM_COMBS; i++){
        combs[i]->resetDelay();
    }
    
    for(int i = 0; i < NUM_ALLPASSES; i++){
        allpasses[i]->resetDelay();
    }
    
}

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
        out += combs[i]->next(in * 0.50f); //scale down to avoid clipping
    }
    
    DspFloatType passOut = 0.0f;
    DspFloatType passOut2 = 0.0f;
    
    passOut = allpasses[0]->next(out);          //1 stage all-pass filtering
    passOut2 = allpasses[1]->next(passOut);     //2 stage all-pass filtering
  
    return passOut2;
    
}


#undef NUM_COMBS
#undef NUM_ALLPASSES

//////////////////////////////////////////////////////////
//  SCHROEDER REVERB
//////////////////////////////////////////////////////////

#define NUM_COMBS 4
#define NUM_ALLPASSES 2
#define NUM_LOWPASSES 1

class ImprovedSchroeder{

public:
    //constructor / destructor
    ImprovedSchroeder(const int sr = 44100, const DspFloatType rt60 = 3.0,
              const DspFloatType cDelay1 = 29.7, const DspFloatType cDelay2 = 37.1, const DspFloatType cDelay3 = 41.1, const DspFloatType cDelay4 = 43.7,
              const DspFloatType aDelay1 = 5.0, const DspFloatType aDelay2 = 1.7, const DspFloatType aGain1 = 0.707, const DspFloatType aGain2 = 0.5, const DspFloatType lCutoff1 = 2300.0f);    //const DspFloatType lCutoff1 = 2300.0f
    ~ImprovedSchroeder();
    
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




inline DspFloatType calcAPGain(const DspFloatType d_ms, const DspFloatType rt60){
    return pow(10.0, ((-3.0 * d_ms) / (rt60 * 1000.0)));
}

//constructor / destructor
ImprovedSchroeder::ImprovedSchroeder(const int sr, const DspFloatType rt60,
          const DspFloatType cDelay1, const DspFloatType cDelay2, const DspFloatType cDelay3, const DspFloatType cDelay4,
          const DspFloatType aDelay1, const DspFloatType aDelay2, const DspFloatType aGain1, const DspFloatType aGain2, const DspFloatType lCutoff1){   //const DspFloatType lCutoff1
    
    decayFactor = rt60;
    DspFloatType d_ms, d_ms_max = 100.0f, gain;
    d_ms = cDelay1;
    bypass = false;
    
    gain = calcCombGain(d_ms, decayFactor);
    combs[0] = new Comb(sr, d_ms, d_ms_max, gain);
    setCombDelay(0,sr,d_ms);
    d_ms = cDelay2;
    gain = calcCombGain(d_ms, decayFactor);
    combs[1] = new Comb(sr, d_ms, d_ms_max, gain);
    setCombDelay(1,sr,d_ms);
    d_ms = cDelay3;
    gain = calcCombGain(d_ms, decayFactor);
    combs[2] = new Comb(sr, d_ms, d_ms_max, gain);
    setCombDelay(2,sr,d_ms);
    d_ms = cDelay4;
    gain = calcCombGain(d_ms, decayFactor);
    combs[3] = new Comb(sr, d_ms, d_ms_max, gain);
    setCombDelay(3,sr,d_ms);
    
    d_ms_max = 20.0f;
    //gain = calcAPGain(aDelay1, 96.83);
    allpasses[0] = new Allpass(sr, aDelay1, d_ms_max, aGain1);
    //gain = calcAPGain(aDelay2, 32.92);
    allpasses[1] = new Allpass(sr, aDelay2, d_ms_max, aGain2);
    
    lowpasses[0] = new Lowpass(sr, lCutoff1);
    //std::cout << "ImprovedSchroeder constructor called" << std::endl;

}
ImprovedSchroeder::~ImprovedSchroeder(){
    //std::cout << "ImprovedSchroeder destructor called" << std::endl;
    for(int i = 0; i < NUM_COMBS; i++){
        delete combs[i];
    }
    for(int i = 0; i < NUM_ALLPASSES; i++){
        delete allpasses[i];
    }
}

//getters
DspFloatType ImprovedSchroeder::getDecayFactor(){return decayFactor;}
DspFloatType ImprovedSchroeder::getCombDelay(const int id){return combs[id]->getDelayTimeMS();}
DspFloatType ImprovedSchroeder::getAllpassDelay(const int id){return allpasses[id]->getDelayTimeMS();}
DspFloatType ImprovedSchroeder::getAllpassGain(const int id){return allpasses[id]->getGain();}
DspFloatType ImprovedSchroeder::getLowpassCutoff(const int id){return lowpasses[id]->getCutoff();}
bool  ImprovedSchroeder::getBypass(){return bypass;}

//setters
void ImprovedSchroeder::setDecayFactor(const DspFloatType df){
    decayFactor = df;
    for(int i = 0; i < NUM_COMBS; i++){
        combs[i]->setGain(calcCombGain(combs[i]->getDelayTimeMS(), decayFactor));
    }
};

void ImprovedSchroeder::setCombDelay(const int id, const DspFloatType sr, const DspFloatType d_ms){
    combs[id]->setGain(calcCombGain(d_ms, decayFactor));
    combs[id]->setDelayTimeMS(sr, d_ms);
}

void ImprovedSchroeder::setAllpassGain(const int id, const DspFloatType g){allpasses[id]->setGain(g * ALLPASS_GAIN_LIMIT);}
void ImprovedSchroeder::setAllpassDelay(const int id, const int sr, const DspFloatType d_ms){allpasses[id]->setDelayTimeMS(sr, d_ms);}
void ImprovedSchroeder::setLowpassCutoff(const int id, const int sr, const DspFloatType cf_hz){lowpasses[id]->setCutoff(sr, cf_hz);}
void ImprovedSchroeder::setBypass(bool bp){bypass = bp;}

//business methods
DspFloatType ImprovedSchroeder::next(const DspFloatType in){
    
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



//////////////////////////////////////////////////////////
//  SCHROEDER REVERB
//////////////////////////////////////////////////////////

//predefined number of comb and allpass filters for array parsing
//plus one all pass filter
#define NUM_COMBS 4
#define NUM_ALLPASSES 2
#define NUM_LOWPASSES 1

class StereoSchroeder{

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
    StereoSchroeder(const int sr = 44100, const DspFloatType rt60 = 3.0,
              const DspFloatType cDelay1 = 29.7, const DspFloatType cDelay2 = 37.1, const DspFloatType cDelay3 = 41.1, const DspFloatType cDelay4 = 43.7,
              const DspFloatType aDelay1 = 5.0, const DspFloatType aDelay2 = 1.7, const DspFloatType aGain1 = 0.07, const DspFloatType aGain2 = 0.5, const DspFloatType lCutoff1 = 2300.0f);    //const DspFloatType lCutoff1 = 2300.0f
    ~StereoSchroeder();
    
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
StereoSchroeder::StereoSchroeder(const int sr, const DspFloatType rt60,
          const DspFloatType cDelay1, const DspFloatType cDelay2, const DspFloatType cDelay3, const DspFloatType cDelay4,
          const DspFloatType aDelay1, const DspFloatType aDelay2, const DspFloatType aGain1, const DspFloatType aGain2, const DspFloatType lCutoff1){   //const DspFloatType lCutoff1
    
    decayFactor = rt60;
    DspFloatType d_ms, d_ms_max = 100.0f, gain;
    bypass = false;
    
    //Comb Filter 1 setup
    d_ms = cDelay1;
    gain = calcCombGain(d_ms, decayFactor);
    combs[0] = new Comb(sr, d_ms, d_ms_max, gain);
    setCombDelay(0,sr,d_ms);
    
    //Comb Filter 2 setup
    d_ms = cDelay2;
    gain = calcCombGain(d_ms, decayFactor);
    combs[1] = new Comb(sr, d_ms, d_ms_max, gain);
    setCombDelay(1,sr,d_ms);
    
    //Comb Filter 3 setup
    d_ms = cDelay3;
    gain = calcCombGain(d_ms, decayFactor);
    combs[2] = new Comb(sr, d_ms, d_ms_max, gain);
    setCombDelay(2,sr,d_ms);
    
    //Comb Filter 4 setup
    d_ms = cDelay4;
    gain = calcCombGain(d_ms, decayFactor);
    combs[3] = new Comb(sr, d_ms, d_ms_max, gain);
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
StereoSchroeder::~StereoSchroeder(){
    //std::cout << "StereoSchroeder destructor called" << std::endl;
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
DspFloatType StereoSchroeder::getDecayFactor(){return decayFactor;}

//-------------------------------------------------------------------------
// getCombDelay : comb id
// get the specified delay time in milliseconds of the indexed comb filter
//-------------------------------------------------------------------------
DspFloatType StereoSchroeder::getCombDelay(const int id){return combs[id]->getDelayTimeMS();}

//-------------------------------------------------------------------------
// getAllpassDelay : allpass id
// get the specified delay time in milliseconds of the indexed allpass filter
//-------------------------------------------------------------------------
DspFloatType StereoSchroeder::getAllpassDelay(const int id){return allpasses[id]->getDelayTimeMS();}

//-------------------------------------------------------------------------
// getAllpassGain : comb id
// get the specified gain scalar value of the indexed comb filter
//-------------------------------------------------------------------------
DspFloatType StereoSchroeder::getAllpassGain(const int id){return allpasses[id]->getGain();}

//-------------------------------------------------------------------------
// getLowpassCutoff : 
// return the cutoff frequency for the low pass filter
//-------------------------------------------------------------------------
DspFloatType StereoSchroeder::getLowpassCutoff(const int id){return lowpasses[id]->getCutoff();}

//-------------------------------------------------------------------------
// getBypass : 
// return the status of the boolean for bypassing the plugin processing
//-------------------------------------------------------------------------
bool StereoSchroeder::getBypass(){return bypass;}

//-------------------------------------------------------------------------
// getCombOutput : input sample, comb id 
// return the output of the comb filter at sample passed in
// this function was added to be able to tap into the output from each
// comb individually. 
// This will be used when incorporating a summing matrix for the output
// to have control over how each individual filter is summed
//-------------------------------------------------------------------------
DspFloatType StereoSchroeder::getCombOutput(DspFloatType in, int id){
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
DspFloatType StereoSchroeder::getAllpassOutput(DspFloatType in, int id){
    return allpasses[id]->next(in);
}

//setters
//-------------------------------------------------------------------------
// setDecayFactor : decayfactor value in seconds
// decay time/desired RT60 is passed from UI to this function
// and the required comb filter gain values that will adhere to that RT60
// are calculated based on this factor
//-------------------------------------------------------------------------
void StereoSchroeder::setDecayFactor(const DspFloatType df){
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
void StereoSchroeder::setCombDelay(const int id, const DspFloatType sr, const DspFloatType d_ms){
    combs[id]->setGain(calcCombGain(d_ms, decayFactor));
    combs[id]->setDelayTimeMS(sr, d_ms);
}

//-------------------------------------------------------------------------
// setAllpassGain : id of comb, gain
// sets the gain for the allpass filter, scaling by the GAIN_LIMIT so as
// not to blow the filter up due to the unstable nature of IIR filters
//-------------------------------------------------------------------------
void StereoSchroeder::setAllpassGain(const int id, const DspFloatType g){allpasses[id]->setGain(g * ALLPASS_GAIN_LIMIT);}

//-------------------------------------------------------------------------
// setAllpassDelay : id of comb, sample rate, delay in milliseconds
// sets the delay time in milliseconds of the all pass delay line
//-------------------------------------------------------------------------
void StereoSchroeder::setAllpassDelay(const int id, const int sr, const DspFloatType d_ms){allpasses[id]->setDelayTimeMS(sr, d_ms);}

//-------------------------------------------------------------------------
// setLowpassCutoff : id lowpass, sample rate, cutoff frequency
// sets the cutoff frequency for the one-pole low pass filter
//-------------------------------------------------------------------------
void StereoSchroeder::setLowpassCutoff(const int id, const int sr, const DspFloatType cf_hz){lowpasses[id]->setCutoff(sr, cf_hz);}

//-------------------------------------------------------------------------
// setBypass : boolean bypass value
// sets a boolean which determines if processing should be bypassed in the
// worker next function
//-------------------------------------------------------------------------
void StereoSchroeder::setBypass(bool bp){bypass = bp;}

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
DspFloatType StereoSchroeder::next(const DspFloatType in){

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

#undef NUM_COMBS
#undef NUM_ALLPASSES
#undef NUM_LOWPASSES

//////////////////////////////////////////////////////////
//  SyncedTap DELAY LINE CLASS (feedback optional)
//  see .cpp file for commenting
//////////////////////////////////////////////////////////

class SyncedTapDelayLine{

public:
    //constructor / destructor
    SyncedTapDelayLine(const int sr = 44100, const DspFloatType d_ms = 0.0f, const DspFloatType feedback = 0.0f, const DspFloatType mixLevel = 0.5f);
    ~SyncedTapDelayLine();
    
    //getters
    DspFloatType getDelayTimeMS();
    DspFloatType getMaxDelayTimeMS();
    DspFloatType getDelay();
    DspFloatType getFeedback();
    DspFloatType getTap1Delay();
    DspFloatType getTap2Delay();
    DspFloatType getTap3Delay();
    DspFloatType getTap4Delay();
    DspFloatType getTap1Level();
    DspFloatType getTap2Level();
    DspFloatType getTap3Level();
    DspFloatType getTap4Level();
    DspFloatType getMix();
    bool getByPass();
    DspFloatType getCurrentFeedbackOutput();
    
    //setters
    void setDelayTimeMS(const int sr, const DspFloatType d_ms);
    void setDelay(DspFloatType d_ms);
    void setTap1Delay(DspFloatType td);
    void setTap2Delay(DspFloatType td);
    void setTap3Delay(DspFloatType td);
    void setTap4Delay(DspFloatType td);
    void setTap1Level(DspFloatType tl);
    void setTap2Level(DspFloatType tl);
    void setTap3Level(DspFloatType tl);
    void setTap4Level(DspFloatType tl);    
    void setFeedback(DspFloatType f_pct);
    void setMix(DspFloatType m_pct);
    void setByPass(bool bp);
    void setCurrentFeedbackInput(DspFloatType fb);
    void setUseExternalFeedback(bool b);
    void setDelaySync(DspFloatType quarter, DspFloatType eighth, DspFloatType sixtenth);
    
    //business functions
    DspFloatType next(const DspFloatType in);
    void resetBuffer();
    void resetDelay();

private:
    int writePos, readPosA, readTap1, readTap2, readTap3, readTap4, MAX_DELAY_SAMPLES;
    DspFloatType delay_ms, delay_samples, fraction, feedback, mixLevel, MAX_DELAY_MS, feedbackIn, 
        tap1delay, tap2delay, tap3delay, tap4delay, tap1level, tap2level, tap3level, tap4level;
    bool delay_bypass, useExternalFeedback;
    DspFloatType *buffer;
    
};



//////////////////////////////////////////////////////////
//  SyncedTap VARIABLE DELAY LINE CLASS (feedback optional)
//////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
// Constructor :
// Predefined sample rate = 44100, feedback level & delay = 0, mixlevel=50%
// set read and write index for delay line
// set read indexes for each of the 4 tap delays
// set the delay and gain level for each of the 4 tap delays
// set max delay to 2 seconds
// create a max delay buffer
//-------------------------------------------------------------------------
SyncedTapDelayLine::SyncedTapDelayLine(const int sr, const DspFloatType d_ms, const DspFloatType fb, const DspFloatType mix){
    //assert(d_ms <= d_ms_max);//check bound on delay time
    
    buffer = NULL;
    readPosA = readTap1 = readTap2 = readTap3 = readTap4 = writePos = feedback = mixLevel = 0;
    tap1delay = tap2delay = tap3delay = tap4delay = tap1level = tap2level = tap3level = tap4level = 0.0f;
    delay_bypass = 0;
    useExternalFeedback = false;
    
    //max delay of 2 seconds
    DspFloatType d_ms_max = 2000.0f;
    delay_samples = 0.0f;
    delay_ms = d_ms;
   
    feedback = fb;
    mixLevel = mix;
    
    MAX_DELAY_SAMPLES = ceil(numSamplesFromMSf(sr, d_ms_max));
    MAX_DELAY_MS = MAX_DELAY_SAMPLES * 1000.0f / sr; //make sure DspFloatType version is set with to integer-rounded buffer size
    
    //number of delay samples
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
SyncedTapDelayLine::~SyncedTapDelayLine(){
    delete[] buffer;
}

//getters
//-------------------------------------------------------------------------
// getDelayTimeMS :
// return the delay in milliseconds
//-------------------------------------------------------------------------
DspFloatType SyncedTapDelayLine::getDelayTimeMS(){return delay_ms;}

//-------------------------------------------------------------------------
// getFeedback :
// return the delay value
//-------------------------------------------------------------------------
DspFloatType SyncedTapDelayLine::getDelay(){return delay_ms;}

//-------------------------------------------------------------------------
// getFeedback :
// return the feedback value
//-------------------------------------------------------------------------
DspFloatType SyncedTapDelayLine::getFeedback(){return feedback;}

//-------------------------------------------------------------------------
// getMix :
// return the mix level, set to 50% by default
//-------------------------------------------------------------------------
DspFloatType SyncedTapDelayLine::getMix(){return mixLevel;}

//-------------------------------------------------------------------------
// getByPass :
// return the boolean value indicating if plugin is bypassed or not
//-------------------------------------------------------------------------
bool SyncedTapDelayLine::getByPass(){return delay_bypass;}

//-------------------------------------------------------------------------
// getCurrentFeedbackOutput :
// return the delay value * feedback value
//-------------------------------------------------------------------------
DspFloatType SyncedTapDelayLine::getCurrentFeedbackOutput(){
        //current feedback is feedback*output
        return (feedback*buffer[readPosA]);
};

//setters
//--------------------------------------------------------------------------------
//  Setter function that determines read position index
//  read position is determined by subtracting the number of samples to delay
//  from the write position index
//
//  readIndex = writeIndex - number of sample delay
//
//--------------------------------------------------------------------------------
void SyncedTapDelayLine::setDelayTimeMS(const int sr, const DspFloatType d_ms){
    assert(d_ms <= MAX_DELAY_MS);//check bound on delay time
    
    //d_ms = d_ms;
    DspFloatType delay_samplesf = numSamplesFromMSf(sr, d_ms);
    delay_samples = floor(delay_samplesf);
    fraction = delay_samplesf - delay_samples;//storing fractional delay time, output will be interpolated
    
    readPosA = writePos - (int)delay_samples;
    
    if(readPosA < 0)
        readPosA += MAX_DELAY_SAMPLES;       
        
}

//-------------------------------------------------------------------------
// setDelay :
// set the delay in milliseconds by the delay value passed to function
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setDelay(DspFloatType d){
    //receiving the delay value through here in milliseconds 0 to 2000
    delay_ms = d;
    setDelayTimeMS(44100,delay_ms);
};

//-------------------------------------------------------------------------
// setTap1Delay :
// set the delay and tap1 read index position in the delay buffer
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setTap1Delay(DspFloatType td){
    //receiving the delay value through here in milliseconds 0 to 2000
    tap1delay = td;
    DspFloatType tap_samples = numSamplesFromMSf(44100, tap1delay);
    readTap1 = writePos - (int)tap_samples;
    
    //wrap if out of bounds
    if(readTap1 < 0)
        readTap1 += MAX_DELAY_SAMPLES;       
        
};

//-------------------------------------------------------------------------
// setTap2Delay :
// set the delay and tap2 read index position in the delay buffer
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setTap2Delay(DspFloatType d){
    //receiving the delay value through here in milliseconds 0 to 2000
    tap2delay = d;
    DspFloatType tap_samples = numSamplesFromMSf(44100, tap2delay);
    readTap2 = writePos - (int)tap_samples;
    
    //wrap if out of bounds
    if(readTap2 < 0)
        readTap2 += MAX_DELAY_SAMPLES; 
};

//-------------------------------------------------------------------------
// setTap3Delay :
// set the delay and tap3 read index position in the delay buffer
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setTap3Delay(DspFloatType d){
    //receiving the delay value through here in milliseconds 0 to 2000
    tap3delay = d;
    DspFloatType tap_samples = numSamplesFromMSf(44100, tap3delay);
    readTap3 = writePos - (int)tap_samples;
    
    //wrap if out of bounds
    if(readTap3 < 0)
        readTap3 += MAX_DELAY_SAMPLES; 
};

//-------------------------------------------------------------------------
// setTap4Delay :
// set the delay and tap4 read index position in the delay buffer
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setTap4Delay(DspFloatType d){
    //receiving the delay value through here in milliseconds 0 to 2000
    tap4delay = d;
    DspFloatType tap_samples = numSamplesFromMSf(44100, tap4delay);
    readTap4 = writePos - (int)tap_samples;
    
    //wrap if out of bounds
    if(readTap4 < 0)
        readTap4 += MAX_DELAY_SAMPLES; 
};

//-------------------------------------------------------------------------
// setTap1Level :
// set the tap 1 level which will be a gain multiplier value
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setTap1Level(DspFloatType tl){
    // allow the max volume coefficient to be 0.5
    tap1level = tl;
};

//-------------------------------------------------------------------------
// setTap2Level :
// set the tap 2 level which will be a gain multiplier value
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setTap2Level(DspFloatType tl){
    tap2level = tl;
};

//-------------------------------------------------------------------------
// setTap3Level :
// set the tap 3 level which will be a gain multiplier value
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setTap3Level(DspFloatType tl){
    tap3level = tl;
};

//-------------------------------------------------------------------------
// setTap4Level :
// set the tap 4 level which will be a gain multiplier value
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setTap4Level(DspFloatType tl){
    tap4level = tl;
};

//-------------------------------------------------------------------------
// setFeedback :
// set the feedback value passed to function, -1 to 1
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setFeedback(DspFloatType f){
    //receiving the feedback here as a value between -100 and +100
    feedback = f/100;
};

//-------------------------------------------------------------------------
// setMix :
// set the mix level, value from plugin is 0-100, value in function 0-1
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setMix(DspFloatType m){
    // receiving the mix value through here as value between 0 and 100
    mixLevel = m/100;
};

//-------------------------------------------------------------------------
// setByPass :
// set the bypass value based on bypass value received from plugin
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setByPass(bool bp){
    // receiving the mix value through here as value between 0 and 100
    delay_bypass = bp;
};

//-------------------------------------------------------------------------
// setCurrentFeedbackInput :
// used when crossing feedback lines to provide a feedback input signal
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setCurrentFeedbackInput(DspFloatType fb){
    //set the feedback sample
    feedbackIn = fb;
};
    
//-------------------------------------------------------------------------
// setUseExternalFeedback :
// used when crossing feedback lines to tell delay to take its feedback
// signal externally
//-------------------------------------------------------------------------
void SyncedTapDelayLine::setUseExternalFeedback(bool b){
    //enable or disable an external feedback source
    useExternalFeedback = b;
};


//business function
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//
//  The following function is the workhorse of the delay line
//  It uses terms similar to the differential equation for delay
//
//      x(n)                : the input at sample n
//      y(n)                : the output at sample n after delay processing
//      buffer              : the circular buffer used
//      readPosA            : the read index of the delay buffer
//      readTapX            : tap delay 1-4 read indexes set when delay values
//                          : for these are set
//      tapXOutput          : output for tap 1-4, delay position* tapXlevel
//      writePos            : the write index of the delay buffer
//      MAX_DELAY_SAMPLES   : Max size of delay buffer
//  
//      y(n) = x(n) + x(n - D)              'delay with no feedback
//
//      y(n) = x(n - D) + fb*y(n - D)       'delay with feedback
//
//      y(n) = x(n) + x(n – D) - fb*x(n-D) + fb*y(n-D)  'feedback with wet/dry mix
//
//      MAX_DELAY_SAMPLES = sr * d_ms * .001;
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
DspFloatType SyncedTapDelayLine::next(const DspFloatType in){
    
    if(delay_bypass)
        return in;
        
    //input
    DspFloatType xn = in;
    
    //output of delay at readPos
    DspFloatType yn = buffer[readPosA];
    
    //read the 4 individual taps from buffer and scale
    DspFloatType tap1Output = buffer[readTap1] * tap1level;
    DspFloatType tap2Output = buffer[readTap2] * tap2level;
    DspFloatType tap3Output = buffer[readTap3] * tap3level;
    DspFloatType tap4Output = buffer[readTap4] * tap4level;
    
    
    //if delay < 1 sample, interpolate between x(n) and x(n-1)
    if(readPosA == writePos && delay_samples < 1.00)
    {
            yn = xn;
    }
    
    //read location at n-1, one behind yn
    int readPos_minus1 = readPosA - 1;
    if(readPos_minus1 < 0)
        readPos_minus1 = MAX_DELAY_SAMPLES - 1;   //MAX_DELAY_SAMPLES - 1 is the last location of the buffer
        
    //get y(n-1)
    DspFloatType yn_minus1 = buffer[readPos_minus1];
    
    //perform linear interpolation of : (0,yn) and (1,yn_minus1) by the ammount of fractional delay(fraction)
    DspFloatType interp = linInterp(0, 1, yn, yn_minus1, fraction);
    
    //if delay value is zero just pass input out
    if(delay_samples == 0)
        yn = xn;
    else
        yn = interp;

    //write the input to the delay
    //check if External Feedback path is enabled
    if(!useExternalFeedback)
        buffer[writePos] = xn + feedback*yn;
    else
        buffer[writePos] = xn + feedbackIn;
    
    
    //add the tap delays that are not zero in length
    DspFloatType tappedOutput = yn;
    if(tap1delay){tappedOutput = tappedOutput + tap1Output;}
    if(tap2delay){tappedOutput = tappedOutput + tap2Output;}
    if(tap3delay){tappedOutput = tappedOutput + tap3Output;}
    if(tap4delay){tappedOutput = tappedOutput + tap4Output;}
    
    //create wet level and write to output buffer
    DspFloatType out = mixLevel*tappedOutput + (1.0 - mixLevel)*xn;
    
    //wrap indexes if out of bounds
    writePos++;
    if(writePos >= MAX_DELAY_SAMPLES)
        writePos = 0;
        
    readPosA++;
    if(readPosA >= MAX_DELAY_SAMPLES)
        readPosA = 0;
    
    readTap1++;
    readTap2++;
    readTap3++;
    readTap4++;
    
    if(readTap1 >= MAX_DELAY_SAMPLES)
        readTap1 = 0;

    if(readTap2 >= MAX_DELAY_SAMPLES)
        readTap2 = 0;

    if(readTap3 >= MAX_DELAY_SAMPLES)
        readTap3 = 0;

    if(readTap4 >= MAX_DELAY_SAMPLES)
        readTap4 = 0;        
    
    return out;
        
}

//-------------------------------------------------------------------------
// resetBuffer :
// delete contents of buffer and instantiate a new buffer
//-------------------------------------------------------------------------
void SyncedTapDelayLine::resetBuffer(){
    
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
void SyncedTapDelayLine::resetDelay(){
 
    if(buffer)
        memset(buffer, 0, MAX_DELAY_SAMPLES*sizeof(DspFloatType));
    
    
    readPosA = readTap1 = readTap2 = readTap3 = readTap4 = writePos = 0;
    delay_bypass = 0;
    
    return;   
}
}