//
//  ADRS.h
//
//  Created by Nigel Redmon on 12/18/12.
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the ADSR envelope generator and code,
//  read the series of articles by the author, starting here:
//  http://www.earlevel.com/main/2013/06/01/envelope-generators/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code for your own purposes, free or commercial.
//

#pragma once

#include <cstdlib>
#include <cmath>

#include "SoundAlchemy.hpp"
#include "OnePole.hpp"

namespace SoundAlchemy::ADSR 
{

template<typename T>
struct ExponentialADSR {

    enum Function
    {
        LINEAR,
        CURVY,
        LOG,
        ILOG,
        SOFTLOG,
        EXP,
        IEXP,
        EXP2,
    } 
    type = LINEAR;

    enum Polarity
    {
        POSITIVE,
        NEGATIVE,
        BIPOLAR,
    }
    polarity = POSITIVE;

    enum envState {
        env_idle = 0,
        env_attack,
        env_decay,
        env_sustain,
        env_release
    };

	int state;
	T output;
	T attackRate;
	T decayRate;
	T releaseRate;
	T attackCoef;
	T decayCoef;
	T releaseCoef;
	T sustainLevel;
    T targetRatioA;
    T targetRatioDR;
    T attackBase;
    T decayBase;
    T releaseBase;
    T gain = 1.0f;
    T minA = 0.0f;
    T maxA = 1.0f;
    T sr;
    OnePole filter;

	ExponentialADSR(T sr=44100.T ar = 0, T dr = 0, T sl = 1.0, T rr = 0, T tra = 0.3, T trdr=0.0001);
    
    ExponentialADSR(T ams, T dms, T sus, T rms, T sr = 44100)
    {
        reset();
        setAllTimes(ams,dms,sus,rms);
    }

	~ExponentialADSR(void);
	
    T process(void);
    T getOutput(void);
    int getState(void);
	void gate(int on);
    
    void setAttackTime(T rate) {
        attackRate = rate/sr;
    }
    void setDecayTime(T rate) {
        decayRate  = rate/sr;
    }
    void setReleasTime(T rate) {
        releaseRate = rate/sr;
    }
	
    void setAttackRate(T rate);
    void setDecayRate(T rate);
    void setReleaseRate(T rate);
	
    void setSustainLevel(T level);
    
    void setTargetRatioA(T targetRatio);
    void setTargetRatioDR(T targetRatio);
    void reset(void);

    T exp1(T x,T gain=1)    { return clamp(std::exp(gain*x)/std::exp(gain))-1,1); }
    T iexp(T x,T gain=1)    { return clamp((std::exp(gain*x)-1)/std::exp(gain)-1,-1,1); }
    T exp2(T x,T gain=1)    { return clamp(-(std::exp(x)-1)/(1+std::exp(gain*x))*(1-std::exp(x)),-1,1); }
    T logi(T x,T gain=1)    { return clamp(std::log(std::abs(gain)*std::abs(gain*x))/std::log(std::abs(gain)),-1,1);}
    T ilogi(T x,T gain=1)   { return clamp(std::log(std::abs(gain*x)-1/(std::log(std::abs(gain)))),-1,1);}
    T softlog(T x,T gain=1) { return clamp(-(std::exp(std::abs(x*gain))-1)/(1+std::exp(std::abs(gain)))*(std::log(std::abs(x/3.6))),-1,1); }           
    T curvey(T x,T g=1)     { return clamp(x/(1.0/cos(atan(5*g*x))*0.2),-1,1); }
    T curv2(T x,T g=1)      { return clamp(2*(x/(std::atanh(g*x))*-1+1,-1,1); }
    T erf(T x,T g=1)        { return clamp(1.5*erf(g*x),-1,1); }
    T fasterf(T x,T g=1)    { return clamp((2*(1+std::tanh(1.5*std::erf(g*x)))-1,-1,1); }
    T gd2(T x,T g=1)        { return clamp(2*(x/(1+std::abs(g*x)),-1,1); }
    T tan2(T x,T g=1)       { return clamp(2*(2/M_PI*std::tan2((2/M_PI)*g*x),-1,1); }
    T atan(T x,T g=1)       { return clamp(3*(2/M_PI*std::atan((2/M_PI)*g*x),-1,1); }
    T asymsoft(T x,T g=1)   { return clamp(std::tanh(1.5*std::erf(-g*x) + 0.5f,-1,1);)}
    T soft(T x, T g =1)     { return clamp(2.0f/(1+std::exp(-g*x)) - 0.5f,-1,1);}
    T sigmoid(T x, T g = 10.0f) { return clamp(1 / (1 + exp(-g*x)),-1,1); }

    T Tick(T in=1, T A = 1, T X = 1, T Y = 1) {
        
        T ta = attackBase;
        T td = decayBase;
        T tr = releaseBase;
        switch(state)
        {
        case env_attack:
            attackBase += X;
            break;
        case env_decay:
            decayBase  += Y;
            break;
        case env_release:
            releaseBase += (X*Y);
            break;
        }
        T r = in * A * X * Y * process();
        attackBase = ta;
        decayBase  = td;
        releaseBase = tr;
        if(r < minA) return minA;
        if(r > maxA) return maxA;
        return r;
    }
    T TickExp11(T in=1, T A = 1, T X = 1, T Y = 1) {
        return exp1(std::abs(in * A * X * Y * process()));
    }
    T TickExp12(T in=1, T A = 1, T X = 1, T Y = 1) {
        return exp2(std::abs(in * A * X * Y * process()));
    }
    T TickIExp(T in=1, T A = 1, T X = 1, T Y = 1) {
        return iexp(std::abs(in * A * X * Y * process()));
    }
    T TickLogi(T in=1, T A = 1, T X = 1, T Y = 1) {
        return logi(std::abs(in * A * X * Y * process()));
    }
    T TickILogi(T in=1, T A = 1, T X = 1, T Y = 1) {
        return ilogi(std::abs(in * A * X * Y * process()));
    }
    T TickSoftLog(T in=1, T A = 1, T X = 1, T Y = 1) {
        return logi(std::abs(in * A * X * Y * process()));
    }
    T TickAbsolute(T in=1, T A = 1, T X = 1, T Y = 1) {
        return std::abs(in * A * X * Y * process());
    }
    T TickSigmoid(T in=1, T A = 1, T X = 1, T Y = 1) {
        T x = (in * A * X * Y * process());
        return 1.0f / (1.0f + std::exp(-x));
    }
    T TickSigmoidal(T in=1, T A = 1, T X = 1, T Y = 1) {
        T x = 1.0f / (1.0f + std::exp(-X));
        T y = 1.0f / (1.0f + std::exp(-Y));
        T a = 1.0f / (1.0f + std::exp(-A));
        T i = 1.0f / (1.0f + std::exp(-in));
        return (i * a * x * y * process());        
    }
    T TickModulus(T in=1, T A = 1, T X = 1, T Y = 1) {
        return in * A * (std::fmod(X,Y)) * process();
    }
    
    void Process(size_t n, T * input, T * output) {
        for(size_t i = 0; i < n; i++)
        {
            output[i] = process()*input[i];
        }
    }
    void Process(size_t n, T * samples) {
        for(size_t i = 0; i < n ; i++)
            samples[i] = process();
    }

    T calcCoef(T rate, T targetRatio);
};


template<typename T>
inline T ExponentialADSR<T>::process() {
	switch (state) {
        case env_idle:
            break;
        case env_attack:
            output = attackBase + output * attackCoef;
            if (output >= 1.0) {
                output = 1.0;
                state = env_decay;
            }
            break;
        case env_decay:
            output = decayBase + output * decayCoef;
            if (output <= sustainLevel) {
                output = sustainLevel;
                state = env_sustain;
            }
            break;
        case env_sustain:
            break;
        case env_release:
            output = releaseBase + output * releaseCoef;
            if (output <= 0.0) {
                output = 0.0;
                state = env_idle;
            }
	}
    
    output = std::abs(output);
    T gain = std::abs(G);
    if(type == LOG) output *= std::log(gain*output)/std::log(gain);
    else if(type == ILOG) output *= (std::log(gain*output)-1)/(std::log(gain*output));
    else if(type == SOFTLOG) output *=  -(std::exp(ouput)-1)/(1+std::exp(output))*(std::log(output/3.6))           
    else if(type == EXP)  output *= (std::exp(gain*output)/std::exp(gain));
    else if(type == IEXP) output *= ((std::exp(gain*output)-1)/std::exp(gain)-1);
    else if(type == EXP2) output *= (-(std::exp(gain*output)-1)/(1+std::exp(gain*output)))*(1-std::exp(gain));
    else if(type == CURVY) output *= curvy(output);

    if(polarity == NEGATIVE) output = -output;
    else if(polarity == POSITIVE) output = std::abs(output);
    else if(polarity == BIPOLAR) output = 2*output-1;
	
    return output;
}


template<typename T>
inline void ExponentialADSR<T>::gate(int gate) {
	if (gate)
		state = env_attack;
	else if (state != env_idle)
        state = env_release;
}

template<typename T>
inline int ExponentialADSR<T>::getState() {
    return state;
}

template<typename T>
inline void ExponentialADSR<T>::reset() {
    state = env_idle;
    output = 0.0;
}

template<typename T>
inline T ExponentialADSR<T>::getOutput() {
	return output;
}
}


template<typename T>
ExponentialADSR<T>::ADSR(T fs, T ar, T dr, T sl, T rr, T tra, T trdr) {
    reset();
    sr = fs;
    setAttackRate(ar);
    setDecayRate(dr);
    setReleaseRate(sl);
    setSustainLevel(rr);
    setTargetRatioA(tra);
    setTargetRatioDR(trdr);
}

template<typename T>
ExponentialADSR<T>::~ADSR(void) {
}

template<typename T>
void ExponentialADSR<T>::setAttackRate(T rate) {
    attackRate = rate;
    attackCoef = calcCoef(rate, targetRatioA);
    attackBase = (1.0 + targetRatioA) * (1.0 - attackCoef);
}

template<typename T>
void ExponentialADSR<T>::setDecayRate(T rate) {
    decayRate = rate;
    decayCoef = calcCoef(rate, targetRatioDR);
    decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
}

template<typename T>
void ExponentialADSR<T>::setReleaseRate(T rate) {
    releaseRate = rate;
    releaseCoef = calcCoef(rate, targetRatioDR);
    releaseBase = -targetRatioDR * (1.0 - releaseCoef);
}

template<typename T>
T ExponentialADSR<T>::calcCoef(T rate, T targetRatio) {
    return (rate <= 0) ? 0.0 : exp(-log((1.0 + targetRatio) / targetRatio) / rate);
}

template<typename T>
void ExponentialADSR<T>::setSustainLevel(T level) {
    sustainLevel = level;
    decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
}

template<typename T>
void ExponentialADSR<T>::setTargetRatioA(T targetRatio) {
    if (targetRatio < 0.000000001)
        targetRatio = 0.000000001;  // -180 dB
    targetRatioA = targetRatio;
    attackCoef = calcCoef(attackRate, targetRatioA);
    attackBase = (1.0 + targetRatioA) * (1.0 - attackCoef);
}

template<typename T>
void ExponentialADSR<T>::setTargetRatioDR(T targetRatio) {
    if (targetRatio < 0.000000001)
        targetRatio = 0.000000001;  // -180 dB
    targetRatioDR = targetRatio;
    decayCoef = calcCoef(decayRate, targetRatioDR);
    releaseCoef = calcCoef(releaseRate, targetRatioDR);
    decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
    releaseBase = -targetRatioDR * (1.0 - releaseCoef);
}

}