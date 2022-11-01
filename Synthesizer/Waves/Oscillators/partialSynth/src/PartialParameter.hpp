//
//  PartialParameter.hpp
//  PartialSynth
//
//  Created by Alex on 08/02/2018.
//  Copyright © 2018 Alex Nadzharov. All rights reserved.
//

#ifndef PartialParameter_hpp
#define PartialParameter_hpp

#include <stdio.h>

class PartialParameter {
    float _value = 0;
    float _previous = 0;
    float _smooth = 0.998;
    float _nsmooth = .002;

public:
    inline
    void process()
    {
        _previous = _previous * _smooth + _value * _nsmooth;
    }

    inline
    void process(size_t s)
    {
        while (s--) {
            process();
        }
    }

    inline
    void setValue(float v)
    {
        _value = v;
        process();
    }
    
    inline
    float value() { return _previous; }
    
    inline float* valuePtr() {return &_previous;}

    inline
    void setSmooth(float v)
    {
        _smooth = v;
        _nsmooth = 1 - v;
    }
    
    inline
    float smooth() { return _smooth; }
};

#endif /* PartialParameter_hpp */
