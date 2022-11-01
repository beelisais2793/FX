//
//  BMCReverb.h
//  CReverb
//
//  Created by Hans on 7/2/16.
//  Copyright © 2016 Hans Anderson. All rights reserved.
//

#ifndef BMCReverb_h
#define BMCReverb_h

#include <stdio.h>

#ifdef __APPLE__
    #include <Accelerate/Accelerate.h>
#else
    #include "BMCrossPlatformVDSP.h"
#endif

// default settings
#define BMCREVERB_WETMIX 0.15 // dryMix = sqrt(1 - wetMix^2)
#define BMCREVERB_NUMDELAYUNITS 4 // each unit contains 4 delays
#define BMCREVERB_PREDELAY 0.007 // (in seconds)
#define BMCREVERB_ROOMSIZE 0.100 // (seconds of sound travel time)
#define BMCREVERB_DEFAULTSAMPLERATE 44100.0
#define BMCREVERB_HIGHSHELFFC 8000.0 // above this frequency decay is faster
#define BMCREVERB_HFDECAYMULTIPLIER 6.0 // HF decay is this many times faster
#define BMCREVERB_HFSLOWDECAYMULTIPLIER 12.0 // used for sustain effect
#define BMCREVERB_RT60 1.2 // overall decay time
#define BMCREVERB_HIGHPASS_FC 30.0 // highpass filter on wet out
#define BMCREVERB_LOWPASS_FC 6000.0 // lowpass filter on wet out
#define BMCREVERB_CROSSSTEREOMIX 0.4 // mixing betwee L and R wet outputs
#define BMCREVERB_SLOWDECAYRT60 8.0 // RT60 time when hold pedal is down

#ifdef __cplusplus
extern "C" {
#endif
    
    // the CReverb struct
    typedef struct BMCReverb {
        float *delayLines, *feedbackBuffers, *mixingBuffers, *fb0, *fb1, *fb2, *fb3, *mb0, *mb1, *mb2, *mb3, *z1, *a1, *b0, *b1, *a1Slow, *b0Slow, *b1Slow, *delayTimes, *decayGainAttenuation, *slowDecayGainAttenuation, *leftOutputTemp, *delayOutputSigns, *dryL, *dryR;
        size_t *bufferLengths, *bufferStartIndices, *bufferEndIndices, *rwIndices;
        float minDelay_seconds, maxDelay_seconds, sampleRate, wetGain, dryGain, inputAttenuation, matrixAttenuation, straightStereoMix, crossStereoMix, hfDecayMultiplier, hfSlowDecayMultiplier, highShelfFC, rt60, slowDecayRT60, highpassFC, lowpassFC;
        size_t delayUnits, newNumDelayUnits, numDelays, halfNumDelays, fourthNumDelays, threeFourthsNumDelays, samplesTillNextWrap, totalSamples;
        float* twoChannelFilterData [2];
        vDSP_biquadm_Setup mainFilterSetup;
        double mainFilterCoefficients[5*2*2], *fcChLSec0, *fcChRSec0, *fcChLSec1, *fcChRSec1;
        bool slowDecay, settingsQueuedForUpdate, autoSustain;
    } BMCReverb;
    
    
    
    /*
     * publicly usable functions
     */
    
    
    // initialisation and cleanup
    void BMCReverbInit(struct BMCReverb* rv);
    void BMCReverbFree(struct BMCReverb* rv);
    
    // main audio processing function
    void BMCReverbProcessBuffer(struct BMCReverb* rv, const float* inputL, const float* inputR, float* outputL, float* outputR, size_t numSamples);
    
    
    /*
     * settings that can be safely changed during reverb operation
     */
    
    // wetGain in [0.0,1.0]. As wet gain increases, dry gain decreases automatically to keep a constant output volume.
    void BMCReverbSetWetGain(struct BMCReverb* rv, float wetGain);
    
    
    // crossMix in [0.0,1.0]. Sets amount of mixing betwee L and R channels.
    // 1.0 meanse that L and R signals are mixed in equal amounts to
    // both channels.  This setting has no effect on the dry signal
    void BMCReverbSetCrossStereoMix(struct BMCReverb* rv, float crossMix);
    
    
    // setting multiplier=n means that high frequencies decay n times faster
    // than the rest of the signal. The filter that does the HF decay is
    // gently sloped, so this setting will have some effect on the decay
    // time of frequencies below the HFDecayFC.
    // multiplier must be >= 1.0;
    void BMCReverbSetHFDecayMultiplier(struct BMCReverb* rv, float multiplier);
    
    
    // sets the cutoff frequency of the filters that do high frequency decay
    void BMCReverbSetHFDecayFC(struct BMCReverb* rv, float fc);
    
    
    // RT60 measures the time it takes for the reverb to decay to -60db
    // relative to its original level. Examples:
    // 0.2 = speaker cabinet simulator
    // 0.5 = closet sized room
    // 0.9 = bedroom
    // 1.3 = moderately sized auditorium
    // 2.3 = huge auditorium
    // 10  = world's longest echoing cathedral
    void BMCReverbSetRT60DecayTime(struct BMCReverb* rv, float rt60);
    
    
    // sets the sustain mode on=true or off=false.  This can be used to
    // simulate a sustain pedal effect by temporarily switching the reverb
    // to a long RT60 decay time.
    void BMCReverbSetSlowDecayState(struct BMCReverb* rv, bool slowDecay);
    
    
    // with auto-sustain on, the reverb watches the input volume and
    // automatically switches the slowDecay on and off.  It sustains
    // as long as the input sustains but goes back to a short decay time
    // when the input volume drops below a threshold.
    void BMCReverbSetAutoSustain(struct BMCReverb* rv, bool autoSustain);
    

    
    
    
    /*
     * Settings for which changes will queue until the end of the next buffer.
     * Call these functions any time, but changes won't take effect until 
     * it's safe to apply them.
     */
    
    // A delay unit is a set of four delay lines.  We are using a sparse
    // mixing matrix that works in groups of four.  So, to get an FDN with
    // 20 delays, set delayUnits = 5.
    //
    // Larger numbers of delay units consume more processing power and produce
    // denser, smoother echoes.  However, the smaller networks have a more
    // dynamic sound that changes over time so using more delayUnits does not
    // necessarily give you better sound.
    void BMCReverbSetNumDelayUnits(struct BMCReverb* rv, size_t delayUnits);
    
    
    // The shortest delay time in the network is the predelay.  Between the
    // moment a signal enters the reverb and when the predelay time is
    // no wet reverb output is generated. Long pre-delay gives the feeling
    // of a very wide, tall room. Examples:
    //
    // preDelay = 0.0001; // speaker cabinet
    // preDelay = 0.001;  // tiny room
    // preDelay = 0.007;  // mid sized room
    // preDelay = 0.015;  // big auditorium
    //
    // setting a long pre-delay makes the sound muddled.
    void BMCReverbSetPreDelay(struct BMCReverb* rv, float preDelay_seconds);
    
    
    // roomSize controls the length of the longest delay in the network.
    // Changing this setting may not actually make you feel like you are
    // hearing a larger room, since preDelay and RT60 also affect the
    // perception of room size.  Setting longer values of roomSize make
    // a sparser reverb, which may sound grainy, especially with drums.
    // When using very long RT60 times, greater than 5 seconds, we recommend
    // using a large setting for roomSize to get a reverb sound that changes
    // over time as it echoes.
    void BMCReverbSetRoomSize(struct BMCReverb* rv, float roomSize_seconds);
    
    // sets the sample rate of the input audio.  Reverb will work at any
    // sample rate you set, even if it's not correct, but setting this
    // correctly will ensure that delay times and filter frequencies are
    // calculated correctly.
    void BMCReverbSetSampleRate(struct BMCReverb* rv, float sampleRate);
    
    
    // sets the cutoff frequency of a second order butterworth highpass
    // filter on the wet signal.  (that's 12db cutoff slope).  This does
    // not affect the dry signal at all.
    void BMCReverbSetHighPassFC(struct BMCReverb* rv, float fc);
    
    
    // sets the cutoff frequency of a second order butterworth lowpass
    // filter on the wet signal.  (that's 12db cutoff slope).  This does
    // not affect the dry signal at all.
    void BMCReverbSetLowPassFC(struct BMCReverb* rv, float fc);
    
    
#ifdef __cplusplus
}
#endif

#endif /* BMCReverb_h */
