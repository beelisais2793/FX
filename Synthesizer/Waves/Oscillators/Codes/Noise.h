#pragma once 

#include <stdint.h>
#include <vector>
#include <queue>
#include "std.h"
#include <algorithm>

using Std::StdRandomUniform;

namespace SoundWave {

struct GaussianWhiteNoise {

    uint32_t g_randn_x;
    uint32_t g_randn_y;
    uint32_t g_randn_z;
    uint32_t g_randn_w;

    GaussianWhiteNoise() {
        g_randn_x = 123456789;
        g_randn_y = 362436069;
        g_randn_z = 521288629;
        g_randn_w = 88675123;
    }


    double Generate() {
        uint32_t t;
        t = g_randn_x ^ (g_randn_x << 11);
        g_randn_x = g_randn_y;
        g_randn_y = g_randn_z;
        g_randn_z = g_randn_w;
        g_randn_w = (g_randn_w ^ (g_randn_w >> 19)) ^ (t ^ (t >> 8));

        uint32_t tmp = g_randn_w >> 4;
        for (int i = 0; i < 11; ++i) {
            t = g_randn_x ^ (g_randn_x << 11);
            g_randn_x = g_randn_y;
            g_randn_y = g_randn_z;
            g_randn_z = g_randn_w;
            g_randn_w = (g_randn_w ^ (g_randn_w >> 19)) ^ (t ^ (t >> 8));
            tmp += g_randn_w >> 4;
        }
        return tmp / 268435456.0 - 6.0;
    }

    double Tick() {
        return Generate();
    }
    void Process(size_t n, std::vector<float> & s) {
        s.resize(n);
        for(size_t i = 0; i < n; i++) s[i] = Generate();        
    }
    void Process(size_t n, float * in, float * s) {        
        for(size_t i = 0; i < n; i++) s[i] = in[i]*Generate();        
    }
    void Process(float * s, size_t n) {        
        for(size_t i = 0; i < n; i++) s[i] = Generate();        
    }
};


struct ModifiedVelvetNoise {

    int base_fs;
    int length_list[2];
    int current_position;
    int chunk_length;
    int count;
    uint64_t g_randi_mvn;
    

    ModifiedVelvetNoise(int fs = 48000) {
        base_fs = fs;
        length_list[0] = 152;
        length_list[1] = 400;
        current_position = 152;
        chunk_length = 152;
        count = 0;
        g_randi_mvn = 88172645463325252ULL;        
    }
    // Note: Behavior of this function is different from that of MATLAB.
    // MATLAB: 1 ... imax
    // C++   : 0 ... imax - 1
    int randi(int imax) {
        g_randi_mvn = g_randi_mvn ^ (g_randi_mvn << 7);
        g_randi_mvn = g_randi_mvn ^ (g_randi_mvn >> 9);
        return g_randi_mvn % imax;
    }

    void UpdateChunk(int *chunk, int *amplitude) {
        // Determination of the chunk_length.
        // td = 4;
        chunk_length = length_list[randi(2)];
        int number_of_pulses = chunk_length / 4;

        // Amplitude of pulse is 2 or -2.
        for (int i = 0; i < number_of_pulses / 2; ++i) amplitude[i] = 2;
        for (int i = number_of_pulses / 2; i < number_of_pulses; ++i)
            amplitude[i] = -2;

        // Randomization of the amplitudes
        int tmp_index, tmp;
        for (int i = 0; i < number_of_pulses; ++i) {
            tmp_index = randi(number_of_pulses);
            tmp = amplitude[tmp_index];
            amplitude[tmp_index] = amplitude[i];
            amplitude[i] = tmp;
        }
        // Temporal positions with an amplitude other than zero are determined.
        for (int i = 0; i < number_of_pulses; ++i) chunk[i] = i * 4 + randi(4);

        // Reset the current_position.
        current_position = 0;
        count = 0;
    }



    void ReseedMVN() {
        g_randi_mvn = 88172645463325252ULL;
        base_fs = 48000;
        length_list[0] = 152;
        length_list[1] = 400;
        current_position = 152;
        chunk_length = 152;
        count = 0;
    }

    void SetSamplingFrequencyMVN(int fs) {
        if (base_fs == fs) return;
        g_randi_mvn = 88172645463325252ULL;
        base_fs = fs;
        length_list[0] = static_cast<int>(152.0 * base_fs / 384000.0) * 8;
        length_list[1] = static_cast<int>(400.0 * base_fs / 384000.0) * 8;
        current_position = length_list[0];
        chunk_length = length_list[0];
        count = 0;
    }

    int Generate() {
        // initialization
        static int chunk[200];
        static int amplitude[200];

        if (current_position == chunk_length)
            UpdateChunk(chunk, amplitude);

        return chunk[count] == current_position++ ? amplitude[count++] : 0;
    }
    
    void Process(size_t n, float * input, float * output) {
        for(size_t i = 0; i < n; i++) output[i] = input[i]*Generate();
        int maxi = *std::max_element(&output[0],&output[n-1]);
        if(maxi == 0) return;
        for(size_t i = 0; i < n; i++) output[i] /= maxi;
    }
    void Process(float * output, size_t n) {
        for(size_t i = 0; i < n; i++) output[i] = Generate();
        int maxi = *std::max_element(&output[0],&output[n-1]);
        if(maxi == 0) return;
        for(size_t i = 0; i < n; i++) output[i] /= maxi;
    }
};


class PinkNoiseGenerator {
private:
    // random noise generator from the JUCE library
    StdRandomUniform noiseSrc;
    // each row effectively holds an independent random number generator
    std::vector<float> pinkRows;
    // running sum for noise output
    float pinkRunSum;
    // the column index, incremented each sample
    int pinkIndex;
    // the row mask, which ensures that the index of the pinkRows vector is never exceeded
    int pinkIndexMask;
    // used to normalize the noise at the output
    float pinkNorm;

public:
    // constructor, overload to initialize with 12 rows, which worked out to be a
    // good number when testing in Octave
    PinkNoiseGenerator(int numRows = 12) {
        pinkIndex = 0;
        // mask the index so it does not spill outside of the pinkRows vector range
        pinkIndexMask = (1 << numRows) - 1;
        // initialize normalization variable
        pinkNorm = 1.0 / (numRows + 1);
        // in testing, I found it was better to initialize the rows with noise
        // this avoids a climb up to some max value during the first run through the rows
        for (int i = 0; i < numRows; i++)
            pinkRows.push_back(noiseSrc.random());
        pinkRunSum = noiseSrc.random();
    }
    
    // generates pink noise one sample at a time
    float Generate() {
        float newRandom, sum;

        // increment and mask index
        pinkIndex = (pinkIndex + 1) & pinkIndexMask;

        // ensure pink index is not zero, if it is, do not update any of the random vals
        if (pinkIndex != 0) {
            // determine the number of trailing zeros in pinkIndex
            int numZeros = 0;
            int n = pinkIndex;
            while ((n & 1) == 0) {
                // bit shift until you run out of trailing zeros
                n = n >> 1;
                numZeros++;
            }
            // McCARTNEY-VOSS ALGORITHM
            // subtract previous value from running sum
            pinkRunSum -= pinkRows[numZeros];
            // generate a new random number
            newRandom = noiseSrc.random();
            // add the new random number
            pinkRunSum += newRandom;
            // replace the row value at index numZeros with the new random value
            pinkRows[numZeros] = newRandom;
        }

        // add extra white noise value
        sum = pinkRunSum + noiseSrc.random();

        // scale and return value
        return (sum * pinkNorm);
    }

    float Tick() {
        return Generate();
    }
    void Process(size_t n, float * input, float * output) {
        for(size_t i = 0; i < n; i++) output[i] = Generate();
    }

    // Changes the number of noise generating rows
    // Note that this overrides the initialization found in the constructor
    // AS WELL AS the pinkRows vector. Therefore, it is advised that this
    // function only be called on initialization
    void setRows(int newRows) {
        // reset pinkIndex
        pinkIndex = 0;
        pinkIndexMask = (1 << newRows) - 1;
        pinkNorm = 1.0 / (newRows + 1);
        // clear the pinkRows vector
        pinkRows.clear();
        // reinitialize the pinkRows vector
        for (int i = 0; i < newRows; i++)
            pinkRows.push_back(noiseSrc.random());
        pinkRunSum = noiseSrc.random();
    }
};

class BrownNoiseGenerator {
private:
    // random noise generator from the JUCE library
    StdRandomUniform noiseSrc;
    // buffer vector of unormalized brown noise
    // (vector instead of queue so that we can use the .begin() and .end() functions)
    std::vector<float> nB, nBn;
    // iterator for the brown noise vectors
    std::vector<float>::iterator itB;
    // max and min values of the brown noise buffer, used in noramlization
    float maxB, minB;
    // op: raw output sample of brown noise gen, ip: input to sample buffer function
    float op;
    int bLength;
    // leaky integrator constant http://sepwww.stanford.edu/sep/prof/pvi/zp/paper_html/node2.html
    float a = 0.95;
    
public:
    // constructor
    BrownNoiseGenerator(int bL = 20000) {
        bLength = bL;
        // intiaize first sample with white noise
        fillBuffer(noiseSrc.random());
        itB = nBn.begin();
    }
    
    // input is the first sample, or seed sample
    void fillBuffer(float input) {
        // clear contents
        nB.clear(); nBn.clear();
        // add first sample to buffer
        nB.push_back(input);
        
        // populate brown noise buffer
        for (int i = 0; i < bLength; i++) {
            nB.push_back(a*nB.back() + 2*(noiseSrc.random()) - 1); // leaky integration
        }
        
        // NORMALIZE and scale by 0.9 to avoid clipping
        // max/min vals for normalization
        maxB = *std::max_element(nB.begin(), nB.end());
        minB = *std::min_element(nB.begin(), nB.end());
        // nBn = 2*(nB - minB)/(maxB - minB) - 1;
        for (auto it = nB.begin(); it != nB.end(); it++) {
            nBn.push_back(0.8*(2*(*it - minB)/(maxB - minB) - 1));
        }
    }
    
    float Generate() {
        // check to see if you hit the end of the buffer, and if yes refill
        if (!(itB < nBn.end())) {
            // use last used sample from unormalized buffer for start of new buffer
            fillBuffer(nB.back() + noiseSrc.random());
            itB = nBn.begin();
        }
        // get output samples and increment buffer iterator
        op = *itB;
        itB++;
        return op;
    }
    float Tick() {
        return Generate();
    }
    void Process(size_t n, float * input, float * output) {
        for(size_t i = 0; i < n; i++) output[i] = Generate();
    }
};

class NoiseFilter {
private:
    // DC blocker variables
    float R;
    float y = 0, xm1 = 0, ym1 = 0;
    
    // smoothing filter variables
    // N - smooth length
    // i - counter for initialization
    int N, i = 0;
    // opS - output of smoothing filter
    // sub - subsitution : x[n] - x[n - N]
    // acc - accumulator
    float sub, acc;
    // mem - memory for x[i - N] values
    std::queue<float> mem;
    
    // use to clear memory queue on changes to N
    void clear( std::queue<float> &q )
    {
       std::queue<float> empty;
       std::swap( q, empty );
    }
    
public:
    // constructor
    NoiseFilter(float R_in = 0.99, int N_in = 4) : R(R_in), N(N_in) {}
    
    // DC blocking filter
    // https://www.dsprelated.com/freebooks/filters/DC_Blocker.html
    float dc_blocking_filter (float ip) {
        y = ip - xm1 + R * ym1;
        xm1 = ip;
        ym1 = y;
        return y;
    }
    
    // moving average/smoothing filter
    // https://zipcpu.com/dsp/2017/10/16/boxcar.html
    float smoothing_filter (float ip) {
        // init
        if (i < N) {
            mem.push(ip); // load memory buffer
            //opS = ip;
            i++;
            return ip;
        }
        else {
            // update sample memory buffer
            mem.push(ip);
            // filter
            //sub = ip - mem;
            sub = ip - mem.front();
            acc += sub;
            // trim sample memory buffer
            mem.pop();
        }
        // return avg/N
        return acc/N;
    }
    
    // sets for UI control
    void setDCfiltConst (float sliderVal) { R = (sliderVal < 1.0) ? sliderVal : R; } // if ip < 1, pass to R, else leave it
    void setSmoothLength (int sliderVal) {
        N = sliderVal;
        // reset memory buffer counter
        i = 0;
        // clear the memory buffer
        clear(mem);
    }
    
    float Tick(float i) {
        return smoothing_filter(i);
    }
    void Process(size_t n, float * input, float * output) {
        for(size_t i = 0; i < n; i++) output[i] = Tick(input[i]);
    }
    void Process(size_t n, float * output) {
        for(size_t i = 0; i < n; i++) output[i] = Tick(output[i]);
    }
};

}
