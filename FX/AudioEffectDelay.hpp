#pragma once

namespace FX
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Delay with mulitap reads
    ////////////////////////////////////////////////////////////////////////////////////////////////
    struct DelayEffect : public StereoFXProcessor
    {
        DspFloatType  delayLength_; // Length of delay line in seconds
        int    delayTime;
        DspFloatType  dryMix_;      // Mix level of original signal (0-1)
        DspFloatType  wetMix_;      // Mix level of delayed signal (0-1)
        DspFloatType  feedback_;    // Feedback level (0-just less than 1)
        size_t numChannels = 2;
        // Circular buffer variables for implementing delay
        std::vector<DspFloatType> delayBuffer[2];
        std::vector<size_t> tap_reads;
        size_t taps;
        int delayBufferLength_;
        int delayReadPosition_[2], delayWritePosition_[2];
        enum {
            PORT_DELAYTIME,
            PORT_DRYMIX,
            PORT_WETMIX,
            PORT_FEEDBACK,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_DELAYTIME: delayTime = v; break;
                case PORT_DRYMIX: dryMix_ = v; break;
                case PORT_WETMIX: wetMix_ = v; break;
                case PORT_FEEDBACK: feedback_ = v; break;
            }
        }
        DelayEffect() : StereoFXProcessor()
        {
            // Set default values:
            delayLength_ = 0.5;
            delayTime = int(delayLength_ * sampleRate);        
            dryMix_ = 0.5;
            wetMix_ = 0.5;
            feedback_ = 0.75;
            taps = 0;
            delayBufferLength_ = (int)(2.0*sampleRate);        
            setData<int>(delayReadPosition_,0);
            setData<int>(delayWritePosition_,delayTime-1);        
            delayBuffer[0].resize(delayBufferLength_);        
            delayBuffer[1].resize(delayBufferLength_);        
            zeros(delayBuffer[0]);
            zeros(delayBuffer[1]);
        }
        void addTap(DspFloatType t) {
            size_t d = t*sampleRate;            
            tap_reads.push_back(d);                                    
            taps++;
        }        
        
        void ProcessBlock(size_t n, DspFloatType ** inputs, DspFloatType ** outputs)
        {
            // Helpful information about this block of samples:
            const int numInputChannels  = numChannels;
            const int numOutputChannels = numChannels;
            const int numSamples = n;
            
            int channel, dpr, dpw; // dpr = delay read pointer; dpw = delay write pointer
            
            // Go through each channel of audio that's passed in. In this example we apply identical
            // effects to each channel, regardless of how many input channels there are. For some effects, like
            // a stereo chorus or panner, you might do something different for each channel.
            
            for (channel = 0; channel < numInputChannels; ++channel)
            {
                // channelData is an array of length numSamples which contains the audio for one channel
                DspFloatType* channelData = inputs[channel];
                DspFloatType* output      = outputs[channel];

                // delayData is the circular buffer for implementing delay on this channel
                DspFloatType* delayData   = delayBuffer[channel].data();
                
                // Make a temporary copy of any state variables declared in PluginProcessor.h which need to be
                // maintained between calls to processBlock(). Each channel needs to be processed identically
                // which means that the activity of processing one channel can't affect the state variable for
                // the next channel.
                
                dpr = delayReadPosition_[channel];
                dpw = delayWritePosition_[channel];
                
                for (int i = 0; i < numSamples; ++i)
                {
                    DspFloatType tapout = 0;
                    DspFloatType sum    = 0;            
                    size_t read;                
                    for(size_t j = 0; j < taps; j++)
                    {                                   
                        read = tap_reads[j];                                
                        DspFloatType x  = delayData[read];
                        DspFloatType f  = x - floor(x); //(int)output;                                        
                        DspFloatType x1 = x;
                        read = (read+1) % delayTime;
                        DspFloatType x2 = delayData[read];
                        tapout  = x1 + (f*(x2-x1));
                        tap_reads[j] = read;
                        sum += tapout;
                    }              
                    if(taps != 0) sum /= taps;                          
                    const DspFloatType in = channelData[i];
                    DspFloatType x1  = delayData[dpr];
                    DspFloatType t   = x1 - floor(x1);                   
                    dpr       = (dpr + 1) % delayTime;                         
                    DspFloatType x2  = delayData[dpr];
                    DspFloatType out = x1 + t*(x2-x1);                
                    
                    //assert(out >= -1 && out <= 1);
                    delayData[dpw] = in + (out * feedback_);                                                
                    dpw = (dpw + 1) % (delayTime);                                
                    // Store the output sample in the buffer, replacing the input          
                    out = 0.5*(out+sum);      
                    output[i] = dryMix_ * in +  wetMix_ * out;
                }
                delayReadPosition_[channel] = dpr;
                delayWritePosition_[channel] = dpw;        
            }        
        }

        void InplaceProcess(size_t n, DspFloatType ** buffer)
        {
            ProcessBlock(n,buffer,buffer);
        }
    };

    struct MonoDelay : public MonoFXProcessor
    {
        DelayEffect * delay;
        DspFloatType  ** ins, ** outs;
        MonoDelay() : MonoFXProcessor()
        {
            delay = new DelayEffect;
            delay->numChannels=1;            
            ins =  (DspFloatType**)calloc(2,sizeof(DspFloatType*));
            outs = (DspFloatType**)calloc(2,sizeof(DspFloatType*));
        }
        ~MonoDelay() {
            if(delay) delete delay;
            if(ins) free(ins);
            if(outs) free(outs);
        }
        void ProcessBlock(size_t numSamples, DspFloatType * in, DspFloatType * out)
        {
            ins[0] = in;
            ins[1] = in;
            outs[0]= out;
            outs[1]= out;
            delay->ProcessBlock(numSamples,ins,outs);
        }
    };
    
}