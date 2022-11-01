

//
//  SineTables.h
//  PartialSynth
//
//  Created by Alex on 10/03/2018.
//  Copyright © 2018 Alex Nadzharov. All rights reserved.
//

#ifndef SineTables_h
#define SineTables_h

#include "math.h"

class SineTable {
    static float _v[4096];

public:
    SineTable();
    float operator()(float x);
};

// ---

class CosTable {
    static float _v[4096];
    
public:
    CosTable();
    float operator()(float x);
};

#endif /* SineTables_h */
