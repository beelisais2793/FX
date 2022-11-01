%{
#include "Gamma/FFT.h"
%}

namespace gam
{
    template <class T>
    class CFFT{
    public:

        typedef T value_type;
        CFFT(int size=0);        
        ~CFFT();

        int size() const;
        void forward(T * buf, bool normalize=true, T nrmGain=1.);
        void forward(gam::Complex<SampleType> * buf, bool normalize=true, T nrmGain=1.);
        void inverse(T * buf);
        void inverse(gam::Complex<SampleType> * buf);
        void resize(int n);
    };

    template <class T>
    class RFFT{
    public:

        typedef T value_type;
      
        RFFT(int size=0);       
        ~RFFT();
        
        int size() const;
        void forward(T * buf, bool complexBuf=false, bool normalize=true, T nrmGain=1.);
        
        /// Perform complex-to-real inverse transform in-place

        /// \param[in,out]	buf			input is complex sequence, output is real sequence
        /// \param[in]		complexBuf	If true, then 
        ///									input is  [r0,  0, r1, i1, ..., r(n/2), 0] and
        ///									output is [ *, x0, x1, x2, ..., x(n),   *].
        ///								If false, then 
        ///									input is  [r0, r1, i1, ..., r(n/2)]  and 
        ///									output is [x0, x1, x2, ..., x(n)  ].
        void inverse(T * buf, bool complexBuf=false);

        /// Set size of transform
        void resize(int n);

    };
}

%template(CFFT) gam::CFFT<SampleType>;
%template(RFFT) gam::RFFT<SampleType>;