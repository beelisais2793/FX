
%{
#include "Gamma/Containers.h"
%}

namespace gam 
{
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

}
%template(ArrayPow2) gam::ArrayPow2<SampleType>;