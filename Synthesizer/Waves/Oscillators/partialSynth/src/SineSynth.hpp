//
//  SineSynth.hpp
//  PartialSynth
//
//  Created by Alex on 04/02/2018.
//  Copyright © 2018 Alex Nadzharov. All rights reserved.
//

#ifndef SineSynth_hpp
#define SineSynth_hpp

#include <stdio.h>

#include "SinePartial.hpp"
#include <vector>

static const int maxNumberOfPartials = 4096;

typedef struct SinePartialData {
    float* freq = 0;
    float* amp = 0;
    size_t size = 0;

    explicit SinePartialData(size_t s)
    {
        freq = new float[s];
        amp = new float[s];
        size = s;
    }
    SinePartialData(SinePartialData& d)
    {
        freq = new float[d.size];
        amp = new float[d.size];
        size = d.size;

        memcpy(freq, d.freq, size);
        memcpy(amp, d.amp, size);

    }

    ~SinePartialData()
    {
        if (freq)
            delete[] freq;
        if (amp)
            delete[] amp;
    }

} SinePartialData;

class SineSynth {
    std::vector<SinePartial*> partials;

    const std::vector<int> freePartialIndices()
    {
        std::vector<int> ret;
        for (int i = 0; i < maxNumberOfPartials; i++) {
            if (!partials[i]->busy())
                ret.push_back(i);
        }
        return ret;
    }

    const std::vector<SinePartial*> activePartials()
    {
        std::vector<SinePartial*> ret;
        for (int i = 0; i < maxNumberOfPartials; i++) {
            if (partials[i]->busy())
                ret.push_back(partials[i]);
        }
        return ret;
    }

public:
    SineSynth()
    {
        for (int i = 0; i < maxNumberOfPartials; i++)
            partials.push_back(new SinePartial);
    }
    void setData(SinePartialData& data)
    {
        muteActivePartials();

        std::vector<int> freeIdx = freePartialIndices();

        int fi = (int)freeIdx.size();
        int s = (data.size < fi) ? (int)data.size : fi;

        for (int i = 0; i < s; i++) {
            int idx = freeIdx[i];
            if (idx > 1024)
                continue;
            partials[idx]->set<SinePartial::pAmp>(data.amp[i]);
            partials[idx]->set<SinePartial::pFreq>(data.freq[i]);
            partials[idx]->setBusy(true);
        }
    };

    void muteActivePartials()
    {
        for (int i = 0; i < maxNumberOfPartials; i++) {
            if (partials[i]->busy()) {
                partials[i]->set<SinePartial::pAmp>(0);
            }
        }
    }

    void process(float* buffer, size_t s)
    {

        for (int i = 0; i < s; i++)
            buffer[i] = 0.;

        for (SinePartial* sp : activePartials())
            sp->process(s, buffer);
    };
};

#endif /* SineSynth_hpp */
