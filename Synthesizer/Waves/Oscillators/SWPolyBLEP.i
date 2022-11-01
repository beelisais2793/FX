%module TPolyBLEP
%{
#include "TPolyBLEP.hpp"
%}

%include "TPolyBLEP.hpp"

%template (FloatPolyBLEP) SoundWave::TPolyBLEP<float>;
%template (DoublePolyBLEP) SoundWave::TPolyBLEP<double>;