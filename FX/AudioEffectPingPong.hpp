#pragma once

namespace FX
{
    struct PingPongDelay : public StereoFXProcessor
    {
        // Adjustable parameters:
        DspFloatType delayLengthLeft_;  // Length of left->right delay line in seconds
        DspFloatType delayLengthRight_; // Length of right->left delay line in seconds
        DspFloatType wetMix_;           // Mix level of delayed signal (0-1)
        DspFloatType feedback_;         // Feedback level (0-just less than 1)
        bool reverseChannels_;   // Whether to reverse the outputs from the delays
        
        std::vector<DspFloatType> delayBuffer_[2];
        int delayBufferLength_;
        int delayReadPositionLeft_, delayReadPositionRight_;
        int delayWritePosition_;
        size_t numChannels = 2;

        enum {
            PORT_DELAY_LEFT,
            PORT_DELAY_RIGHT,
            PORT_WETMIX,
            PORT_FEEDBACK,
            PORT_REVERSECHANNELS
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_DELAY_LEFT: delayLengthLeft_ = v; break;
                case PORT_DELAY_RIGHT: delayLengthRight_ = v; break;
                case PORT_WETMIX: wetMix_ = v; break;
                case PORT_FEEDBACK: feedback_ = v; break;
                case PORT_REVERSECHANNELS: reverseChannels_ = v; break;
                default: printf("No port %d\n",port);
            }
        }
        PingPongDelay() : StereoFXProcessor()
        {
            // Set default values:
            delayLengthLeft_ = delayLengthRight_ = 0.5;
            wetMix_ = 0.5;
            feedback_ = 0.75;
            delayBufferLength_ = 1;
            reverseChannels_ = false;
            
            // Start the circular buffer pointers at the beginning        
            delayWritePosition_ = 0;

            delayBufferLength_ = (int)(2.0*sampleRate);
            if(delayBufferLength_ < 1)
                delayBufferLength_ = 1;
            
            for(size_t i = 0; i < 2; i++)
            {
                delayBuffer_[i].resize(delayBufferLength_);
                zeros(delayBuffer_[i]);
            }
            delayReadPositionLeft_  = (int)(delayWritePosition_ - (delayLengthLeft_ * sampleRate)
                                        + delayBufferLength_) % delayBufferLength_;
            delayReadPositionRight_ = (int)(delayWritePosition_ - (delayLengthRight_ * sampleRate)
                                        + delayBufferLength_) % delayBufferLength_; 

        }  
        void ProcessBlock (size_t numSamples, DspFloatType ** inputs, DspFloatType ** outputs)
        {
            // Helpful information about this block of samples:
            const int numInputChannels  = numChannels;
            const int numOutputChannels = numChannels;
            
            // This shouldn't happen, but we need a sanity check: this effect only makes sense
            // if there are at least 2 channels to work with (and in this case only 2...)
            if(numChannels < 2)
                return;
            
            // If there is one input only, the second channel may not contain anything useful.
            // start with a blank buffer in this case
            if(numChannels < 2)
                memset(inputs[1],0,numSamples*sizeof(DspFloatType));
            
            // channelDataL and channelDataR are arrays of length numSamples which contain
            // the audio for one channel
            DspFloatType *channelDataL = inputs[0];
            DspFloatType *channelDataR = inputs[1];
            DspFloatType *outputChannelL = outputs[0];
            DspFloatType *outputChannelR = outputs[1];

            // delayDataL and delayDataR are the circular buffers for implementing delay
            DspFloatType* delayDataL = delayBuffer_[0].data();
            DspFloatType* delayDataR = delayBuffer_[1].data();
            
            for (int i = 0; i < numSamples; ++i)
            {
                const DspFloatType inL = channelDataL[i];
                const DspFloatType inR = channelDataR[i];
                DspFloatType outL, outR;
                
                if(reverseChannels_)
                {
                    outL = (inL + wetMix_ * delayDataR[delayReadPositionLeft_]);
                    outR = (inR + wetMix_ * delayDataL[delayReadPositionRight_]);
                }
                else
                {
                    outL = (inL + wetMix_ * delayDataL[delayReadPositionLeft_]);
                    outR = (inR + wetMix_ * delayDataR[delayReadPositionRight_]);
                }
                    
                // Store the output of one delay buffer into the other, producing
                // the ping-pong effect
                delayDataR[delayWritePosition_] = inR + (delayDataL[delayReadPositionLeft_] * feedback_);
                delayDataL[delayWritePosition_] = inL + (delayDataR[delayReadPositionRight_] * feedback_);
                
                if (++delayReadPositionLeft_ >= delayBufferLength_) delayReadPositionLeft_ = 0;
                if (++delayReadPositionRight_ >= delayBufferLength_) delayReadPositionRight_ = 0;
                if (++delayWritePosition_ >= delayBufferLength_) delayWritePosition_ = 0;
                
                // Store the output samples in the buffer, replacing the input
                outputChannelL[i] = outL;
                outputChannelR[i] = outR;
            }            
        }
        void InplaceProcess(size_t n, DspFloatType ** buffer)
        {
            ProcessBlock(n,buffer,buffer);
        }
    };
}