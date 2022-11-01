/*
  ==============================================================================

    PolyBlepOSC.h
    Created: 21 Mar 2022 3:45:13pm
    Author:  Keepin' Soul

  ==============================================================================
*/

#ifndef PolyBlepOSC_h
#define PolyBlepOSC_h

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
    inline float getSample(int channel)
    {
        float trivial_saw = 0.f;
        float blep_saw = 0.f;
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
    inline float doPolyBLEP(float fModulo, float fInc, float fHeight, bool bRisingEdge)
    {
        float fPolyBLEP = 0.f;
        
        // leftside of discontinuity
        // -1 < t < 0
        if(fModulo > 1.f - fInc)
        {
            // calculate distance
            float t = (fModulo-1.f)/fInc;
            // calculate residual
            fPolyBLEP = fHeight*(t*t + 2.f*t + 1.f);
        }
        // rightside of discontinuity
        // 0 <= t < 1
        else if(fModulo < fInc)
        {
            // calculate distance
            float t = fModulo/fInc;
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
    void setFreq(float f)
    {
        freq = f;
        cookFrequency();
    };
    void setSampleRate(float sr)
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
    float getFreq()
    {
        return freq;
    };
    // =============================================
    
private:

    float modulo[2];
    float inc;
    
    float currentSampleRate;
    float freq;
    
    //flag to start/stop oscillator
    bool run;
    bool blep;
    
    float currentSample[2];
};

#endif /* PolyBlepOSC_h */
