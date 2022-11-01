
%{
#include "Gamma/Conversion.h"
%}

namespace gam 
{
    template<> struct Twiddle<float>{
        Twiddle(const float& v);
        Twiddle(const uint32_t& v);
        Twiddle(const int32_t& v);
        
        %extend {
            int32_t get_i() const { return i; }
            int32_t get_u() const { return u; }
            float   get_f() const { return f; }
        }
    };

    template<> struct Twiddle<double>{
        Twiddle(const double& v);
        Twiddle(const uint64_t& v);
        Twiddle(const int64_t& v);

        %extend {
            int64_t get_i() const { return i; }
            int64_t get_u() const { return u; }
            double  get_f() const { return f; }
        }
    };

    uint32_t bits(const char * string);
    uint32_t bitsToUInt(const char * bits);
    void blockSubnormal(float& v);
    void blockSubnormal(double& v);
    int32_t castIntRound(double v);
    template <class T> long castIntTrunc(T v);
    uint32_t floatExponent(float v);
    float floatMantissa(float v);
    int32_t floatToInt(float v);
    uint32_t floatToUInt(float v);
    float fraction(uint32_t bits, uint32_t phase);
    float intToUnit(int16_t v);
    inline float punUF(uint32_t v);
    inline uint32_t punFU( float v);
    inline  int32_t punFI( float v);
    inline  int64_t punFI(  double v);
    inline uint64_t punFU(  double v);
    inline   double punUF(uint64_t v);
    inline   double punIF( int64_t v);
    float split(float value, int32_t& intPart);
    template<class T> T uintToUnit (uint32_t v);
    template<class T> T uintToUnitS(uint32_t v);
    uint32_t unitToUInt(float u);
    uint32_t unitToUInt2(float u);
    uint8_t unitToUInt8(float u);

}
%template(castIntTrunc) gam::castIntTrunc<SampleType>;
%template(uintToUnit) gam::uintToUnit<SampleType>;
%template(uintToUnitS) gam::uintToUnitS<SampleType>;

 