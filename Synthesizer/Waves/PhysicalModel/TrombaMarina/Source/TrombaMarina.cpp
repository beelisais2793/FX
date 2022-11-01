/*
 ==============================================================================
 
 TrombaMarina.cpp
 Created: 25 Mar 2018 5:07:12pm
 Author:  Nikolaj Schwab Andersson
 
 ==============================================================================
 */

#include "TrombaMarina.h"
#include <cmath>
#include <iostream>

void TrombaMarina::setSampleRate(double sampleRate)
{
    fs = sampleRate;
    stringLength = (sampleRate/freq);
    spring.setSampleRate(fs);
    spring.setFrequency(freq, 0.1);

    brigdeDelay.initDelay(0.01, sampleRate);
    brigdeDelay.setDelayLengthInSamples(stringLength*pb);
    nutDelay.initDelay(0.01, sampleRate);
    nutDelay.setDelayLengthInSamples(stringLength*(1-pb));
}

void TrombaMarina::setFrequency(float frequency)
{
    freq = frequency;
    spring.setFrequency(freq, 0.1);

    stringLength = (fs/freq) * 0.5;
    brigdeDelay.setDelayLengthInSamples(stringLength*pb);
    nutDelay.setDelayLengthInSamples(stringLength*(1-pb));
}

void TrombaMarina::setBowingPoint(float bp)
{
    pb = bp;
    
    brigdeDelay.setDelayLengthInSamples(stringLength*pb);
    nutDelay.setDelayLengthInSamples(stringLength*(1-pb));
}

void TrombaMarina::calculateV(double& v, double vh, float vb)
{
    double v1, v2, vtemp, fv;
    
    double B2 = -0.2 * zslope - 0.3 * fb - zslope * vb - zslope * vh;
    double C2 = 0.06 * fb + zslope * vh * vb + 0.2 * zslope * vh + 0.3 * vb * fb + 0.1 * fb;
    double delta2 = B2 * B2 - 4 * zslope * C2;
    
    if (vh == vb) // MIDDLE
    {
        v = vb;
        stick = 1;
    }
    else if (delta2 < 0)
    {
        v = vb;    // the only possible solution is the stickone
        stick = 1;
    }
    else
    {
        if (stick)    //% if we are in the stick condition, we keep it........
        {
            fv = zslope*(vb-vh);
            
            if ((fv <= mus * fb) && (fv > 0)) // %.....if we did not find a value above the maximum bow force.
            {
                v = vb;
            }
            else
            {
                v1 = (-B2+sqrt(B2*B2-4*zslope*C2))/(2*zslope);
                v2 = (-B2-sqrt(B2*B2-4*zslope*C2))/(2*zslope);
                
                vtemp = fmin(v1, v2);  // we choose the minimum solution, because it is NOT the one in the middle
                
                stick = 0;
                
                if(vtemp > vb) //% we do not want a solution v>Vb
                {
                    v = vb;
                    stick = 1;
                }
                else
                {
                    v = vtemp;
                }
            }
        }
        else //% we are in SLIP, so we stay in SLIP.....
        {
            v1 = (-B2+sqrt(B2*B2-4*zslope*C2))/(2*zslope);
            v2 = (-B2-sqrt(B2*B2-4*zslope*C2))/(2*zslope);
            v = fmin(v1, v2);   // we choose the minimum solution, because it is NOT the one in the middle
            
            stick = 0;
            
            if(v > vb)
            {
                v = vb;
                stick = 1;
            }
        }
    }
}

float TrombaMarina::getOutput(float bowForce, float vb)
{
    fb = bowForce;
    double v = 0;

    double vin = nutDelay.getLPOutput();
    double vib = brigdeDelay.getLPOutput();
    double vh = vin + vib;
    
    calculateV(v, vh, vb);
    
    double f = zslope*(v - vh);
    

    double von = -(vib + (f/(2*Z)));  // new outgoing velocity to nut
    double vob = -(vin +  (f/(2*Z))); //new outgoing velocity to bridge

    
    spring.setInput(vob*100);
    spring.run();
    // update delay
    brigdeDelay.tick(vob);
    nutDelay.tick(von);
    
    return vob;// + spring.getOutput();
}

