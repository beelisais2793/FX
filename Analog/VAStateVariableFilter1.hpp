#pragma once
#include <cmath>


using std::pow;
using std::tan;
using std::sqrt;

namespace Filters::StateVariableFilter2 {

//==============================================================================


//==============================================================================

/** The type of filter that the State Variable Filter will output. */
enum SVFType {
    SVFLowpass = 0,
    SVFBandpass,
    SVFHighpass,
    SVFUnitGainBandpass,
    SVFBandShelving,
    SVFNotch,
    SVFAllpass,
    SVFPeak
};

//==============================================================================
class StateVariableFilter : public FilterProcessor
{
public:
    /** Create and initialize the filter with default values defined in constructor. */
    StateVariableFilter();

    //------------------------------------------------------------------------------

    ~StateVariableFilter();
    
    //------------------------------------------------------------------------------
    
    /**	Sets the type of the filter that processAudioSample() or processAudioBlock() will 
        output. This filter can choose between 8 different types using the enums listed 
        below or the int given to each.
        0: SVFLowpass
        1: SVFBandpass
        2: SVFHighpass
        3: SVFUnitGainBandpass
        4: SVFBandShelving
        5: SVFNotch
        6: SVFAllpass
        7: SVFPeak
    */
    void setFilterType(const int& newType);

    //------------------------------------------------------------------------------
    /**	Used for changing the filter's cutoff parameter logarithmically by 
        pitch (MIDI note #)
    */
    void setCutoffPitch(const DspFloatType& newCutoff);

    //------------------------------------------------------------------------------
    /**	Used for changing the filter's cutoff parameter linearly by frequency (Hz) */
    void setCutoffFreq(const DspFloatType& newCutoff);

    //------------------------------------------------------------------------------
    /** Used for setting the resonance amount. This is then converted to a Q
        value, which is used by the filter.
        Range: (0-1)
    */
    void setResonance(const DspFloatType& newResonance);

    //------------------------------------------------------------------------------
    /** Used for setting the filter's Q amount. This is then converted to a
        damping parameter called R, which is used in the original filter.
    */
    void setQ(const DspFloatType& newQ);

    //------------------------------------------------------------------------------
    /**	Sets the gain of the shelf for the BandShelving filter only. */
    void setShelfGain(const DspFloatType& newGain);

    //------------------------------------------------------------------------------
    /**	Statically set the filters parameters. */
    void setFilter(const int& newType, const DspFloatType& newCutoff, 
                   const DspFloatType& newResonance, const DspFloatType& newShelfGain);

    //------------------------------------------------------------------------------	
    /**	Set the sample rate used by the host. Needs to be used to accurately
        calculate the coefficients of the filter from the cutoff.
        Note: This is often used in AudioProcessor::prepareToPlay
    */
    void setSampleRate(const DspFloatType& newSampleRate);

    //------------------------------------------------------------------------------
    /**	Sets the time that it takes to interpolate between the previous value and
        the current value. For this filter, the smoothing is only happening for
        the filters cutoff frequency.
    */
    //void setSmoothingTimeInMs(const DspFloatType& newSmoothingTimeMs);

    //------------------------------------------------------------------------------
    /** Sets whether the filter will process data or not.
        - If (isActive = true) then the filter will process data
        - If (isActive = false) then the filter will be bypassed
    */
    void setIsActive(bool isActive);

    //------------------------------------------------------------------------------
    /**	Performs the actual processing for one sample of data, on 2 channels.
        If 2 channels are needed (stereo), use channel index (channelIdx) to 
        specify which channel is being processed (i.e. 0 for left, 1 for right).
    */
    DspFloatType processAudioSample(const DspFloatType& input, const int& channelIndex);

    //------------------------------------------------------------------------------	
    /**	Performs the actual processing for a block of samples, on 2 channels.
        If 2 channels are needed (stereo), use channel index (channelIdx) to 
        specify which channel is being processed (i.e. 0 for left, 1 for right).
        Note:
        This processes the information sent to the samples argument and 
        does it through a pointer. Therefore, no value needs to be
        returned.
    */
    void processAudioBlock(DspFloatType* const samples, const int& numSamples,
                           const int& channelIndex);

    //------------------------------------------------------------------------------

    enum {
        PORT_CUTOFF,
        PORT_FILTERTYPE,
        PORT_Q,
        PORT_SHELFGAIN,
    };
    void setPort(int port, DspFloatType v) 
    {
        switch(port)
        {
            case PORT_CUTOFF: setFilter(filterType,v,Q,shelfGain); break;
            case PORT_FILTERTYPE: setFilter(v,cutoffFreq,Q,shelfGain); break;
            case PORT_Q: setFilter(filterType,cutoffFreq,v,shelfGain); break;
            case PORT_SHELFGAIN: setFilter(filterType,cutoffFreq,Q,v); break;
        }
    }
    DspFloatType Tick(DspFloatType input) {        
        return processAudioSample(input,0);        
    }
    void ProcessBlock(size_t n, DspFloatType * input, DspFloatType * output) {
        for(size_t i = 0; i < n; i++) output[i] = Tick(input[i]);
    }
    
    
private:
    //==============================================================================
    //	Calculate the coefficients for the filter based on parameters.
    void calcFilter();

    //	Parameters:
    int filterType;
    DspFloatType cutoffFreq;
    DspFloatType Q;
    DspFloatType shelfGain;

    DspFloatType sampleRate;
    bool active = true;	// is the filter processing or not

    //	Coefficients:
    DspFloatType gCoeff;		// gain element 
    DspFloatType RCoeff;		// feedback damping element
    DspFloatType KCoeff;		// shelf gain element

    DspFloatType z1_A[2], z2_A[2];		// state variables (z^-1)

};

//==============================================================================

// Calculates the frequency of a given pitch (MIDI) value.
DspFloatType pitchToFreq(DspFloatType pitch)
{
    return pow(2, (pitch - 69) / 12) * 440;
}

//==============================================================================
// Calculates the pitch (MIDI) of a given frequency value
DspFloatType freqToPitch(DspFloatType freq)
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
DspFloatType clipMinMax(DspFloatType in, DspFloatType minValue, DspFloatType maxValue)
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
DspFloatType clipMin(DspFloatType in, DspFloatType minValue)
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
DspFloatType xFadeLin(DspFloatType xFadeCtrl, DspFloatType in0, DspFloatType in1)
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
DspFloatType parCtrlShaper(DspFloatType input, DspFloatType bend)
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
DspFloatType normalizeRange(DspFloatType input, DspFloatType start, DspFloatType end)
{
    return (input - start) / (end - start);
}


DspFloatType resonanceToQ(DspFloatType resonance)
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
    Q = static_cast<DspFloatType>(resonanceToQ(0.5));

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

void StateVariableFilter::setCutoffPitch(const DspFloatType& newCutoffPitch)
{
    if (active) {
        cutoffFreq = static_cast<DspFloatType>(pitchToFreq(newCutoffPitch));
        //cutoffLinSmooth.setValue(cutoffFreq);
        calcFilter();
    }
}

void StateVariableFilter::setCutoffFreq(const DspFloatType& newCutoffFreq)
{
    if (active) {
        cutoffFreq = newCutoffFreq;
        calcFilter();
    }
}

void StateVariableFilter::setResonance(const DspFloatType& newResonance)
{
    if (active) {
        Q = static_cast<DspFloatType>(resonanceToQ(newResonance));
        calcFilter();
    }
}

void StateVariableFilter::setQ(const DspFloatType& newQ)
{
    if (active) {
        Q = newQ;
        calcFilter();
    }
}

void StateVariableFilter::setShelfGain(const DspFloatType& newGain)
{
    if (active) {
        shelfGain = newGain;
        calcFilter();
    }
}

void StateVariableFilter::setFilter(const int& newType, const DspFloatType& newCutoffFreq,
                                      const DspFloatType& newResonance, const DspFloatType& newShelfGain)
{
    filterType = newType;
    cutoffFreq = newCutoffFreq;
    Q = static_cast<DspFloatType>(resonanceToQ(newResonance));
    shelfGain = newShelfGain;
    calcFilter();
}

void StateVariableFilter::setSampleRate(const DspFloatType& newSampleRate)
{
    sampleRate = newSampleRate;
    //cutoffLinSmooth.reset(sampleRate, smoothTimeMs);
    calcFilter();
}

/*void StateVariableFilter::setSmoothingTimeInMs(const DspFloatType & newSmoothingTimeMs)
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
        DspFloatType wd = static_cast<DspFloatType>(cutoffFreq * 2.0f * M_PI);
        DspFloatType T = 1.0f / (DspFloatType)sampleRate;
        DspFloatType wa = (2.0f / T) * tan(wd * T / 2.0f);

        // Calculate g (gain element of integrator)
        gCoeff = wa * T / 2.0f;			// Calculate g (gain element of integrator)

        // Calculate Zavalishin's R from Q (referred to as damping parameter)
        RCoeff = 1.0f / (2.0f * Q);		
        
        // Gain for BandShelving filter
        KCoeff = shelfGain;				
    }
}

DspFloatType StateVariableFilter::processAudioSample(const DspFloatType& input, const int& channelIndex)
{
    if (active) {

        // Do the cutoff parameter smoothing per sample.
        //cutoffFreq = cutoffLinSmooth.getNextValue();
        //calcFilter();

        // Filter processing:
        const DspFloatType HP = (input - (2.0f * RCoeff + gCoeff) * z1_A[channelIndex] - z2_A[channelIndex])
            / (1.0f + (2.0f * RCoeff * gCoeff) + gCoeff * gCoeff);

        const DspFloatType BP = HP * gCoeff + z1_A[channelIndex];

        const DspFloatType LP = BP * gCoeff + z2_A[channelIndex];

        const DspFloatType UBP = 2.0f * RCoeff * BP;

        const DspFloatType BShelf = input + UBP * KCoeff;

        const DspFloatType Notch = input - UBP;

        const DspFloatType AP = input - (4.0f * RCoeff * BP);

        const DspFloatType Peak = LP - HP;

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

void StateVariableFilter::processAudioBlock(DspFloatType* const samples,  const int& numSamples, 
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
            const DspFloatType input = samples[i];

            const DspFloatType HP = (input - (2.0f * RCoeff + gCoeff) * z1_A[channelIndex] - z2_A[channelIndex])
                       / (1.0f + (2.0f * RCoeff * gCoeff) + gCoeff * gCoeff);
            
            const DspFloatType BP = HP * gCoeff + z1_A[channelIndex];
            
            const DspFloatType LP = BP * gCoeff + z2_A[channelIndex];

            const DspFloatType UBP = 2.0f * RCoeff * BP;

            const DspFloatType BShelf = input + UBP * KCoeff;

            const DspFloatType Notch = input - UBP;

            const DspFloatType AP = input - (4.0f * RCoeff * BP);

            const DspFloatType Peak = LP - HP;

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

};

