

%{
#include "Gamma/Containers.h"
%}

namespace gam 
{

    template <class T >
    class Array  {
    public:
        explicit Array(uint32_t size);
        Array(uint32_t size, const T& init);
        Array(T * src, uint32_t size);
        Array();
        explicit Array(const Array& src);
        virtual ~Array();    

        /// arraybase
        %extend {
            T __getitem__(uint32_t i) { return (*$self)[i]; }
            void __setitem__(uint32_t i, T val) { (*$self)[i] = val; }
        }

        /// Sets all elements to value
        Array& assign(const T& v);

        
        //template <class Arr>
        //ArrayBase& assign(const Arr& src);        
        Array& assign(const T& v, uint32_t end, uint32_t stride=1, uint32_t start=0);

        
        Array& zero();
        T * elems();					///< Get writable pointer to elements	
        const T * elems() const;		///< Get read-only pointer to elements
        uint32_t size() const;			///< Returns number of elements in array

        T * begin();
        const T * begin() const;
        T * end();
        const T * end() const;
        
        void clear();
        void own();
        bool isSoleOwner() const;
        bool usingExternalSource() const;
        bool valid() const;
        void resize(uint32_t newSize, const T& c=T());
        bool source(Array<T>& src);
        bool source(T * src, uint32_t size);
        virtual void onResize();
        static int references(T* m);
    };
}

%template(Array) gam::Array<SampleType>;
%template(IntArray) gam::Array<int>;
%template(UIntArray) gam::Array<unsigned int>;
%template(LongArray) gam::Array<long>;
%template(ULongArray) gam::Array<unsigned long>;
%template(FloatArray) gam::Array<float>;
%template(DoubleArray) gam::Array<double>;