#pragma once

namespace Analog::Filters::SVSmooth
{

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
    class VirtualAnalogStateVariableFilter : public MonoFXProcessor {
    public:
        /** Create and initialize the filter with default values defined in constructor. */
        VirtualAnalogStateVariableFilter();

        //------------------------------------------------------------------------------

        ~VirtualAnalogStateVariableFilter();

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
        void setCutoffFreq(DspFloatType newCutoffFreq);

        //------------------------------------------------------------------------------
        /** Used for setting the resonance amount. This is then converted to a Q
            value, which is used by the filter.
            Range: (0-1)
        */
        void setResonance(DspFloatType newResonance);

        //------------------------------------------------------------------------------
        /** Used for setting the filter's Q amount. This is then converted to a
            damping parameter called R, which is used in the original filter.
        */
        void setQ(DspFloatType newQ);

        //------------------------------------------------------------------------------
        /**    Sets the gain of the shelf for the BandShelving filter only. */
        void setShelfGain(DspFloatType newGain);

        //------------------------------------------------------------------------------
        /**    Statically set the filters parameters. */
        void setFilter(int newType, DspFloatType newCutoff,
                    DspFloatType newResonance, DspFloatType newShelfGain);

        //------------------------------------------------------------------------------
        /**    Set the sample rate used by the host. Needs to be used to accurately
            calculate the coefficients of the filter from the cutoff.
            Note: This is often used in AudioProcessor::prepareToPlay
        */
        void setSampleRate(DspFloatType newSampleRate);

        //------------------------------------------------------------------------------
        /**    Performs the actual processing.
        */
        void process(DspFloatType gain, const DspFloatType *input, DspFloatType *output, unsigned count);

        //------------------------------------------------------------------------------
        /**    Reset the state variables.
        */
        void clear();

        //------------------------------------------------------------------------------
        /**    Compute the transfer function at given frequency.
        */
        std::complex<DspFloatType> calcTransfer(DspFloatType freq) const;

        //------------------------------------------------------------------------------


        DspFloatType getCutoffFreq() const { return cutoffFreq; }

        DspFloatType getFilterType() const { return filterType; }

        DspFloatType getQ() const { return Q; }

        DspFloatType getShelfGain() const { return shelfGain; }

        enum {
            PORT_CUTOFF,
            PORT_FILTERTYPE,
            PORT_Q,
            PORT_SHELFGAIN,
        };
        void setPort(int port, DspFloatType v) 
        {
            switch(port)
            {
                case PORT_CUTOFF: setFilter(filterType,v,Q,shelfGain); break;
                case PORT_FILTERTYPE: setFilter(v,cutoffFreq,Q,shelfGain); break;
                case PORT_Q: setFilter(filterType,cutoffFreq,v,shelfGain); break;
                case PORT_SHELFGAIN: setFilter(filterType,cutoffFreq,Q,v); break;
            }
        }
        void ProcessBlock(size_t n, DspFloatType * input, DspFloatType * output) {
            process(1.0,input,output,n);
        }
    private:
        //==============================================================================
        //    Calculate the coefficients for the filter based on parameters.
        void calcFilter();

        //
        template <int FilterType>
        void processInternally(DspFloatType gain, const DspFloatType *input, DspFloatType *output, unsigned count);

        //    Parameters:
        int filterType;
        DspFloatType cutoffFreq;
        DspFloatType Q;
        DspFloatType shelfGain;

        DspFloatType sampleRate;

        //    Coefficients:
        DspFloatType gCoeff;        // gain element
        DspFloatType RCoeff;        // feedback damping element
        DspFloatType KCoeff;        // shelf gain element

        DspFloatType z1_A, z2_A;        // state variables (z^-1)

    private:
        static std::complex<DspFloatType> calcTransferLowpass(DspFloatType w, DspFloatType wc, DspFloatType r);
        static std::complex<DspFloatType> calcTransferBandpass(DspFloatType w, DspFloatType wc, DspFloatType r);
        static std::complex<DspFloatType> calcTransferHighpass(DspFloatType w, DspFloatType wc, DspFloatType r);
        static std::complex<DspFloatType> calcTransferUnitGainBandpass(DspFloatType w, DspFloatType wc, DspFloatType r);
        static std::complex<DspFloatType> calcTransferBandShelving(DspFloatType w, DspFloatType wc, DspFloatType r, DspFloatType k);
        static std::complex<DspFloatType> calcTransferNotch(DspFloatType w, DspFloatType wc, DspFloatType r);
        static std::complex<DspFloatType> calcTransferAllpass(DspFloatType w, DspFloatType wc, DspFloatType r);
        static std::complex<DspFloatType> calcTransferPeak(DspFloatType w, DspFloatType wc, DspFloatType r);
    };

    #if __cplusplus >= 201703L
    # define if_constexpr if constexpr
    #else
    # define if_constexpr if
    #endif

    //==============================================================================

    static DspFloatType resonanceToQ(DspFloatType resonance)
    {
        return 1.0 / (2.0 * (1.0 - resonance));
    }

    //==============================================================================

    VirtualAnalogStateVariableFilter::VirtualAnalogStateVariableFilter() : MonoFXProcessor()
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

    VirtualAnalogStateVariableFilter::~VirtualAnalogStateVariableFilter()
    {
    }

    // Member functions for setting the filter's parameters (and sample rate).
    //==============================================================================
    void VirtualAnalogStateVariableFilter::setFilterType(int newType)
    {
        filterType = newType;
    }

    void VirtualAnalogStateVariableFilter::setCutoffFreq(DspFloatType newCutoffFreq)
    {
        if (cutoffFreq == newCutoffFreq)
            return;

        cutoffFreq = newCutoffFreq;
        calcFilter();
    }

    void VirtualAnalogStateVariableFilter::setResonance(DspFloatType newResonance)
    {
        setQ(resonanceToQ(newResonance));
    }

    void VirtualAnalogStateVariableFilter::setQ(DspFloatType newQ)
    {
        if (Q == newQ)
            return;

        Q = newQ;
        calcFilter();
    }

    void VirtualAnalogStateVariableFilter::setShelfGain(DspFloatType newGain)
    {
        if (shelfGain == newGain)
            return;

        shelfGain = newGain;
        calcFilter();
    }

    void VirtualAnalogStateVariableFilter::setFilter(int newType, DspFloatType newCutoffFreq,
                                        DspFloatType newResonance, DspFloatType newShelfGain)
    {
        DspFloatType newQ = resonanceToQ(newResonance);

        if (filterType == newType && cutoffFreq == newCutoffFreq && Q == newQ && shelfGain == newShelfGain)
            return;

        filterType = newType;
        cutoffFreq = newCutoffFreq;
        Q = newQ;
        shelfGain = newShelfGain;
        calcFilter();
    }

    void VirtualAnalogStateVariableFilter::setSampleRate(DspFloatType newSampleRate)
    {
        if (sampleRate == newSampleRate)
            return;

        sampleRate = newSampleRate;
        calcFilter();
    }

    //==============================================================================
    void VirtualAnalogStateVariableFilter::calcFilter()
    {
        // prewarp the cutoff (for bilinear-transform filters)
        DspFloatType wd = cutoffFreq * (2.0 * M_PI);
        DspFloatType T = 1.0 / sampleRate;
        DspFloatType wa = (2.0 / T) * std::tan(wd * T / 2.0);

        // Calculate g (gain element of integrator)
        gCoeff = wa * T / 2.0;            // Calculate g (gain element of integrator)

        // Calculate Zavalishin's R from Q (referred to as damping parameter)
        RCoeff = 1.0 / (2.0 * Q);

        // Gain for BandShelving filter
        KCoeff = shelfGain;
    }

    static DspFloatType analogSaturate(DspFloatType x)
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
    void VirtualAnalogStateVariableFilter::processInternally(DspFloatType gain, const DspFloatType *input, DspFloatType *output, unsigned count)
    {
        const DspFloatType gCoeff = this->gCoeff;
        const DspFloatType RCoeff = this->RCoeff;
        const DspFloatType KCoeff = this->KCoeff;

        DspFloatType z1_A = this->z1_A;
        DspFloatType z2_A = this->z2_A;

        for (unsigned i = 0; i < count; ++i) {
            DspFloatType in = gain * input[i];

            DspFloatType HP = (in - ((2.0 * RCoeff + gCoeff) * z1_A) - z2_A)
                * (1.0 / (1.0 + (2.0 * RCoeff * gCoeff) + gCoeff * gCoeff));
            DspFloatType BP = HP * gCoeff + z1_A;
            DspFloatType LP = BP * gCoeff + z2_A;

            z1_A = analogSaturate(gCoeff * HP + BP);        // unit delay (state variable)
            z2_A = analogSaturate(gCoeff * BP + LP);        // unit delay (state variable)

            // Selects which filter type this function will output.
            DspFloatType out = 0.0;
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

    void VirtualAnalogStateVariableFilter::process(DspFloatType gain, const DspFloatType *input, DspFloatType *output, unsigned count)
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

    void VirtualAnalogStateVariableFilter::clear()
    {
        z1_A = 0;
        z2_A = 0;
    }

    std::complex<DspFloatType> VirtualAnalogStateVariableFilter::calcTransfer(DspFloatType freq) const
    {
        DspFloatType w = 2 * M_PI * freq;
        DspFloatType wc = 2 * M_PI * cutoffFreq;

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

    std::complex<DspFloatType> VirtualAnalogStateVariableFilter::calcTransferLowpass(DspFloatType w, DspFloatType wc, DspFloatType r)
    {
        std::complex<DspFloatType> s = w * std::complex<DspFloatType>(0, 1);
        return (wc * wc) / (s * s + 2.0 * r * wc * s + wc * wc);
    }

    std::complex<DspFloatType> VirtualAnalogStateVariableFilter::calcTransferBandpass(DspFloatType w, DspFloatType wc, DspFloatType r)
    {
        std::complex<DspFloatType> s = w * std::complex<DspFloatType>(0, 1);
        return (wc * s) / (s * s + 2.0 * r * wc * s + wc * wc);
    }

    std::complex<DspFloatType> VirtualAnalogStateVariableFilter::calcTransferHighpass(DspFloatType w, DspFloatType wc, DspFloatType r)
    {
        std::complex<DspFloatType> s = w * std::complex<DspFloatType>(0, 1);
        return (s * s) / (s * s + 2.0 * r * wc * s + wc * wc);
    }

    std::complex<DspFloatType> VirtualAnalogStateVariableFilter::calcTransferUnitGainBandpass(DspFloatType w, DspFloatType wc, DspFloatType r)
    {
        return 2.0 * r * calcTransferBandpass(w, wc, r);
    }

    std::complex<DspFloatType> VirtualAnalogStateVariableFilter::calcTransferBandShelving(DspFloatType w, DspFloatType wc, DspFloatType r, DspFloatType k)
    {
        return 1.0 + k * calcTransferUnitGainBandpass(w, wc, r);
    }

    std::complex<DspFloatType> VirtualAnalogStateVariableFilter::calcTransferNotch(DspFloatType w, DspFloatType wc, DspFloatType r)
    {
        return calcTransferBandShelving(w, wc, r, -1.0);
    }

    std::complex<DspFloatType> VirtualAnalogStateVariableFilter::calcTransferAllpass(DspFloatType w, DspFloatType wc, DspFloatType r)
    {
        return calcTransferBandShelving(w, wc, r, -2.0);
    }

    std::complex<DspFloatType> VirtualAnalogStateVariableFilter::calcTransferPeak(DspFloatType w, DspFloatType wc, DspFloatType r)
    {
        std::complex<DspFloatType> s = w * std::complex<DspFloatType>(0, 1);
        return (wc * wc - s * s) / (s * s + 2.0 * r * wc * s + wc * wc);
    }


    struct VirtualAnalogStateVariableFilterProcessor : public FilterProcessorPlugin<VirtualAnalogStateVariableFilter>
    {
        VirtualAnalogStateVariableFilterProcessor() :FilterProcessorPlugin<VirtualAnalogStateVariableFilter>()
        {
            
            this->setFilter(0,1000.0,0.5,1.0);
        }
        void setCutoff(DspFloatType f) {
            this->setCutoffFreq(cv2freq(f));
        }
        void setResonance(DspFloatType r) {
            dynamic_cast<VirtualAnalogStateVariableFilter*>(this)->setResonance(r);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) override
        {
            DspFloatType in = I;
            DspFloatType out = 0;
            this->process(A,&in,&out,1);
            return out;
        }
    };
}