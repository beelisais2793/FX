#pragma once 
#include "librosa.h"
#include <std.h>

namespace STFT {
using StdComplex = std::complex<float>;

typedef std::vector<std::vector<StdComplex>> ComplexMatrix ;
typedef std::vector<StdComplex>              ComplexVector;
typedef std::vector<std::vector<float>>      FloatMatrix;

ComplexMatrix stft( StdVector<float> &x,
                    int n_fft, int n_hop,
                    const std::string &win, bool center,
                    const std::string &mode)
{
    librosa::Feature f;
    ComplexMatrix r = f.stft(x.vector,n_fft,n_hop, win, center, mode);
    return r;
}            

FloatMatrix melspectrogram(StdVector<float> &x, int sr, 
                                int n_fft, int n_hop, const std::string &win, bool center, const std::string &mode,
                                float power, int n_mels, int fmin, int fmax)
{
    librosa::Feature f;
    FloatMatrix r = f.melspectrogram(x.vector,sr,n_fft,n_hop,win,center,mode,power,n_mels,fmin,fmax) ;
    return r;
}

FloatMatrix  mfcc(StdVector<float> &x, int sr,
                        int n_fft, int n_hop, const std::string &win, bool center, const std::string &mode,
                        float power, int n_mels, int fmin, int fmax,
                        int n_mfcc, bool norm, int type)
{
    librosa::Feature f;
    FloatMatrix r = f.mfcc(x.vector,sr,n_fft,n_hop,win,center,mode,power,n_mels,fmin,fmax,n_mfcc,norm,type);
    return r;
}                                              

}