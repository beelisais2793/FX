
%{
#include "Gamma/Access.h"
%}

namespace gam 
{
    template <class T>
    class Slice{
    public:

        Slice(T * src, int count_, int stride_=1, int offset_=0);
        
        %extend {
            Slice get(int cnt, int str=1, int off=0) { return (*$self)(cnt,str,off); }
            T __getitem__(int i) { return (*$self)[i]; }
            void __setitem__(int i, T value) { (*$self)[i] = value; }
        }

        // maybe if it is useful I can put them in the interface
        //template <class Gen>
        //const Slice& operator  = (const Gen& v) const { L1{ (*this)[i] =v(); } return *this; }
        const Slice& operator  = (const   T& v) const;

        //template <class Gen>
        //bool operator == (const Gen& v) const { L1{ if(v() != (*this)[i]) return false; } return true; }
        bool operator == (const   T& v) const;

        //template <class U>
        //const Slice& operator += (const Slice<U>& v) const { L2{ (*this)[i]+=T(v[i]); } return *this; }

        //template <class Gen>
        //const Slice& operator += (const Gen& v) const { L1{ (*this)[i]+=v(); } return *this; }
        
        // it wont wrap for lua but will wrap with python so ignore the warning
        const Slice& operator += (const   T& v) const;

        //template <class U>
        //const Slice& operator -= (const Slice<U>& v) const { L2{ (*this)[i]-=T(v[i]); } return *this; }

        //template <class Gen>
        //const Slice& operator -= (const Gen& v) const { L1{ (*this)[i]-=v(); } return *this; }
        const Slice& operator -= (const   T& v) const;

        //template <class U>
        //const Slice& operator *= (const Slice<U>& v) const { L2{ (*this)[i]*=T(v[i]); } return *this; }

        //template <class Gen>
        //const Slice& operator *= (const Gen& v) const { L1{ (*this)[i]*=v(); } return *this; }
        const Slice& operator *= (const   T& v) const;

        //template <class U>
        //const Slice& operator /= (const Slice<U>& v) const { L2{ (*this)[i]/=T(v[i]); } return *this; }

        //template <class Gen>
        //const Slice& operator /= (const Gen& v) const { L1{ (*this)[i]/=v(); } return *this; }
        const Slice& operator /= (const   T& v) const;

        /// Copy elements from another slice.
        
        /// Source elements are statically cast to the type of the destination slice.
        ///
        //template <class U>
        //const Slice& copy(const Slice<U>& v) const { L2{ (*this)[i]=v[i]; } return *this; }

        /// Apply filter in-place
        //template <class Fil>
        //const Slice& filter(const Fil& v) const { L1{ (*this)[i]=v((*this)[i]); } return *this; }
        
        /// Apply C-style unary function in-place, x = func(x, a1)
        //template <class R, class X, class A1>
        //const Slice& filter(R (* const func)(X, A1), const A1& a1){
        //    L1{ (*this)[i] = func((*this)[i], a1); }
        //    return *this;
       // }

        /// Apply C-style binary function in-place, x = func(x, a1, a2)
        //template <class R, class X, class A1, class A2>
        //const Slice& filter(R (* const func)(X, A1,A2), const A1& a1, const A2& a2){
        //    L1{ (*this)[i] = func((*this)[i], a1,a2); }
        //    return *this;
        //}

        
        const Slice& reverse();        
        Slice reversed() const;
        const Slice& set(const T& v=T()) const;

        /// Swaps elements
        /*
        template <class U>
        const Slice& swap(const Slice<U>& v) const {
            L2{ T t=(*this)[i]; (*this)[i]=v[i]; v[i]=t; }
            return *this;
        }
        */

       
        T mean() const;
        T sum() const;
        int count() const;
        int offset() const;
        int stride() const;
        int N() const;

        Slice& count(int v);
        Slice& offset(int v);
        Slice& stride(int v);

    };

    template <class T>
    Slice<T> slice(T * src, int cnt, int str=1, int off=0);
}

%template(IndexMap) gam::IndexMap<SampleType>;
%template(Slice) gam::Slice<SampleType>;
%template(slice) gam::slice<SampleType>;