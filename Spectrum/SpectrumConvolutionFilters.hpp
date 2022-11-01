#pragma once

#include "FFT/vcvfft.hpp"

namespace Spectrum::ConvolutionFilters
{
    struct MonoFFTConvolver : public MonoFXProcessor
    {
        fftconvolver::FFTConvolver convolver;

        MonoFFTConvolver(const char * impulse_file)    
        : MonoFXProcessor()
        {
            //
            sample_vector<float> lexicon;
            SndFileReaderFloat sndfile(impulse_file);
            
            std::cout << sndfile.channels() << std::endl;
            std::cout << sndfile.frames() << std::endl;
            std::cout << sndfile.samplerate() << std::endl;
            std::vector<float> channelv(sndfile.frames()*sndfile.channels());
                        
            lexicon.resize(sndfile.frames());
            sndfile >> channelv;

            for(size_t i = 0; i < sndfile.frames(); i++)
            {        
                lexicon[i] = channelv[i*sndfile.channels()];                        
            }

            convolver.init(BufferSize,lexicon.data(),lexicon.size());            
            
        }
        ~MonoFFTConvolver()
        {

        }
        void ProcessBlock(size_t framesPerBuffer, float * in, float * out)
        {
            convolver.process(in,out,framesPerBuffer);            
        }
        void InplaceProcess(size_t n, float * buffer)
        {
            ProcessBlock(n,buffer,buffer);
        }
    };
    struct StereoFFTConvolver : public StereoFXProcessor
    {
        fftconvolver::FFTConvolver convolverL,convolverR;

        StereoFFTConvolver(const char * impulse_file) : StereoFXProcessor() 
        {
            //
            sample_vector<float> lexiconL,lexiconR;
            SndFileReaderFloat sndfile(impulse_file);
            
            //std::cout << sndfile.channels() << std::endl;
            //std::cout << sndfile.frames() << std::endl;
            //std::cout << sndfile.samplerate() << std::endl;
            assert(sndfile.channels()==2);
            std::vector<float> channelv(sndfile.frames()*sndfile.channels());
            
            sndfile >> channelv;

            lexiconL.resize(sndfile.frames());
            lexiconR.resize(sndfile.frames());

            for(size_t i = 0; i < sndfile.frames(); i++)
            {        
                lexiconL[i] = channelv[i*2];        
                lexiconR[i] = channelv[i*2+1];
            }

            convolverL.init(BufferSize,lexiconL.data(),lexiconL.size());
            convolverR.init(BufferSize,lexiconR.data(),lexiconR.size());
            
        }
        ~StereoFFTConvolver()
        {

        }
        void ProcessBlock(size_t framesPerBuffer, float ** in, float ** out)
        {
            convolverL.process(in[0],out[0],framesPerBuffer);
            convolverR.process(in[1],out[1],framesPerBuffer);
        }
        void InplaceProcess(size_t n, float ** buffer)
        {
            ProcessBlock(n,buffer,buffer);
        }
    };

    struct MonoPffastConvolver : public MonoFXProcessor
    {
        VCVFFT::Convolution * conv;
                
        MonoPffastConvolver(const char * impulse_file) : MonoFXProcessor()
        {
            //
            sample_vector<float> lexicon;
            SndFileReaderFloat sndfile(impulse_file);
            
            //std::cout << sndfile.channels() << std::endl;
            //std::cout << sndfile.frames() << std::endl;
            //std::cout << sndfile.samplerate() << std::endl;
            std::vector<float> channelv(sndfile.frames()*sndfile.channels());
            
            sndfile >> channelv;

            lexicon.resize(sndfile.frames());
            
            for(size_t i = 0; i < sndfile.frames(); i++)
            {        
                lexicon[i] = channelv[i*sndfile.channels()];                        
            }            
            conv = new VCVFFT::Convolution(lexicon.data(), lexicon.size(),BufferSize);        
        }
        ~MonoPffastConvolver()
        {
            if(conv) delete conv;
        }
        void ProcessBlock(size_t framesPerBuffer, float * in, float * out)
        {
            conv->Process(framesPerBuffer,in,out);        
        }
        void InplaceProcess(size_t n, float * buffer)
        {
            ProcessBlock(n,buffer,buffer);
        }
    };
    struct StereoPffastConvolver : public StereoFXProcessor
    {
        VCVFFT::StereoConvolution * conv;
                
        StereoPffastConvolver(const char * impulse_file)  : StereoFXProcessor() 
        {
            //
            sample_vector<float> lexiconL,lexiconR;
            SndFileReaderFloat sndfile(impulse_file);
            
            ///std::cout << sndfile.channels() << std::endl;
            ///std::cout << sndfile.frames() << std::endl;
            //std::cout << sndfile.samplerate() << std::endl;
            assert(sndfile.channels()==2);
            std::vector<float> channelv(sndfile.frames()*sndfile.channels());
            
            sndfile >> channelv;

            lexiconL.resize(sndfile.frames());
            lexiconR.resize(sndfile.frames());

            for(size_t i = 0; i < sndfile.frames(); i++)
            {        
                lexiconL[i] = channelv[i*2];        
                lexiconR[i] = channelv[i*2+1];
            }

            //convolverL.init(BufferSize,lexiconL.data(),lexiconL.size());
            //convolverR.init(BufferSize,lexiconR.data(),lexiconR.size());
            conv = new VCVFFT::StereoConvolution(lexiconL.data(), lexiconR.data(), lexiconL.size(),BufferSize);        
        }
        ~StereoPffastConvolver()
        {
            if(conv) delete conv;
        }
        void ProcessBlock(size_t framesPerBuffer, float ** in, float ** out)
        {
            conv->Process(framesPerBuffer,in,out);        
        }
        void InplaceProcess(size_t n, float ** buffer)
        {
            ProcessBlock(n,buffer,buffer);
        }
    };
}
