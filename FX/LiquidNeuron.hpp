#pragma once

#include "SlewLimiter.hpp"
#include "CSmoothFilters.hpp"
#include "SVF.hpp"

namespace Liquid
{
    DspFloatType function_morph(DspFloatType x, DspFloatType y, DspFloatType frac)
    {
        return x + frac*(y-x);
    }

    struct LiquidNeuron
    {
        
        DspFloatType prev;
        Analog::RateLimiters::SlewLimiter limiter;
        DspFloatType time = 10.0f;
        DspFloatType range = 1.0f;
        DspFloatType sr;
        
        LiquidNeuron(DspFloatType sample_rate = sampleRate, DspFloatType ms = 10.0f, DspFloatType limit=1.0f) :
            limiter(sampleRate,10.0f,1)
        {    
            prev  = 0;
            sr = sample_rate;
            time = ms;
            range = limit;
        }
        
        void setTime(DspFloatType milliseconds)
        {
            time = milliseconds;
            limiter.setParams(sr,milliseconds,range);
        }
        void setRange(DspFloatType R)
        {
            range = R;
            limiter.setParams(sr,time,range);
        }
        void setSampleRate(DspFloatType s)
        {
            sr = s;
            limiter.setParams(sr,time,range);
        }

        DspFloatType Tick(DspFloatType target) {
            prev = limiter.next(target);
            return prev;
        }
    };

    struct LiquidPole
    {
        DspFloatType prev;
        FX::Filters::Smoothers::CSmoothFilter filter;

        LiquidPole(DspFloatType h=0.01, DspFloatType sr=sampleRate) :
        filter(sr,h) {

        }
        void setSampleRate(DspFloatType s) {

        }
        void setTime(DspFloatType t) {
            filter.setCutoff(1.0/t);
        }
        void setHz(DspFloatType h) {
            filter.setCutoff(h);
        }
        DspFloatType Tick(DspFloatType target) {
            prev = filter.process(target);
            return prev;
        }
    };

    struct LiquidTwoPole
    {
        DspFloatType prev;
        Filters::StateVariableFilter filter;

        LiquidTwoPole(DspFloatType h=0.01) :
        filter(h,sampleRate,0.5) {

        }
        void setTime(DspFloatType t) {
            filter.setCutoff(1.0/t);
        }
        void setHz(DspFloatType h) {
            filter.setCutoff(h);
        }
        DspFloatType Tick(DspFloatType target) {
            prev = filter.Tick(target);
            return prev;
        }
    };

    struct LiquidInterpolator
    {
        DspFloatType prev;    
        DspFloatType interp = 0.5;
        LiquidNeuron neuron;

        LiquidInterpolator(DspFloatType i = 0.5) : interp(i)
        {
        
        }
        DspFloatType Tick(DspFloatType target) {        
            prev = prev + interp*( neuron.Tick(target) - prev);
            return prev;
        }
    };

    struct LiquidIntegrator
    {
        DspFloatType prev;    
        LiquidNeuron neuron;

        LiquidIntegrator()
        {

        }
        DspFloatType Tick(DspFloatType target) {        
            DspFloatType val = prev + neuron.Tick(target);
            prev = target;
            return val;
        }
    };

    struct LiquidDifferencer
    {
        DspFloatType prev;    
        LiquidNeuron neuron;

        LiquidDifferencer()
        {

        }
        DspFloatType Tick(DspFloatType target) {        
            DspFloatType val = prev - neuron.Tick(target);
            prev = target;
            return val;
        }
    };

    struct LiquidComparator
    {
        LiquidNeuron neuron;

        LiquidComparator()
        {

        }
        DspFloatType Tick(DspFloatType target) {              
            if(neuron.Tick(target) > 0) return 1.0f;
            return -1.0f;
        }
    };

    struct LiquidSampleHoldMS
    {
        DspFloatType rate = 0;
        DspFloatType p = 0;
        DspFloatType current = 0;
        LiquidNeuron neuron;

        LiquidSampleHoldMS(DspFloatType ms) {
            rate = (ms/1000.0f) / sampleRate;
            p = 0;
        }
        void setNeuronTime(DspFloatType dt) {

        }
        DspFloatType Tick(DspFloatType x)
        {
            p = p + rate;
            if( p >= 1.0f) {
                current = x;            
            }
            p = fmod(p,1);
            return current;        
        }
    };

    struct LiquidSampleHoldHZ
    {
        DspFloatType rate = 0;
        DspFloatType p = 0;
        DspFloatType current = 0;
        LiquidNeuron neuron;

        LiquidSampleHoldHZ(DspFloatType hz) {
            rate = hz / sampleRate;
            p = 0;
        }
        DspFloatType Tick(DspFloatType x)
        {
            p = p + rate;
            if( p >= 1.0f) {
                current = x;            
            }
            p = fmod(p,1);
            return neuron.Tick(current);        
        }
    };

    struct LiquidMemory
    {
        LiquidNeuron neuron;
        LiquidPole   pole;
        DspFloatType prev = 0;
        LiquidMemory()
        {

        }
        void setCutoff(DspFloatType f) {
            pole.setHz(f);
        }
        void setTime(DspFloatType t) {
            neuron.setTime(t);
        }
        DspFloatType operator()() {
            return prev;
        }
        DspFloatType operator()(DspFloatType input) {
            return Tick(input);
        }
        DspFloatType Tick(DspFloatType input)
        {
            prev = neuron.Tick(pole.Tick(input));
            return prev;
        }
    };

    struct LiquidNeuronDelay
    {
        std::vector<DspFloatType> caps;
        std::vector<LiquidPole> neurons;
        int r,w;

        LiquidNeuronDelay(size_t n) {
            caps.resize(n);
            neurons.resize(4);
            r=0;
            w = n-1;
        }
        DspFloatType Tick(DspFloatType I)
        {
            DspFloatType n = 0;
            DspFloatType x = caps[r++];
            r = r % caps.size();    
            
            for(size_t i = 0; i < neurons.size(); i++)
            {
                x += I*neurons[i].Tick(x);                             
            }
            if(x < -0.75) x = -0.75;
            if(x > 0.75) x = 0.75;
            caps[w++] = tanh(0.5*(I + 0.9*x));
            
            w = w % caps.size();
            return 0.5*I + 0.5*x;
        }
    };
}