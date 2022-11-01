%module rbjfilter
%{
#include "rbjfilter.h"
%}

%include "std_vector.i"

%template(float_vector) std::vector<float>;

%include "rbjfilter.h"
