#pragma once

namespace Spectrum::FFT
{
    /////////////////////////////////////////////////////////////////////////////////////
    // Pad FFT with zeros
    /////////////////////////////////////////////////////////////////////////////////////
    struct FFTPadProcessor
    {
        sample_vector<float> buffer;    
        int32_t bufptr;
        uint32_t blocksize;
        Spectrum::R2CF forward;
        Spectrum::C2RF inverse;
        FFTPadProcessor(uint32_t blocks, uint32_t zero=2) {
            
            bufptr = 0;        
            blocksize = blocks;        
            buffer.resize(blocksize*2*zero);
            forward.init(blocksize*2*zero);
            inverse.init(blocksize*2*zero);        
            zeros(buffer);
        }
        void ProcessBlock(size_t hopsize, float * input, float * output) {
            Spectrum::complex_vector<float> x;
            sample_vector<float> out;                
            memcpy(buffer.data(),input,hopsize*sizeof(float));
            forward.set_input(buffer);
            forward.Execute();
            forward.normalize();
            x = forward.get_output();
            inverse.set_input(x);
            inverse.Execute();
            out = inverse.get_output();     

            for(size_t i = 0; i < hopsize; i++)
            {            
                output[i]  = out[i];
            }                                                         
        }
    };
    struct FFTWaveTableGenerator
    {
        // number of harmonics for note 
        // 0 = DC
        // 44100/4096 = 10.766
        // 1 = f0
        // 2 = f1
        // 4 = f2
        // 8 = f3
    };

    
    // experiment that seems to work
    struct FFTOverlap
    {
        Spectrum::complex_vector<float> buffer,ola;
        sample_vector<float> temp;
        Spectrum::Rectangle<float> window;
        Spectrum::R2CF forward;
        Spectrum::C2RF inverse;

        FFTOverlap(size_t size=1024) : window(size) {                
            ola.resize(size);
            buffer.resize(size);
            temp.resize(size);
            forward.init(size);
            inverse.init(size);
            zeros(buffer);
            zeros(temp);
            memset(ola.data(),0,ola.size()*sizeof(std::complex<float>));
        }
        void ProcessBlock(size_t n, float * input, float * output)
        {
            memcpy(temp.data(),input,n*sizeof(float));
            for(size_t i = 0; i < temp.size(); i++)
            {            
                temp[i] *= window[i];            
            }        
            forward.set_input(temp);        
            forward.Execute();
            forward.normalize();
            forward.get_output(buffer);                        
            for(size_t i = n; i < buffer.size()-n; i++)
            {
                std::complex<float> x = ola[i];
                ola[i-n] = buffer[i];
                buffer[i] += x;
            }
            inverse.set_input(buffer);
            inverse.Execute();
            inverse.get_output(temp.data());
            for(size_t i = 0; i < n; i++)       
            {     
                output[i] = temp[i];                              
                ola[i+n] = buffer[i+n];
            }
        }
    };

    
    struct FFTOverlapAdd
    {
        Spectrum::complex_vector<float> buffer;
        sample_vector<float> temp,ola;
        Spectrum::Rectangle<float> window;
        Spectrum::R2CF forward;
        Spectrum::C2RF inverse;

        FFTOverlapAdd(size_t size=1024) : window(size) {                
            ola.resize(size);
            buffer.resize(size);
            temp.resize(size);
            forward.init(size);
            inverse.init(size);
            zeros(buffer);
            zeros(temp);
            memset(ola.data(),0,ola.size()*sizeof(std::complex<float>));
        }
        void ProcessBlock(size_t n, float * input, float * output)
        {
            memcpy(temp.data(),input,n*sizeof(float));
            for(size_t i = 0; i < temp.size(); i++)
            {            
                temp[i] *= window[i];            
            }        
            forward.set_input(temp);        
            forward.Execute();
            forward.normalize();
            forward.get_output(buffer);                                    
            inverse.set_input(buffer);
            inverse.Execute();
            inverse.get_output(temp.data());
            for(size_t i = 0; i < n; i++)       
            {     
                output[i] = temp[i] + ola[i];                                              
            }
            for(size_t i = n; i < ola.size(); i++)
                ola[i] = temp[i];
            for(size_t i = ola.size()-n; i < ola.size(); i++)
                ola[i] = 0;
        }
    };
}