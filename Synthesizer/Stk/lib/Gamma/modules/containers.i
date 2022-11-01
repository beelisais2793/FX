
%{
#include "Gamma/Containers.h"
%}

namespace gam 
{

    template <class T>
    class ArrayBase {
    public:
        typedef T value_type;
        
        ArrayBase();
        ArrayBase(uint32_t size);
        ArrayBase(uint32_t size, const T& init);
        ArrayBase(T * src, uint32_t size);
        ArrayBase(const ArrayBase<T>& src);
        virtual ~ArrayBase();

        
        %extend {
            T __getitem__(uint32_t i) { return (*$self)[i]; }
            void __setitem__(uint32_t i, T val) { (*$self)[i] = val; }
        }

        /// Sets all elements to value
        ArrayBase& assign(const T& v);

        
        //template <class Arr>
        //ArrayBase& assign(const Arr& src);        
        ArrayBase& assign(const T& v, uint32_t end, uint32_t stride=1, uint32_t start=0);

        
        ArrayBase& zero();
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
        bool source(ArrayBase<T>& src);
        bool source(T * src, uint32_t size);
        virtual void onResize();
        static int references(T* m);
    };

    
    template <class T >
    class Array 
    {
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

    template <class T>
    class Ring : public Array<T> {
    public:

        explicit Ring(uint32_t size=0, const T& value=T());        
        T& readBack();
        const T& readBack() const;        
        T& readFront();
        const T& readFront() const;
        T& read(uint32_t ago);
        const T& read(uint32_t ago) const;    
        void read(T * dst, uint32_t len, int32_t delay=-1) const;
        void readFrom(T * dst, uint32_t len, int32_t delay, uint32_t from) const;
        uint32_t pos() const;			///< Return absolute index of frontmost (newest) element
        bool reachedEnd() const;		///< Returns whether the last element written was at the end of the array
        uint32_t indexBack() const;		///< Returns absolute index of backmost (oldest) element
        uint32_t indexFront() const;	///< Returns absolute index of frontmost (newest) element
        uint32_t indexPrev(uint32_t ago) const;	///< Returns absolute index of a previously written element
        void operator()(const T& v);	///< Write new element
        void pos(uint32_t index);		///< Set absolute buffer index of writer
        void reset();					///< Reset write position to beginning
        void writeClip(const T& v);		///< Writes element unless at end of buffer
    };

    template <class T>
    class DoubleRing : public Ring<T>{
    public:
        
        explicit DoubleRing(uint32_t size=0, const T& value=T());
        const Array<T>& readBuf() const;

        T * read();
        T * copy();
        void resize(int n);
    };

    template <class T >
    struct DelayN: public Ring<T>{

        DelayN(){}

        explicit DelayN(uint32_t size, const T& value=T());
                
        %extend {
            T __getitem__(const T & input) { return (*$self)(input); }
        }
    };


}

%template(Array) gam::Array<SampleType>;
%template(ArrayPow2) gam::ArrayPow2<SampleType>;
%template(Ring) gam::Ring<SampleType>;
%template(DoubleRing) gam::DoubleRing<SampleType>;
%template(DelayN) gam::DelayN<SampleType>;