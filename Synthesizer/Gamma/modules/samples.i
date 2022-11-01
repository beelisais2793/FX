%{
#include "Gamma/Sample.h"
%}

namespace gam
{
    template <class To, class From> To sampleTo(From v);

    template<> inline double sampleTo<double,double>(double v);
    template<> inline double sampleTo<double,float>(float v);
    template<> inline double sampleTo<double,int32_t>(int32_t v);
    template<> inline double sampleTo<double,short>(short v);
    template<> inline double sampleTo<double,char>(char v);
    template<> inline float sampleTo<float,double>(double v);
    template<> inline float sampleTo<float,float>(float v);
    template<> inline float sampleTo<float,int32_t>(int32_t v);
    template<> inline float sampleTo<float,short>(short v);
    template<> inline float sampleTo<float,char>(char v);
    template <class I, class R> inline I roundTo(R v);    
    template<> inline int32_t sampleTo<int32_t,double>(double v);
    template<> inline int32_t sampleTo<int32_t,float>(float v);
    template<> inline int32_t sampleTo<int32_t,int32_t>(int32_t v);
    template<> inline int32_t sampleTo<int32_t,short>(short v);
    template<> inline int32_t sampleTo<int32_t,char>(char v);
    template<> inline short sampleTo<short,double>(double v);
    template<> inline short sampleTo<short,float>(float v);
    template<> inline short sampleTo<short,int32_t>(int32_t v);
    template<> inline short sampleTo<short,short>(short v);
    template<> inline short sampleTo<short,char>(char v);
    template<> inline char sampleTo<char,double>(double v);
    template<> inline char sampleTo<char,float>(float v);
    template<> inline char sampleTo<char,int32_t>(int32_t v);
    template<> inline char sampleTo<char,short>(short v);
    template<> inline char sampleTo<char,char>(char v);

    /// Sample data
    template <class T>
    struct Sample{
        typedef T value_type;

        std::vector<value_type> data;	///< Sample data array
        double frameRate;
        int channels;
        float gain;
        bool interleaved;
        
        int frames() const;
        float length() const;
        bool empty() const;
        T peak() const;
        float normalize(float peakGain = 1.);
    };
}