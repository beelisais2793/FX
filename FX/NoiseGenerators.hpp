#pragma once

////////////////////////////////////////////////////
// Biquad
// RBJ
// Zolzer/Nigel
////////////////////////////////////////////////////
#include "FX/Filters.h"
#include "Util.h"


extern Std::RandomMersenne noise;

/////////////////////////////////////////////////////////////////////////////////////////
// Noise
/////////////////////////////////////////////////////////////////////////////////////////
#define PINK_NOISE_NUM_STAGES 3

inline float WhiteNoise() {
    return noise.rand();
}

class PinkNoise2 {
public:
  PinkNoise2() {
  srand ( time(NULL) ); // initialize random generator
    clear();
  }

  void clear() {
    for( size_t i=0; i< PINK_NOISE_NUM_STAGES; i++ )
      state[ i ] = 0.0;
    }

  float tick() {
    static const float RMI2 = 2.0 / float(RAND_MAX); // + 1.0; // change for range [0,1)
    static const float offset = A[0] + A[1] + A[2];

  // unrolled loop
    float temp = float( noise.rand() );
    state[0] = P[0] * (state[0] - temp) + temp;
    temp = float( noise.rand() );
    state[1] = P[1] * (state[1] - temp) + temp;
    temp = float( noise.rand() );
    state[2] = P[2] * (state[2] - temp) + temp;
    return ( A[0]*state[0] + A[1]*state[1] + A[2]*state[2] )*RMI2 - offset;
  }

protected:
  float state[ PINK_NOISE_NUM_STAGES ];
  static const float A[ PINK_NOISE_NUM_STAGES ];
  static const float P[ PINK_NOISE_NUM_STAGES ];
};

const float PinkNoise2::A[] = { 0.02109238, 0.07113478, 0.68873558 }; // rescaled by (1+P)/(1-P)
const float PinkNoise2::P[] = { 0.3190,  0.7756,  0.9613  };

inline float Pink() {
    static PinkNoise2 pink;
    return pink.tick();
}
inline float GaussianWhiteNoise()
{
    float R1 = noise.rand();
    float R2 = noise.rand();

    return (float) sqrt( -2.0f * log( R1 )) * cos( 2.0f * M_PI * R2 );
}
inline float GaussRand (int m, float s)
{
   static int pass = 0;
   static float y2;
   float x1, x2, w, y1;

   if (pass)
   {
      y1 = y2;
   } else  {
      do {
         x1 = 2.0f * noise.rand () - 1.0f;
         x2 = 2.0f * noise.rand () - 1.0f;
         w = x1 * x1 + x2 * x2;
      } while (w >= 1.0f);

      w = (float)sqrt (-2.0 * log (w) / w);
      y1 = x1 * w;
      y2 = x2 * w;
   }
   pass = !pass;

   return ( (y1 * s + (float) m));
}

inline float PinkNoise()
{
    static Filters::PinkingFilter pink;
    return pink.process(noise.rand());
}
inline float BrownNoise()
{
    static Filters::BrowningFilter brown;
    return brown.process(noise.rand());
}
