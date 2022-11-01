#pragma once


namespace FX
{

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Phaser
    /////////////////////////////////////////////////////////////////////////////////////////////

    #include "OnePoleAllPassFilter.hpp"

    inline Filters::FilterCoefficients makeTwoPoleAllpass (DspFloatType centreFrequency,
                                                                DspFloatType Q) 
    {
        if(Q <= 0) Q = 0.001;
        const DspFloatType discreteFrequency = 2.0 * M_PI * centreFrequency * inverseSampleRate;
        const DspFloatType alpha = sin(discreteFrequency) / (2.0 * Q);
        const DspFloatType cos_w0 = -2.0 * cos(discreteFrequency);    
        Filters::FilterCoefficients c;
        DspFloatType a0 = 1.0 + alpha;
        c.b[0] = (1.0 - alpha)/a0;    
        c.b[1] = cos_w0/a0;    
        c.b[2] = (1.0 + alpha)/a0;
        c.b[2] /= a0;
        c.a[0] =  cos_w0 / a0;
        c.a[1] = (1.0 - alpha)/a0;    
        return c;
    }

    struct Phaser : public StereoFXProcessor
    {
        // Adjustable parameters:
        DspFloatType baseFrequency_; // Lowest frequency of allpass filters
        DspFloatType sweepWidth_;    // Amount of change from min to max delay
        DspFloatType depth_;         // Mix level for phase-shifted signal (0-1)
        DspFloatType feedback_;      // Feedback level for feedback phaser (0-<1)
        DspFloatType lfoFrequency_;  // LFO frequency (Hz)
        int   filtersPerChannel_; // How many allpass filters to use
        Waveform   waveform_;      // What shape should be used for the LFO
        int   stereo_;        // Whether to use stereo phasing        
        DspFloatType lfoPhase_;   // Phase of the low-frequency oscillator    
        unsigned int sampleCount_; // Counter to keep track of samples elapsed, to
                                // keep track of filter updates
        unsigned int filterUpdateInterval_; // How often to update filter coefficients
        
        // Bank of allpass filters that do the phasing; N filters x M channels
        OnePoleAllpassFilter **allpassFilters_;
        

        // Storage of the last output sample from each bank of filters, for use in
        // feedback loop
        DspFloatType *lastFilterOutputs_;
        int numLastFilterOutputs_;

        int totalNumFilters_;
        size_t numChannels=2;
        enum {
            PORT_BASEFREQ,
            PORT_SWEEPWIDTH,
            PORT_DEPTH,
            PORT_FEEDBACK,
            PORT_LFOFREQ,
            PORT_WAVEFORM,
            PORT_LFOPHASE,        
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_BASEFREQ: baseFrequency_ = v; break;
                case PORT_SWEEPWIDTH: sweepWidth_ = v; break;
                case PORT_DEPTH: depth_ = v; break;
                case PORT_FEEDBACK: feedback_ = v; break;
                case PORT_LFOFREQ: lfoFrequency_ = v; break;
                case PORT_WAVEFORM: waveform_ = (Waveform)v; break;
                case PORT_LFOPHASE: lfoPhase_ = v; break;
                default: printf("No port %d\n",port);
            }
        }
        Phaser() : StereoFXProcessor() {
            // Set default values:
            baseFrequency_ = 200.0;
            sweepWidth_ = 2000.0;
            depth_ = 1.0;
            feedback_ = 0.0;
            lfoFrequency_ = 0.5;
            waveform_ = kWaveformSine;
            stereo_ = 0;
            
            // Start with no filters (at least until we have some channels)
            allpassFilters_ = 0;
            filtersPerChannel_ = 4;
            totalNumFilters_ = 0;
            lastFilterOutputs_ = 0;
            numLastFilterOutputs_ = 0;
            
            lfoPhase_ = 0.0;        
            sampleCount_ = 0;
            filterUpdateInterval_ = 8;

            totalNumFilters_ = numChannels * filtersPerChannel_;
            if(totalNumFilters_ > 0) {
                allpassFilters_ = (OnePoleAllpassFilter**)malloc(totalNumFilters_ * sizeof(OnePoleAllpassFilter*));
                if(allpassFilters_ == 0)
                    totalNumFilters_ = 0;
                else {
                    for(int i = 0; i < totalNumFilters_; i++)
                        allpassFilters_[i] = new OnePoleAllpassFilter;
                }
            }
            
            numLastFilterOutputs_ = numChannels;
            lastFilterOutputs_ = (DspFloatType *)malloc(numLastFilterOutputs_ * sizeof(DspFloatType));
            for(int i = 0; i < numLastFilterOutputs_; i++)
                lastFilterOutputs_[i] = 0.0f;
        }
        ~Phaser()
        {
            for(int i = 0; i < totalNumFilters_; i++)
                delete allpassFilters_[i];
            if(totalNumFilters_ != 0)
                free(allpassFilters_);
            totalNumFilters_ = 0;
            allpassFilters_ = 0;
            
            if(numLastFilterOutputs_ != 0)
                free(lastFilterOutputs_);
            numLastFilterOutputs_ = 0;
            lastFilterOutputs_ = 0;
        }
        void ProcessBlock (size_t numSamples, DspFloatType ** inputs, DspFloatType ** outputs)
        {
            //const ScopedLock sl (lock_);
            

            // Helpful information about this block of samples:
            const int numInputChannels  = numChannels;     // How many input channels for our effect?
            const int numOutputChannels = numChannels;   // How many output channels for our effect?        
            DspFloatType ph, channel0EndPhase  = lfoPhase_;
            unsigned int sc;
            
            // Go through each channel of audio that's passed in, applying one or more allpass filters
            // to each. Each channel will be treated identically in a (non-stereo) phaser, but we have
            // to have separate filter objects for each channel since the filters store the last few samples
            // passed through them.
            
            // Filters are stored with all channel 0 filters first, then all channel 1 filters, etc.
            
            for(int channel = 0; channel < numInputChannels; ++channel)
            {
                // channelData is an array of length numSamples which contains the audio for one channel
                DspFloatType* channelData = inputs[channel];
                DspFloatType* channelOut  = outputs[channel];

                ph = lfoPhase_;
                sc = sampleCount_;
                
                // For stereo phasing, keep the channels 90 degrees out of phase with each other
                if(stereo_ != 0 && channel != 0)
                    ph = fmodf(ph + 0.25, 1.0);
                
                for (int sample = 0; sample < numSamples; ++sample)
                {
                    DspFloatType out = channelData[sample];
                    
                    // If feedback is enabled, include the feedback from the last sample in the
                    // input of the allpass filter chain. This is actually not accurate to how
                    // analog phasers work because there is a sample of delay between output and
                    // input, which adds a further phase shift of up to 180 degrees at half the
                    // sampling frequency. To truly model an analog phaser with feedback involves
                    // modelling a delay-free loop, which is beyond the scope of this example.
                    
                    if(feedback_ != 0.0 && channel < numLastFilterOutputs_)
                        out += feedback_ * lastFilterOutputs_[channel];
                    
                    // See OnePoleAllpassFilter.cpp for calculation of coefficients and application
                    // of filter to audio data. The filter processes the audio buffer in place,
                    // putting the output sample in place of the input.
                    
                    for(int j = 0; j < filtersPerChannel_; ++j)
                    {
                        // Safety check
                        if(channel * filtersPerChannel_ + j >= totalNumFilters_)
                            continue;
                        
                        // First, update the current allpass filter coefficients depending on the parameter
                        // settings and the LFO phase
                        
                        // Recalculating the filter coefficients is much more expensive than calculating
                        // a sample. Only update the coefficients at a fraction of the sample rate; since
                        // the LFO moves slowly, the difference won't generally be audible.
                        if(sc % filterUpdateInterval_ == 0)
                        {
                            allpassFilters_[channel * filtersPerChannel_ + j]->makeAllpass(inverseSampleRate,
                                    baseFrequency_ + sweepWidth_* LFO_GetSample(ph, waveform_));
                        }
                        out = allpassFilters_[channel * filtersPerChannel_ + j]->processSingleSampleRaw(out);
                    }
                    
                    if(channel < numLastFilterOutputs_)
                        lastFilterOutputs_[channel] = out;
                    
                    // Add the allpass signal to the output, though maintaining constant level
                    // depth = 0 --> input only ; depth = 1 --> evenly balanced input and output
                    channelOut[sample] = (1.0f-0.5f*depth_)*channelData[sample] + 0.5f*depth_*out;
                
                    // Update the LFO phase, keeping it in the range 0-1
                    ph += lfoFrequency_*inverseSampleRate;
                    if(ph >= 1.0) ph -= 1.0;
                    sc++;
                }

                // Use channel 0 only to keep the phase in sync between calls to processBlock()
                // Otherwise quadrature phase on multiple channels will create problems.
                if(channel == 0)
                    channel0EndPhase = ph;
            }        
            lfoPhase_ = channel0EndPhase;
            sampleCount_ = sc;                
        }
        void InplaceProcess(size_t n, DspFloatType ** buffer)
        {
            ProcessBlock(n,buffer,buffer);
        }
    };
}