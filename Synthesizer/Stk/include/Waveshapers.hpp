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


float sinular(float x, float K=3) {
    return K*(2/M_PI)*std::sin((2/M_PI)*x*x)
}
float asym_tanh(float x, float dc=0, float gain=1.0f) {
    return std::tanh(gain*x+dc);
}
inline float wave_clamp(float in, float min = -1.0f, float max = 1.0f) {
    return in < min? min: in > max? max : in;
}
inline float biased_clamp(float in, float x, float min = -1.0f, float max = 1.0f) {
    return wave_clamp(in+x,min,max);
}


float chebyshev(float x, float *A, int order)
{
   // To = 1
   // T1 = x
   // Tn = 2.x.Tn-1 - Tn-2
   // out = sum(Ai*Ti(x)) , i C {1,..,order}
   float Tn_2 = 1.0f;
   float Tn_1 = x;
   float Tn;
   float out = A[0]*Tn_1;

   for(int n=2;n<=order;n++)
   {
      Tn    =   2.0f*x*Tn_1 - Tn_2;
      out    +=   A[n-1]*Tn;
      Tn_2 =   Tn_1;
      Tn_1 =  Tn;
   }
   return out;
}

float chebyshev2(float x, int order, float A = 1.0f, float dim = 0.5f)
{
   // To = 1
   // T1 = x
   // Tn = 2.x.Tn-1 - Tn-2
   // out = sum(Ai*Ti(x)) , i C {1,..,order}
   float Tn_2 = 1.0f;
   float Tn_1 = x;
   float Tn;
   float out = A*Tn_1;   
   for(int n=2;n<=order;n++)
   {
      Tn    =   2.0f*x*Tn_1 - Tn_2;
      out    +=   A*Tn;
      Tn_2 =   Tn_1;
      Tn_1 =  Tn;
      A *= dim;
   }
   return out;
}

float chebyshev3(float x, int order, float A = 1.0f, float X = 0.5f, float Y = 1.0f)
{
   // To = 1
   // T1 = x
   // Tn = 2.x.Tn-1 - Tn-2
   // out = sum(Ai*Ti(x)) , i C {1,..,order}
   float Tn_2 = 1.0f;
   float Tn_1 = x;
   float Tn;
   float out = A*Tn_1;   
   for(int n=2;n<=order;n++)
   {
      Tn    =   2.0f*x*Tn_1 - Tn_2;
      out    +=   A*Tn;
      Tn_2 =   Tn_1;
      Tn_1 =  Tn;
      A *= X*Y;
   }
   return out;
}

float chebyshev4(float x, int order, float A = 1.0f, float X = 0.5f, float Y = 1.0f)
{
   // To = 1
   // T1 = x
   // Tn = 2.x.Tn-1 - Tn-2
   // out = sum(Ai*Ti(x)) , i C {1,..,order}
   float Tn_2 = 1.0f;
   float Tn_1 = x;
   float Tn;
   float out = A*Tn_1;   
   for(int n=2;n<=order;n++)
   {
      Tn    =   2.0f*x*Tn_1 - Tn_2;
      out    +=   A*Tn;
      Tn_2 =   Tn_1;
      Tn_1 =  Tn;
      A *= std::fmod(X,Y);
   }
   return out;
}

float chebyshev5(float x, int order, float A = 1.0f, float B = 1.0f, float C = 0.0f)
{
   // To = 1
   // T1 = x
   // Tn = 2.x.Tn-1 - Tn-2
   // out = sum(Ai*Ti(x)) , i C {1,..,order}
   float Tn_2 = 1.0f;
   float Tn_1 = x;
   float Tn;
   float out = A*Tn_1;   
   if(A == 0.0f) return 0;
   float X = -B * std::sqrt(B*B - 4*A*C)/(2*A);
   float Y = 1.0f;
   if(B != 0.0f)    
        Y = -A * std::sqrt(A*A - 4*B*C)/(2*B);

   for(int n=2;n<=order;n++)
   {
      Tn    =   2.0f*x*Tn_1 - Tn_2;
      out    +=   A*Tn;
      Tn_2 =   Tn_1;
      Tn_1 =  Tn;
      A *= X*Y;
   }
   return out;
}

float waveshaper(float x, float a = 1)
{
    return std::tanh(x*(std::abs(x) + a)/((x*x) + (a-1)*std::abs(x) + 1));
}
float waveshaper2(float x, float a = 1)
{
    return std::tanh(x*(std::abs(x) + a)/((x*x*x) + (a-1)*std::abs(x) + 1));
}
float waveshaper3(float x, float a = 1)
{
    return std::tanh(std::fmod(x,(std::abs(x) + a)/((x*x*x) + (a-1)*std::abs(x) + 1));
}
float waveshaper4(float x, float a = 1)
{
    return std::tanh(x*(std::abs(x) + a)/(std::fmod(x*x)) + (a-1)*std::abs(x) + 1);
}
float waveshaper5(float x, float y,, float a = 1)
{
    return std::tanh(x*(std::abs(x) + a)/((std::pow(x,y)) + (a-1)*std::abs(x) + 1));
}
float waveshaper6(float x, float a = 1)
{
    return std::tanh(x*(std::abs(x) + a)/((x*x) + std::fmod((a-1),std::abs(x) + 1)));
}
float waveshaper7(float x, float y,, float a = 1)
{
    return std::tanh(std::pow(x,(std::abs(x) + a)/((std::pow(x,y)) + (a-1)*std::abs(x) + 1),y));
}

float tanh_normalized(float x, float y=1, float a=1) {
    return a*(std::tanh(x)/std::tanh(y))
}
float  sinshaper(float x, float a=1)
{
    float z = M_PI * a;
    float s = 1/std::sin(z)
    float b = 1/a

    if (x > b)
        return 1;
    else
        return std::tanh(std::sin(z*x)*s);
}
float dsf_line(float x, float amount=1)
{    
    float k = 2*amount/(1-amount);
    return std::tanh(((1+k)*x)/((1+k*std::abs(x))));
}
float dc_shaper(float x, float dc, float a=1)
{
    float r = x + dc;
    return tanh_normalized(r,1,a);
}
	
float cubic_waveshaper( float in, float a = 1 ) {
  return std::tanh(a*(1.5f * in - 0.5f * in *in * in));
}

float gloubi_waveshaper(float in ,float amp=1) {
    const double x = input * 0.686306;
    const double a = 1 + std::exp (std::sqrt (std::fabs (x)) * -0.75);
    return std::tanh(amp*(std::exp (x) - std::exp (-x * a)) / (std::exp (x) + std::exp (-x)));
}

float crusher( float input, float bits );
    float step = std::pow(2,bits);    
    return step * std::floor( input(i)/step + 0.5 );
}
