
%{
#include "Gamma/Analysis.h"
#include <cassert>
%}




namespace gam {

    template <class Tv=gam::real>
    class MaxAbs : public PCounter {
    public:

        MaxAbs(int winSize=256);        
        const Tv& value() const;
        const Tv& valueL() const;
        
		%extend
		{
            Tv Tick(const Tv& in) { return (*$self)(in); }        			
		}

    };
}

%template(MaxAbs) gam::MaxAbs<SampleType>;