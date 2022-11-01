#pragma once

#include <stdlib.h>
#include <cmath>
#include <string.h>
#include <string.h>
#include <vector>

#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    
    enum
    {
        W_BLACKMAN = 1,
        W_HANNING  = 2,
        W_HAMMING  = 3,
        W_KAISER   = 4,
        W_COSROW   = 5,
        W_SQUARE   = 6,
        W_SINC     = 7,
        W_KAISERBETA=8,
    };
    enum
    {
        LPF,
        HPF,
        BPF,
        BEF,
    };

    struct Window
    {
        std::vector<DspFloatType> w;
        #ifdef DSPFLOATDOUBLE
            fv3::firwindow_ fw;
        #else
            fv3::firwindow_f fw;
        #endif

        Window(size_t n)
        {
          w.resize(n);          
        }
        void Square() {
            fw.Square(w.data(),w.size());
        }
        void Hamming() {
            fw.Hamming(w.data(),w.size());
        }
        void Hanning() {
            fw.Hanning(w.data(),w.size());
        }
        void Blackman() {
            fw.Blackman(w.data(),w.size());
        }
        void Kaiser(DspFloatType beta) {
            fw.Kaiser(w.data(),w.size(),beta);
        }        
        void CosROW(DspFloatType fc, DspFloatType alpha) {
            fw.CosROW(w.data(),w.size(),fc,alpha);
        }
        void Sinc(DspFloatType fc) {
            fw.Sinc(w.data(),w.size(),fc);
        }        
        std::vector<DspFloatType> operator * (std::vector<DspFloatType> & v)
        {
            assert(w.size() == v.size());
            std::vector<DspFloatType> r(w.size());
            for(size_t i=0; i < v.size(); i++)
              r[i] = w[i] * v[i];
            return r;
        }        
    };

    struct FIR
    {
        std::vector<DspFloatType> filter;

        FIR(size_t n, int type, long window, DspFloatType fc, DspFloatType fc2=0, DspFloatType p=0)
        {
            filter.resize(n);
            #ifdef DSPFLOATDOUBLE
                fv3::firfilter_ f;
            #else
                fv3::firfilter_f f;
            #endif
            
            switch(type)
            {
            case LPF: f.lpf(filter.data(), n, window,fc,p); break;
            case HPF: f.hpf(filter.data(), n, window,fc,p); break;
            case BPF: f.bpf(filter.data(), n, window,fc,fc2,p); break;
            case BEF: f.bef(filter.data(), n, window,fc,fc2,p); break;
            }
        }
    };
}    