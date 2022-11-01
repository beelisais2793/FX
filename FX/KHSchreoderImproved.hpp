#pragma once

#include <algorithm>
#include <cmath>
#include <cassert>
#include <cstdlib>
#include <iostream>

namespace FX::KHDelays::SchreoderImproved
{
    //////////////////////////////////////////////////////////
    //  DELAY LINE CLASS (for use with Schroeder Reverb)
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
        //KH***DspFloatType next(const DspFloatType in, const DspFloatType gain);
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


    //////////////////////////////////////////////////////////
    //    ALLPASS FILTER CLASS
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
//  BASIC LOWPASS FILTER CLASS
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
//  COMB FILTER CLASS
//////////////////////////////////////////////////////////

class Comb{
    
public:
    //constructor / destructor
    Comb(const int sr, const DspFloatType d_ms, const DspFloatType d_ms_max, const DspFloatType g);
    ~Comb();
    
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
    Lowpass *lpFilter;
    
};


//////////////////////////////////////////////////////////
//  SCHROEDER REVERB
//////////////////////////////////////////////////////////

#define NUM_COMBS 4
#define NUM_ALLPASSES 2
#define NUM_LOWPASSES 1

class Schroeder{

public:
    //constructor / destructor
    Schroeder(const int sr = 44100, const DspFloatType rt60 = 3.0,
              const DspFloatType cDelay1 = 29.7, const DspFloatType cDelay2 = 37.1, const DspFloatType cDelay3 = 41.1, const DspFloatType cDelay4 = 43.7,
              const DspFloatType aDelay1 = 5.0, const DspFloatType aDelay2 = 1.7, const DspFloatType aGain1 = 0.707, const DspFloatType aGain2 = 0.5, const DspFloatType lCutoff1 = 2300.0f);    //const DspFloatType lCutoff1 = 2300.0f
    ~Schroeder();
    
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
//  ALLPASS FILTER CLASS
//////////////////////////////////////////////////////////

//constructor
Allpass::Allpass(const int sr, const DspFloatType d_ms, const DspFloatType d_ms_max, const DspFloatType g){
    gain = g;
    delay = new DelayLine(sr, d_ms, d_ms_max);
}

Allpass::~Allpass(){
    delete delay;
}

//getters
DspFloatType Allpass::getGain(){return gain;}
DspFloatType Allpass::getDelayTimeMS(){return delay->getDelayTimeMS();}

//setters
void Allpass::setGain(const DspFloatType g){gain = g;}
void Allpass::setDelayTimeMS(const DspFloatType sr, const DspFloatType d_ms){return delay->setDelayTimeMS(sr, d_ms);}

//business methods
DspFloatType Allpass::next(const DspFloatType in){
  //KH****  DspFloatType out = delay->next(in, gain);
  //KH***  return out + (in * -gain);
    DspFloatType dL = delay->readDelay();
    DspFloatType fW = in + (gain*dL);
    DspFloatType out = -gain*fW + dL;
    delay->writeDelay(fW);
    return out;
}


//////////////////////////////////////////////////////////
//  BASIC COMB FILTER CLASS
//////////////////////////////////////////////////////////

//constructor / destructor
Comb::Comb(const int sr, const DspFloatType d_ms, const DspFloatType d_ms_max, const DspFloatType g){
    gain = g;
    delay = new DelayLine(sr, d_ms, d_ms_max);
    lpFilter = new Lowpass(44100, 10000.0f);
}

Comb::~Comb(){
    delete delay;
    delete lpFilter;
}

//getters
DspFloatType Comb::getGain(){return gain;}
DspFloatType Comb::getDelayTimeMS(){return delay->getDelayTimeMS();}

//setters
void Comb::setGain(const DspFloatType g){gain = g;}
void Comb::setDelayTimeMS(const DspFloatType sr, const DspFloatType d_ms){return delay->setDelayTimeMS(sr, d_ms);}

//business methods
DspFloatType Comb::next(const DspFloatType in){
    //KH***return delay->next(in, gain);
    DspFloatType dL = delay->readDelay();
    
    //DspFloatType lpOut = dL * gain;
    //DspFloatType lpRetVal = lpFilter->next(lpOut);
    DspFloatType lpRetVal = lpFilter->next(dL);
    
    //DspFloatType dLW = in + lpRetVal;
    DspFloatType dLW = in + lpRetVal*gain;
    delay->writeDelay(dLW);
    return dL;
    
}

//helper functions
inline DspFloatType numSamplesFromMSf(const int sr, const DspFloatType d_ms){
    return sr * d_ms * .001;
}

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

//constructor / destructor
DelayLine::DelayLine(const int sr, const DspFloatType d_ms, const DspFloatType d_max){
    //assert(d_ms <= d_ms_max);//check bound on delay time
    
    buffer = NULL;
    readPosA = writePos = 0;
    
    //DspFloatType d_ms_max = 2000.0f;
    delay_samples = 0.0f;
    delay_ms = d_ms;
    
    MAX_DELAY_SAMPLES = ceil(numSamplesFromMSf(sr, d_max));
    MAX_DELAY_MS = MAX_DELAY_SAMPLES * 1000.0f / sr; //make sure DspFloatType version is set with to integer-rounded buffer size
    
    DspFloatType delay_samplesf = numSamplesFromMSf(sr, d_ms);
    delay_samples = floor(delay_samplesf);
    fraction = delay_samplesf - delay_samples;//storing fractional delay time, will be interpolated
    
    buffer = new DspFloatType[MAX_DELAY_SAMPLES];
    memset(buffer, 0, MAX_DELAY_SAMPLES*sizeof(DspFloatType));
    
    //KH***added
    //setDelayTimeMS(sr,delay_ms);
}

DelayLine::~DelayLine(){
    delete[] buffer;
}

//getters
DspFloatType DelayLine::getDelayTimeMS(){return delay_ms;}
DspFloatType DelayLine::getMaxDelayTimeMS(){return MAX_DELAY_MS;}

//setters
void DelayLine::setDelayTimeMS(const int sr, const DspFloatType d_ms){
    assert(d_ms <= MAX_DELAY_MS);//check bound on delay time
    
    DspFloatType delay_samplesf = numSamplesFromMSf(sr, d_ms);
    delay_samples = floor(delay_samplesf);
    fraction = delay_samplesf - delay_samples;//storing fractional delay time, output will be interpolated
    
    readPosA = writePos - delay_samples; //KHH***(int)delay_samples;
    if(readPosA < 0)
        readPosA += MAX_DELAY_SAMPLES;
        
        
}

void DelayLine::setDelay(DspFloatType d){
    //receiving the delay value through here in milliseconds 0 to 2000
    delay_ms = d;
    setDelayTimeMS(44100,delay_ms);
};


DspFloatType DelayLine::readDelay(){
       //output of delay at readPos
    DspFloatType yn = buffer[readPosA];
    
    //if delay < 1 sample, interpolate between x(n) and x(n-1)
   /* if(readPosA == writePos && delay_samples < 1.00)
    {
            yn = xn;
    }
    */
    
    //read location at n-1, one behind yn
    int readPos_minus1 = readPosA - 1;
    if(readPos_minus1 < 0)
        readPos_minus1 = MAX_DELAY_SAMPLES - 1;   //MAX_DELAY_SAMPLES - 1 is the last location of the buffer
        
    //get y(n-1)
    DspFloatType yn_minus1 = buffer[readPos_minus1];
    
    //perform linear interpolation of : (0,yn) and (1,yn_minus1) by the ammount of fractional delay(fraction)
    return linInterp(0, 1, yn, yn_minus1, fraction);
}

void DelayLine::writeDelay(DspFloatType in){
    
    buffer[writePos] = in;
       
    writePos++;
    if(writePos >= MAX_DELAY_SAMPLES)
        writePos = 0;
        
    readPosA++;
    if(readPosA >= MAX_DELAY_SAMPLES)
        readPosA = 0;
}

//business function
//KH***DspFloatType DelayLine::next(const DspFloatType in, const DspFloatType gain){
DspFloatType DelayLine::next(const DspFloatType in){
        
    //input
    DspFloatType xn = in;
/** readDelay    
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
readDelay end **/ 

    //if delay value is zero just pass input out
    DspFloatType yn;
    if(delay_samples == 0)
        yn = xn;
    else
        yn = readDelay();  //***interp was here before the split of functions

    writeDelay(xn);
    //****DspFloatType out = buffer[writePos];
    DspFloatType out = yn;

/**writeDelay
    //write the input to the delay
    buffer[writePos] = in; //KHH*****+ yn*gain;
 
    
    writePos++;
    if(writePos >= MAX_DELAY_SAMPLES)
        writePos = 0;
        
    readPosA++;
    if(readPosA >= MAX_DELAY_SAMPLES)
        readPosA = 0;
writeDelay End **/        
    
    return out;
        
}

void DelayLine::resetBuffer(){
    
    if(buffer)
        delete [] buffer;
        
    buffer = new DspFloatType[MAX_DELAY_SAMPLES];
    
    resetDelay();
    setDelayTimeMS(44100,delay_ms);
    
    return;
    
}
        
void DelayLine::resetDelay(){
 
    if(buffer)
        memset(buffer, 0, MAX_DELAY_SAMPLES*sizeof(DspFloatType));
    
    readPosA = writePos = 0;
    
    return;
    
}


//////////////////////////////////////////////////////////
//  BASIC LOWPASS FILTER CLASS
//////////////////////////////////////////////////////////

//constructor
Lowpass::Lowpass(const int sr, const DspFloatType cf_hz){
    prev = 0;
    cutoff = cf_hz;
    DspFloatType costh = 2.0 - cos(2.0 * M_PI * cutoff / sr);
    coef = sqrt(costh * costh - 1.0) - costh;
}

//getters
DspFloatType Lowpass::getCutoff(){return cutoff;}

//setters
void Lowpass::setCutoff(const int sr, const DspFloatType cf_hz){
    cutoff = cf_hz;
    DspFloatType costh = 2.0 - cos(2.0 * M_PI * cutoff / sr);
    coef = sqrt(costh * costh - 1.0) - costh;
}

//business methods
DspFloatType Lowpass::next(const DspFloatType in){
    prev = in * (1 + coef) - (prev * coef);
    
    return prev;
}

//////////////////////////////////////////////////////////
//  SCHROEDER REVERB
//////////////////////////////////////////////////////////


#define NUM_COMBS 4
#define NUM_ALLPASSES 2

//helper functions
inline DspFloatType calcCombGain(const DspFloatType d_ms, const DspFloatType rt60){
    return pow(10.0, ((-3.0 * d_ms) / (rt60 * 1000.0)));
    //(((return pow(10.0, (-3.0 * (d_ms*44100*.001) * (1.0/44100)) / (rt60 * 1000.0));

}

inline DspFloatType calcAPGain(const DspFloatType d_ms, const DspFloatType rt60){
    return pow(10.0, ((-3.0 * d_ms) / (rt60 * 1000.0)));
}

//constructor / destructor
Schroeder::Schroeder(const int sr, const DspFloatType rt60,
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
DspFloatType Schroeder::getLowpassCutoff(const int id){return lowpasses[id]->getCutoff();}
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
void Schroeder::setLowpassCutoff(const int id, const int sr, const DspFloatType cf_hz){lowpasses[id]->setCutoff(sr, cf_hz);}
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
