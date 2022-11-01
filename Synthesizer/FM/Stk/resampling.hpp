//
//  Resampling.hpp
//
//  Created by Raphael Pistachio
//  Copyright (c) 2017 Raphael Pistachio. All rights reserved.
//

#pragma once

#include <algorithm>

#include "AllpassCrossover.hpp"


namespace afx {
namespace emqf {
    

template <typename T, bool UPWARD>
class Resampler {
    // members -------------------------
    Crossover<T, false> xover;
    int numStages;
    int maxNumStages;

    T* tmpBuffer; // temporary buffer for the xover
public:
    Resampler(int numChannels = 0, int numPairs = 0, int maxNumStages = 3, T* tmpBuffer = nullptr)
        :xover(numChannels*maxNumStages, numPairs+1)
        ,numStages(0)
        ,maxNumStages(maxNumStages)
        ,tmpBuffer(tmpBuffer)
    {}
    
    inline void setNumStages(int numStages) {
        if ((this->numStages) != numStages) {
            this->numStages = numStages;
            xover.resetState();
        }
    }
    
    inline void setTempBuffer(T* tmp) {
        tmpBuffer = tmp;
    }
    
    inline void setEMQFHalfbandFilter(T* betas, int numPairs) {
        xover.setEMQFHalfbandFilter(betas, numPairs);
    }
    
    inline int _stuffZeros(int numSamples, T* input, T* output) {
        int newSize = numSamples*2;
        
        int outputOffset = newSize-2;
        for (int i = (numSamples-1); i >= 0; i--) {
            output[outputOffset]   = input[i]*T(2); // compensate energy loss // Lyons' Book: [10.5.1] Time and Frequency Properties of Interpolation
            output[outputOffset+1] = T(0);
            
            outputOffset -= 2;
        }

        return newSize;
    }
    
    // remove each 2nd sample // numSamples must be even, because the decimation is stateless!
    inline int _decimate(int numSamples, T* input, T* output) {
        int newSize = numSamples/2;
        
        int inputIdx = 0;
        for (int i = 0; i < newSize; i++) {
            output[i] = input[inputIdx];
            
            inputIdx += 2;
        }
        
        return newSize;
    }
    
    // returns the new number of samples
    inline int process(int channel, int numSamples, T* samples) {
        int currentNumSamples = numSamples;
        
        for (int i = 0; i < numStages; i++) {
            int mappedFilterChannel = (maxNumStages*channel) + i;
            
            if (!UPWARD) { // downward
                xover.process(mappedFilterChannel, currentNumSamples, samples, samples, tmpBuffer, true);
            }
            
            
            if (UPWARD) {
                currentNumSamples = _stuffZeros(currentNumSamples, samples, samples);
            } else { // downward
                currentNumSamples = _decimate  (currentNumSamples, samples, samples);
            }
            
            
            if (UPWARD) {
                xover.process(mappedFilterChannel, currentNumSamples, samples, samples, tmpBuffer, true);
            }
        }
        
        return currentNumSamples;
    }
};


} // namespace emqf
} // namespace afx