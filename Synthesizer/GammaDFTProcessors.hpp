#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

// some kinds of automatic dft processors

namespace Gamma::DFT
{

    template<typename T>
    std::vector<std::complex<T>> GammaComplexToStdComplex(size_t n, gam::Complex<T> * v)
    {
        std::vector<std::complex<T>> r(n);
        for(size_t i = 0; i < n; i++) 
        {
            r[i].real(v[i].real());
            r[i].imag(v[i].imag());
        }
        return r;
    }
    
    // DFT Processor
    // Signal => Bins
    // x = Bins * Vector
    // IDFT(x) => signal

    struct DFT 
    {
        gam::DFT dft;
        
        std::vector<std::complex<float>> V;
        DFT(const std::vector<std::complex<float>> & x,unsigned winSize, unsigned padSize=0, gam::SpectralType specType=COMPLEX, unsigned numAux = 0)
        : dft(winSize,padSize,specType,numAux),
          V(x)
        {
            
        }
        void ProcessBlock(size_t n, float * input, float * output)
        {
            dft.forward(input);
            gam::Complex<float> * bins = dft.bins();            
            for(size_t i = 0; i < dft.numBins(); i++)
            {
                bins[i].real() *= V[i].real();
                bins[i].imag() *= V[i].imag();
            }
            dft.inverse(output);
        }
    };   

    struct DFTCallback
    {
        gam::DFT dft;
        std::function<void (DFTCallback* c, gam::Complex<float> *)> callback = []((DFTCallback* c, gam::Complex<float> *){};
        
        DFT(unsigned winSize, unsigned padSize=0, gam::SpectralType specType=COMPLEX, unsigned numAux = 0)
        : dft(winSize,padSize,specType,numAux          
        {
            
        }
        void ProcessBlock(size_t n, float * input, float * output)
        {
            dft.forward(input);
            gam::Complex<float> * bins = dft.bins();                        
            callback(this,bins);
            dft.inverse(output);
        }
    };   
}