#pragma once

#include "fftw3.h"

namespace FX
{
    struct PVPitchShifter : public StereoFXProcessor
    {
        enum Window
        {
            kWindowRectangular = 1,
            kWindowBartlett,
            kWindowHann,
            kWindowHamming
        };
        
        enum HopSize
        {
            kHopSize1Window = 1,
            kHopSize1_2Window,
            kHopSize1_4Window,
            kHopSize1_8Window
        };
        
        enum PitchShift      // (⊙_⊙) pitch shift from 6 semitones rise to 6 semitones drop
        {
            kShift0 = 1,
            kShiftP1,
            kShiftP2,
            kShiftP3,
            kShiftP4,
            kShiftP5,
            kShiftP6,
            kShiftM1,
            kShiftM2,
            kShiftM3,
            kShiftM4,
            kShiftM5,
            kShiftM6
            
        };
        
        // This parameter indicates the FFT size for phase vocoder computation. It is selected
        // by the GUI and may temporarily differ from the actual size used in calculations.
        int fftSelectedSize_;
        int hopSelectedSize_;       // Hop size, chosen from one of the options above
        int windowType_;            // Type of window used
        int pitchSelectedShift_;         // (⊙_⊙)semitones of pitch rise or drop

        
        // Whether the FFT has been initialised and is therefore ready to go
        bool fftInitialised_;
        
        // Variables for calculating the FFT and IFFT: complex data structures and the
        // "plan" used by the fftw library to calculate the transforms.
        fftw_complex *fftTimeDomain_, *fftFrequencyDomain_;
        fftw_plan fftForwardPlan_, fftBackwardPlan_;
        
        // Size of the FFT (generally a power of two) and the hop size (in samples, generally a fraction of FFT size)
        int fftActualTransformSize_;
        int hopActualSize_;
        
        // (⊙_⊙) The actual pitch shift by ratios
        double pitchActualShift_;      // the frequency ratio of original and shifted pitch
        double pitchActualShiftRec_;   // the reciprocal of of pitchActualShift_, just for computing efficiency
        double actualRatio_;
        double omega_[2048];           // the arrays for storing the phases
        double phi_[2048][2];
        double phi0_[2048][2];
        double dphi_[2048][2];
        double psi_[2048][2];
        
        // Amount by which to scale the inverse FFT to return to original amplitude: depends on the
        // transform size (because of fftw implementation) and the hop size (because of inherent overlap)
        double fftScaleFactor_;
        
        // Circular buffer gathers audio samples from the input until enough are available
        // for the FFT calculation
        std::vector<DspFloatType> inputBuffer_[2];
        int inputBufferLength_;
        int inputBufferWritePosition_;
        
        // Circular buffer that collects output samples from the FFT overlap-add process
        // before they are ready to be sent to the output stream
        std::vector<DspFloatType> outputBuffer_[2];
        int outputBufferLength_;
        int outputBufferReadPosition_, outputBufferWritePosition_;
        
        // How many samples since the last FFT?
        int samplesSinceLastFFT_;
        
        // Stored window function for pre-processing input frames
        double *windowBuffer_;
        double *synthesisWindowBuffer_;
        int windowBufferLength_;
        int synthesisWindowBufferLength_;
        
        // Whether or not prepareToPlay() has been called, i.e. that resources are in use
        bool preparedToPlay_;
        
        size_t numChannels = 2;

        enum {
            PORT_SEMITONE
        };
        void setPort(int port, double v)
        {
            if(port == PORT_SEMITONE) setPitchShift(v);
        }
        PVPitchShifter() : StereoFXProcessor()
        {
            // Set default values:
            fftSelectedSize_ = 1024;
            hopSelectedSize_ = kHopSize1_8Window;
            windowType_ = kWindowHann;
            
            // (⊙_⊙)
            pitchSelectedShift_ = kShift0;
            pitchActualShift_ = 1.0;
            pitchActualShiftRec_ = 1.0;
            actualRatio_ = 1.0;
            synthesisWindowBufferLength_ = 1024;
            for (int i = 0; i<2048; i++)
            {
                omega_[i] = 0.25*M_PI*i;  // 0.25 corresponding to 1/8 window (2*hopsize/windowlength)
            }

            fftInitialised_ = false;
            fftActualTransformSize_ = 0;
            inputBufferLength_ = 1;
            outputBufferLength_ = 1;
            inputBufferWritePosition_ = outputBufferWritePosition_ = outputBufferReadPosition_ = 0;        
            samplesSinceLastFFT_ = 0;
            windowBuffer_ = 0;
            synthesisWindowBuffer_ = 0;
            windowBufferLength_ = 0;
            synthesisWindowBufferLength_ = 0;
            preparedToPlay_ = false;
            fftScaleFactor_ = 0.0;

            initFFT(fftSelectedSize_);
            initWindow(fftSelectedSize_, windowType_);
            initSynthesisWindow(floor(fftSelectedSize_*pitchActualShiftRec_), windowType_);
            preparedToPlay_ = true;
        }
        ~PVPitchShifter()
        {
            deinitFFT();
            deinitWindow();
            deinitSynthesisWindow();
            preparedToPlay_ = false;
        }
        void initFFT(int length)
        {
            if(fftInitialised_)
                deinitFFT();
            
            // Save the current length so we know how big our results are later
            fftActualTransformSize_ = length;
            
            // Here we allocate the complex-number buffers for the FFT. This uses
            // a convenient wrapper on the more general fftw_malloc()
            fftTimeDomain_ = fftw_alloc_complex(length);
            fftFrequencyDomain_ = fftw_alloc_complex(length);
            
            // FFTW_ESTIMATE doesn't necessarily produce the fastest executing code (FFTW_MEASURE
            // will get closer) but it carries a minimum startup cost. FFTW_MEASURE might stall for
            // several seconds which would be annoying in an audio plug-in context.
            fftForwardPlan_ = fftw_plan_dft_1d(fftActualTransformSize_, fftTimeDomain_,
                                            fftFrequencyDomain_, FFTW_FORWARD, FFTW_ESTIMATE);
            fftBackwardPlan_ = fftw_plan_dft_1d(fftActualTransformSize_, fftFrequencyDomain_,
                                            fftTimeDomain_, FFTW_BACKWARD, FFTW_ESTIMATE);
            
            // Allocate the buffer that the samples will be collected in
            inputBufferLength_ = fftActualTransformSize_;
            inputBuffer_[0].resize(inputBufferLength_);
            inputBuffer_[1].resize(inputBufferLength_);
            zeros(inputBuffer_[0]);
            zeros(inputBuffer_[1]);
            inputBufferWritePosition_ = 0;
            samplesSinceLastFFT_ = 0;
            
            // Allocate the output buffer to be twice the size of the FFT
            // This will be enough for all hop size cases
            outputBufferLength_ = 2*fftActualTransformSize_;
            outputBuffer_[0].resize(outputBufferLength_);
            outputBuffer_[1].resize(outputBufferLength_);
            zeros(outputBuffer_[0]);
            zeros(outputBuffer_[1]);
            outputBufferReadPosition_ = 0;
            
            updateHopSize();
            
            //(⊙_⊙)
            updatePitchShift();
            
            fftInitialised_ = true;
        }

        // Free the FFT data structures
        void deinitFFT()
        {
            if(!fftInitialised_)
                return;
            
            // Prevent this variable from changing while an audio callback is running.
            // Once it has changed, the next audio callback will find that it's not
            // initialised and will return silence instead of attempting to work with the
            // (invalid) FFT structures. This produces an audible glitch but no crash,
            // and is the simplest way to handle parameter changes in this example code.
            //fftSpinLock_.enter();
            fftInitialised_ = false;
            //fftSpinLock_.exit();
            
            fftw_destroy_plan(fftForwardPlan_);
            fftw_destroy_plan(fftBackwardPlan_);
            fftw_free(fftTimeDomain_);
            fftw_free(fftFrequencyDomain_);
            
            // Leave the input buffer in memory until the plugin is released
        }

        //==============================================================================
        // Create a new window of a given length and type
        void initWindow(int length, int windowType)
        {
            if(windowBuffer_ != 0)
                deinitWindow();
            if(length == 0) // Sanity check
                return;
            
            // Allocate memory for the window
            windowBuffer_ = (double *)malloc(length * sizeof(double));
            
            // Write the length as a double here to simplify the code below (otherwise
            // typecasts would be wise)
            double windowLength = length;
            
            // Set values for the window, depending on its type
            for(int i = 0; i < length; i++)
            {
                // Window functions are typically defined to be symmetrical. This will cause a
                // problem in the overlap-add process: the windows instead need to be periodic
                // when arranged end-to-end. As a result we calculate the window of one sample
                // larger than usual, and drop the last sample. (This works as long as N is even.)
                // See Julius Smith, "Spectral Audio Signal Processing" for details.
                switch(windowType)
                {
                    case kWindowBartlett:
                        windowBuffer_[i] = (2.0/(windowLength + 2.0))*
                            (0.5*(windowLength + 2.0) - abs((double)i - 0.5*windowLength));
                        break;
                    case kWindowHann:
                        windowBuffer_[i] = 0.5*(1.0 - cos(2.0*M_PI*(double)i/windowLength));
                        break;
                    case kWindowHamming:
                        windowBuffer_[i] = 0.54 - 0.46*cos(2.0*M_PI*(double)i/windowLength);
                        break;
                    case kWindowRectangular:
                    default:
                        windowBuffer_[i] = 1.0;
                        break;
                }
            }
            
            windowBufferLength_ = length;
            updateScaleFactor();
        }

        //==============================================================================
        // Create a new synthesis window of a given length and type
        void initSynthesisWindow(int length, int windowType)
        {
            if(synthesisWindowBuffer_ != 0)
                deinitSynthesisWindow();
            if(length == 0) // Sanity check
                return;
            
            // Allocate memory for the window
            synthesisWindowBuffer_ = (double *)malloc(length * sizeof(double));
            
            // Write the length as a double here to simplify the code below (otherwise
            // typecasts would be wise)
            double windowLength = length;
            
            // Set values for the window, depending on its type
            for(int i = 0; i < length; i++)
            {
                // Window functions are typically defined to be symmetrical. This will cause a
                // problem in the overlap-add process: the windows instead need to be periodic
                // when arranged end-to-end. As a result we calculate the window of one sample
                // larger than usual, and drop the last sample. (This works as long as N is even.)
                // See Julius Smith, "Spectral Audio Signal Processing" for details.
                switch(windowType)
                {
                    case kWindowBartlett:
                        synthesisWindowBuffer_[i] = (2.0/(windowLength + 2.0))*
                        (0.5*(windowLength + 2.0) - abs((double)i - 0.5*windowLength));
                        break;
                    case kWindowHann:
                        synthesisWindowBuffer_[i] = 0.5*(1.0 - cos(2.0*M_PI*(double)i/windowLength));
                        break;
                    case kWindowHamming:
                        synthesisWindowBuffer_[i] = 0.54 - 0.46*cos(2.0*M_PI*(double)i/windowLength);
                        break;
                    case kWindowRectangular:
                    default:
                        synthesisWindowBuffer_[i] = 1.0;
                        break;
                }
            }
            
            synthesisWindowBufferLength_ = length;
            updateScaleFactor();
        }

        // Free the window buffer
        void deinitWindow()
        {
            if(windowBuffer_ == 0)
                return;
            
            // Delay clearing the window until the audio thread is not running
            // to avoid a crash if the code tries to access an invalid window
            // we might need this kaka later but not a Juce crapola
            //fftSpinLock_.enter();
            windowBufferLength_ = 0;
            //fftSpinLock_.exit();
            
            free(windowBuffer_);
            windowBuffer_ = 0;
        }

        // Free the synthesis window buffer
        void deinitSynthesisWindow()
        {
            if(synthesisWindowBuffer_ == 0)
                return;
            
            // Delay clearing the window until the audio thread is not running
            // to avoid a crash if the code tries to access an invalid window
            /// damned kaka
            ///fftSpinLock_.enter();
            synthesisWindowBufferLength_ = 0;
            ///fftSpinLock_.exit();
            
            free(synthesisWindowBuffer_);
            synthesisWindowBuffer_ = 0;
        }
        // can shift any balls
        void setPitchShift(DspFloatType semi) {
            pitchActualShift_ = pow(2.0,semi/12.0);
            actualRatio_ = round(pitchActualShift_*hopActualSize_)/hopActualSize_;
            pitchActualShiftRec_ = 1/pitchActualShift_;
        }
        // silly rabbit
        void updatePitchShift()
        {
            switch(pitchSelectedShift_)
            {
                case kShift0:
                    pitchActualShift_ = 1.0;
                    break;
                case kShiftP1:
                    pitchActualShift_ = pow(2.0, 1.0/12.0 );
                    break;
                case kShiftP2:
                    pitchActualShift_ = pow(2.0, 2.0/12.0 );
                    break;
                case kShiftP3:
                    pitchActualShift_ = pow(2.0, 3.0/12.0 );
                    break;
                case kShiftP4:
                    pitchActualShift_ = pow(2.0, 4.0/12.0 );
                    break;
                case kShiftP5:
                    pitchActualShift_ = pow(2.0, 5.0/12.0 );
                    break;
                case kShiftP6:
                    pitchActualShift_ = pow(2.0, 6.0/12.0 );
                    break;
                    
                case kShiftM1:
                    pitchActualShift_ = pow(2.0, -1.0/12.0 );
                    break;
                case kShiftM2:
                    pitchActualShift_ = pow(2.0, -2.0/12.0 );
                    break;
                case kShiftM3:
                    pitchActualShift_ = pow(2.0, -3.0/12.0 );
                    break;
                case kShiftM4:
                    pitchActualShift_ = pow(2.0, -4.0/12.0 );
                    break;
                case kShiftM5:
                    pitchActualShift_ = pow(2.0, -5.0/12.0 );
                    break;
                case kShiftM6:
                    pitchActualShift_ = pow(2.0, -6.0/12.0 );
                    break;
            }
            actualRatio_ = round(pitchActualShift_*hopActualSize_)/hopActualSize_;
            pitchActualShiftRec_ = 1/pitchActualShift_;
        }
        // (⊙_⊙) principle phase argument mod(phasein+pi,-2*pi)+pi;
        double princArg(double phaseIn)
        {
            if (phaseIn >= 0)
                return fmod(phaseIn + M_PI, 2*M_PI) - M_PI;
            else
                return fmod(phaseIn + M_PI, -2*M_PI) + M_PI;
        }
        // Update the actual hop size depending on the window size and hop size settings
        // Hop size is expressed as a fraction of a window in the parameters.
        void updateHopSize()
        {
            switch(hopSelectedSize_)
            {
                case kHopSize1Window:
                    hopActualSize_ = fftActualTransformSize_;
                    break;
                case kHopSize1_2Window:
                    hopActualSize_ = fftActualTransformSize_ / 2;
                    break;
                case kHopSize1_4Window:
                    hopActualSize_ = fftActualTransformSize_ / 4;
                    break;
                case kHopSize1_8Window:
                    hopActualSize_ = fftActualTransformSize_ / 8;
                    break;
            }
            
            // Update the factor by which samples are scaled to preserve unity gain
            updateScaleFactor();
            
            // Read pointer lags the write pointer to allow for FFT buffers to accumulate and
            // be processed. Total latency is sum of the FFT size and the hop size.
            outputBufferWritePosition_ = hopActualSize_ + fftActualTransformSize_;
        }
        // Update the factor by which each output sample is scaled. This needs to update
        // every time FFT size, hop size, and window type are changed.
        void updateScaleFactor()
        {
            // The gain needs to be normalised by the sum of the window, which implicitly
            // accounts for the length of the transform and the window type. From there
            // we also update based on hop size: smaller hop means more overlap means the
            // overall gain should be reduced.
            double windowSum = 0.0;
            
            for(int i = 0; i < windowBufferLength_; i++)
            {
                windowSum += windowBuffer_[i];
            }
            
            if(windowSum == 0.0)
                fftScaleFactor_ = 0.0; // Catch invalid cases and mute output
            else
            {
                switch(hopSelectedSize_)
                {
                    case kHopSize1Window:   // 0dB
                        fftScaleFactor_ = 1.0/(double)windowSum;
                        break;
                    case kHopSize1_2Window: // -6dB
                        fftScaleFactor_ = 0.5/(double)windowSum;
                        break;
                    case kHopSize1_4Window: // -12dB
                        fftScaleFactor_ = 0.25/(double)windowSum;
                        break;
                    case kHopSize1_8Window: // -18dB
                        fftScaleFactor_ = 0.125/(double)windowSum;
                        break;
                }
            }
        }

        void ProcessBlock (size_t numSamples, DspFloatType ** inputs, DspFloatType ** outputs)
        {
            // Helpful information about this block of samples:
            const int numInputChannels  = numChannels;
            const int numOutputChannels = numChannels;
            
            
            int channel, inwritepos, sampsincefft;
            int outreadpos, outwritepos;
            
            // Grab the lock that prevents the FFT settings from changing
            //fftSpinLock_.enter();
            
            // Check that we're initialised and ready to go. If not, set output to 0
            if(!fftInitialised_)
            {
                memset(outputs[0],0,numSamples*sizeof(DspFloatType));
                memset(outputs[1],0,numSamples*sizeof(DspFloatType));
                return;
            }

            // Go through each channel of audio that's passed in. Collect the samples in the input
            // buffer. When we've reached the next hop interval, calculate the FFT.
            for (channel = 0; channel < numInputChannels; ++channel)
            {
                // (⊙_⊙) 
                //double amplitude[fftActualTransformSize_];
                
        //        double phi[fftActualTransformSize_];
        //        double phi0[fftActualTransformSize_];
        //        double dphi[fftActualTransformSize_];
        //        double psi[fftActualTransformSize_];
        //        double omega[fftActualTransformSize_];
        //        for (int i = 0; i<fftActualTransformSize_; i++)
        //        {
        //            omega[i] = 2*M_PI*hopActualSize_*i/fftActualTransformSize_;
        //        }
                
                // (⊙_⊙) variables prepared for resampling
                double* grain2 = new double[fftActualTransformSize_ + 1];
                double* grain3 = new double[(int)floor(pitchActualShiftRec_*fftActualTransformSize_)];
                double lx;
                double x;
                int ix;
                double dx;

                
                
                // channelData is an array of length numSamples which contains the audio for one channel
                DspFloatType* channelData = inputs[channel];
                DspFloatType* channelOut  = outputs[channel];

                // inputBufferData is the circular buffer for collecting input samples for the FFT
                DspFloatType* inputBufferData  = inputBuffer_[channel].data();
                DspFloatType* outputBufferData = outputBuffer_[channel].data();
                
                // State variables need to be temporarily cached for each channel. We don't want the
                // operations on one channel to affect the identical behaviour of the next channel
                inwritepos = inputBufferWritePosition_;
                outwritepos = outputBufferWritePosition_;
                outreadpos = outputBufferReadPosition_;
                sampsincefft = samplesSinceLastFFT_;
                
                for (int i = 0; i < numSamples; ++i)
                {
                    const DspFloatType in = channelData[i];
                    
                    // Store the next buffered sample in the output. Do this first before anything
                    // changes the output buffer-- we will have at least one FFT size worth of data
                    // stored and ready to go. Set the result to 0 when finished in preparation for the
                    // next overlap/add procedure.
                    channelOut[i] = outputBufferData[outreadpos];
                    outputBufferData[outreadpos] = 0.0;
                    if(++outreadpos >= outputBufferLength_)
                        outreadpos = 0;

                    // Store the current sample in the input buffer, incrementing the write pointer. Also
                    // increment how many samples we've stored since the last transform. If it reaches the
                    // hop size, perform an FFT and any frequency-domain processing.
                    inputBufferData[inwritepos] = in;
                    if (++inwritepos >= inputBufferLength_)
                        inwritepos = 0;
                    if (++sampsincefft >= hopActualSize_)
                    {
                        sampsincefft = 0;
                        
                        // Find the index of the starting sample in the buffer. When the buffer length
                        // is equal to the transform size, this will be the current write position but
                        // this code is more general for larger buffers.
                        int inputBufferStartPosition = (inwritepos + inputBufferLength_
                                                        - fftActualTransformSize_) % inputBufferLength_;
                        
                        // Window the buffer and copy it into the FFT input
                        int inputBufferIndex = inputBufferStartPosition;
                        for(int fftBufferIndex = 0; fftBufferIndex < fftActualTransformSize_; fftBufferIndex++)
                        {
                            // Set real part to windowed signal; imaginary part to 0.
                            fftTimeDomain_[fftBufferIndex][1] = 0.0;
                            if(fftBufferIndex >= windowBufferLength_) // Safety check, in case window isn't ready
                                fftTimeDomain_[fftBufferIndex][0] = 0.0;
                            else
                                fftTimeDomain_[fftBufferIndex][0] = windowBuffer_[fftBufferIndex]
                                                                        * inputBufferData[inputBufferIndex];
                            inputBufferIndex++;
                            if(inputBufferIndex >= inputBufferLength_)
                                inputBufferIndex = 0;
                        }

                        // Perform the FFT on the windowed data, going into the frequency domain.
                        // Result will be in fftFrequencyDomain_
                        fftw_execute(fftForwardPlan_);
                        
                        // ********** PHASE VOCODER PROCESSING GOES HERE **************
                        // This is the place where frequency-domain calculations are made
                        // on the transformed signal. Put the result back into fftFrequencyDomain_
                        // before transforming back.
                        
                        // (⊙_⊙)
                        
                        for (int i = 0; i<fftActualTransformSize_; i++)
                        {
                            // (⊙_⊙) first turn the fft from real-imaginary to amplitude-phase
                            double amplitude = sqrt(fftFrequencyDomain_[i][0]*fftFrequencyDomain_[i][0]+fftFrequencyDomain_[i][1]*fftFrequencyDomain_[i][1]);
                            double phase = atan2(fftFrequencyDomain_[i][1], fftFrequencyDomain_[i][0]);
                            
                            // (⊙_⊙) change the phase 
                            dphi_[i][channel]= /*princArg(phase - phi0_[i][channel]);*/omega_[i]+ princArg(phase - phi0_[i][channel] - omega_[i]);
                            phi0_[i][channel] = phase;
                            psi_[i][channel] = princArg(psi_[i][channel] + dphi_[i][channel]*actualRatio_);
                            
                            // (⊙_⊙) turn back to real-imaginary form
                            fftFrequencyDomain_[i][0] = amplitude*cos(psi_[i][channel]);
                            fftFrequencyDomain_[i][1] = amplitude*sin(psi_[i][channel]);
                                
                        }
                        
                        // In this example, we don't do anything except reconstruct the original
                        // signal to show that the whole infrastructure works.
                        // ************************************************************
                        
                        // Perform the inverse FFT to get back to the time domain. Result wll be
                        // in fftTimeDomain_. If we've done it right (kept the frequency domain
                        // symmetric), the time domain resuld should be strictly real allowing us
                        // to ignore the imaginary part.
                        fftw_execute(fftBackwardPlan_);
                        
                        // (⊙_⊙) gain2 is actually same with the ifft frame except that it is one element longer
                        for (int i = 0;i<fftActualTransformSize_; i++)
                            grain2[i] = fftTimeDomain_[i][0];
                        
                        // (⊙_⊙) resampling using linear interpolation and get grain3
                        for (int i = 0; i<floor(pitchActualShiftRec_*fftActualTransformSize_); i++)
                        {
                            lx = floor(pitchActualShiftRec_*fftActualTransformSize_);
                            x = i*fftActualTransformSize_/lx;
                            ix = floor(x);
                            dx = x - (double)ix;
                            grain3 [i] = grain2[ix]*(1.0 - dx) + grain2[ix+1]*dx;
                        }

                        // Add the result to the output buffer, starting at the current write position
                        // (Output buffer will have been zeroed after reading the last time around)
                        // Output needs to be scaled by the transform size to get back to original amplitude:
                        // this is a property of how fftw is implemented. Scaling will also need to be adjusted
                        // based on hop size to get the same output level (smaller hop size produces more overlap
                        // and hence higher signal level)
                        int outputBufferIndex = outwritepos;
                        
                        
                        // (⊙_⊙) Synthesizing
                        //for(int fftBufferIndex = 0; fftBufferIndex < fftActualTransformSize_; fftBufferIndex++)
                        for(int fftBufferIndex = 0; fftBufferIndex < floor(pitchActualShiftRec_*fftActualTransformSize_); fftBufferIndex++)
                        {
                            if (fftBufferIndex > synthesisWindowBufferLength_)
                                outputBufferData[outputBufferIndex] += 0;
                            else
                                outputBufferData[outputBufferIndex] += grain3[fftBufferIndex] * fftScaleFactor_ *synthesisWindowBuffer_[fftBufferIndex];
                            
                            if(++outputBufferIndex >= outputBufferLength_)
                                outputBufferIndex = 0;
                        }
                        
                        // Advance the write position within the buffer by the hop size
                        outwritepos = (outwritepos + hopActualSize_) % outputBufferLength_;
                    }
                }

                delete[] grain2;
                delete[] grain3;            
            }   
            // we dont actually need two of them
            inputBufferWritePosition_ = inwritepos;
            outputBufferWritePosition_ = outwritepos;
            outputBufferReadPosition_ = outreadpos;
            samplesSinceLastFFT_ = sampsincefft;                 
        }
    };  
}