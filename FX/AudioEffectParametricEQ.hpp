#pragma once

namespace FX
{
    inline SoundAlchemy::Filters::FilterCoefficients makeParametricEQ( DspFloatType discreteFrequency,
                                                                DspFloatType Q,
                                                                DspFloatType gainFactor) 
    {
        assert (discreteFrequency > 0);
        assert (Q > 0);
        
        /* Limit the bandwidth so we don't get a nonsense result from tan(B/2) */
        const DspFloatType bandwidth = std::min(discreteFrequency / Q, M_PI * 0.99);
        const DspFloatType two_cos_wc = -2.0*std::cos(discreteFrequency);
        const DspFloatType tan_half_bw = std::tan(bandwidth / 2.0);
        const DspFloatType g_tan_half_bw = gainFactor * tan_half_bw;
        const DspFloatType sqrt_g = std::sqrt(gainFactor);
        
        
        SoundAlchemy::Filters::FilterCoefficients c;

        DspFloatType a0 = sqrt_g + tan_half_bw;    
        c.b[0] = (sqrt_g + g_tan_half_bw) / a0; /* b0 */
        c.b[1] = (sqrt_g * two_cos_wc) / a0;  /* b1 */
        c.b[2] = (sqrt_g - g_tan_half_bw) / a0; /* b2 */
        c.a[0] = (sqrt_g * two_cos_wc) / a0;  /* a1 */
        c.a[1] = (sqrt_g - tan_half_bw) / a0;  /* a2 */
        
        return c;
    }

    struct ParametricEQParameters
    {
        DspFloatType centreFrequency_, q_, gainDecibels_;    
        size_t numChannels;
    };

    struct ParametricEQ : public StereoFXProcessor
    {
        DspFloatType centreFrequency_, q_, gainDecibels_;    
        SoundAlchemy::Filters::BiquadFilter **eqFilters_;
        int numEqFilters_;
        size_t numChannels = 2;
        size_t numInputChannels=2;
        size_t numOutputChannels=2;

        enum {
            PORT_FREQ,
            PORT_Q,
            PORT_GAIN
        };
        int setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_FREQ: centreFrequency_ = v; break;
                case PORT_Q: q_ = v; break;
                case PORT_GAIN: gainDecibels_ = v; break;
                default: printf("No port %d\n",port);
            }
        }
        ParametricEQ() : StereoFXProcessor()
        {
        // Set default values:
            centreFrequency_ = 1000.0;
            q_ = 2.0;
            gainDecibels_ = 0.0;
            
            // Initialise the filters later when we know how many channels
            eqFilters_ = 0;
            numEqFilters_ = 0;

            // Use this method as the place to do any pre-playback
            // initialisation that you need..

            // Create as many filters as we have input channels
            numEqFilters_ = numChannels;
            eqFilters_ = (SoundAlchemy::Filters::BiquadFilter**)malloc(numEqFilters_ * sizeof(SoundAlchemy::Filters::BiquadFilter*));
            if(eqFilters_ == 0)
            numEqFilters_ = 0;
            else {
            for(int i = 0; i < numEqFilters_; i++)
                eqFilters_[i] = new SoundAlchemy::Filters::BiquadFilter(sampleRate);
            }

            // Update the filter settings to work with the current parameters and sample rate
            updateEQFilter(sampleRate);   
        }
        ~ParametricEQ()
        {
            // When playback stops, you can use this as an opportunity to free up any
            // spare memory, etc.
            for(int i = 0; i < numEqFilters_; i++)
                delete eqFilters_[i];
            if(numEqFilters_ != 0)
                free(eqFilters_);
            numEqFilters_ = 0;
            eqFilters_ = 0;
        }

        void updateEQFilter(DspFloatType sampleRate)
        {
            for(int i = 0; i < numEqFilters_; i++)
                eqFilters_[i]->setCoefficients(makeParametricEQ(2.0 * M_PI * centreFrequency_ / sampleRate, q_, powf(10.0f, gainDecibels_ / 20.0f)));
        }

        void ProcessBlock (size_t numSamples, DspFloatType ** inputs, DspFloatType **outputs)
        {
            // Helpful information about this block of samples:
            const int numInputChannels  = numChannels;
            const int numOutputChannels = numChannels;
            
            int channel;
            
            // Go through each channel of audio that's passed in
            
            for (channel = 0; channel < std::min((int32_t)numInputChannels, numEqFilters_); ++channel)
            {
                // channelData is an array of length numSamples which contains the audio for one channel
                DspFloatType* channelData = inputs[channel];
                DspFloatType * channelOut = outputs[channel];
            
                // Run the samples through the IIR filter whose coefficients define the parametric
                // equaliser. See juce_IIRFilter.cpp for the implementation.
                //eqFilters_[channel]->processSamples(channelData, numSamples);
                eqFilters_[channel]->Process(numSamples,channelData,channelOut);
            }                
        }
    };
}