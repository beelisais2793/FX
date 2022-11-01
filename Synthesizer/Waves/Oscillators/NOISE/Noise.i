%module Noise
%{
#include "TNoise.hpp"
%}

%include "TNoise.hpp"

%template (FloatGaussianWhiteNoise) SoundWave::TGaussianWhiteNoise<float>;
%template (FloatModifiedVelvetNoise) SoundWave::TModifiedVelvetNoise<float>;
%template (FloatPinkNoiseGenerator) SoundWave::TPinkNoiseGenerator<float>;
%template (FloatBrownNoiseGenerator) SoundWave::TBrownNoiseGenerator<float>;


%template (DoubleGaussianWhiteNoise) SoundWave::TGaussianWhiteNoise<double>;
%template (DoubleModifiedVelvetNoise) SoundWave::TModifiedVelvetNoise<double>;
%template (DoublePinkNoiseGenerator) SoundWave::TPinkNoiseGenerator<double>;
%template (DoubleBrownNoiseGenerator) SoundWave::TBrownNoiseGenerator<double>;
