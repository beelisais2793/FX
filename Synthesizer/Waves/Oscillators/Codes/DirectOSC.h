

/*
  ==============================================================================
    DirectOSC.h
    Created: 21 Mar 2022 3:45:13pm
    Author:  Keepin' Soul
  ==============================================================================
*/

#ifndef DirectOSC_h
#define DirectOSC_h

class DirectOSC {
public:
    DirectOSC(){
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
        float f_wT = (2.f * juce::MathConstants<double>::pi*freq) / (float)currentSampleRate;
        // coefficients according to design equations
        b1 = -2.f * std::cos(f_wT);
        b2 = 1.f;
        // set initial conditions so that first sample out is 0.0
        y_z1[0] = std::sin(-1.f * f_wT); // sin(wnT) = sin(w(–1)T)
        y_z2[0] = std::sin(-2.f * f_wT); // sin(wnT) = sin(w(–2)T)
        y_z1[1] = std::sin(-1.f * f_wT); // sin(wnT) = sin(w(–1)T)
        y_z2[1] = std::sin(-2.f * f_wT); // sin(wnT) = sin(w(–2)T)
    };
    
    float getSample(int channel)
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
    
    void setFreq(float f)
    {
        freq = f;
        cookFrequency();
    };
    
    void setSampleRate(float sr)
    {
        currentSampleRate = sr;
    };
    
    bool getRunOSC()
    {
        return runOSC;
    }
    
    float getFreq()
    {
        return freq;
    }

    
private:

    float currentSampleRate;
    float freq;
    
    // coefficients, 2nd Order FB
    float b1;
    float b2;
    // delay elements, 2nd Order FB
    float y_z1[2];
    float y_z2[2];
    //flag to start/stop oscillator
    bool runOSC;
    
    float currentSample[2];
};

#endif /* DirectOSC_h */

