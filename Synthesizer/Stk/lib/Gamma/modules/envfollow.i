
%{
#include "Gamma/Analysis.h"
#include <cassert>
%}



namespace gam
{
    template <class Tv=gam::real, class Tp=gam::real, class Td=gam::GAM_DEFAULT_DOMAIN>
    class EnvFollow 
    {
    public:

        
        EnvFollow(Tp freq=10);
        
        Tv operator()(Tv i0);

        %extend {
            Tv Tick(Tv i0) { return (*$self)(i0); }

            gam::OnePole<Tv,Tp,Td>& get_filter() { return $self->lpf; }
        }
        
        Tv value() const;        
        EnvFollow& lag(Tp v);        
        bool done(Tv eps=0.001) const;
        
    };

}

%template(EnvFollow) gam::EnvFollow<SampleType>;