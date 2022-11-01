#ifndef FIR_FILTER_H
#define FIR_FILTER_H

#include <stdlib.h>
#include <cmath>
#include <string.h>
#include <string.h>
#include <vector>

class FIR_filter
{
public:
    FIR_filter( int taps=0, DspFloatType f1=0, DspFloatType f2=0, char* type="", 
                char* window="");
    ~FIR_filter();

    std::vector<DspFloatType> getCoefficients();

    DspFloatType filter(DspFloatType new_sample);

private:
    std::vector<DspFloatType> lowPass_coefficient( int taps, DspFloatType f);
    std::vector<DspFloatType> highPass_coefficient(int taps, DspFloatType f);
    std::vector<DspFloatType> bandPass_coefficient(int taps, DspFloatType f1, DspFloatType f2);
    std::vector<DspFloatType> bandStop_coefficient(int taps, DspFloatType f1, DspFloatType f2);

    std::vector<DspFloatType> window_hammig(int taps);
    std::vector<DspFloatType> window_triangle(int taps);
    std::vector<DspFloatType> window_hanning(int taps);
    std::vector<DspFloatType> window_blackman(int taps);

    std::vector<DspFloatType> h;       // FIR coefficients
    std::vector<DspFloatType> samples; // FIR delay

    int idx;        // Round robin index
    int taps;       // Number of taps of the filter
};


#endif // FIR_FILTER_H
