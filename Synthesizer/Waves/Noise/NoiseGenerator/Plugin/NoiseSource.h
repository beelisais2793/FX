/*
  ==============================================================================

    NoiseSource.h
    Created: 28 Sep 2021 11:01:42am
    Author:  John McRae

    Generates pink noise and brown noise.
    
    - Pink Noise -
    
    Implements the McCartney-Voss algorithm based on Burk's implementation
    using the JUCE framework
    http://www.firstpr.com.au/dsp/pink-noise/phil_burk_19990905_patest_pink.c

    Generate Pink Noise using the Richard Voss method.
    Optimization suggested by James McCartney uses a tree
    to select which random value to replace.

    x x x x x x x x x x x x x x x x
     x   x   x   x   x   x   x   x
       x       x       x       x
           x               x
                   x
    
    - Brown Noise -
    
    Integrates a white noise vector to produce brown noise. Integration
    is implented using a right Riemann sum and propagates over a buffer
    to produce new samples.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class PinkNoise {
private:
    // random noise generator from the JUCE library
    juce::Random noiseSrc;
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
    PinkNoise(int numRows = 12) {
        pinkIndex = 0;
        // mask the index so it does not spill outside of the pinkRows vector range
        pinkIndexMask = (1 << numRows) - 1;
        // initialize normalization variable
        pinkNorm = 1.0 / (numRows + 1);
        // in testing, I found it was better to initialize the rows with noise
        // this avoids a climb up to some max value during the first run through the rows
        for (int i = 0; i < numRows; i++)
            pinkRows.push_back(noiseSrc.nextFloat());
        pinkRunSum = noiseSrc.nextFloat();
    }
    
    // generates pink noise one sample at a time
    float generate() {
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
            newRandom = noiseSrc.nextFloat();
            // add the new random number
            pinkRunSum += newRandom;
            // replace the row value at index numZeros with the new random value
            pinkRows[numZeros] = newRandom;
        }

        // add extra white noise value
        sum = pinkRunSum + noiseSrc.nextFloat();

        // scale and return value
        return (sum * pinkNorm);
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
            pinkRows.push_back(noiseSrc.nextFloat());
        pinkRunSum = noiseSrc.nextFloat();
    }
};

class BrownNoise {
private:
    // random noise generator from the JUCE library
    juce::Random noiseSrc;
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
    BrownNoise(int bL = 20000) {
        bLength = bL;
        // intiaize first sample with white noise
        fillBuffer(noiseSrc.nextFloat());
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
            nB.push_back(a*nB.back() + 2*(noiseSrc.nextFloat()) - 1); // leaky integration
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
    
    float generate() {
        // check to see if you hit the end of the buffer, and if yes refill
        if (!(itB < nBn.end())) {
            // use last used sample from unormalized buffer for start of new buffer
            fillBuffer(nB.back() + noiseSrc.nextFloat());
            itB = nBn.begin();
        }
        // get output samples and increment buffer iterator
        op = *itB;
        itB++;
        return op;
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
    
};
