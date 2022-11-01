#pragma once

#include "SlewLimiter.hpp"
#include "Filters/CSmoothFilter.hpp"
#include "Analog/SVF.hpp"

namespace Liquid
{
    double function_morph(double x, double y, double frac)
    {
        return x + frac*(y-x);
    }

    struct LiquidNeuron
    {
        
        double prev;
        Analog::RateLimiters::SlewLimiter limiter;
        double time = 10.0f;
        double range = 1.0f;
        double sr;
        
        LiquidNeuron(double sample_rate = sampleRate, double ms = 10.0f, double limit=1.0f) :
            limiter(sampleRate,10.0f,1)
        {    
            prev  = 0;
            sr = sample_rate;
            time = ms;
            range = limit;
        }
        
        void setTime(double milliseconds)
        {
            time = milliseconds;
            limiter.setParams(sr,milliseconds,range);
        }
        void setRange(double R)
        {
            range = R;
            limiter.setParams(sr,time,range);
        }
        void setSampleRate(double s)
        {
            sr = s;
            limiter.setParams(sr,time,range);
        }

        double Tick(double target) {
            prev = limiter.next(target);
            return prev;
        }
    };

    struct LiquidPole
    {
        double prev;
        Filters::CSmoothFilter filter;

        LiquidPole(double h=0.01, double sr=sampleRate) :
        filter(sr,h) {

        }
        void setSampleRate(double s) {

        }
        void setTime(double t) {
            filter.setCutoff(1.0/t);
        }
        void setHz(double h) {
            filter.setCutoff(h);
        }
        double Tick(double target) {
            prev = filter.process(target);
            return prev;
        }
    };

    struct LiquidTwoPole
    {
        double prev;
        Filters::StateVariableFilter filter;

        LiquidTwoPole(double h=0.01) :
        filter(h,sampleRate,0.5) {

        }
        void setTime(double t) {
            filter.setCutoff(1.0/t);
        }
        void setHz(double h) {
            filter.setCutoff(h);
        }
        double Tick(double target) {
            prev = filter.Tick(target);
            return prev;
        }
    };

    struct LiquidInterpolator
    {
        double prev;    
        double interp = 0.5;
        LiquidNeuron neuron;

        LiquidInterpolator(double i = 0.5) : interp(i)
        {
        
        }
        double Tick(double target) {        
            prev = prev + interp*( neuron.Tick(target) - prev);
            return prev;
        }
    };

    struct LiquidIntegrator
    {
        double prev;    
        LiquidNeuron neuron;

        LiquidIntegrator()
        {

        }
        double Tick(double target) {        
            double val = prev + neuron.Tick(target);
            prev = target;
            return val;
        }
    };

    struct LiquidDifferencer
    {
        double prev;    
        LiquidNeuron neuron;

        LiquidDifferencer()
        {

        }
        double Tick(double target) {        
            double val = prev - neuron.Tick(target);
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
        double Tick(double target) {              
            if(neuron.Tick(target) > 0) return 1.0f;
            return -1.0f;
        }
    };

    struct LiquidSampleHoldMS
    {
        double rate = 0;
        double p = 0;
        double current = 0;
        LiquidNeuron neuron;

        LiquidSampleHoldMS(double ms) {
            rate = (ms/1000.0f) / sampleRate;
            p = 0;
        }
        void setNeuronTime(double dt) {

        }
        double Tick(double x)
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
        double rate = 0;
        double p = 0;
        double current = 0;
        LiquidNeuron neuron;

        LiquidSampleHoldHZ(double hz) {
            rate = hz / sampleRate;
            p = 0;
        }
        double Tick(double x)
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
        double prev = 0;
        LiquidMemory()
        {

        }
        void setCutoff(double f) {
            pole.setHz(f);
        }
        void setTime(double t) {
            neuron.setTime(t);
        }
        double operator()() {
            return prev;
        }
        double operator()(double input) {
            return Tick(input);
        }
        double Tick(double input)
        {
            prev = neuron.Tick(pole.Tick(input));
            return prev;
        }
    };

    struct LiquidNeuronDelay
    {
        std::vector<double> caps;
        std::vector<LiquidPole> neurons;
        int r,w;

        LiquidNeuronDelay(size_t n) {
            caps.resize(n);
            neurons.resize(4);
            r=0;
            w = n-1;
        }
        double Tick(double I)
        {
            double n = 0;
            double x = caps[r++];
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