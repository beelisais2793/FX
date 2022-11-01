%{
#include "Gamma/Oscillator.h"
%}

namespace gam {

template <class Sp = phsInc::Loop, class Td = GAM_DEFAULT_DOMAIN>
    class Sweep : public Accum<Sp, Td> {
    public:
      
        Sweep(float frq=440, float phs=0);

        %extend {
            float Tick() { return (*$self)(); }
        }
    };   

}

%template (Sweep) gam::Sweep<>;