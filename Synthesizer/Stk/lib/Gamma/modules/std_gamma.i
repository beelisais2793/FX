%{
#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"
using namespace gam;
using namespace gam::phsInc;
%}
%include "stdint.i"
%include "std_vector.i"

%inline %{
typedef float SampleType;
%}


%template(stdvector) std::vector<SampleType>;
%template(realvector) std::vector<gam::real>;
