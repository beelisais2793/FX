%module sample
%{
#include "Sample/SampleVector.h"
%}

%include "stdint.i"
%include "std_vector.i"

%template(float_vector) std::vector<float>;
%template(double_vector) std::vector<double>;

%include "Sample/SampleVector.h"

%template(FloatSampleVector) Sample::SampleVector<float>;
