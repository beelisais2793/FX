%{
#include "Gamma/UnitMaps.h"
}

namespace gam
{
    /// Maps value in unit interval to a tabulated function
    template<
        class T,
        template<class> class Sipl=ipl::Linear,
        class Sacc=acc::Wrap,
        class A=gam::Allocator<T>
    >
    class LookupTable : public Array<T,A>{
    public:
        using Base::elems; using Base::size;
        explicit LookupTable(unsigned size=2048, const T& init=T(0));
        virtual ~LookupTable();

        Array<T,A>& array();
        const Array<T,A>& array() const;

        %extend {
            T Tick(double x) { return (*$self)(x); }
        }

        
        LookupTable& endpoints(index_t min, index_t max);

        /*
        template <class Gen>
        LookupTable& operator+=(Gen& g){
            for(unsigned i=0; i<size(); ++i) (*this)[i] += g();
            return *this;
        }

        template <class Gen>
        LookupTable& operator+=(const Gen& g){
            for(unsigned i=0; i<size(); ++i) (*this)[i] += g();
            return *this;
        }
        */
    };


    template <uint32_t B, class T>
    class TablePow2{
    public:
        
        TablePow2();        
        TablePow2(const T& fillValue);
    
        TablePow2& assign(const SampleType * src){
            for(int i=0; i<N; ++i) mElems[i] = src[i];
            return *this;
        }

        %extend {
            T __getitem__(unsigned i) { return (*$self)[i]; }
            void __setitem__(unsigned i, T value) { (*$self)[i] = value; }
        }

        const T * begin() const;
        T * begin();
        const T * end() const;
        T * end();


        const T& read(double phase) const;
        const T& read(uint32_t phase) const;
        T readL(uint32_t phase) const;

        static constexpr uint32_t mask();
        static constexpr uint32_t size();
        static constexpr uint32_t bits();
        static constexpr uint32_t shift();
        static constexpr uint32_t oneIndex();
    };


    template <unsigned B=10, unsigned D=2, class TComplex=Complex<double> >
    class CSinTable{
    public:
        typedef TablePow2<B, TComplex> Arc;

        CSinTable();

        
        %extend {
            TComplex Tick(double x) { return (*$self)(x); }
            TComplex Op(uint32_t p) { return (*$self)(p); }
        }


        static uint32_t shift();
    };

    template <class T>
    class UnitMapper{
    public:

        /// Mapping types
        enum MapType{
            MAP_LIN,		/**< b0 + u * (b1-b0)		*/
            MAP_POW,		/**< b0 + u^p * (b1-b0)		*/
            MAP_EXP2		/**< p 2^[b0 + u * (b1-b0)]	*/
        };

        T min, max, p1;
        MapType type;
        bool clip;


        UnitMapper();
        UnitMapper(T max, T min=0., T p1=1., MapType type = MAP_POW, bool clip=true);
        UnitMapper& set(T max, T min=0., T p1=1., MapType type = MAP_POW, bool clip=true);
        
        T map(T unit);			///< Map a unit value
        T unmap(T value);		///< Unmap a value to a unit value 
    };
}

%template(LookupTable) gam::LookupTable<SampleType>;
%template(UnitMapper) gam::UnitMapper<SampleType>;