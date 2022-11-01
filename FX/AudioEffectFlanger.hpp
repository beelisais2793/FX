#pragma once

namespace FX
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Flanger
    ////////////////////////////////////////////////////////////////////////////////////////////////
    struct Flanger : public StereoFXProcessor
    {
        // Adjustable parameters:
        DspFloatType delay_;      // Minimum length of delay line in seconds
        DspFloatType sweepWidth_; // Amount of change from min to max delay
        DspFloatType depth_;      // Mix level of delayed signal (0-1)
        DspFloatType feedback_;   // Feedback level for feedback flanger (0-<1)
        DspFloatType frequency_;  // LFO frequency (Hz)
        Waveform   waveform_;   // What shape should be used for the LFO
        Interpolation   interpolation_; // What type of interpolation to use
        bool  stereo_;     // Whether to use stereo (quadrature-phase) flanging
        
        size_t numChannels=2;
        
        enum {
            PORT_SWEEPWIDTH,
            PORT_DEPTH,
            PORT_FEEDBACK,
            PORT_FREQUENCY,
            PORT_WAVEFORM,
            PORT_INTERP
        };

        void setPort(int port, DspFloatType v) {
            switch(port)
            {
                case PORT_SWEEPWIDTH: sweepWidth_ = v; break;
                case PORT_DEPTH: depth_ = v; break;
                case PORT_FEEDBACK: feedback_ = v; break;
                case PORT_FREQUENCY: frequency_ = v; break;
                case PORT_WAVEFORM: waveform_ = (Waveform)v; break;
                case PORT_INTERP: interpolation_ = (Interpolation)v; break;
                default: printf("No port %d\n",port);
            }
        }
        // Circular buffer variables for implementing delay
        std::vector<DspFloatType> delayBuffer_[2];
        int delayBufferLength_;
        int delayWritePosition_[2];
        
        DspFloatType lfoPhase_[2];            // Phase of the low-frequency oscillator
        
        const DspFloatType MaximumDelay = 0.02;
        const DspFloatType MaximumSweepWidth = 0.02;

        Flanger() : StereoFXProcessor()
        {
            // Set default values:
            delay_ = .0025;
            sweepWidth_ = .010;
            depth_ = 1.0;
            feedback_ = 0.6;
            frequency_ = 0.2;
            waveform_ = kWaveformSine;
            interpolation_ = kInterpolationLinear;
            stereo_ = true;
            
            delayBufferLength_ = 1;
            setData<DspFloatType>(lfoPhase_,0);        
            setData<int>(delayWritePosition_,0);
            
            delayBufferLength_ = (int)((MaximumDelay + MaximumSweepWidth)*sampleRate) + 3;
            delayBuffer_[0].resize(delayBufferLength_);
            delayBuffer_[1].resize(delayBufferLength_);
            
            zeros(delayBuffer_[0]);
            zeros(delayBuffer_[1]);
        }

        void ProcessBlock(size_t numSamples, DspFloatType ** inputs, DspFloatType ** outputs)
        {
            // Helpful information about this block of samples:
            const int numInputChannels  = numChannels;
            const int numOutputChannels = numChannels;
            
            
            int channel, dpw; // dpr = delay read pointer; dpw = delay write pointer
            DspFloatType dpr, currentDelay, ph;
            
            
            // Go through each channel of audio that's passed in. In this example we apply identical
            // effects to each channel, regardless of how many input channels there are. For some effects, like
            // a stereo chorus or panner, you might do something different for each channel.
            
            for (channel = 0; channel < numInputChannels; ++channel)
            {
                DspFloatType channel0EndPhase = lfoPhase_[channel];
                // channelData is an array of length numSamples which contains the audio for one channel
                DspFloatType* channelData = inputs[channel];
                
                // delayData is the circular buffer for implementing delay on this channel
                DspFloatType* delayData = delayBuffer_[channel].data();
                DspFloatType * output = outputs[channel];

                // Make a temporary copy of any state variables declared in PluginProcessor.h which need to be
                // maintained between calls to processBlock(). Each channel needs to be processed identically
                // which means that the activity of processing one channel can't affect the state variable for
                // the next channel.
                
                dpw = delayWritePosition_[channel];
                ph = lfoPhase_[channel];
                
                // For stereo flanging, keep the channels 90 degrees out of phase with each other
                if(stereo_ != 0 && channel != 0)
                    ph = fmodf(ph + 0.25, 1.0);
                
                for (int i = 0; i < numSamples; ++i)
                {
                    const DspFloatType in = channelData[i];
                    DspFloatType interpolatedSample = 0.0;
                    
                    // Recalculate the read pointer position with respect to the write pointer. A more efficient
                    // implementation might increment the read pointer based on the derivative of the LFO without
                    // running the whole equation again, but this format makes the operation clearer.
                    
                    currentDelay = delay_ + sweepWidth_*LFO_GetSample(ph, waveform_);
                    dpr = fmodf((DspFloatType)dpw - (DspFloatType)(currentDelay * sampleRate) + (DspFloatType)delayBufferLength_,
                                (DspFloatType)delayBufferLength_);
                    
                    // In this example, the output is the input plus the contents of the delay buffer (weighted by delayMix)
                    // The last term implements a tremolo (variable amplitude) on the whole thing.
        
                    if(interpolation_ == kInterpolationLinear)
                    {
                        // Find the fraction by which the read pointer sits between two
                        // samples and use this to adjust weights of the samples
                        DspFloatType fraction = dpr - floorf(dpr);
                        int previousSample = (int)floorf(dpr);
                        int nextSample = (previousSample + 1) % delayBufferLength_;
                        interpolatedSample = fraction*delayData[nextSample]
                        + (1.0f-fraction)*delayData[previousSample];
                    }
                    else if(interpolation_ == kInterpolationCubic)
                    {
                        // Cubic interpolation will produce cleaner results at the expense
                        // of more computation. This code uses the Catmull-Rom variant of
                        // cubic interpolation. To reduce the load, calculate a few quantities
                        // in advance that will be used several times in the equation:
                        
                        int sample1 = (int)floorf(dpr);
                        int sample2 = (sample1 + 1) % delayBufferLength_;
                        int sample3 = (sample2 + 1) % delayBufferLength_;
                        int sample0 = (sample1 - 1 + delayBufferLength_) % delayBufferLength_;
                        
                        DspFloatType fraction = dpr - floorf(dpr);
                        DspFloatType frsq = fraction*fraction;
                        
                        DspFloatType a0 = -0.5f*delayData[sample0] + 1.5f*delayData[sample1]
                                    - 1.5f*delayData[sample2] + 0.5f*delayData[sample3];
                        DspFloatType a1 = delayData[sample0] - 2.5f*delayData[sample1]
                                    + 2.0f*delayData[sample2] - 0.5f*delayData[sample3];
                        DspFloatType a2 = -0.5f*delayData[sample0] + 0.5f*delayData[sample2];
                        DspFloatType a3 = delayData[sample1];
                        
                        interpolatedSample = a0*fraction*frsq + a1*frsq + a2*fraction + a3;
                    }
                    else // Nearest neighbour interpolation
                    {
                        // Find the nearest input sample by rounding the fractional index to the
                        // nearest integer. It's possible this will round it to the end of the buffer,
                        // in which case we need to roll it back to the beginning.
                        int closestSample = (int)floorf(dpr + 0.5f);
                        if(closestSample == delayBufferLength_)
                            closestSample = 0;
                        interpolatedSample = delayData[closestSample];
                    }
                    
                    // Store the current information in the delay buffer. With feedback, what we read is
                    // included in what gets stored in the buffer, otherwise it's just a simple delay line
                    // of the input signal.
                    
                    delayData[dpw] = in + (interpolatedSample * feedback_);
                    
                    // Increment the write pointer at a constant rate. The read pointer will move at different
                    // rates depending on the settings of the LFO, the delay and the sweep width.
                    
                    if (++dpw >= delayBufferLength_)
                        dpw = 0;

                    // Store the output sample in the buffer, replacing the input
                    output[i] = in + depth_ * interpolatedSample;

                    // Update the LFO phase, keeping it in the range 0-1
                    ph += frequency_*inverseSampleRate;
                    if(ph >= 1.0)
                        ph -= 1.0;
                }
                
                // Use channel 0 only to keep the phase in sync between calls to processBlock()
                // Otherwise quadrature phase on multiple channels will create problems.
                if(channel == 0)
                    channel0EndPhase = ph;

                delayWritePosition_[channel] = dpw;
                lfoPhase_[channel] = channel0EndPhase;                
            }                        
        }
        void InplaceProcess(size_t n, DspFloatType ** buffer)
        {
            ProcessBlock(n,buffer,buffer);
        }
    };
}