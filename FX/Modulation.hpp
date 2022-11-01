/* ///////////////////////////////////////////////////////////////////////////////////////////////////////////
 * Modulation
 * ///////////////////////////////////////////////////////////////////////////////////////////////////////////
 * Parametric
 * Circular/Trig
 * Hyperbolic
 * Polar
 * Quadratic
 * Parabolic
 * Bipolar    = M[2] M[0]=Positive,M[1]=Negative
 * Quadrature = W[4] W[0]=I,W[1]=II,W[2]=III,W[3]=4
 * ///////////////////////////////////////////////////////////////////////////////////////////////////////////*/

#pragma once
#include <cmath>

// Modulation
// unipolar +
// unipolar - 
// bipolar [-1..1]
// parametric F(R,X,Y)
// circular F(R,X,Y)
// quadratic ax+by+c=0

// AM * x*y
// AMplus  * (x+1)/2
// AMminus * -(x+1)/2
// AM modulus % fmod(x,y)
// Bipolar Ring *
// Bipolar pos(),neg()
// Bipolar Modulus %

float Modulate(float x, float y) { return x*y; }
float Circulate(float x, float y) { return std::fmod(x,y); }
float LoopModulate(int n, float x, float y) {
    float r = 1;
    for(size_t i = 0; i < n; i++) r *= x*y;
    return r;
}
float LoopCirculate(int n, float x, float y) {
    float r = 1;
    for(size_t i = 0; i < n; i++) r *= std::fmod(x,y);
    return r;
}
float DualModulate1(float x, float y) { return Modulate(Modulate(x,y), Modulate(-x,-y)); }
float DualModulate2(float x, float y) { return Modulate(Modulate(-x,y), Modulate(x,-y)); }
float DualModulate3(float x, float y) { return Modulate(Modulate(x,-y), Modulate(-x,y)); }
float DualModulate4(float x, float y) { return Modulate(Modulate(-x,-y), Modulate(-x,-y)); }

float AM(float x, float y) {
    return std::abs(x) * y;
}

float AMPlus(float x, float y) {
    float t1 = (x+1)/2;
    float t2 = (y+1)/2;
    float r  = t1*t2;    
    return r;
}
float AMMinus(float x, float y) {
    float t1 = -(x+1)/2;
    float t2 = -(y+1)/2;
    float r  = t1*t2;    
    return r;
}
float BipolarPlus(float x, float y) {
    if(x < 0) return -x*y;
    return x*y;
}
float BipolarMinus(float x, float y) {
    if(x > 0) return -x*y;
    return x*y;
}
float RingMod(float x, float y) {
    return x*y;
}
float CircularMod(float x, float y) {
    return std::fmod(x,y);
}
float AbsMod(float x, float y)
{
    return std::abs(x) * std::abs(y);
}

float Shape1(float in, float A=1, float X=1, float Y=1)
{
    return (A*in)*(X*Y);
}
float Shape2(float in, float A=1, float X=1, float Y=1)
{
    return std::fmod(A,in)*(X*Y);
}
float Shape3(float in, float A=1, float X=1, float Y=1)
{
    return (A*in)*std::fmod(X,Y);
}
float Shape4(float in, float A=1, float X=1, float Y=1)
{
    return std::fmod(A,in)*std::fmod(X,Y);
}
float Shape5(float in, float A=1, float X=1, float Y=1)
{
    return std::fmod(std::fmod(A,in),std::fmod(X,Y));
}

float RandomMode(float x, float y) {
    int n = std::rand() % 20;
    switch(n)
    {
        case 0: return AbsMod(x,y);
        case 1: return CircularMod(x,y);
        case 2: return RingMod(x,y);
        case 3: return BipolarPlus(x,y);
        case 4: return BipolarMinus(x,y);
        case 5: return AMPlus(x,y);
        case 6: return AMMinus(x,y);
        case 7: return AM(x,y);
        case 8: return DualModulate1(x,y);
        case 9: return DualModulate2(x,y);
        case 10: return DualModulate3(x,y);
        case 11: return DualModulate4(x,y);
        case 12: return LoopCirculate(1+rand()%6,x,y);
        case 13: return LoopModulate(1+rand()%6,x,y);
        case 14: return Circulate(x,y);
        case 15:
        default: return Modulate(x,y);
    }
}
float RandomShape(float in, float A, float X, float Y)
{
    int n = std::rand() % 5;
    switch(n) {
        case 0: return Shape1(in,A,X,Y);
        case 1: return Shape2(in,A,X,Y);
        case 2: return Shape3(in,A,X,Y);
        case 3: return Shape4(in,A,X,Y);
        case 4: return Shape5(in,A,X,Y);
    }
    return in;
}

