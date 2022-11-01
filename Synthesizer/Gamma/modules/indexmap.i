%{
#include "Gamma/Access.h"
%}

namespace gam 
{
    
    typedef int index_t;

    /// Returns last index of an arithmetic iteration starting from 0
    inline unsigned indexLast(unsigned len, unsigned str);

    /// Maps a position in [-1, 1] to an index in [0, n). No boundary operations are performed.
    inline index_t posToInd(float v, index_t n);

    template <class T>
    class IndexMap{

    public:
        IndexMap(index_t idxMax=1, const T& posMax=T(1));
        
        index_t operator()(const T& x) const;
        
        index_t operator()(const T& x, T& f) const {
            f = x*mMul;
            index_t i = cast(f);
            f -= cast(i); 
            return i;
        }
        
        
        %extend {
            T get(index_t i) { return (*$self)(i); }
            T __getitem__(const T& x) { return (*$self)(x); }

            index_t get(const T & x, T & f) { return (*$self)(x,f);}
        }
        void max(index_t idxMax, const T& posMax);    
    };
}

%template(IndexMap) gam::IndexMap<SampleType>;