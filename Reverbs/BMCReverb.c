//
//  BMCReverb.c
//  A feedback delay network reverb for Macintosh and iOS
//
//  Created by bluemangoo@bluemangoo.com on 7/2/16.
//  This file is provided free without any restrictions on its use.
//

#include "BMCReverb.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>


#ifdef __cplusplus
extern "C" {
#endif
    
#define BMCREVERB_MATRIXATTENUATION 0.5 // 1/sqrt(4) keep the mixing unitary
#define BMCREVERB_TEMPBUFFERLENGTH 256 // buffered operation in chunks of 256
    
#define BM_MAX(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define BM_MIN(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
    
    
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
    
#ifndef M_E
#define M_E 2.71828182845904523536
#endif
    
#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880
#endif
    
    
    /*
     * these functions should be called only from functions within this file
     */
    void BMCReverbInitIndices(struct BMCReverb* rv);
    void BMCReverbIncrementIndices(struct BMCReverb* rv);
    void BMCReverbUpdateDelayTimes(struct BMCReverb* rv);
    void BMCReverbUpdateDecayHighShelfFilters(struct BMCReverb* rv);
    void BMCReverbUpdateRT60DecayTime(struct BMCReverb* rv);
    double BMCReverbDelayGainFromRT60(double rt60, double delayTime);
    void BMCReverbProcessWetSample(struct BMCReverb* rv, float inputL, float inputR, float* outputL, float* outputR);
    void BMCReverbUpdateNumDelayUnits(struct BMCReverb* rv);
    void BMCReverbPointersToNull(struct BMCReverb* rv);
    void BMCReverbRandomiseOrder(float* list, size_t seed, size_t length);
    void BMCReverbInitDelayOutputSigns(struct BMCReverb* rv);
    void BMCReverbUpdateMainFilter(struct BMCReverb* rv);
    void BMCReverbUpdateSettings(struct BMCReverb* rv);
    
    
    
    
    
    
    /*
     * Initialization: this MUST be called before running the reverb
     */
    void BMCReverbInit(struct BMCReverb* rv){
        // initialize all pointers to NULL
        BMCReverbPointersToNull(rv);
        
        // create pointers to the various filter sections
        rv->fcChLSec0 = rv->mainFilterCoefficients + 0*2*5 + 0*5;
        rv->fcChLSec1 = rv->mainFilterCoefficients + 1*2*5 + 0*5;
        rv->fcChRSec0 = rv->mainFilterCoefficients + 0*2*5 + 1*5;
        rv->fcChRSec1 = rv->mainFilterCoefficients + 1*2*5 + 1*5;
        
        // initialize default settings
        rv->sampleRate = BMCREVERB_DEFAULTSAMPLERATE;
        rv->delayUnits = BMCREVERB_NUMDELAYUNITS;
        rv->matrixAttenuation = BMCREVERB_MATRIXATTENUATION;
        rv->highShelfFC = BMCREVERB_HIGHSHELFFC;
        rv->hfDecayMultiplier = BMCREVERB_HFDECAYMULTIPLIER;
        rv->hfSlowDecayMultiplier = BMCREVERB_HFSLOWDECAYMULTIPLIER;
        rv->minDelay_seconds = BMCREVERB_PREDELAY;
        rv->maxDelay_seconds = BMCREVERB_ROOMSIZE;
        rv->rt60 = BMCREVERB_RT60;
        rv->delayUnits = BMCREVERB_NUMDELAYUNITS;
        rv->slowDecay = false;
        rv->settingsQueuedForUpdate = false;
        rv->slowDecayRT60 = BMCREVERB_SLOWDECAYRT60;
        rv->newNumDelayUnits = BMCREVERB_NUMDELAYUNITS;
        rv->autoSustain=false;
        BMCReverbSetHighPassFC(rv, BMCREVERB_HIGHPASS_FC);
        BMCReverbSetLowPassFC(rv, BMCREVERB_LOWPASS_FC);
        BMCReverbSetWetGain(rv, BMCREVERB_WETMIX);
        BMCReverbSetCrossStereoMix(rv, BMCREVERB_CROSSSTEREOMIX);
        
        // initialize the filter setup
        rv->mainFilterSetup = vDSP_biquadm_CreateSetup(rv->mainFilterCoefficients, 2, 2);
        
        // initialize all the delays and delay-dependent settings
        BMCReverbUpdateNumDelayUnits(rv);
    }
    
    
    
    
    /*
     * works in place and allows left and right inputs to point to
     * the same data for mono to stereo operation
     */
    void BMCReverbProcessBuffer(struct BMCReverb* rv, const float* inputL, const float* inputR, float* outputL, float* outputR, size_t numSamples){
        
        // don't process anything if there are nan values in the input
        if (isnan(inputL[0]) || isnan(inputR[0])) {
            memset(outputL, 0, sizeof(float)*numSamples);
            memset(outputR, 0, sizeof(float)*numSamples);
            return;
        }
        
        
        // this requires buffer memory so we do it in limited sized chunks to
        // avoid having to adjust the buffer length at runtime
        size_t samplesLeftToMix = numSamples;
        size_t samplesMixingNext = BM_MIN(BMCREVERB_TEMPBUFFERLENGTH,samplesLeftToMix);
        size_t bufferedProcessingIndex = 0;
        while (samplesLeftToMix != 0) {
            
            
            if(rv->autoSustain){
                // check volume of the current frame
                float volume;
                vDSP_svesq(inputL+bufferedProcessingIndex, 1, &volume, samplesMixingNext);
                
                // if the volume is high, enable sustain mode
                if((volume / (float)samplesMixingNext) > 0.001)
                    BMCReverbSetSlowDecayState(rv, true);
                
                // if the volume is very low, disable sustain mode
                if((volume / (float)samplesMixingNext) < 0.00001)
                    BMCReverbSetSlowDecayState(rv, false);
            }
            
            
            // backup the input to allow in place processing
            memcpy(rv->dryL, inputL+bufferedProcessingIndex, sizeof(float)*samplesMixingNext);
            memcpy(rv->dryR, inputR+bufferedProcessingIndex, sizeof(float)*samplesMixingNext);
            
            
            
            // process the reverb to get the wet signal
            for (size_t i=bufferedProcessingIndex; i < bufferedProcessingIndex+samplesMixingNext; i++)
                BMCReverbProcessWetSample(rv, inputL[i], inputR[i], &outputL[i], &outputR[i]);
            
            
            
            // mix R and L wet signals
            // mix left and right to left temp
            vDSP_vsmsma(outputL+bufferedProcessingIndex, 1, &rv->straightStereoMix, outputR+bufferedProcessingIndex, 1, &rv->crossStereoMix, rv->leftOutputTemp, 1, samplesMixingNext);
            // mix right and left to right
            vDSP_vsmsma(outputR+bufferedProcessingIndex, 1, &rv->straightStereoMix, outputL+bufferedProcessingIndex, 1, &rv->crossStereoMix, outputR+bufferedProcessingIndex, 1, samplesMixingNext);
            // copy left temp back to left output
            memcpy(outputL+bufferedProcessingIndex, rv->leftOutputTemp, sizeof(float)*samplesMixingNext);
            
            
            
            // mix dry and wet signals
            vDSP_vsmsma(rv->dryL, 1, &rv->dryGain, outputL+bufferedProcessingIndex, 1, &rv->wetGain, outputL+bufferedProcessingIndex, 1, samplesMixingNext);
            vDSP_vsmsma(rv->dryR, 1, &rv->dryGain, outputR+bufferedProcessingIndex, 1, &rv->wetGain, outputR+bufferedProcessingIndex, 1, samplesMixingNext);
            
            
            
            // update the number of samples left to process in the buffer
            samplesLeftToMix -= samplesMixingNext;
            bufferedProcessingIndex += samplesMixingNext;
            samplesMixingNext = BM_MIN(BMCREVERB_TEMPBUFFERLENGTH,samplesLeftToMix);
        }
        
        
        
        // filter the wet output signal (highpass and lowpass)
        //
        // combine the two channels into a single 2-dimensional array as required
        // by vDSP_biquadm
        rv->twoChannelFilterData[0] = outputL;
        rv->twoChannelFilterData[1] = outputR;
        // apply a multilevel biquad filter to both channels
        vDSP_biquadm(rv->mainFilterSetup, (const float**)rv->twoChannelFilterData, 1, rv->twoChannelFilterData, 1, numSamples);
        
        
        
        /*
         * if an update requiring memory allocation was requested, do it now.
         */
        if (rv->settingsQueuedForUpdate)
            BMCReverbUpdateSettings(rv);
    }
    
    
    
    void BMCReverbUpdateSettings(struct BMCReverb* rv){
        BMCReverbUpdateNumDelayUnits(rv);
        BMCReverbUpdateMainFilter(rv);
    }
    
    void BMCReverbSetSlowDecayState(struct BMCReverb* rv, bool slowDecay){
        rv->slowDecay = slowDecay;
    }
    
    void BMCReverbSetAutoSustain(struct BMCReverb* rv, bool autoSustain){
        rv->autoSustain = autoSustain;
    }
    
    
    void BMCReverbUpdateMainFilter(struct BMCReverb* rv){
        if(rv->mainFilterSetup)
            vDSP_biquadm_DestroySetup(rv->mainFilterSetup);
        
        rv->mainFilterSetup = vDSP_biquadm_CreateSetup(rv->mainFilterCoefficients, 2, 2);
    }
    
    
    // second order butterworth highpass filter on wet signal
    void BMCReverbSetHighPassFC(struct BMCReverb* rv, float fc){
        rv->highpassFC = fc;
        
        double coeffs[5];
        
        double* b0 = coeffs;
        double* b1 = b0 + 1;
        double* b2 = b1 + 1;
        double* a1 = b2 + 1;
        double* a2 = a1 + 1;
        
        // if the cutoff frequency is less than 2 hz, bypass the filter
        if (fc < 2.0){
            *b0 = 1.0;
            *b1 = *b2 = *a1 = *a2 = 0.0;
        }
        
        // else, for useful settings of fc, setup the filter
        else {
            double gamma = tan(M_PI * fc / (double)rv->sampleRate);
            double gamma_2 = gamma * gamma;
            double gamma_x_sqrt_2 = gamma * M_SQRT2;
            double one_over_denominator = 1.0 / (gamma_2 + gamma_x_sqrt_2 + 1.0);
            
            *b0 = 1.0 * one_over_denominator;
            *b1 = -2.0 * one_over_denominator;
            *b2 = *b0;
            
            *a1 = 2.0 * (gamma_2 - 1.0) * one_over_denominator;
            *a2 = (gamma_2 - gamma_x_sqrt_2 + 1.0) * one_over_denominator;
        }
        
        // copy the same filter coefficients to both channels
        memcpy(rv->fcChLSec0, coeffs, sizeof(double)*5);
        memcpy(rv->fcChRSec0, coeffs, sizeof(double)*5);
        rv->settingsQueuedForUpdate = true;
    }
    
    
    
    
    
    
    // second order butterworth lowpass on wet signal
    void BMCReverbSetLowPassFC(struct BMCReverb* rv, float fc){
        rv->lowpassFC = fc;
        
        double coeffs [5];
        
        double* b0 = coeffs;
        double* b1 = b0 + 1;
        double* b2 = b1 + 1;
        double* a1 = b2 + 1;
        double* a2 = a1 + 1;
        
        // if fc is greater than 99% of the nyquyst frequency, bypass the filter
        if (fc > 0.99*0.5*rv->sampleRate){
            *b0 = 1.0;
            *b1 = *b2 = *a1 = *a2 = 0.0;
        }
        
        // else, the cutoff frequency is non-trivial
        else {
            double gamma = tan(M_PI * fc / (double)rv->sampleRate);
            double gamma_2 = gamma * gamma;
            double gamma_x_sqrt_2 = gamma * M_SQRT2;
            double one_over_denominator = 1.0 / (gamma_2 + gamma_x_sqrt_2 + 1.0);
            
            *b0 = gamma_2 * one_over_denominator;
            *b1 = 2.0 * *b0;
            *b2 = *b0;
            
            *a1 = 2.0 * (gamma_2 - 1.0) * one_over_denominator;
            *a2 = (gamma_2 - gamma_x_sqrt_2 + 1.0) * one_over_denominator;
        }
        
        // copy the same filter coefficients to both channels
        memcpy(rv->fcChLSec1, coeffs, sizeof(double)*5);
        memcpy(rv->fcChRSec1, coeffs, sizeof(double)*5);
        rv->settingsQueuedForUpdate = true;
    }
    
    
    
    
    // this is the decay time of the reverb in normal operation
    void BMCReverbSetRT60DecayTime(struct BMCReverb* rv, float rt60){
        assert(rt60 >= 0.0);
        rv->rt60 = rt60;
        BMCReverbUpdateRT60DecayTime(rv);
        BMCReverbUpdateDecayHighShelfFilters(rv);
    }
    
    
    
    
    // this is the decay time for when the hold pedal is down
    void BMCReverbSetSlowRT60DecayTime(struct BMCReverb* rv, float slowRT60){
        assert(slowRT60 >= 0.0);
        rv->slowDecayRT60 = slowRT60;
        BMCReverbUpdateRT60DecayTime(rv);
    }
    
    
    
    void BMCReverbInitDelayOutputSigns(struct BMCReverb* rv){
        // init delay output signs with an equal number of + and - for each channel
        float one = 1.0, negativeOne = -1.0;
        // left
        vDSP_vfill(&one, rv->delayOutputSigns+0*rv->fourthNumDelays, 1, rv->fourthNumDelays);
        vDSP_vfill(&negativeOne, rv->delayOutputSigns+1*rv->fourthNumDelays, 1, rv->fourthNumDelays);
        // right
        vDSP_vfill(&one, rv->delayOutputSigns+2*rv->fourthNumDelays, 1, rv->fourthNumDelays);
        vDSP_vfill(&negativeOne, rv->delayOutputSigns+3*rv->fourthNumDelays, 1, rv->fourthNumDelays);
        
        // randomise the order of the signs for each channel
        unsigned int seed = 1;
        // left
        BMCReverbRandomiseOrder(rv->delayOutputSigns, seed, rv->halfNumDelays);
        //right
        BMCReverbRandomiseOrder(rv->delayOutputSigns+rv->halfNumDelays, seed, rv->halfNumDelays);
    }
    
    
    
    
    void BMCReverbUpdateRT60DecayTime(struct BMCReverb* rv){
        for (size_t i=0; i<rv->numDelays; i++){
            
            // set gain for normal operation
            rv->decayGainAttenuation[i] = BMCReverbDelayGainFromRT60(rv->rt60, rv->delayTimes[i]);
            
            // set gain for when the hold pedal is down
            rv->slowDecayGainAttenuation[i] = BMCReverbDelayGainFromRT60(rv->slowDecayRT60, rv->delayTimes[i]);
        }
    }
    
    
    
    
    
    void BMCReverbSetSampleRate(struct BMCReverb* rv, float sampleRate){
        rv->sampleRate = sampleRate;
        rv->settingsQueuedForUpdate = true;
    }
    
    
    
    
    
    // computes the appropriate feedback gain attenuation
    // to get an exponential decay envelope with the specified RT60 time
    // (in seconds) from a delay line of the specified length.
    //
    // This formula comes from solving EQ 11.33 in DESIGNING AUDIO EFFECT PLUG-INS IN C++ by Will Pirkle
    // which is attributed to Jot, originally.
    double BMCReverbDelayGainFromRT60(double rt60, double delayTime){
        return pow(10.0, (-3.0 * delayTime) / rt60 );
    }
    
    
    
    
    // sets the cutoff frequency of the high shelf filters that increase
    // the decay rate of high frequencies in the reverb.
    void BMCReverbSetHFDecayFC(struct BMCReverb* rv, float fc){
        assert(fc <= 18000.0 && fc > 100.0f);
        rv->highShelfFC = fc;
        BMCReverbUpdateDecayHighShelfFilters(rv);
    }
    
    
    
    
    
    // Multiplies the decay rate for high Frequencies
    //
    // RT60 decay time of frequencies above FC is
    //
    //     (RT60 time below fc)/multiplier
    //
    // So multiplier divides the RT60 time, thereby multiplying the decay rate
    //
    // Input restriction:
    // hfMult >= 1
    //
    // the formulae for calculating the filter coefficients are from Digital Filters for Everyone,
    // second ed., by Rusty Alred.  Section 2.3.10: Shelf Filters
    //
    void BMCReverbSetHFDecayMultiplier(struct BMCReverb* rv, float multiplier){
        assert(multiplier >= 1.0);
        rv->hfDecayMultiplier = multiplier;
        BMCReverbUpdateDecayHighShelfFilters(rv);
    }
    
    
    
    
    // updates the high shelf filters after a change in FC or gain
    void BMCReverbUpdateDecayHighShelfFilters(struct BMCReverb* rv){
        double g,D;
        double gamma;
        
        gamma = tan((M_PI * rv->highShelfFC) / rv->sampleRate);
        
        // bypass the filters if the multiplier is  1
        if (rv->hfDecayMultiplier == 1.0) {
            for (size_t i = 0; i < rv->numDelays; i++){
                
                // set the filter coefficients
                rv->b0[i] = 1.0f;
                rv->b1[i] = 0.0f;
                rv->a1[i] = 0.0f;
                
                // set the slow decay filter coefficients
                rv->b0Slow[i] = 1.0f;
                rv->b1Slow[i] = 0.0f;
                rv->a1Slow[i] = 0.0f;
            }
        } else
        {
            
            for (size_t i = 0; i < rv->numDelays; i++){
                // set the filter gains
                //
                // here we find the gain already applied to each delay line
                double broadbandGain = BMCReverbDelayGainFromRT60(rv->rt60, rv->delayTimes[i]);
                double desiredHFGain = BMCReverbDelayGainFromRT60(rv->rt60 / rv->hfDecayMultiplier, rv->delayTimes[i]);
                double broadbandSlowGain = BMCReverbDelayGainFromRT60(rv->slowDecayRT60, rv->delayTimes[i]);
                double desiredHFSlowGain = BMCReverbDelayGainFromRT60(rv->slowDecayRT60 / rv->hfSlowDecayMultiplier, rv->delayTimes[i]);
                // and now find how much additional filter gain to apply for the given increase in HF decay speed
                g = desiredHFGain / broadbandGain;
                
                // just a temp variable
                D= 1.0 / ((g * gamma) + 1.0);
                
                // set the filter coefficients
                rv->b0[i] = g * (gamma + 1.0) * D;
                rv->b1[i] = g * (gamma - 1.0) * D;
                // Rusty Allred omits the negative sign in the next line.  We use it to avoid a subtraction in the optimized filter code.
                rv->a1[i] = -1.0 * ((g * gamma) - 1.0) * D;
                
                
                // set the slow filter coefficients
                g = desiredHFSlowGain / broadbandSlowGain;
                D= 1.0 / ((g * gamma) + 1.0);
                rv->b0Slow[i] = g * (gamma + 1.0) * D;
                rv->b1Slow[i] = g * (gamma - 1.0) * D;
                rv->a1Slow[i] = -1.0 * ((g * gamma) - 1.0) * D;
            }
        }
    }
    
    
    
    
    
    // sets the length in seconds of the shortest delay in the network
    void BMCReverbSetPreDelay(struct BMCReverb* rv, float preDelay_seconds){
        assert(preDelay_seconds > 0.0 && preDelay_seconds < rv->maxDelay_seconds);
        rv->minDelay_seconds = preDelay_seconds;
        rv->settingsQueuedForUpdate = true;
    }
    
    
    
    
    
    // sets the length in seconds of the longest delay in the network
    void BMCReverbSetRoomSize(struct BMCReverb* rv, float roomSize_seconds){
        assert(roomSize_seconds > rv->minDelay_seconds);
        rv->maxDelay_seconds = roomSize_seconds;
        rv->settingsQueuedForUpdate = true;
    }
    
    
    
    
    // sets the amount of mixing between the two stereo channels
    void BMCReverbSetCrossStereoMix(struct BMCReverb* rv, float crossMix){
        assert(crossMix >= 0 && crossMix <=1);
        
        // maximum mix setting is equal amounts of L and R in both channels
        float maxMix = 1.0/sqrt(2.0);
        
        rv->crossStereoMix = crossMix*maxMix;
        rv->straightStereoMix = sqrtf(1.0f - crossMix*crossMix*maxMix*maxMix);
    }
    
    
    
    // wet and dry gain are balanced so that the total output level remains
    // constant as you as you adjust wet mix
    void BMCReverbSetWetGain(struct BMCReverb* rv, float wetGain){
        assert(wetGain >=0.0 && wetGain <=1.0);
        rv->wetGain = sinf(M_PI_2*wetGain);
        rv->dryGain = cosf(M_PI_2*wetGain);
    }
    
    
    
    __inline void BMCReverbIncrementIndices(struct BMCReverb* rv){
        // add one to each index
        for (size_t i=0; i<rv->numDelays; i++) rv->rwIndices[i]++;
        
        rv->samplesTillNextWrap--;
        
        // if any rwIndices need to wrap back to the beginning, wrap them and calculate the time until the next wrap is needed
        if (rv->samplesTillNextWrap==0) {
            rv->samplesTillNextWrap = SIZE_MAX;
            
            for (size_t i=0; i<rv->numDelays; i++) {
                // find the distance until we reach the end marker
                size_t distToEnd = rv->bufferEndIndices[i] - rv->rwIndices[i];
                
                // wrap if we reached the marker already
                if (distToEnd==0){
                    rv->rwIndices[i] = rv->bufferStartIndices[i];
                    distToEnd = rv->bufferLengths[i];
                }
                
                // find the shortest distance to the next wrap
                if (rv->samplesTillNextWrap > distToEnd) rv->samplesTillNextWrap=distToEnd;
            }
        }
    }
    
    
    
    
    
    // generate an evenly spaced but randomly jittered list of times between min and max
    void BMCReverbUpdateDelayTimes(struct BMCReverb* rv){
        
        float spacing = (rv->maxDelay_seconds - rv->minDelay_seconds) / (float)rv->halfNumDelays;
        
        // generate an evenly spaced list of times between min and max
        // left channel
        vDSP_vramp(&rv->minDelay_seconds, &spacing, rv->delayTimes, 1, rv->halfNumDelays);
        // right channel
        vDSP_vramp(&rv->minDelay_seconds, &spacing, rv->delayTimes+rv->halfNumDelays, 1, rv->halfNumDelays);
        
        // Seed the random number generator for consistency. Doing this ensures
        // that we get the same delay times every time we run the reverb.
        srand(111);
        
        // jitter the times so that the spacing is not perfectly even
        for (size_t i = 0; i < rv->numDelays; i++)
            rv->delayTimes[i] += spacing * ((float)rand() / (float) RAND_MAX);
        
        
        // randomise the order of the list of delay times
        // left channel
        BMCReverbRandomiseOrder(rv->delayTimes, 17, rv->halfNumDelays);
        // right channel
        BMCReverbRandomiseOrder(rv->delayTimes+rv->halfNumDelays, 4, rv->halfNumDelays);
        
        
        // convert times from milliseconds to samples and count the total
        rv->totalSamples = 0;
        for (size_t i = 0; i < rv->numDelays; i++) {
            rv->bufferLengths[i] = (size_t)round(rv->sampleRate*rv->delayTimes[i]);
            rv->totalSamples += rv->bufferLengths[i];
        }
        
        
        // allocate memory for the main delays in the network
        if (rv->delayLines) free(rv->delayLines);
        rv->delayLines = malloc(sizeof(float)*rv->totalSamples);
        vDSP_vclr(rv->delayLines,1, rv->totalSamples);
        
        
        
        // The following depend on delay time and have to be updated
        // whenever there is a change
        BMCReverbUpdateRT60DecayTime(rv);
        BMCReverbUpdateDecayHighShelfFilters(rv);
        BMCReverbInitIndices(rv);
    }
    
    
    
    
    
    void BMCReverbSetNumDelayUnits(struct BMCReverb* rv, size_t delayUnits){
        rv->newNumDelayUnits = delayUnits;
        rv->settingsQueuedForUpdate = true;
    }
    
    
    
    
    
    void BMCReverbUpdateNumDelayUnits(struct BMCReverb* rv){
        /*
         * before beginning, calculate some frequently reused values
         */
        size_t delayUnits = rv->newNumDelayUnits;
        rv->numDelays = delayUnits*4;
        rv->delayUnits = delayUnits;
        rv->halfNumDelays = delayUnits*2;
        rv->fourthNumDelays = delayUnits;
        rv->threeFourthsNumDelays = delayUnits*3;
        // we compute attenuation on half delays because the reverb is stereo
        rv->inputAttenuation = 1.0f/sqrt((float)rv->halfNumDelays);
        
        
        
        // free old memory if necessary
        if (rv->bufferLengths) BMCReverbFree(rv);
        
        
        /*
         * allocate memory for smaller buffers
         */
        rv->bufferLengths = malloc(sizeof(size_t)*rv->numDelays);
        rv->delayTimes = malloc(rv->numDelays*sizeof(float));
        rv->feedbackBuffers = malloc(rv->numDelays*sizeof(float));
        vDSP_vclr(rv->feedbackBuffers, 1, rv->numDelays);
        rv->rwIndices = malloc(rv->numDelays*sizeof(size_t));
        rv->bufferStartIndices = malloc(rv->numDelays*sizeof(size_t));
        rv->bufferEndIndices = malloc(rv->numDelays*sizeof(size_t));
        rv->mixingBuffers = malloc(rv->numDelays*sizeof(float));
        rv->z1 = malloc(rv->numDelays*sizeof(float));
        vDSP_vclr(rv->z1, 1, rv->numDelays);
        rv->a1 = malloc(rv->numDelays*sizeof(float));
        rv->b0 = malloc(rv->numDelays*sizeof(float));
        rv->b1 = malloc(rv->numDelays*sizeof(float));
        rv->a1Slow = malloc(rv->numDelays*sizeof(float));
        rv->b0Slow = malloc(rv->numDelays*sizeof(float));
        rv->b1Slow = malloc(rv->numDelays*sizeof(float));
        rv->decayGainAttenuation = malloc(rv->numDelays*sizeof(float));
        rv->slowDecayGainAttenuation = malloc(rv->numDelays*sizeof(float));
        rv->leftOutputTemp = malloc(BMCREVERB_TEMPBUFFERLENGTH*sizeof(float));
        rv->dryL = malloc(BMCREVERB_TEMPBUFFERLENGTH*sizeof(float));
        rv->dryR = malloc(BMCREVERB_TEMPBUFFERLENGTH*sizeof(float));
        rv->delayOutputSigns = malloc(rv->numDelays*sizeof(float));
        
        
        /*
         * set randomised signs for the output taps
         */
        BMCReverbInitDelayOutputSigns(rv);
        
        
        /*
         * create pointers to facilitate mixing
         */
        rv->fb0 = rv->feedbackBuffers+(0*rv->fourthNumDelays);
        rv->fb1 = rv->feedbackBuffers+(1*rv->fourthNumDelays);
        rv->fb2 = rv->feedbackBuffers+(2*rv->fourthNumDelays);
        rv->fb3 = rv->feedbackBuffers+(3*rv->fourthNumDelays);
        rv->mb0 = rv->mixingBuffers+(0*rv->fourthNumDelays);
        rv->mb1 = rv->mixingBuffers+(1*rv->fourthNumDelays);
        rv->mb2 = rv->mixingBuffers+(2*rv->fourthNumDelays);
        rv->mb3 = rv->mixingBuffers+(3*rv->fourthNumDelays);
        
        
        
        /*
         * Allocate memory for the network delay buffers and update all the
         * delay-time-dependent parameters
         */
        BMCReverbUpdateDelayTimes(rv);
        
        
        rv->settingsQueuedForUpdate = false;
    }
    
    
    
    // randomise the order of a list of floats
    void BMCReverbRandomiseOrder(float* list, size_t seed, size_t length){
        // seed the random number generator so we get the same result every time
        srand((int)seed);
        
        for (int i = 0; i<length; i++) {
            int j = rand() % (int)length;
            
            // swap i with j
            float temp = list[i];
            list[i] = list[j];
            list[j] = temp;
        }
    }
    
    
    
    void BMCReverbInitIndices(struct BMCReverb* rv){
        size_t idx = 0;
        rv->samplesTillNextWrap = SIZE_MAX;
        for (size_t i = 0; i<rv->numDelays; i++) {
            // set the initial location of the rw pointer
            rv->rwIndices[i] = idx;
            
            // set a mark at the start location
            rv->bufferStartIndices[i] = idx;
            
            // set a mark at the end of the buffer (after the end)
            idx += rv->bufferLengths[i];
            rv->bufferEndIndices[i] = idx;
            
            // find the shortest distance until the next index wrap-around
            if (rv->bufferLengths[i] < rv->samplesTillNextWrap)
                rv->samplesTillNextWrap = rv->bufferLengths[i];
        }
    }
    
    
    
    
    void BMCReverbPointersToNull(struct BMCReverb* rv){
        rv->delayLines = NULL;
        rv->bufferLengths = NULL;
        rv->feedbackBuffers = NULL;
        rv->bufferStartIndices = NULL;
        rv->bufferEndIndices = NULL;
        rv->rwIndices = NULL;
        rv->mixingBuffers = NULL;
        rv->z1 = NULL;
        rv->a1 = NULL;
        rv->b0 = NULL;
        rv->b1 = NULL;
        rv->a1Slow = NULL;
        rv->b0Slow = NULL;
        rv->b1Slow = NULL;
        rv->delayTimes = NULL;
        rv->decayGainAttenuation = NULL;
        rv->slowDecayGainAttenuation = NULL;
        rv->leftOutputTemp = NULL;
        rv->delayOutputSigns = NULL;
        rv->dryL = NULL;
        rv->dryR = NULL;
        rv->mainFilterSetup = NULL;
    }
    
    
    
    
    void BMCReverbFree(struct BMCReverb* rv){
        free(rv->delayLines);
        free(rv->bufferLengths);
        free(rv->feedbackBuffers);
        free(rv->bufferStartIndices);
        free(rv->bufferEndIndices);
        free(rv->rwIndices);
        free(rv->mixingBuffers);
        free(rv->z1);
        free(rv->a1);
        free(rv->b0);
        free(rv->b1);
        free(rv->a1Slow);
        free(rv->b0Slow);
        free(rv->b1Slow);
        free(rv->delayTimes);
        free(rv->decayGainAttenuation);
        free(rv->slowDecayGainAttenuation);
        free(rv->leftOutputTemp);
        free(rv->delayOutputSigns);
        free(rv->dryL);
        free(rv->dryR);
        vDSP_biquadm_DestroySetup(rv->mainFilterSetup);
        
        BMCReverbPointersToNull(rv);
    }
    
    
    
    
    
    // process a single sample of input from right and left channels
    // the output is 100% wet
    __inline void BMCReverbProcessWetSample(struct BMCReverb* rv, float inputL, float inputR, float* outputL, float* outputR){
        
        /*
         * mix feedback from previous sample with the fresh inputs
         */
        // attenuate the input to preserve the volume before splitting the signal
        float attenuatedInputL = inputL * rv->inputAttenuation;
        float attenuatedInputR = inputR * rv->inputAttenuation;
        // left channel mixes into the first n/2 delays
        vDSP_vsadd(rv->feedbackBuffers, 1, &attenuatedInputL, rv->feedbackBuffers, 1, rv->halfNumDelays);
        // right channel mixes into the second n/2 delays
        vDSP_vsadd(rv->feedbackBuffers+rv->halfNumDelays, 1, &attenuatedInputR, rv->feedbackBuffers+rv->halfNumDelays, 1, rv->halfNumDelays);
        
        
        
        
        /*
         * slowDecay allows implementation of a sustain-pedal effect that
         * switches over to a really long decay time when slowDecay is on
         */
        if (!rv->slowDecay){
            
            /*
             * All Frequencies Decay with slow and standard decay rates
             * slow rate is used for a hold / sustain pedal control
             */
            vDSP_vmul(rv->feedbackBuffers, 1, rv->decayGainAttenuation, 1, rv->feedbackBuffers, 1, rv->numDelays);
            
            /*
             * High Frequency Decay
             */
            // apply a first order high-shelf filter to the feedback path
            //
            // This filter structure is direct form 2 from figure 14 in section 1.1.6
            // of Digital Filters for Everyone by Rusty Alred, second ed.
            //
            // feedbackBuffers = feedbackBuffers + (a1 * z1);
            vDSP_vma(rv->a1, 1, rv->z1, 1, rv->feedbackBuffers, 1, rv->feedbackBuffers, 1, rv->numDelays);
            // feedbackBuffers = b0*feedbackBuffers + b1*z1;
            vDSP_vmma(rv->b0, 1, rv->feedbackBuffers, 1, rv->b1, 1, rv->z1, 1, rv->feedbackBuffers, 1, rv->numDelays);
            // z1 = feedbackBuffers;
            memcpy(rv->z1, rv->feedbackBuffers, rv->numDelays * sizeof(float));
            
            
            /*
             * the else case uses different paramaters to the the decay, resulting
             * in longer sustain
             */
        } else {
            // broadband decay
            vDSP_vmul(rv->feedbackBuffers, 1, rv->slowDecayGainAttenuation, 1, rv->feedbackBuffers, 1, rv->numDelays);
            
            // high-frequency filtering
            vDSP_vma(rv->a1Slow, 1, rv->z1, 1, rv->feedbackBuffers, 1, rv->feedbackBuffers, 1, rv->numDelays);
            vDSP_vmma(rv->b0Slow, 1, rv->feedbackBuffers, 1, rv->b1Slow, 1, rv->z1, 1, rv->feedbackBuffers, 1, rv->numDelays);
            memcpy(rv->z1, rv->feedbackBuffers, rv->numDelays * sizeof(float));
        }
        
        
        
        
        /*
         * write the mixture of input and feedback back into the delays
         */
        for (size_t i=0; i < rv->numDelays; i++)
            rv->delayLines[rv->rwIndices[i]] = rv->feedbackBuffers[i];
        
        
        
        /*
         * increment delay indices
         */
        BMCReverbIncrementIndices(rv);
        
        
        
        /*
         * read output from delays into the feedback buffers
         */
        // vDSP_vgathr indexes 1 as the first element of the array so we have to
        // add +1 to the reference to delayLines to compensate
        vDSP_vgathr(rv->delayLines+1, rv->rwIndices, 1, rv->feedbackBuffers, 1, rv->numDelays);
        //for (size_t i=0; i < rv->numDelays; i++)
        //    rv->feedbackBuffers[i] = rv->delayLines[rv->rwIndices[i]];
        
        
        
        
        
        /*
         * sum the delay line outputs to right and left channel outputs
         */
        // randomise the signs of the output from each delay, caching in mb0
        vDSP_vmul(rv->feedbackBuffers, 1, rv->delayOutputSigns, 1, rv->mb0, 1, rv->numDelays);
        // first half of delays sum to left out
        vDSP_sve(rv->mb0, 1, outputL, rv->halfNumDelays);
        // second half of delays sum to right out
        vDSP_sve(rv->mb2, 1, outputR, rv->halfNumDelays);
        
        
        
        
        /*
         * Mix the feedback signal
         *
         * The code below does the first two stages of a fast hadamard transform,
         * followed by some swapping.
         * Leaving the transform incomplete is equivalent to using a
         * block-circulant mixing matrix. Typically, block circulant mixing is
         * done using the last two stages of the fast hadamard transform. Here
         * we use the first two stages instead because it permits us to do
         * vectorised additions and subtractions with a stride of 1.
         *
         * Regarding block-circulant mixing, see: https://www.researchgate.net/publication/282252790_Flatter_Frequency_Response_from_Feedback_Delay_Network_Reverbs
         */
        //
        // Stage 1 of Fast Hadamard Transform
        //
        // 1st half + 2nd half => 1st half
        vDSP_vadd(rv->fb0, 1, rv->fb2, 1, rv->mb0, 1, rv->halfNumDelays);
        // 1st half - 2nd half => 2nd half
        vDSP_vsub(rv->fb0, 1, rv->fb2, 1, rv->mb2, 1, rv->halfNumDelays);
        //
        //
        // Stage 2 of Fast Hadamard Transform
        //
        // 1st fourth + 2nd fourth => 1st fourth
        vDSP_vadd(rv->mb0, 1, rv->mb1, 1, rv->fb0, 1, rv->fourthNumDelays);
        // 1st fourth - 2nd fourth => 2nd fourth
        vDSP_vsub(rv->mb0, 1, rv->mb1, 1, rv->fb1, 1, rv->fourthNumDelays);
        // 3rd fourth + 4th fourth => 3rd fourth
        vDSP_vadd(rv->mb2, 1, rv->mb3, 1, rv->fb2, 1, rv->fourthNumDelays);
        // 3rd fourth - 4th fourth => 4th fourth
        vDSP_vsub(rv->mb2, 1, rv->mb3, 1, rv->fb3, 1, rv->fourthNumDelays);
        //
        // attenuate to keep the mixing transformation unitary
        //float endElement = rv->feedbackBuffers[(rv->numDelays)-1];
        vDSP_vsmul(rv->feedbackBuffers, 1, &rv->matrixAttenuation, rv->feedbackBuffers, 1, rv->numDelays);
        //
        // rotate the values in the feedback buffer by one position
        //
        // the rotation ensures that a signal entering delay n does not
        // return back to the nth delay until after it has passed through
        // numDelays/4 other delays.
        float endElement = rv->feedbackBuffers[(rv->numDelays)-1];
        memmove(rv->feedbackBuffers+1, rv->feedbackBuffers, sizeof(float)*(rv->numDelays-1));
        rv->feedbackBuffers[0] = endElement;
    }
    
    
#ifdef __cplusplus
}
#endif