/*
    Simple One Pole Filter with a nice topology
    by Nikolaj Andersson, 27. March 2018
    
*/
#pragma once

class OnePoleFilter
{
public:
    void setSampleRate(double sampleRate)
    {
        T = 1.0/sampleRate;
    }
    void setCutoff(float cutoff)
    {
        g = cutoff;//std::clamp(cutoff, 0, 1);
    }
    float getLowpass(float input)
    {
        float v = (input-state) * g/(1+g);
        float out = v + state;
        state = out + v;
        return out;
    }
    float getHighpass(float input)
    {
        return input - getLowpass(input);
    }
private:

    float state = 0.0f;
    float T = 0.0f;
    float g = 0.45f;
};
