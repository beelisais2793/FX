 #pragma once
#include "Undenormal.hpp"
#include "SoundObject.hpp"

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

namespace Filters::IIR::Biquad
{

    ///////////////////////////////////////////////////
    enum class FilterType
    {
        LowPass = 1,
        HighPass,
        BandPass,
        Notch,
        AllPass,
        Peaking,
        LowShelf,
        HighShelf,
        OnePoleZeroLP,
        OnePoleZeroHP,        
    };

    struct Parameters
    {
        FilterType filterType;
        DspFloatType fs;
        DspFloatType f0;
        DspFloatType Q;
        DspFloatType dBGain;
    };

    class Biquad : public FilterProcessor
    {
    private:
        FilterType mfilterType;
        
        Parameters mparams;

        // coefficients
        DspFloatType ma0, ma1, ma2, mb0, mb1, mb2;

        // buffers
        DspFloatType mx_z1, mx_z2, my_z1, my_z2;
        // prev x,y delayline
        // delayline x_prev;
        // delayline y_prev;
        
        void calculateCoeffs();
        
    public:
        Biquad(){};
        ~Biquad(){};
        void setParams(const Parameters& params);
        Parameters getParams();
        DspFloatType process(DspFloatType x);
        void setFilterType(FilterType type) {
            mparams.filterType = type;
            setParams(mparams);
        }
        void setCutoff(DspFloatType f) {
            mparams.f0 = f;
            setParams(mparams);
        }
        void setQ(DspFloatType q) {
            mparams.Q = q;
            setParams(mparams);
        }
        void setGain(DspFloatType g) {
            mparams.dbGain = g;
            setParams(mparams);
        }
        enum {
            PORT_TYPE,
            PORT_CUTOFF,
            PORT_Q,
            PORT_GAIN,
        };
        void setPort(int port, DspFloatType v) {
            switch(port)
            {
                case PORT_TYPE: setFilterType((FilterType)v); break;
                case PORT_CUTOFF: setCutoff(v); break;
                case PORT_Q: setQ(v); break;
                case PORT_GAIN: setGain(v); break;
            }
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return process(I);
        }
        void morph(Biquad & other, float f = 0.5)
        {
            ma0 = ma0 + f*(other.ma0 - ma0);
            ma1 = ma1 + f*(other.ma1 - ma1);
            ma2 = ma2 + f*(other.ma2 - ma2);
            mb0 = mb0 + f*(other.mb0 - mb0);
            mb1 = mb1 + f*(other.mb1 - mb1);
            mb2 = mb2 + f*(other.mb2 - mb2);
            m_param.f0 = m_param.f0 + f*(other.m_param.f0 - m_param.f0);
            m_param.Q = m_param.Q + f*(other.m_param.Q - m_param.Q);
            m_param.dbGain = m_param.dbGain + f*(other.m_param.dbGain - m_param.dbGain);
        }
    };



    inline void Biquad::setParams(const Parameters& params)
    {
        mparams = params;
        calculateCoeffs();
    }

    inline Parameters Biquad::getParams()
    {
        return mparams;
    }

    inline void Biquad::calculateCoeffs()
    {
        DspFloatType omega0 = 2.0f * M_PI * (mparams.f0 / mparams.fs);
        DspFloatType alpha = std::sin(omega0) / (2.0 * mparams.Q);
        DspFloatType A = std::pow(10, mparams.dBGain / 40.0);
        switch (mparams.filterType)
        {
        case FilterType::LowPass:
        {
            ma0 = 1.0 + alpha;
            ma1 = -2.0 * std::cos(omega0);
            ma2 = 1.0 - alpha;
            mb0 = (1.0 - std::cos(omega0)) / 2.0;
            mb1 = 1.0 - std::cos(omega0);
            mb2 = (1.0 - std::cos(omega0)) / 2.0;
            break;
        }
        case FilterType::HighPass:
        {
            ma0 = 1.0 + alpha;
            ma1 = -2.0 * std::cos(omega0);
            ma2 = 1.0 - alpha;
            mb0 = (1.0 + std::cos(omega0)) / 2.0;
            mb1 = -(1.0 + std::cos(omega0));
            mb2 = (1.0 + std::cos(omega0)) / 2.0;
            break;
        }
        case FilterType::BandPass:
        {
            ma0 = 1.0 + alpha;
            ma1 = -2.0 * std::cos(omega0);
            ma2 = 1.0 - alpha;
            mb0 = alpha;
            mb1 = 0;
            mb2 = -alpha;
            break;
        }
        case FilterType::Notch:
        {
            ma0 = 1.0 + alpha;
            ma1 = -2.0 * std::cos(omega0);
            ma2 = 1.0 - alpha;
            mb0 = 1.0;
            mb1 = -2.0 * std::cos(omega0);
            mb2 = 1.0;
            break;
        }
        case FilterType::AllPass:
        {
            ma0 = 1.0 + alpha;
            ma1 = -2.0 * std::cos(omega0);
            ma2 = 1.0 - alpha;
            mb0 = 1.0 - alpha;
            mb1 = -2.0 * std::cos(omega0);
            mb2 = 1.0 + alpha;
            break;
        }
        case FilterType::Peaking:
        {
            ma0 = 1.0 + alpha / A;
            ma1 = -2.0 * std::cos(omega0);
            ma2 = 1.0 - alpha / A;
            mb0 = 1.0 + alpha * A;
            mb1 = -2.0 * std::cos(omega0);
            mb2 = 1.0 - alpha * A;
            break;
        }
        case FilterType::LowShelf:
        {
            ma0 = (A + 1.0) + (A - 1.0) * std::cos(omega0) + 2.0 * std::sqrt(A) * alpha;
            ma1 = -2.0 * ((A - 1.0) + (A + 1.0) * std::cos(omega0));
            ma2 = (A + 1.0) + (A - 1.0) * std::cos(omega0) - 2.0 * std::sqrt(A) * alpha;
            mb0 = A * ((A + 1.0) - (A - 1.0) * std::cos(omega0) + 2.0 * std::sqrt(A) * alpha);
            mb1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * std::cos(omega0));
            mb2 = A * ((A + 1.0) - (A - 1.0) * std::cos(omega0) - 2.0 * std::sqrt(A) * alpha);
            break;
        }
        case FilterType::HighShelf:
        {
            ma0 = (A + 1.0) - (A - 1.0) * std::cos(omega0) + 2.0 * std::sqrt(A) * alpha;
            ma1 = 2.0 * ((A - 1.0) - (A + 1.0) * std::cos(omega0));
            ma2 = (A + 1.0) - (A - 1.0) * std::cos(omega0) - 2.0 * std::sqrt(A) * alpha;
            mb0 = A * ((A + 1.0) + (A - 1.0) * std::cos(omega0) + 2.0 * std::sqrt(A) * alpha);
            mb1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * std::cos(omega0));
            mb2 = A * ((A + 1.0) + (A - 1.0) * std::cos(omega0) - 2.0 * std::sqrt(A) * alpha);
            break;
        }
        default:
            break;
        }
    }

    inline DspFloatType Biquad::process(DspFloatType x)
    {    
        Undenormal denormal;
        DspFloatType y = (mb0 / ma0) * x + (mb1 / ma0) * mx_z1 + (mb2 / ma0) * mx_z2 - (ma1 / ma0) * my_z1 - (ma2 / ma0) * my_z2;

        mx_z2 = mx_z1;
        mx_z1 = x;

        my_z2 = my_z1;
        my_z1 = y;

        return y;
    }

    template<typename SIMD>
    class VecBiquad
    {
    private:
        FilterType mfilterType;
        
        Parameters mparams;

        // coefficients
        DspFloatType ma0, ma1, ma2, mb0, mb1, mb2;

        // buffers
        SIMD mx_z1, mx_z2, my_z1, my_z2;
        
        void calculateCoeffs();
        
    public:
        VecBiquad(){};
        ~VecBiquad(){};
        void setParams(const Parameters& params);
        Parameters getParams();
        SIMD Tick(SIMD x);    
        void  ProcessBuffer(size_t n, float * input, float * output);

        void setFilterType(FilterType type) {
            mparams.filterType = type;
            setParams(mparams);
        }
        void setCutoff(DspFloatType f) {
            mparams.f0 = f;
            setParams(mparams);
        }
        void setQ(DspFloatType q) {
            mparams.Q = q;
            setParams(mparams);
        }
        void setGain(DspFloatType g) {
            mparams.dbGain = g;
            setParams(mparams);
        }
        enum {
            PORT_TYPE,
            PORT_CUTOFF,
            PORT_Q,
            PORT_GAIN,
        };        
        void setPort(int port, DspFloatType v) {
            switch(port)
            {
                case PORT_TYPE: setFilterType((FilterType)v); break;
                case PORT_CUTOFF: setCutoff(v); break;
                case PORT_Q: setQ(v); break;
                case PORT_GAIN: setGain(v); break;
            }
        }
    };

    template<typename T>
    void VecBiquad<T>::setParams(const Parameters& params)
    {
        mparams = params;
        calculateCoeffs();
    }

    template<typename T>
    Parameters VecBiquad<T>::getParams()
    {
        return mparams;
    }

    template<typename T>
    void VecBiquad<T>::calculateCoeffs()
    {
        DspFloatType omega0 = 2.0f * M_PI * (mparams.f0 / mparams.fs);
        DspFloatType alpha = std::sin(omega0) / (2.0 * mparams.Q);
        DspFloatType A = std::pow(10, mparams.dBGain / 40.0);
        switch (mparams.filterType)
        {
        case FilterType::LowPass:
        {
            ma0 = 1.0 + alpha;
            ma1 = -2.0 * std::cos(omega0);
            ma2 = 1.0 - alpha;
            mb0 = (1.0 - std::cos(omega0)) / 2.0;
            mb1 = 1.0 - std::cos(omega0);
            mb2 = (1.0 - std::cos(omega0)) / 2.0;
            break;
        }
        case FilterType::HighPass:
        {
            ma0 = 1.0 + alpha;
            ma1 = -2.0 * std::cos(omega0);
            ma2 = 1.0 - alpha;
            mb0 = (1.0 + std::cos(omega0)) / 2.0;
            mb1 = -(1.0 + std::cos(omega0));
            mb2 = (1.0 + std::cos(omega0)) / 2.0;
            break;
        }
        case FilterType::BandPass:
        {
            ma0 = 1.0 + alpha;
            ma1 = -2.0 * std::cos(omega0);
            ma2 = 1.0 - alpha;
            mb0 = alpha;
            mb1 = 0;
            mb2 = -alpha;
            break;
        }
        case FilterType::Notch:
        {
            ma0 = 1.0 + alpha;
            ma1 = -2.0 * std::cos(omega0);
            ma2 = 1.0 - alpha;
            mb0 = 1.0;
            mb1 = -2.0 * std::cos(omega0);
            mb2 = 1.0;
            break;
        }
        case FilterType::AllPass:
        {
            ma0 = 1.0 + alpha;
            ma1 = -2.0 * std::cos(omega0);
            ma2 = 1.0 - alpha;
            mb0 = 1.0 - alpha;
            mb1 = -2.0 * std::cos(omega0);
            mb2 = 1.0 + alpha;
            break;
        }
        case FilterType::Peaking:
        {
            ma0 = 1.0 + alpha / A;
            ma1 = -2.0 * std::cos(omega0);
            ma2 = 1.0 - alpha / A;
            mb0 = 1.0 + alpha * A;
            mb1 = -2.0 * std::cos(omega0);
            mb2 = 1.0 - alpha * A;
            break;
        }
        case FilterType::LowShelf:
        {
            ma0 = (A + 1.0) + (A - 1.0) * std::cos(omega0) + 2.0 * std::sqrt(A) * alpha;
            ma1 = -2.0 * ((A - 1.0) + (A + 1.0) * std::cos(omega0));
            ma2 = (A + 1.0) + (A - 1.0) * std::cos(omega0) - 2.0 * std::sqrt(A) * alpha;
            mb0 = A * ((A + 1.0) - (A - 1.0) * std::cos(omega0) + 2.0 * std::sqrt(A) * alpha);
            mb1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * std::cos(omega0));
            mb2 = A * ((A + 1.0) - (A - 1.0) * std::cos(omega0) - 2.0 * std::sqrt(A) * alpha);
            break;
        }
        case FilterType::HighShelf:
        {
            ma0 = (A + 1.0) - (A - 1.0) * std::cos(omega0) + 2.0 * std::sqrt(A) * alpha;
            ma1 = 2.0 * ((A - 1.0) - (A + 1.0) * std::cos(omega0));
            ma2 = (A + 1.0) - (A - 1.0) * std::cos(omega0) - 2.0 * std::sqrt(A) * alpha;
            mb0 = A * ((A + 1.0) + (A - 1.0) * std::cos(omega0) + 2.0 * std::sqrt(A) * alpha);
            mb1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * std::cos(omega0));
            mb2 = A * ((A + 1.0) + (A - 1.0) * std::cos(omega0) - 2.0 * std::sqrt(A) * alpha);
            break;
        }
        default:
            break;
        }
        if(ma0 != 0.0f)
        {
            mb0 /= ma0;
            mb1 /= ma0;
            mb2 /= ma0;
            ma1 /= ma0;
            ma2 /= ma0;
        }
    }

    template<typename SIMD>
    SIMD VecBiquad<SIMD>::Tick(SIMD x)
    {
        Undenormal denormals;
        SIMD y = mb0 * x + mb1 * mx_z1 + mb2 * mx_z2 - ma1 * my_z1 - ma2 * my_z2;

        mx_z2 = mx_z1;
        mx_z1 = x;

        my_z2 = my_z1;
        my_z1 = y;

        return y;
    }


    template<typename SIMD>
    void VecBiquad<SIMD>::ProcessBuffer(size_t n, float * in, float * out)
    {
        SIMD r,rout;
        for(size_t i = 0; i < n; i += 8)
        {
            r.load(in+i);
            rout = Tick(r);
            rout.store(out+i);
        }
    }

    using VecBiquad4 = VecBiquad<Vec4f>;
    using VecBiquad8 = VecBiquad<Vec8f>;
}

namespace Filters::IIR::Nigel
{
    enum {
    bq_type_lowpass = 0,
    bq_type_highpass,
    bq_type_bandpass,
    bq_type_notch,
    bq_type_peak,
    bq_type_lowshelf,
    bq_type_highshelf
    };

    class Biquad {
    public:
        Biquad();
        Biquad(int type, DspFloatType Fc, DspFloatType Q, DspFloatType peakGainDB);
        ~Biquad();

        void setType(int type);
        void setQ(DspFloatType Q);
        void setFc(DspFloatType Fc);
        void setPeakGain(DspFloatType peakGainDB);

        enum {
            PORT_TYPE,
            PORT_CUTOFF,
            PORT_Q,
            PORT_GAIN,
        };        
        void setPort(int port, DspFloatType v) {
            switch(port)
            {
                case PORT_TYPE: setType((int)v); break;
                case PORT_CUTOFF: setFc(v); break;
                case PORT_Q: setQ(v); break;
                case PORT_GAIN: setPeakGain(v); break;
            }
        }

        void setBiquad(int type, DspFloatType Fc, DspFloatType Q, DspFloatType peakGain);
        float process(float in);
        
    protected:
        void calcBiquad(void);

        int type;
        DspFloatType a0, a1, a2, b1, b2;
        DspFloatType Fc, Q, peakGain;
        DspFloatType z1, z2;
    };

    inline float Biquad::process(float in) {
        DspFloatType out = in * a0 + z1;
        z1 = in * a1 + z2 - b1 * out;
        z2 = in * a2 - b2 * out;
        return out;
    }

    Biquad::Biquad() {
        type = bq_type_lowpass;
        a0 = 1.0;
        a1 = a2 = b1 = b2 = 0.0;
        Fc = 0.50;
        Q = 0.707;
        peakGain = 0.0;
        z1 = z2 = 0.0;
    }

    Biquad::Biquad(int type, DspFloatType Fc, DspFloatType Q, DspFloatType peakGainDB) {
        setBiquad(type, Fc, Q, peakGainDB);
        z1 = z2 = 0.0;
    }

    Biquad::~Biquad() {
    }

    void Biquad::setType(int type) {
        this->type = type;
        calcBiquad();
    }

    void Biquad::setQ(DspFloatType Q) {
        this->Q = Q;
        calcBiquad();
    }

    void Biquad::setFc(DspFloatType Fc) {
        this->Fc = Fc;
        calcBiquad();
    }

    void Biquad::setPeakGain(DspFloatType peakGainDB) {
        this->peakGain = peakGainDB;
        calcBiquad();
    }
        
    void Biquad::setBiquad(int type, DspFloatType Fc, DspFloatType Q, DspFloatType peakGainDB) {
        this->type = type;
        this->Q = Q;
        this->Fc = Fc;
        setPeakGain(peakGainDB);
    }

    void Biquad::calcBiquad(void) {
        DspFloatType norm;
        DspFloatType V = pow(10, fabs(peakGain) / 20.0);
        DspFloatType K = tan(M_PI * Fc);
        switch (this->type) {
            case bq_type_lowpass:
                norm = 1 / (1 + K / Q + K * K);
                a0 = K * K * norm;
                a1 = 2 * a0;
                a2 = a0;
                b1 = 2 * (K * K - 1) * norm;
                b2 = (1 - K / Q + K * K) * norm;
                break;
                
            case bq_type_highpass:
                norm = 1 / (1 + K / Q + K * K);
                a0 = 1 * norm;
                a1 = -2 * a0;
                a2 = a0;
                b1 = 2 * (K * K - 1) * norm;
                b2 = (1 - K / Q + K * K) * norm;
                break;
                
            case bq_type_bandpass:
                norm = 1 / (1 + K / Q + K * K);
                a0 = K / Q * norm;
                a1 = 0;
                a2 = -a0;
                b1 = 2 * (K * K - 1) * norm;
                b2 = (1 - K / Q + K * K) * norm;
                break;
                
            case bq_type_notch:
                norm = 1 / (1 + K / Q + K * K);
                a0 = (1 + K * K) * norm;
                a1 = 2 * (K * K - 1) * norm;
                a2 = a0;
                b1 = a1;
                b2 = (1 - K / Q + K * K) * norm;
                break;
                
            case bq_type_peak:
                if (peakGain >= 0) {    // boost
                    norm = 1 / (1 + 1/Q * K + K * K);
                    a0 = (1 + V/Q * K + K * K) * norm;
                    a1 = 2 * (K * K - 1) * norm;
                    a2 = (1 - V/Q * K + K * K) * norm;
                    b1 = a1;
                    b2 = (1 - 1/Q * K + K * K) * norm;
                }
                else {    // cut
                    norm = 1 / (1 + V/Q * K + K * K);
                    a0 = (1 + 1/Q * K + K * K) * norm;
                    a1 = 2 * (K * K - 1) * norm;
                    a2 = (1 - 1/Q * K + K * K) * norm;
                    b1 = a1;
                    b2 = (1 - V/Q * K + K * K) * norm;
                }
                break;
            case bq_type_lowshelf:
                if (peakGain >= 0) {    // boost
                    norm = 1 / (1 + sqrt(2) * K + K * K);
                    a0 = (1 + sqrt(2*V) * K + V * K * K) * norm;
                    a1 = 2 * (V * K * K - 1) * norm;
                    a2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
                    b1 = 2 * (K * K - 1) * norm;
                    b2 = (1 - sqrt(2) * K + K * K) * norm;
                }
                else {    // cut
                    norm = 1 / (1 + sqrt(2*V) * K + V * K * K);
                    a0 = (1 + sqrt(2) * K + K * K) * norm;
                    a1 = 2 * (K * K - 1) * norm;
                    a2 = (1 - sqrt(2) * K + K * K) * norm;
                    b1 = 2 * (V * K * K - 1) * norm;
                    b2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
                }
                break;
            case bq_type_highshelf:
                if (peakGain >= 0) {    // boost
                    norm = 1 / (1 + sqrt(2) * K + K * K);
                    a0 = (V + sqrt(2*V) * K + K * K) * norm;
                    a1 = 2 * (K * K - V) * norm;
                    a2 = (V - sqrt(2*V) * K + K * K) * norm;
                    b1 = 2 * (K * K - 1) * norm;
                    b2 = (1 - sqrt(2) * K + K * K) * norm;
                }
                else {    // cut
                    norm = 1 / (V + sqrt(2*V) * K + K * K);
                    a0 = (1 + sqrt(2) * K + K * K) * norm;
                    a1 = 2 * (K * K - 1) * norm;
                    a2 = (1 - sqrt(2) * K + K * K) * norm;
                    b1 = 2 * (K * K - V) * norm;
                    b2 = (V - sqrt(2*V) * K + K * K) * norm;
                }
                break;
        }
        
        return;
    }
}