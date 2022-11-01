
%{
#include "Gamma/Analysis.h"
#include <cassert>
%}


namespace gam 
{
    template<class Tv=gam::real>
    class ZeroCross{
    public:

        ZeroCross(Tv prev = Tv(0));

        %extend {
            int Tick(Tv input) { return (*$self)(input); }

            bool zero_cross(Tv input) { return (*$self)(input) == 0; }
            bool negative(Tv input) { return (*$self)(input) < 0; }
        }
    };
}