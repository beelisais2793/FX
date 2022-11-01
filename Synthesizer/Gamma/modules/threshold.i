
%{
#include "Gamma/Analysis.h"
#include <cassert>
%}


namespace gam
{
    template <class T=gam::real>
    class Threshold{
    public:
        Threshold(T thresh, T freq=10);
        
        %extend {
            T Tick(T in) { return (*$self)(in); }
            T Process(T in, T hi, T lo) { return (*$self)(in,hi,lo); }

            gam::OnePole<T> get_lpf() { return $self->lpf; }
            T get_thresh() { return $self->thresh; }
        }
        /// Returns 1 if less than threshold, 0 otherwise
        T inv(T in);
        
    };
}

%template(Threshold) gam::Threshold<SampleType>;