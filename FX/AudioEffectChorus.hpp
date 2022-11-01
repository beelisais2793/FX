///////////////////////////////////////////////////////////////////////////////
// Reiss Chorus
///////////////////////////////////////////////////////////////////////////////
#pragma once

namespace FX
{
    const DspFloatType kMaximumDelay = 0.05;
    const DspFloatType kMaximumSweepWidth = 0.05;

    struct Chorus : public StereoFXProcessor
    {
        std::vector<DspFloatType> chorus_delay_buffer[2];

        int   chorus_num_voices[2];
        DspFloatType chorus_delay[2];
        DspFloatType chorus_sweep_width[2];
        DspFloatType chorus_depth[2];
        DspFloatType chorus_frequency[2];  // LFO frequency (Hz)

        Waveform        chorus_waveform[2];   // What shape should be used for the LFO
        Interpolation   chorus_interpolation[2]; // What type of interpolation to use

        int     chorus_delayBufferLength[2];
        int     chorus_delayWritePosition[2];
        DspFloatType   chorus_lfoPhase[2];   // Phase of the low-frequency oscillator
            
        int numInputChannels  = 2;
        int numOutputChannels = 2;
            
        enum {
            PORT_NUM_VOICES,
            PORT_DELAY,
            PORT_SWEEP_WIDTH,
            PORT_DEPTH,
            PORT_FREQUENCY,
            PORT_WAVEFORM,
            PORT_INTERP,
            PORT_NUM_VOICES1,
            PORT_DELAY1,
            PORT_SWEEP_WIDTH1,
            PORT_DEPTH1,
            PORT_FREQUENCY1,
            PORT_WAVEFORM1,
            PORT_INTERP1,
            PORT_NUM_VOICES2,
            PORT_DELAY2,
            PORT_SWEEP_WIDTH2,
            PORT_DEPTH2,
            PORT_FREQUENCY2,
            PORT_WAVEFORM2,
            PORT_INTERP2,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_NUM_VOICES: { for(size_t i = 0; i < 2; i++) chorus_num_voices[i] = v; break; }
                case PORT_DELAY: { for(size_t i = 0; i < 2; i++) chorus_delay[i] = v; break; }
                case PORT_SWEEP_WIDTH: { for(size_t i = 0; i < 2; i++) chorus_sweep_width[i] = v; break; }
                case PORT_DEPTH: { for(size_t i = 0; i < 2; i++) chorus_depth[i] = v; break; }
                case PORT_FREQUENCY: { for(size_t i = 0; i < 2; i++) chorus_frequency[i] = v; break; }
                case PORT_WAVEFORM: { for(size_t i = 0; i < 2; i++) chorus_waveform[i] = (Waveform)v; break; }
                case PORT_INTERP: { for(size_t i = 0; i < 2; i++) chorus_interpolation[i] = (Interpolation)v; break; }

                case PORT_NUM_VOICES1: { chorus_num_voices[0] = v; break; }
                case PORT_DELAY1: { chorus_delay[0] = v; break; }
                case PORT_SWEEP_WIDTH1: { chorus_sweep_width[0] = v; break; }
                case PORT_DEPTH1: { chorus_depth[0] = v; break; }
                case PORT_FREQUENCY1: { chorus_frequency[0] = v; break; }
                case PORT_WAVEFORM1: { chorus_waveform[0] = (Waveform)v; break; }
                case PORT_INTERP1: { chorus_interpolation[0] = (Interpolation)v; break; }

                case PORT_NUM_VOICES2: { chorus_num_voices[1] = v; break; }
                case PORT_DELAY2: { chorus_delay[1] = v; break; }
                case PORT_SWEEP_WIDTH2: { chorus_sweep_width[1] = v; break; }
                case PORT_DEPTH2: { chorus_depth[1] = v; break; }
                case PORT_FREQUENCY2: { chorus_frequency[1] = v; break; }
                case PORT_WAVEFORM2: { chorus_waveform[1] = (Waveform)v; break; }
                case PORT_INTERP2: { chorus_interpolation[1] = (Interpolation)v; break; }

                default: printf("No port %d\n",port);
            }
        }
        Chorus(DspFloatType delay=30, DspFloatType sweep=0.02,DspFloatType depth=1.0, DspFloatType freq=0.2f,Waveform waveform=kWaveformSine,
                    Interpolation interp = kInterpolationLinear, int voices=2) :
                    StereoFXProcessor()
        {
            setData<DspFloatType>(chorus_delay,delay/1000.0f);
            setData<DspFloatType>(chorus_sweep_width,sweep);
            setData<DspFloatType>(chorus_depth, depth);
            setData<Waveform>(chorus_waveform,waveform);
            setData<Interpolation>(chorus_interpolation,interp);        
            setData<int>(chorus_num_voices,voices);
            setData<DspFloatType>(chorus_frequency,freq);
            setData<DspFloatType>(chorus_lfoPhase,0);
            setData<int>(chorus_delayWritePosition,0);
            
            int len = (int)((kMaximumDelay + kMaximumSweepWidth)*sampleRate)+3;    
            setData<int>(chorus_delayBufferLength,len);
            chorus_delay_buffer[0].resize(len);    
            chorus_delay_buffer[1].resize(len);
            memset(chorus_delay_buffer[0].data(),0,len*sizeof(DspFloatType));    
            memset(chorus_delay_buffer[1].data(),0,len*sizeof(DspFloatType));    
        }      
        // process mono or stereo I just left mono there because I already done it
        void ProcessBlock (size_t n, DspFloatType **inputs, DspFloatType **outputs)
        {
            // Helpful information about this block of samples:
            const int numSamples        = n;
            
            int channel, dpw; // dpr = delay read pointer; dpw = delay write pointer
            DspFloatType dpr, currentDelay, ph;
            
            // Go through each channel of audio that's passed in. In this example we apply identical
            // effects to each channel, regardless of how many input channels there are. For some effects, like
            // a stereo chorus or panner, you might do something different for each channel.
            
            for (channel = 0; channel < numInputChannels; ++channel)
            {
                // channelData is an array of length numSamples which contains the audio for one channel
                DspFloatType* channelData = inputs[channel];
                DspFloatType  phaseOffset = 0.0;

                // delayData is the circular buffer for implementing delay on this channel
                DspFloatType* delayData = chorus_delay_buffer[channel].data();
                
                // Make a temporary copy of any state variables declared in PluginProcessor.h which need to be
                // maintained between calls to processBlock(). Each channel needs to be processed identically
                // which means that the activity of processing one channel can't affect the state variable for
                // the next channel.
                
                dpw = chorus_delayWritePosition[channel];
                ph  = chorus_lfoPhase[channel];

                for (int i = 0; i < numSamples; ++i)
                {             
                    const DspFloatType in = channelData[i];
                    DspFloatType interpolatedSample = 0.0;            
                    DspFloatType weight;
                    
                    // Chorus can have more than 2 voices (where the original, undelayed signal counts as a voice).
                    // In this implementation, all voices use the same LFO, but with different phase offsets. It
                    // is also possible to use different waveforms and different frequencies for each voice.
                    outputs[channel][i] = in;
                    phaseOffset=0;
                    for(int j = 0; j < chorus_num_voices[channel]-1; ++j)
                    {
                        if(chorus_num_voices[channel] > 2)
                        {
                            // A stereo chorus pans each voice to a different location in the stereo field.
                            // How this is done depends on the number of voices:
                            // -- 2 voices: N/A (need at least 2 delayed voices for stereo chorus)
                            // -- 3 voices: 1 voice left, 1 voice right (0, 1)
                            // -- 4 voices: 1 voice left, 1 voice centre, 1 voice right (0, 0.5, 1)
                            // -- 5 voices: 1 voice left, 1 voice left-centre,
                            //              1 voice right-centre, 1 voice right (0, 0.33, 0.66, 1)
                            
                            weight = (DspFloatType)j/(DspFloatType)(chorus_num_voices[channel]- 2);
                            
                            // Left and right channels are mirrors of each other in weight
                            if(channel != 0)
                                weight = 1.0 - weight;
                        }
                        else
                            weight = 1.0;

                        // Add the voice to the mix if it has nonzero weight
                        if(weight != 0.0)
                        {
                            // Recalculate the read pointer position with respect to the write pointer. A more efficient
                            // implementation might increment the read pointer based on the derivative of the LFO without
                            // running the whole equation again, but this format makes the operation clearer.
                            
                            currentDelay = chorus_delay[channel] + chorus_sweep_width[channel] * LFO_GetSample(fmodf(ph + phaseOffset, 1.0f), chorus_waveform[channel]);
                            dpr = fmodf((DspFloatType)dpw - (DspFloatType)(currentDelay * sampleRate) + (DspFloatType)chorus_delayBufferLength[channel],
                                        (DspFloatType)chorus_delayBufferLength[channel]);
                            
                            // In this example, the output is the input plus the contents of the delay buffer (weighted by delayMix)
                            // The last term implements a tremolo (variable amplitude) on the whole thing.
                
                            if(chorus_interpolation[channel] == kInterpolationLinear)
                            {
                                // Find the fraction by which the read pointer sits between two
                                // samples and use this to adjust weights of the samples
                                DspFloatType fraction = dpr - floorf(dpr);
                                int previousSample = (int)floorf(dpr);
                                int nextSample = (previousSample + 1) % chorus_delayBufferLength[channel];
                                interpolatedSample = fraction*delayData[nextSample] + (1.0f-fraction)*delayData[previousSample];
                            }
                            else if(chorus_interpolation[channel] == kInterpolationCubic)
                            {
                                // Cubic interpolation will produce cleaner results at the expense
                                // of more computation. This code uses the Catmull-Rom variant of
                                // cubic interpolation. To reduce the load, calculate a few quantities
                                // in advance that will be used several times in the equation:
                                
                                int sample1 = (int)floorf(dpr);
                                int sample2 = (sample1 + 1) % chorus_delayBufferLength[channel];
                                int sample3 = (sample2 + 1) % chorus_delayBufferLength[channel];
                                int sample0 = (sample1 - 1 + chorus_delayBufferLength[channel]) % chorus_delayBufferLength[channel];
                                
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
                                if(closestSample == chorus_delayBufferLength[channel])
                                    closestSample = 0;                       
                                interpolatedSample = delayData[closestSample];
                            }

                            // Store the output sample in the buffer, which starts by containing the input sample
                            outputs[channel][i] += chorus_depth[channel] * weight * interpolatedSample;
                        }
                        
                        // 3-voice chorus uses two voices in quadrature phase (90 degrees apart). Otherwise,
                        // spread the voice phases evenly around the unit circle. (For 2-voice chorus, this
                        // code doesn't matter since the loop only runs once.)
                        if(chorus_num_voices[channel] < 3)
                            phaseOffset += 0.25f;
                        else
                            phaseOffset += 1.0f / (DspFloatType)(chorus_num_voices[channel]- 1);
                    }
                    
                    // Store the current input in the delay buffer (no feedback in a chorus, unlike a flanger).
                    delayData[dpw] = in;
                    
                    // Increment the write pointer at a constant rate. The read pointer will move at different
                    // rates depending on the settings of the LFO, the delay and the sweep width.
                    
                    if (++dpw >= chorus_delayBufferLength[channel]) dpw = 0;

                    // Update the LFO phase, keeping it in the range 0-1
                    ph = fmod(ph + chorus_frequency[channel] * inverseSampleRate,1);            
                }            
                chorus_delayWritePosition[channel] = dpw;
                chorus_lfoPhase[channel] = ph;    
            }            
        }
        void InplaceProcess(size_t n, DspFloatType ** buffer)
        {
            ProcessBlock(n,buffer,buffer);
        }
    };   


    struct MonoChorus : public MonoFXProcessor
    {
        Chorus * chorus;
        DspFloatType  ** ins, ** outs;
        MonoChorus() : MonoFXProcessor()
        {
            chorus = new Chorus;
            chorus->numInputChannels=1;
            chorus->numOutputChannels=1;
            ins =  (DspFloatType**)calloc(2,sizeof(DspFloatType*));
            outs = (DspFloatType**)calloc(2,sizeof(DspFloatType*));
        }
        ~MonoChorus() {
            if(chorus) delete chorus;
            if(ins) free(ins);
            if(outs) free(outs);
        }
        void ProcessBlock(size_t numSamples, DspFloatType * in, DspFloatType * out)
        {
            ins[0] = in;
            ins[1] = in;
            outs[0]= out;
            outs[1]= out;
            chorus->ProcessBlock(numSamples,ins,outs);
        }
    };
}

