
%{
#include "Gamma/Analysis.h"
#include <cassert>
%}


namespace gam
{
    class SilenceDetect{
    public:
        SilenceDetect(unsigned count = 1000);

        SilenceDetect& count(unsigned v);
        
        void reset();

        %extend {
            bool Tick(const SampleType& input, const SampleType& threshold = SampleType(0.001)) { return (*$self)(input,threshold); }
        }


        bool done() const;
   };
}

