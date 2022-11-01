/*
 * Waveshapers
 * Chebyshev
 * Polynomial
 * Parametric
 * Circular/Trig
 * Hyperbolic
 * Polar
 * Quadratic
 * Parabolic
 * Bipolar    = M[2] M[0]=Positive,M[1]=Negative
 * Quadrature = W[4] W[0]=I,W[1]=II,W[2]=III,W[3]=4
 */

#include <cmath>



DspFloatType sinular(DspFloatType x, DspFloatType K=3) {
    return K*(2/M_PI)*std::sin((2/M_PI)*x*x);
}

DspFloatType asym_tanh(DspFloatType x, DspFloatType dc=0, DspFloatType gain=1.0f) {
    return std::tanh(gain*x+dc);
}
inline DspFloatType wave_clamp(DspFloatType in, DspFloatType min = -1.0f, DspFloatType max = 1.0f) {
    return in < min? min: in > max? max : in;
}
inline DspFloatType biased_clamp(DspFloatType in, DspFloatType x, DspFloatType min = -1.0f, DspFloatType max = 1.0f) {
    return wave_clamp(in+x,min,max);
}

DspFloatType waveshaper(DspFloatType x, DspFloatType a = 1)
{
    return std::tanh(x*(std::abs(x) + a)/((x*x) + (a-1)*std::abs(x) + 1));
}
DspFloatType waveshaper2(DspFloatType x, DspFloatType a = 1)
{
    return std::tanh(x*(std::abs(x) + a)/((x*x*x) + (a-1)*std::abs(x) + 1));
}
DspFloatType waveshaper3(DspFloatType x, DspFloatType a = 1)
{
    return std::tanh(std::fmod(x,(std::abs(x) + a)/((x*x*x) + (a-1)*std::abs(x) + 1)));
}
DspFloatType waveshaper4(DspFloatType x, DspFloatType a = 1)
{
    return std::tanh(x*(std::abs(x) + a)/(std::fmod(x*x, + (a-1)*std::abs(x) + 1)));
}
DspFloatType waveshaper5(DspFloatType x, DspFloatType y, DspFloatType a = 1)
{
    return std::tanh(x*(std::abs(x) + a)/((std::pow(x,y)) + (a-1)*std::abs(x) + 1));
}
DspFloatType waveshaper6(DspFloatType x, DspFloatType a = 1)
{
    return std::tanh(x*(std::abs(x) + a)/((x*x) + std::fmod((a-1),std::abs(x) + 1)));
}
DspFloatType waveshaper7(DspFloatType x, DspFloatType y, DspFloatType a = 1)
{
    return std::tanh(std::pow(x,(std::abs(x) + a))/((std::pow(x,y)) + (a-1)*std::abs(x) + 1));
}

DspFloatType tanh_normalized(DspFloatType x, DspFloatType y=1, DspFloatType a=1) {
    return a*(std::tanh(x)/std::tanh(y));
}
DspFloatType  sinshaper(DspFloatType x, DspFloatType a=1)
{
    DspFloatType z = M_PI * a;
    DspFloatType s = 1/std::sin(z);
    DspFloatType b = 1/a;

    if (x > b)
        return 1;
    else
        return std::tanh(std::sin(z*x)*s);
}
DspFloatType dsf_line(DspFloatType x, DspFloatType amount=1)
{    
    DspFloatType k = 2*amount/(1-amount);
    return std::tanh(((1+k)*x)/((1+k*std::abs(x))));
}
DspFloatType dc_shaper(DspFloatType x, DspFloatType dc, DspFloatType a=1)
{
    DspFloatType r = x + dc;
    return tanh_normalized(r,1,a);
}
	
DspFloatType cubic_waveshaper( DspFloatType in, DspFloatType a = 1 ) {
  return std::tanh(a*(1.5f * in - 0.5f * in *in * in));
}

DspFloatType gloubi_waveshaper(DspFloatType in ,DspFloatType amp=1) {
    const DspFloatType x = in * 0.686306;
    const DspFloatType a = 1 + std::exp (std::sqrt (std::fabs (x)) * -0.75);
    return std::tanh(amp*(std::exp (x) - std::exp (-x * a)) / (std::exp (x) + std::exp (-x)));
}

DspFloatType crusher( DspFloatType input, DspFloatType bits ) {
    DspFloatType step  = std::pow(2,bits);    
    return step * std::floor( input/step + 0.5 );
}
