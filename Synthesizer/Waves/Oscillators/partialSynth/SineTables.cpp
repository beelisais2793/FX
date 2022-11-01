//
//  SineTables.cpp
//  PartialSynth
//
//  Created by Alex on 10/03/2018.
//  Copyright © 2018 Alex Nadzharov. All rights reserved.
//

#include <stdio.h>

#include "SineTables.h"

float SineTable::_v[4096];
float CosTable::_v[4096];

SineTable::SineTable()
{
    for (int i = 0; i < 4096; i++) {
        _v[i] = sinf(float(i) / 4096 * 3.1415926 * 2);
    }
}

float SineTable::operator()(float x)
{
    int idx = x / 3.1415926 * 2048;
    idx = (idx > 0) ? (idx % 4096) : 4096 - (-idx % 4096);
    return _v[idx];
};

CosTable::CosTable()
{
    for (int i = 0; i < 4096; i++) {
        _v[i] = cosf(float(i) / 4096 * 3.1415926 * 2);
    }
}

float CosTable::operator()(float x)
{
    int idx = x / 3.1415926 * 2048;
    idx = (idx > 0) ? (idx % 4096) : 4096 - (-idx % 4096);
    return _v[idx];
};
