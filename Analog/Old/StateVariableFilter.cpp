/*
  ==============================================================================

    StateVariableFilter.cpp
    Created: 10 May 2015 2:41:43pm
    Author:  Jordan Harris

  ==============================================================================
*/

#include "StateVariableFilter.h"

using namespace SoundWave;

//==============================================================================

// Calculates the frequency of a given pitch (MIDI) value.
double pitchToFreq(double pitch)
{
    return pow(2, (pitch - 69) / 12) * 440;
}

//==============================================================================
// Calculates the pitch (MIDI) of a given frequency value
double freqToPitch(double freq)
{
    return 69 + 12 * log2(freq / 440);
}

//==============================================================================

/** 
    Takes a value as input and clips it according to the min and max values.
    Returns the input if (minValue <= in <= maxValue). 
    If (in < minValue), then return minValue.
    If (in > maxValue), then return maxValue.
*/
double clipMinMax(double in, double minValue, double maxValue)
{
    if (in < minValue)
        return minValue;
    else if (in > maxValue)
        return maxValue;
    else
        return in;
}

//==============================================================================

/** 
    Takes a value as input and clips it according to the min value.
    Returns the input if (minValue <= in). 
    If (in < minValue), then return minValue.
*/
double clipMin(double in, double minValue)
{
    if (in < minValue)
        return minValue;
    else
        return in;
}

//==============================================================================

/**
    Crossfades linearly between two values (in0, in1). The value returned is 
    determined by the value of the xFadeCtrl argument.
    xFadeCtrl Range: 0->1
    - xFadeCtrl = 0    (only in0 comes through)
    - xFadeCtrl = 0.5  (equal mix of in0 and in1)
    - xfadeCtrl = 1    (only in1 comes through)
*/
double xFadeLin(double xFadeCtrl, double in0, double in1)
{
    // Clip the xFade parameter to only have range of 0->1
    xFadeCtrl = clipMinMax(xFadeCtrl, 0.0, 1.0);
    // Perform crossfading and return the value
    return (in0 * (1.0 - xFadeCtrl) + in1 * xFadeCtrl);
}

//==============================================================================

/**
    Parabolic Controller Shaper:
    "Bends" the controller curve torwards the X or Y axis.
    input range: (-1..0..1) maps to output range: (-1..0..1).
    bend range: (-1..0..1)
    - bend = -1 (max bend towards X axis)
    - bend = 0 (don't bend)
    - bend = 1 (max bend towards Y axis)
*/
double parCtrlShaper(double input, double bend)
{
    // clip input and bend because the shaper only works in that range.
    input = clipMinMax(input, -1.0, 1.0);
    bend = clipMinMax(bend, -1.0, 1.0);
    return input * ((bend + 1) - abs(input) * bend);
}

//==============================================================================

/**
    Normalizes a range of values to the range 0->1.
    (start/end should probably be the range of a parameter)
    - input: the value to be normalized
    - start: the start of the input's range
    - end: the end of the input's range
    Note: (start < end) and (start > end) are both valid.
*/
double normalizeRange(double input, double start, double end)
{
    return (input - start) / (end - start);
}


double resonanceToQ(double resonance)
{
    return 1.0 / (2.0 * (1.0 - resonance));
}


//==============================================================================

StateVariableFilter::StateVariableFilter()
{
    sampleRate = 44100.0f;				// default sample rate when constructed
    filterType = SVFLowpass;			// lowpass filter by default

    gCoeff = 1.0f;
    RCoeff = 1.0f;
    KCoeff = 0.0f;

    cutoffFreq = 1000.0f;
    Q = static_cast<float>(resonanceToQ(0.5));

    z1_A[0] = z2_A[0] = 0.0f;
    z1_A[1] = z2_A[1] = 0.0f;

    //smoothTimeMs = 0.0;		        // 0.0 milliseconds
}

StateVariableFilter::~StateVariableFilter()
{
}

// Member functions for setting the filter's parameters (and sample rate).
//==============================================================================
void StateVariableFilter::setFilterType(const int& newType)
{
    filterType = newType;
}

void StateVariableFilter::setCutoffPitch(const float& newCutoffPitch)
{
    if (active) {
        cutoffFreq = static_cast<float>(pitchToFreq(newCutoffPitch));
        //cutoffLinSmooth.setValue(cutoffFreq);
        calcFilter();
    }
}

void StateVariableFilter::setCutoffFreq(const float& newCutoffFreq)
{
    if (active) {
        cutoffFreq = newCutoffFreq;
        calcFilter();
    }
}

void StateVariableFilter::setResonance(const float& newResonance)
{
    if (active) {
        Q = static_cast<float>(resonanceToQ(newResonance));
        calcFilter();
    }
}

void StateVariableFilter::setQ(const float& newQ)
{
    if (active) {
        Q = newQ;
        calcFilter();
    }
}

void StateVariableFilter::setShelfGain(const float& newGain)
{
    if (active) {
        shelfGain = newGain;
        calcFilter();
    }
}

void StateVariableFilter::setFilter(const int& newType, const float& newCutoffFreq,
                                      const float& newResonance, const float& newShelfGain)
{
    filterType = newType;
    cutoffFreq = newCutoffFreq;
    Q = static_cast<float>(resonanceToQ(newResonance));
    shelfGain = newShelfGain;
    calcFilter();
}

void StateVariableFilter::setSampleRate(const float& newSampleRate)
{
    sampleRate = newSampleRate;
    //cutoffLinSmooth.reset(sampleRate, smoothTimeMs);
    calcFilter();
}

/*void StateVariableFilter::setSmoothingTimeInMs(const float & newSmoothingTimeMs)
{
    smoothTimeMs = newSmoothingTimeMs;
}*/

void StateVariableFilter::setIsActive(bool isActive)
{
    active = isActive;
}

//==============================================================================
void StateVariableFilter::calcFilter()
{
    if (active) {

        // prewarp the cutoff (for bilinear-transform filters)
        float wd = static_cast<float>(cutoffFreq * 2.0f * M_PI);
        float T = 1.0f / (float)sampleRate;
        float wa = (2.0f / T) * tan(wd * T / 2.0f);

        // Calculate g (gain element of integrator)
        gCoeff = wa * T / 2.0f;			// Calculate g (gain element of integrator)

        // Calculate Zavalishin's R from Q (referred to as damping parameter)
        RCoeff = 1.0f / (2.0f * Q);		
        
        // Gain for BandShelving filter
        KCoeff = shelfGain;				
    }
}

float StateVariableFilter::processAudioSample(const float& input, const int& channelIndex)
{
    if (active) {

        // Do the cutoff parameter smoothing per sample.
        //cutoffFreq = cutoffLinSmooth.getNextValue();
        //calcFilter();

        // Filter processing:
        const float HP = (input - (2.0f * RCoeff + gCoeff) * z1_A[channelIndex] - z2_A[channelIndex])
            / (1.0f + (2.0f * RCoeff * gCoeff) + gCoeff * gCoeff);

        const float BP = HP * gCoeff + z1_A[channelIndex];

        const float LP = BP * gCoeff + z2_A[channelIndex];

        const float UBP = 2.0f * RCoeff * BP;

        const float BShelf = input + UBP * KCoeff;

        const float Notch = input - UBP;

        const float AP = input - (4.0f * RCoeff * BP);

        const float Peak = LP - HP;

        z1_A[channelIndex] = gCoeff * HP + BP;		// unit delay (state variable)
        z2_A[channelIndex] = gCoeff * BP + LP;		// unit delay (state variable)

        // Selects which filter type this function will output.
        switch (filterType) {
        case SVFLowpass:
            return LP;
            break;
        case SVFBandpass:
            return BP;
            break;
        case SVFHighpass:
            return HP;
            break;
        case SVFUnitGainBandpass:
            return UBP;
            break;
        case SVFBandShelving:
            return BShelf;
            break;
        case SVFNotch:
            return Notch;
            break;
        case SVFAllpass:
            return AP;
            break;
        case SVFPeak:
            return Peak;
            break;
        default:
            return 0.0f;
            break;
        }
    }
    else {	// If not active, return input
        return input;
    }
}

void StateVariableFilter::processAudioBlock(float* const samples,  const int& numSamples, 
                                              const int& channelIndex)
{
    // Test if filter is active. If not, bypass it
    if (active) {

        // Loop through the sample block and process it
        for (int i = 0; i < numSamples; ++i) {
            
            // Do the cutoff parameter smoothing per sample.
            //cutoffFreq = cutoffLinSmooth.getNextValue();
            //calcFilter();       // calculate the coefficients for the smoother

            // Filter processing:
            const float input = samples[i];

            const float HP = (input - (2.0f * RCoeff + gCoeff) * z1_A[channelIndex] - z2_A[channelIndex])
                       / (1.0f + (2.0f * RCoeff * gCoeff) + gCoeff * gCoeff);
            
            const float BP = HP * gCoeff + z1_A[channelIndex];
            
            const float LP = BP * gCoeff + z2_A[channelIndex];

            const float UBP = 2.0f * RCoeff * BP;

            const float BShelf = input + UBP * KCoeff;

            const float Notch = input - UBP;

            const float AP = input - (4.0f * RCoeff * BP);

            const float Peak = LP - HP;

            z1_A[channelIndex] = gCoeff * HP + BP;		// unit delay (state variable)
            z2_A[channelIndex] = gCoeff * BP + LP;		// unit delay (state variable)

            // Selects which filter type this function will output.
            switch (filterType) {
            case SVFLowpass:
                samples[i] = LP;
                break;
            case SVFBandpass:
                samples[i] = BP;
                break;
            case SVFHighpass:
                samples[i] = HP;
                break;
            case SVFUnitGainBandpass:
                samples[i] = UBP;
                break;
            case SVFBandShelving:
                samples[i] = BShelf;
                break;
            case SVFNotch:
                samples[i] = Notch;
                break;
            case SVFAllpass:
                samples[i] = AP;
                break;
            case SVFPeak:
                samples[i] = Peak;
                break;
            default:
                samples[i] = 0.0f;
                break;
            }
        }
    }
}

//==============================================================================
