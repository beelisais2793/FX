//
//  SinePartial.hpp
//  PartialSynth
//
//  Created by Alex on 04/02/2018.
//  Copyright © 2018 Alex Nadzharov. All rights reserved.
//

#ifndef SinePartial_hpp
#define SinePartial_hpp

#include "BasePartial.hpp"
#include "math.h"
#include <cmath>
#include <stdio.h>
#include <stdlib.h>

class SinePartial : public BasePartial {

    unsigned long _counter = 0;

    //inline
    float sinOsc(float f)
    {
        float ret = sinf(float(_counter) / (_sampleRate / f) * 3.1415 * 2);

        _counter++;

        return ret;
    }

public:
    SinePartial()
    {
        par<pFreq>().setSmooth(0);
    }

    void process(size_t s, float* buffer)
    {
        float* b = buffer;
        while (s--) {
            par<pFreq>().process();
            par<pAmp>().process();

            *b += sinOsc(par<pFreq>().value()) * par<pAmp>().value();
            b++;

            if (par<pAmp>().value() < 0.000001)
                _busy = false;
        }
    }

    static const int pFreq = 0;
    static const int pAmp = 1;
};

#endif /* SinePartial_hpp */

