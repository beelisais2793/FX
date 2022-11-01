#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include "samples/sample_dsp.hpp"


void print_complex(std::vector<std::complex<float>> r)
{
    for(size_t i = 0; i < r.size(); i++) {
        std::cout << r[i] << ",";            
    }
    std::cout << std::endl;
}

void print_complex2(DSP::complex_vector<float> r)
{
    for(size_t i = 0; i < r.size(); i++) {
        std::cout << r[i] << ",";            
    }
    std::cout << std::endl;
}

void print_real(std::vector<std::complex<float>> r)
{
    for(size_t i = 0; i < r.size(); i++) {
        std::cout << abs(r[i]) << ",";            
    }
    std::cout << std::endl;
}
void print_vector(std::vector<float> v)
{
    for(size_t i = 0; i < v.size(); i++) {
        std::cout << v[i] << ",";            
    }
    std::cout << std::endl;
}
void print_vector2(sample_vector<float> v)
{
    for(size_t i = 0; i < v.size(); i++) {
        std::cout << v[i] << ",";            
    }
    std::cout << std::endl;
}
int main()
{
    sample_vector<float> test2 = {1,2,3,4,0,0,0,0,0,0,0,0,5,6,7,8};
    sample_vector<float> temp1;
    DSP::complex_vector<float> c;
    DSP::R2CF rfft(16);
    rfft.set_input(test2);
    rfft.Execute();
    c = rfft.get_output();
    for(size_t i = 0; i < c.size(); i++)
        c[i] /= test2.size();
    print_complex2(c);
    DSP::C2RF ifft(16);
    ifft.set_input(c);
    ifft.Execute();
    test2 = ifft.get_output();
    print_vector2(test2);
}