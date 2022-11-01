%module waveTableOsc
%{
#include "WaveTableOsc.h"
%}
%include "std_vector.i"
%include "WaveTableOsc.h"

namespace std {
   %template(vectorf) vector<float>;
   %template(vectord) vector<double>;
};
