#pragma once
#include "FeedbackDelay.h"

class NonLinearAllpass
{
public:
    // ====== COEFFICIENTS =======
    void setCoefficients (float coeffA, float coeffB) // Takes values between -1 and 1
    {
        coeffA1 = coeffA;
        coeffA2 = coeffB;
    }

    // ====== NON-LINEAR ALLPASS =======
    float process (float& input)
    {
        float aIn = input + oldy;
        
        // ====== NON-LINEARITY =======
        if (aIn >= 0)
            aIn = 0;
        else
            aIn = 1;
        
        coeffA1 = coeffA1 * (aIn * -1);
        coeffA2 = coeffA2 * aIn * -1;
        
        float c = coeffA1 + coeffA2;
        
        // ====== ALLPASS EQUATION =======
        float output = c * input + oldx - c * oldy;
        oldx = input;
        oldy = output;
        
        return oldy;
    }
    
private:
    // ====== COEFFICIENTS =======
    float coeffA1;
    float coeffA2;
    
    Delay sampleDelay;
    
    float oldy = 0.0f;
    float oldx = 0.0f;
};


