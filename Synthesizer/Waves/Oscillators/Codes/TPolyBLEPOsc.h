/*
  ==============================================================================
    PolyBlepOSC.h
    Created: 21 Mar 2022 3:45:13pm
    Author:  Keepin' Soul
  ==============================================================================
*/

#pragma once

namespace SoundWave {

template<typename T>
class PolyBlepOSC {
public:
    PolyBlepOSC(){

    };
    void cookFrequency()
    {
        inc = freq/currentSampleRate;
        modulo[0] = 0.5f;
        modulo[1] = 0.5f;
    };
    inline T getSample(int channel)
    {
        T trivial_saw = 0.f;
        T blep_saw = 0.f;
        if (run)
        {
            // ============= TRIVIAL SAW ======================================
            if(!blep)
            {
                // wrap
                if (modulo[channel] >= 1.f)
                    modulo[channel] -= 1.f;
                
                //unipolar to bipolar
                trivial_saw = 2.f * modulo[channel] - 1.f;
                
                //increment counter
                modulo[channel]+=inc;
                
                return trivial_saw;
            }
            // ============= TRIVIAL SAW + POLYBLEP ============================
            else
            {
                // wrap
                if (modulo[channel] >= 1.f)
                    modulo[channel] -= 1.f;
                
                //unipolar to bipolar
                trivial_saw = 2.f * modulo[channel] - 1.f;
                blep_saw = trivial_saw + doPolyBLEP(modulo[channel], abs(inc), 1.f, false);
                
                //increment counter
                modulo[channel]+=inc;
                
                return blep_saw;
            }
        }
        else
        {
            return 0.f;
        }
    };
    inline T doPolyBLEP(T fModulo, T fInc, T fHeight, bool bRisingEdge)
    {
        T fPolyBLEP = 0.f;
        
        // leftside of discontinuity
        // -1 < t < 0
        if(fModulo > 1.f - fInc)
        {
            // calculate distance
            T t = (fModulo-1.f)/fInc;
            // calculate residual
            fPolyBLEP = fHeight*(t*t + 2.f*t + 1.f);
        }
        // rightside of discontinuity
        // 0 <= t < 1
        else if(fModulo < fInc)
        {
            // calculate distance
            T t = fModulo/fInc;
            // calculate residual
            fPolyBLEP = fHeight*(2.f*t - t*t - 1.f);
        }
        
        // substract for falling, add for rising edge
        if(!bRisingEdge)
            fPolyBLEP *= -1.f;
        
        return fPolyBLEP;
    };
    
    // ============== toggle buttons =============
    void toggleOSC(bool r)
    {
        run = r;
        cookFrequency();
    };
    void toggleBlep(bool r)
    {
        blep = r;
        cookFrequency();
    };
    // =============================================
    // =============== setters ====================
    void setFreq(T f)
    {
        freq = f;
        cookFrequency();
    };
    void setSampleRate(T sr)
    {
        currentSampleRate = sr;
    };
    // =============================================
    // =============== getters ====================
    bool getRunOSC()
    {
        return run;
    };
    bool getBlepOSC()
    {
        return blep;
    };
    T getFreq()
    {
        return freq;
    };
    // =============================================
    
private:

    T modulo[2];
    T inc;
    
    T currentSampleRate;
    T freq;
    
    //flag to start/stop oscillator
    bool run;
    bool blep;
    
    T currentSample[2];
};

}