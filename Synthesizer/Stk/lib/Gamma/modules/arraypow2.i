

%{
#include "Gamma/Containers.h"
%}

namespace gam 
{
    template <class T>
    class ArrayPow2  {
    public:
        explicit ArrayPow2(uint32_t size);
        ArrayPow2(uint32_t size, const T& initial);
        ArrayPow2(T * src, uint32_t size);
        ArrayPow2();
        explicit ArrayPow2(const ArrayPow2& src);

        virtual ~ArrayPow2();

        uint32_t fracBits() const;				///< Returns number of bits in fraction (32 - bits())
        float fraction(uint32_t phase) const;	///< Get floating-point fractional part of fixed-point phase
        uint32_t index(uint32_t phase) const;	///< Get integer part of fixed-point phase
        uint32_t log2Size() const;				///< Returns log base-2 of the number of array elements
        uint32_t oneIndex() const;				///< Returns 32-bit phase increment for one element index

        const T& atPhase(uint32_t phase) const;	///< Get element at truncated fixed-point phase
        void putPhase(uint32_t phase, T v);		///< Set element at truncated fixed point phase


        /// arraybase
        %extend {
            T __getitem__(uint32_t i) { return (*$self)[i]; }
            void __setitem__(uint32_t i, T val) { (*$self)[i] = val; }
        }

        /// Sets all elements to value
        ArrayPow2& assign(const T& v);

        
        //template <class Arr>
        //ArrayBase& assign(const Arr& src);        
        ArrayPow2& assign(const T& v, uint32_t end, uint32_t stride=1, uint32_t start=0);

        
        ArrayPow2& zero();
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
        bool source(ArrayPow2<T>& src);
        bool source(T * src, uint32_t size);
        virtual void onResize();
        static int references(T* m);

    };
}

%template(ArrayPow2) gam::ArrayPow2<SampleType>;

%template(IntArrayPow2) gam::ArrayPow2<int>;
%template(UIntArrayPow2) gam::ArrayPow2<unsigned int>;
%template(LongArrayPow2) gam::ArrayPow2<long>;
%template(ULongArrayPow2) gam::ArrayPow2<unsigned long>;
%template(FloatArrayPow2) gam::ArrayPow2<float>;
%template(DoubleArrayPow2) gam::ArrayPow2<double>;