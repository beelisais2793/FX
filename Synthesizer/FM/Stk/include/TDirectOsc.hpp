#pragma once
#include "SABase.hpp"

namespace SoundAlchemy::Oscillators
{
    template<typename T>
    class TDirectOSC : public Object<T>
    {
    public:
        TDirectOSC(){
            // Finish initializations here
            // turn off
            //runOSC = true;
            // set 0
            b1 = 0;
            b2 = 0;
            // flush memory
            y_z1[0] = 0;
            y_z2[0] = 0;
            y_z1[1] = 0;
            y_z2[1] = 0;
            // call the cooking function to calc coeffs // and set initial states
            cookFrequency();
        };
        void cookFrequency()
        {
            // Oscillation Rate = theta = wT = w/fs
            T f_wT = (2.f * M_PI*freq) / (T)currentSampleRate;
            // coefficients according to design equations
            b1 = -2.f * std::cos(f_wT);
            b2 = 1.f;
            // set initial conditions so that first sample out is 0.0
            y_z1[0] = std::sin(-1.f * f_wT); // sin(wnT) = sin(w(–1)T)
            y_z2[0] = std::sin(-2.f * f_wT); // sin(wnT) = sin(w(–2)T)
            y_z1[1] = std::sin(-1.f * f_wT); // sin(wnT) = sin(w(–1)T)
            y_z2[1] = std::sin(-2.f * f_wT); // sin(wnT) = sin(w(–2)T)
        };
        
        T getSample(int channel)
        {
            //if not running, write 0s and bail
            if(!runOSC)
            {
                currentSample[channel] = 0.f; // zero it all out
                return 0.f;
            }
            // otherwise, do the oscillator
            else
            {
                // do difference equation y(n) = –b1y(n-2) – b2y(n–2)
                currentSample[channel] = - b1 * y_z1[channel] - b2 * y_z2[channel];
                // shuffle memory
                y_z2[channel] = y_z1[channel];
                y_z1[channel] = currentSample[channel];
                
                return y_z1[channel];
            }
        };
        
        void toggleOSC(bool r)
        {
            runOSC = r;
            cookFrequency();
        };
        
        void setFreq(T f)
        {
            freq = f;
            cookFrequency();
        };
        
        void setSampleRate(T sr)
        {
            currentSampleRate = sr;
        };
        
        bool getRunOSC()
        {
            return runOSC;
        }
        
        T getFreq()
        {
            return freq;
        }

        T Tick(int channel=0) {
            return getSample(channel);
        }
        T Tick(T I, int channel=0) {
            return I*Tick(channel);
        }
        T Tick(T I, T A = 1, T F = 1, T P = 1) { 
                A = clamp(A,-1,1);
                F = clamp(F,-1,1);
                P = clamp(P,-1,1);
                return A * F * Tick(P*I);
        }
            
    private:

        T currentSampleRate;
        T freq;
        
        // coefficients, 2nd Order FB
        T b1;
        T b2;
        // delay elements, 2nd Order FB
        T y_z1[2];
        T y_z2[2];
        //flag to start/stop oscillator
        bool runOSC;
        
        T currentSample[2];
    };
}