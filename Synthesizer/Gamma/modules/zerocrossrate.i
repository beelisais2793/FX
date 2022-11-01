%{
#include "Gamma/Analysis.h"
#include <cassert>
%}

namespace gam {
    template <class Tv=gam::real>
    class ZeroCrossRate : public PCounter {
    public:        
        ZeroCrossRate(int winSize=256);
        float value() const;
        %extend {
            Tv Tick(Tv input) { return (*$self)(input); }
        }
   };
}

%template(ZeroCrossRate) gam::ZeroCrossRate<SampleType>;