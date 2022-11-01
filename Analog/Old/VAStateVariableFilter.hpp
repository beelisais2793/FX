/*
  ==============================================================================

    VAStateVariableFilter.h
    Created: 10 May 2015 2:41:43pm
    Author:  Jordan Harris

    Notes:
    From The Art of VA Filter Design, by Vadim Zavalishin

    TPT State Variable Filter:

    TPT -> Topology Preserving Transform

    This filter is based off of the trapezoidal integrator, which produces
    a bilinear transformation. This filter does the proper cutoff prewarping
    needed for the TPT structure, as explained by Zavalishin. It also handles
    the problem of delayless feedback loops that the trapezoidal integrators
    introduce.

    The SVF has two feedback paths sharing a common path segment. In order to
    obtain a single feedback equation (rather than equation system), a signal
    on the common path should be selected as the unknown variable. The HP filter
    path is chosen as the zero-delay feedback equation's unknown in this filter.
    Using the HP filter path, the rest of the filter's signals in the structure
    can be defined.

  ==============================================================================
*/

#pragma once
#include <complex>
#include <cmath>
#include <string>

namespace Analog::Filters::StateVariableFilter
{
    //==============================================================================

    /** The type of filter that the State Variable Filter will output. */
    enum SVFType {
        SVFLowpass = 0,
        SVFBandpass,
        SVFHighpass,
        SVFUnitGainBandpass,
        SVFBandShelving,
        SVFNotch,
        SVFAllpass,
        SVFPeak
    };

    //==============================================================================
    class VAStateVariableFilter {
    public:
        /** Create and initialize the filter with default values defined in constructor. */
        VAStateVariableFilter();

        //------------------------------------------------------------------------------

        ~VAStateVariableFilter();

        //------------------------------------------------------------------------------

        /**    Sets the type of the filter that processAudioSample() or processAudioBlock() will
            output. This filter can choose between 8 different types using the enums listed
            below or the int given to each.
            0: SVFLowpass
            1: SVFBandpass
            2: SVFHighpass
            3: SVFUnitGainBandpass
            4: SVFBandShelving
            5: SVFNotch
            6: SVFAllpass
            7: SVFPeak
        */
        void setFilterType(int newType);

        //------------------------------------------------------------------------------
        /**    Used for changing the filter's cutoff parameter linearly by frequency (Hz) */
        void setCutoffFreq(double newCutoffFreq);

        //------------------------------------------------------------------------------
        /** Used for setting the resonance amount. This is then converted to a Q
            value, which is used by the filter.
            Range: (0-1)
        */
        void setResonance(double newResonance);

        //------------------------------------------------------------------------------
        /** Used for setting the filter's Q amount. This is then converted to a
            damping parameter called R, which is used in the original filter.
        */
        void setQ(double newQ);

        //------------------------------------------------------------------------------
        /**    Sets the gain of the shelf for the BandShelving filter only. */
        void setShelfGain(double newGain);

        //------------------------------------------------------------------------------
        /**    Statically set the filters parameters. */
        void setFilter(int newType, double newCutoff,
                    double newResonance, double newShelfGain);

        //------------------------------------------------------------------------------
        /**    Set the sample rate used by the host. Needs to be used to accurately
            calculate the coefficients of the filter from the cutoff.
            Note: This is often used in AudioProcessor::prepareToPlay
        */
        void setSampleRate(double newSampleRate);

        //------------------------------------------------------------------------------
        /**    Performs the actual processing.
        */
        void process(float gain, const float *input, float *output, unsigned count);

        //------------------------------------------------------------------------------
        /**    Reset the state variables.
        */
        void clear();

        //------------------------------------------------------------------------------
        /**    Compute the transfer function at given frequency.
        */
        std::complex<double> calcTransfer(double freq) const;

        //------------------------------------------------------------------------------


        double getCutoffFreq() const { return cutoffFreq; }

        double getFilterType() const { return filterType; }

        double getQ() const { return Q; }

        double getShelfGain() const { return shelfGain; }

    private:
        //==============================================================================
        //    Calculate the coefficients for the filter based on parameters.
        void calcFilter();

        //
        template <int FilterType>
        void processInternally(float gain, const float *input, float *output, unsigned count);

        //    Parameters:
        int filterType;
        double cutoffFreq;
        double Q;
        double shelfGain;

        double sampleRate;

        //    Coefficients:
        double gCoeff;        // gain element
        double RCoeff;        // feedback damping element
        double KCoeff;        // shelf gain element

        double z1_A, z2_A;        // state variables (z^-1)

    private:
        static std::complex<double> calcTransferLowpass(double w, double wc, double r);
        static std::complex<double> calcTransferBandpass(double w, double wc, double r);
        static std::complex<double> calcTransferHighpass(double w, double wc, double r);
        static std::complex<double> calcTransferUnitGainBandpass(double w, double wc, double r);
        static std::complex<double> calcTransferBandShelving(double w, double wc, double r, double k);
        static std::complex<double> calcTransferNotch(double w, double wc, double r);
        static std::complex<double> calcTransferAllpass(double w, double wc, double r);
        static std::complex<double> calcTransferPeak(double w, double wc, double r);
    };

    #if __cplusplus >= 201703L
    # define if_constexpr if constexpr
    #else
    # define if_constexpr if
    #endif

    //==============================================================================

    inline double resonanceToQ(double resonance)
    {
        return 1.0 / (2.0 * (1.0 - resonance));
    }

    //==============================================================================

    inline VAStateVariableFilter::VAStateVariableFilter()
    {
        sampleRate = 44100.0;                // default sample rate when constructed
        filterType = SVFLowpass;            // lowpass filter by default

        gCoeff = 1.0;
        RCoeff = 1.0;
        KCoeff = 0.0;

        cutoffFreq = 1000.0;
        Q = resonanceToQ(0.5);
        shelfGain = 1.0;

        z1_A = 0.0;
        z2_A = 0.0;
    }

    inline VAStateVariableFilter::~VAStateVariableFilter()
    {
    }

    // Member functions for setting the filter's parameters (and sample rate).
    //==============================================================================
    inline void VAStateVariableFilter::setFilterType(int newType)
    {
        filterType = newType;
    }

    inline void VAStateVariableFilter::setCutoffFreq(double newCutoffFreq)
    {
        if (cutoffFreq == newCutoffFreq)
            return;

        cutoffFreq = newCutoffFreq;
        calcFilter();
    }

    inline void VAStateVariableFilter::setResonance(double newResonance)
    {
        setQ(resonanceToQ(newResonance));
    }

    inline void VAStateVariableFilter::setQ(double newQ)
    {
        if (Q == newQ)
            return;

        Q = newQ;
        calcFilter();
    }

    inline void VAStateVariableFilter::setShelfGain(double newGain)
    {
        if (shelfGain == newGain)
            return;

        shelfGain = newGain;
        calcFilter();
    }

    inline void VAStateVariableFilter::setFilter(int newType, double newCutoffFreq,
                                        double newResonance, double newShelfGain)
    {
        double newQ = resonanceToQ(newResonance);

        if (filterType == newType && cutoffFreq == newCutoffFreq && Q == newQ && shelfGain == newShelfGain)
            return;

        filterType = newType;
        cutoffFreq = newCutoffFreq;
        Q = newQ;
        shelfGain = newShelfGain;
        calcFilter();
    }

    inline void VAStateVariableFilter::setSampleRate(double newSampleRate)
    {
        if (sampleRate == newSampleRate)
            return;

        sampleRate = newSampleRate;
        calcFilter();
    }

    //==============================================================================
    inline void VAStateVariableFilter::calcFilter()
    {
        // prewarp the cutoff (for bilinear-transform filters)
        double wd = cutoffFreq * (2.0 * M_PI);
        double T = 1.0 / sampleRate;
        double wa = (2.0 / T) * std::tan(wd * T / 2.0);

        // Calculate g (gain element of integrator)
        gCoeff = wa * T / 2.0;            // Calculate g (gain element of integrator)

        // Calculate Zavalishin's R from Q (referred to as damping parameter)
        RCoeff = 1.0 / (2.0 * Q);

        // Gain for BandShelving filter
        KCoeff = shelfGain;
    }

    inline  double analogSaturate(double x)
    {
        // simple filter analog saturation

        if (x > +1)
            x = 2. / 3.;
        else if (x < -1)
            x = -2. / 3.;
        else
            x = x - (x * x * x) * (1.0 / 3.0);

        return x;
    }

    template <int FilterType>
    void VAStateVariableFilter::processInternally(float gain, const float *input, float *output, unsigned count)
    {
        const double gCoeff = this->gCoeff;
        const double RCoeff = this->RCoeff;
        const double KCoeff = this->KCoeff;

        double z1_A = this->z1_A;
        double z2_A = this->z2_A;

        for (unsigned i = 0; i < count; ++i) {
            double in = gain * input[i];

            double HP = (in - ((2.0 * RCoeff + gCoeff) * z1_A) - z2_A)
                * (1.0 / (1.0 + (2.0 * RCoeff * gCoeff) + gCoeff * gCoeff));
            double BP = HP * gCoeff + z1_A;
            double LP = BP * gCoeff + z2_A;

            z1_A = analogSaturate(gCoeff * HP + BP);        // unit delay (state variable)
            z2_A = analogSaturate(gCoeff * BP + LP);        // unit delay (state variable)

            // Selects which filter type this function will output.
            double out = 0.0;
            if_constexpr (FilterType == SVFLowpass)
                out = LP;
            else if_constexpr (FilterType == SVFBandpass)
                out = BP;
            else if_constexpr (FilterType == SVFHighpass)
                out = HP;
            else if_constexpr (FilterType == SVFUnitGainBandpass)
                out = 2.0 * RCoeff * BP;
            else if_constexpr (FilterType == SVFBandShelving)
                out = in + 2.0 * RCoeff * KCoeff * BP;
            else if_constexpr (FilterType == SVFNotch)
                out = in - 2.0 * RCoeff * BP;
            else if_constexpr (FilterType == SVFAllpass)
                out = in - 4.0 * RCoeff * BP;
            else if_constexpr (FilterType == SVFPeak)
                out = LP - HP;

            output[i] = out;
        }

        this->z1_A = z1_A;
        this->z2_A = z2_A;
    }

    inline void VAStateVariableFilter::process(float gain, const float *input, float *output, unsigned count)
    {
        switch (filterType) {
        case SVFLowpass:
            processInternally<SVFLowpass>(gain, input, output, count);
            break;
        case SVFBandpass:
            processInternally<SVFBandpass>(gain, input, output, count);
            break;
        case SVFHighpass:
            processInternally<SVFHighpass>(gain, input, output, count);
            break;
        case SVFUnitGainBandpass:
            processInternally<SVFUnitGainBandpass>(gain, input, output, count);
            break;
        case SVFBandShelving:
            processInternally<SVFBandShelving>(gain, input, output, count);
            break;
        case SVFNotch:
            processInternally<SVFNotch>(gain, input, output, count);
            break;
        case SVFAllpass:
            processInternally<SVFAllpass>(gain, input, output, count);
            break;
        case SVFPeak:
            processInternally<SVFPeak>(gain, input, output, count);
            break;
        default:
            for (unsigned i = 0; i < count; ++i)
                output[i] = gain * input[i];
        }
    }

    inline void VAStateVariableFilter::clear()
    {
        z1_A = 0;
        z2_A = 0;
    }

    inline std::complex<double> VAStateVariableFilter::calcTransfer(double freq) const
    {
        double w = 2 * M_PI * freq;
        double wc = 2 * M_PI * cutoffFreq;

        switch (filterType) {
        case SVFLowpass:
            return calcTransferLowpass(w, wc, RCoeff);
        case SVFBandpass:
            return calcTransferBandpass(w, wc, RCoeff);
        case SVFHighpass:
            return calcTransferHighpass(w, wc, RCoeff);
        case SVFUnitGainBandpass:
            return calcTransferUnitGainBandpass(w, wc, RCoeff);
        case SVFBandShelving:
            return calcTransferBandShelving(w, wc, RCoeff, shelfGain);
        case SVFNotch:
            return calcTransferNotch(w, wc, RCoeff);
        case SVFAllpass:
            return calcTransferAllpass(w, wc, RCoeff);
        case SVFPeak:
            return calcTransferPeak(w, wc, RCoeff);
        default:
            return 0.0;
        }
    }

    //==============================================================================

    inline std::complex<double> VAStateVariableFilter::calcTransferLowpass(double w, double wc, double r)
    {
        std::complex<double> s = w * std::complex<double>(0, 1);
        return (wc * wc) / (s * s + 2.0 * r * wc * s + wc * wc);
    }

    inline std::complex<double> VAStateVariableFilter::calcTransferBandpass(double w, double wc, double r)
    {
        std::complex<double> s = w * std::complex<double>(0, 1);
        return (wc * s) / (s * s + 2.0 * r * wc * s + wc * wc);
    }

    inline std::complex<double> VAStateVariableFilter::calcTransferHighpass(double w, double wc, double r)
    {
        std::complex<double> s = w * std::complex<double>(0, 1);
        return (s * s) / (s * s + 2.0 * r * wc * s + wc * wc);
    }

    inline std::complex<double> VAStateVariableFilter::calcTransferUnitGainBandpass(double w, double wc, double r)
    {
        return 2.0 * r * calcTransferBandpass(w, wc, r);
    }

    inline std::complex<double> VAStateVariableFilter::calcTransferBandShelving(double w, double wc, double r, double k)
    {
        return 1.0 + k * calcTransferUnitGainBandpass(w, wc, r);
    }

    inline std::complex<double> VAStateVariableFilter::calcTransferNotch(double w, double wc, double r)
    {
        return calcTransferBandShelving(w, wc, r, -1.0);
    }

    inline std::complex<double> VAStateVariableFilter::calcTransferAllpass(double w, double wc, double r)
    {
        return calcTransferBandShelving(w, wc, r, -2.0);
    }

    inline std::complex<double> VAStateVariableFilter::calcTransferPeak(double w, double wc, double r)
    {
        std::complex<double> s = w * std::complex<double>(0, 1);
        return (wc * wc - s * s) / (s * s + 2.0 * r * wc * s + wc * wc);
    }

    //==============================================================================

}