#pragma once
#ifndef RCH_OSCILLATORS_TEMPLATES_BANDLIMITED_SAW_H_INCLUDED
#define RCH_OSCILLATORS_TEMPLATES_BANDLIMITED_SAW_H_INCLUDED
// ---- MODULE CODE STARTS BELOW ---- //


// BAND-LIMITED SAWTOOTH OSCILLATOR
//
// Implemented after Barry Truax' Handbook for Acoustic Ecology
// https://www.sfu.ca/sonic-studio/handbook/index.html
//
/** Creates a band-limited mono per-sample sawtooth oscillator. */
class Saw : private Helpers::SkeletonBandlimited
{
public:
    
    Saw  () {}
    ~Saw () {}
    
    //==============================================================================
    using Helpers::SkeletonBandlimited::reset;
    
    using Helpers::SkeletonBandlimited::setSampleRate;
    using Helpers::SkeletonBandlimited::setFrequency;
    using Helpers::SkeletonBandlimited::setAmplitude;
    using Helpers::SkeletonBandlimited::setVolume;
    using Helpers::SkeletonBandlimited::setState;
    using Helpers::SkeletonBandlimited::setPhaseOffset;
    using Helpers::SkeletonBandlimited::setDirection;
    using Helpers::SkeletonBandlimited::setMaxHarmonics;
    
    using Helpers::SkeletonBandlimited::getSampleRate;
    using Helpers::SkeletonBandlimited::getFrequency;
    using Helpers::SkeletonBandlimited::getAmplitude;
    using Helpers::SkeletonBandlimited::getVolume;
    using Helpers::SkeletonBandlimited::getState;
    using Helpers::SkeletonBandlimited::getPhaseOffset;
    using Helpers::SkeletonBandlimited::getDirection;
    using Helpers::SkeletonBandlimited::getMaxHarmonics;
    
    //==============================================================================
    /** Calculates and returns the next square wave sample. */
    const double& tick () override
    {
        // Make sure the values we'll work with are correctly set
        assert(samplerate > 0.0 && "Samplerate not correctly set");
        assert(nyquist == samplerate * 0.5 && "Samplerate not correctly set");
        assert(frequency <= nyquist && "Frequency can't be above nyquist");
        
        // Increase phase by +1 step
        phase += fractionFrequency;
        
        // Constrain/wrap phase value to sensible boundaries [0,1]
        phase += ((phase >= 1.0) * -1.0) + ((phase < 0.0) * 1.0);
        
        // Reset the output value to 0, generated harmonics will be added to this later
        state = 0.0;
        
        // Reset the number of already calculated harmonics to zero
        dunHarmonics = 0;
        
        // Iterate through as many harmonic overtones as should be generated
        for (unsigned int harmonic=1; dunHarmonics<numHarmonics; ++harmonic)
        {
            // This is the number of the currently generated harmonic step,
            // can be used as amplitude modifier later.
            harmonicStep = static_cast<double>(harmonic);
            
            // The phase position for the currently generated harmonic. It's
            // basically multiplying the frequency, doubling the speed of the
            // rotation, etc. which results in a shifted wave. The sawtooth's
            // angle (rising/falling) is influenced by the direction factor.
            harmonicPhase = (phase * -direction) * harmonicStep;
            
            // Generate this harmonic sine wave, scale it down by harmonicStep
            // and add it to the already existing wave.
            state += std::sin(harmonicPhase * M_2PI) / harmonicStep;
            
            // Inrement the number of generated harmonics.
            ++dunHarmonics;
        }
        
        // Adjust the volume
        state *= M_2_PI * amplitude;
        
        // Return the generated sample value
        return state;
    }
    
}; // end class RCH::Oscillators::Templates::Bandlimited::Saw


// ---- MODULE CODE ENDS ABOVE ---- //
#endif // #ifndef RCH_OSCILLATORS_TEMPLATES_BANDLIMITED_SAW_H_INCLUDED
