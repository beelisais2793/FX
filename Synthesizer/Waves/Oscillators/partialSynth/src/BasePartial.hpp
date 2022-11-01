//
//  BasePartial.hpp
//  PartialSynth
//
//  Created by Alex on 04/02/2018.
//  Copyright © 2018 Alex Nadzharov. All rights reserved.
//

#ifndef BasePartial_hpp
#define BasePartial_hpp

#include <stdio.h>

#include "PartialParameter.hpp"

class BasePartial {
protected:
    bool _busy = false;
    float _sampleRate = 44100;
    
    PartialParameter _param[3];
    
    template<int I>
    inline
    PartialParameter& par()
    {
        return _param[I];
    };

public:
    void process(size_t s) {}
    
    template <int I>
    inline
    void set(float f)
    {
        par<I>().setValue(f);
    };
    
    template <int I>
    inline
    float value()
    {
        return par<I>().value();
    };

    template <int I>
    inline
    void setSmooth(float f)
    {
        par<I>().setSmooth(f);
    };

    bool busy() { return _busy; }
    void setBusy(bool b) { _busy = b; }
    
    void setSampleRate(float sr){_sampleRate = sr;}
};

#endif /* BasePartial_hpp */
