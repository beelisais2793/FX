#pragma once
#include <cmath>

struct Integrator
{
    float prev;

    Integrator() : prev(0) {}

    T Tick(float I, float A=1,float X=-1, float Y=1)
    {
        float r = A*(I + prev);
        prev = I;
        if(r < X) r = X;
        if(r > Y) r = Y;
        return r;
    }
};


struct Differencer
{
    float prev;

    Differencer() : prev(0) {}

    T Tick(float I, float A=1,float X=-1, float Y=1)
    {
        float r = A*(I - prev);
        prev = I;
        if(r < X) r = X;
        if(r > Y) r = Y;
        return r;
    }
};

struct Modulator
{
    float prev;

    Modulator() : prev(0) {}

    T Tick(float I, float A=1,float X=-1, float Y=1)
    {
        float r = A*(I * prev);
        prev = I;
        if(r < X) r = X;
        if(r > Y) r = Y;
        return r;
    }
};

struct Circulator
{
    float prev;

    Circulator() : prev(0) {}

    T Tick(float I, float A=1,float X=-1, float Y=1)
    {
        float r = A*(std::fmod(I,prev));
        prev = I;
        if(r < X) r = X;
        if(r > Y) r = Y;
        return r;
    }
};