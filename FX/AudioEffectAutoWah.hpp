#pragma once

namespace FX
{
    ///////////////////////////////////////////////////////////////////////////////
    // AutoWah
    ///////////////////////////////////////////////////////////////////////////////
    inline Filters::FilterCoefficients makeWahWahFilter(DspFloatType centreFrequency, DspFloatType Q, DspFloatType gain)
    {
        if( Q <= 0) Q = 0.001;
        
        // Use inverse sample rate since multiplication is more efficient than
        // division, and the sample rate is unlikely to change each call.
        const DspFloatType discreteFrequency = 2.0 * M_PI * centreFrequency * inverseSampleRate;
        const DspFloatType alpha = sin(discreteFrequency) / (2.0 * Q);
        const DspFloatType cos_w0 = cos(discreteFrequency);

        /* setCoefficients() takes arguments: b0, b1, b2, a0, a1, a2
        * It will normalise the filter according to the value of a0
        * to allow standard time-domain implementations
        */
        Filters::FilterCoefficients c;
        c.b[0]  = 0.5*(1.0 - cos_w0)*gain;
        c.b[1]  = (1.0 - cos_w0)*gain;
        c.b[2]  = 0.5*(1.0 - cos_w0)*gain;
        //c.a[0]  = 1.0 + alpha;
        c.a[0]  = -2.0 * cos_w0;
        c.a[1]  = 1.0 - alpha;

        DspFloatType a0 =  1.0 + alpha;
        c.b[0] /= a0;
        c.b[1] /= a0;
        c.b[2] /= a0;
        c.a[0] /= a0;
        c.a[1] /= a0;
        
        return c;
    }

    struct AutoWah : public StereoFXProcessor
    {
        DspFloatType baseFrequency_, q_;
        DspFloatType lfoFrequency_, lfoWidth_;
        DspFloatType envelopeWidth_, envelopeAttack_, envelopeDecay_;
        DspFloatType lfoPhase_;
        DspFloatType * envelopes_;
        int numEnvelopes_;
        
        // Convert the attack and decay time constants to a multiplier for
        // a first-order lowpass filter
        DspFloatType attackMultiplier_, decayMultiplier_;
        
        Filters::BiquadFilter *wahFilters_[2];
        int numWahFilters_;
        size_t numChannels = 2;

        enum {
            PORT_BASE_FREQ,
            PORT_Q,
            PORT_LFO_FREQ,
            PORT_LFO_WIDTH,
            PORT_LFO_PHASE,
            PORT_ENV_WIDTH,
            PORT_ENV_ATK,
            PORT_ENV_DCY,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_BASE_FREQ: baseFrequency_ = v; break;
                case PORT_Q: q_ = v; break;
                case PORT_LFO_FREQ: lfoFrequency_ = v; break;
                case PORT_LFO_WIDTH: lfoWidth_ = v; break;
                case PORT_LFO_PHASE: lfoPhase_ = v; break;
                case PORT_ENV_WIDTH: envelopeWidth_ = v; break;
                case PORT_ENV_ATK: envelopeAttack_ = v; break;
                case PORT_ENV_DCY: envelopeDecay_ = v; break;
                default: printf("No port %d\n",port);
            }
        }
        void randomize()
        {
            baseFrequency_ = 10.0*noise.rand();
            q_ = 10.0*noise.rand() + 0.5;
            lfoFrequency_   = 10.0*noise.rand();
            lfoWidth_       = 2000.0*noise.rand();   
            envelopeWidth_  = noise.rand();
            envelopeAttack_ = noise.rand();
            envelopeDecay_  = noise.rand();
            attackMultiplier_ = pow(1.0 / M_E, inverseSampleRate / envelopeAttack_);
            decayMultiplier_  = pow(1.0 / M_E, inverseSampleRate / envelopeDecay_);
        }
        AutoWah() : StereoFXProcessor()
        {
            // Set default values:
            baseFrequency_  = 350.0;
            q_              = 5.0;
            lfoFrequency_   = 2.0;
            lfoWidth_       = 1000.0;
            envelopeWidth_  = 0.2;
            envelopeAttack_ = 0.005;
            envelopeDecay_  = 0.1;
                    
            numWahFilters_ = 2;
            wahFilters_[0] = new Filters::BiquadFilter(sampleRate);
            wahFilters_[1] = new Filters::BiquadFilter(sampleRate);    

            numEnvelopes_ = 2;
            envelopes_ = (DspFloatType*)calloc(numEnvelopes_,sizeof(DspFloatType));        

            attackMultiplier_ = pow(1.0 / M_E, inverseSampleRate / envelopeAttack_);
            decayMultiplier_ = pow(1.0 / M_E, inverseSampleRate / envelopeDecay_);
        }

        void ProcessBlock (size_t numSamples, DspFloatType ** inputs, DspFloatType ** outputs)
        {
            // Helpful information about this block of samples:
            const int numInputChannels  = numChannels;
            const int numOutputChannels = numChannels;        
            int channel;
            DspFloatType ph;
            
            // Go through each channel and put it through the resonant lowpass filter, updating
            // the coefficients as we go along. Each channel is processed identically in this effect.
            
            for(channel = 0; channel < std::min(numInputChannels, numWahFilters_); ++channel)
            {
                // channelData is an array of length numSamples which contains the audio for one channel
                DspFloatType* channelData = inputs[channel];
                DspFloatType* output = outputs[channel];
                ph = lfoPhase_;
                
                for (int sample = 0; sample < numSamples; ++sample)
                {
                    const DspFloatType in = channelData[sample];
                    DspFloatType centreFrequency = baseFrequency_;
                    
                    // Calculate the envelope of the signal. Do this even if we're not currently
                    // changing the frequeny based on it, since it involves maintaining a history
                    // of the signal's behaviour.
                    
                    if(channel < numEnvelopes_) {   // Safety check
                        if(fabs(in) > envelopes_[channel]) {
                            envelopes_[channel] += (1.0 - attackMultiplier_) * (fabs(in) - (DspFloatType)envelopes_[channel]);
                        }
                        else
                            envelopes_[channel] *= decayMultiplier_;
                    }
                    
                    // Calculate the centre frequency of the filter based on the LFO and the
                    // signal envelope
                    if(lfoWidth_ > 0.0) {
                        centreFrequency += lfoWidth_ * (0.5f + 0.5f*sinf(2.0 * M_PI * ph));
                    }
                    if(envelopeWidth_ > 0.0 && channel < numEnvelopes_) {
                        centreFrequency += envelopeWidth_ * envelopes_[channel];
                    }
                    
                    // Update filter coefficients (see ResonantLowpassFilter.cpp for calculation)
                    Filters::FilterCoefficients c = makeWahWahFilter(centreFrequency,
                                                            q_,
                                                            0.5);
                    
                    wahFilters_[channel]->setCoefficients(c);
                    // Process one sample and store it back in place. See juce_IIRFilter.cpp for the
                    // application of the IIR filter.
                    output[sample] = wahFilters_[channel]->Tick(in);
                    
                    // Update the LFO phase, keeping it in the range 0-1
                    ph += lfoFrequency_*inverseSampleRate;
                    if(ph >= 1.0)
                        ph -= 1.0;
                }
            }
            
            lfoPhase_ = ph;                
        }
        void InplaceProcess(size_t n, DspFloatType ** buffer)
        {
            ProcessBlock(n,buffer,buffer);
        }
    };
}