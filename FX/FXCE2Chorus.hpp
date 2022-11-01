#pragma once

namespace FX
{
    
	
	// inputValue: a value between zero and one.
	// Returns:
	//    a logarithmically tapered value between 0.01 and 1, having a 
	//    value of 0.1 for an input of 0.5. A common taper curve used by most amp
	//    manufacturers (except for Fender).

	inline DspFloatType AudioTaper(DspFloatType inputValue)
	{
		return std::exp(std::log10(inputValue-1)*2);
	}
	const DspFloatType MIN_DB = -200;
	const DspFloatType  MIN_DB_AMPLITUDE = 1e-10f;

	inline DspFloatType Af2Db(DspFloatType value)
	{
		if (value < MIN_DB_AMPLITUDE) return MIN_DB;
		return 20.0f*std::log10(value);
	}

	inline DspFloatType Db2Af(DspFloatType value)
	{
		if (value < MIN_DB) return 0;
		return std::exp(value*(std::log10(0.05f)));
	}

	uint32_t NextPowerOfTwo(uint32_t value);

	inline DspFloatType Undenormalize(DspFloatType value)
	{
		return 1E-18 +value+ 1E-18;
	}
	inline DspFloatType Undenormalize(DspFloatType value)
	{
		return 1E-6f +value+ 1E-6f;
	}

	constexpr int MIDI_A440_NOTE = 69;

	inline DspFloatType FrequencyToMidiNote(DspFloatType frequency, DspFloatType aReference = 440.0)
	{
		return 12*std::log2(frequency/aReference) + MIDI_A440_NOTE;
	}

    uint32_t NextPowerOfTwo(uint32_t value)
    {
        value--;
        value |= value >> 1;
        value |= value >> 2;
        value |= value >> 4;
        value |= value >> 8;
        value |= value >> 16;
        return value+1;
    }

    class FilterCoefficients2 {
	public:
		FilterCoefficients2()
		{
			a[0] = a[1] = a[2] = 0;
			b[0] = b[1] = b[2] = 0;
		}
		FilterCoefficients2(DspFloatType b0, DspFloatType b1, DspFloatType b2,DspFloatType a0, DspFloatType a1, DspFloatType a2)
		{
			a[0] = a0; a[1] = a1; a[2] = a2;
			b[0] = b0; b[1] = b1; b[2] = b2;
		}
		FilterCoefficients2(const FilterCoefficients2& other)
		{
			a[0] = other.a[0];
			a[1] = other.a[1];
			a[2] = other.a[2];
			b[0] = other.b[0];
			b[1] = other.b[1];
			b[2] = other.b[2];
		}
		void Disable()
		{
			a[0] = b[0] = 1;
			a[1] = a[2] = b[1] = b[2] = 0;
		}

		DspFloatType a[3];
		DspFloatType b[3];
	};

    struct AudioFilter2 {
	
		FilterCoefficients2 prototype;
		FilterCoefficients2 zTransformCoefficients;
		DspFloatType cutoffFrequency;
	
		DspFloatType T = 1;
		DspFloatType referenceFrequency;
		DspFloatType x[2];
		DspFloatType y[2];
		DspFloatType xR[2];
		DspFloatType yR[2];
		DspFloatType disabledFrequency = -1;
        DspFloatType minFreq,maxFreq;

		FilterCoefficients2*GetPrototype() { return &prototype; }
			
		AudioFilter2()		
		{
			disabledFrequency = -1;
		}
	
		AudioFilter2(const FilterCoefficients2& prototype, DspFloatType minFrequency,DspFloatType maxFrequency, DspFloatType disableFrequency = -1)		
		{
			this->prototype = prototype;
			this->disabledFrequency = disableFrequency;
			Reset();
			this->referenceFrequency = 1.0f;
            this->minFreq = minFrequency;
            this->maxFreq = maxFrequency;
		}

		AudioFilter2(const FilterCoefficients2& prototype, DspFloatType referenceFreqency = 1.0f)		
		{
			disabledFrequency = -1;
			Reset();
			this->prototype = prototype;
			this->referenceFrequency = referenceFreqency;
		}
		
		void Reset();

		void SetSampleRate(DspFloatType sampleRate)
		{
			T = 1.0 / sampleRate;
		}

		void Disable();

		virtual void SetCutoffFrequency(DspFloatType frequency)
		{
			this->cutoffFrequency = frequency;
			BilinearTransform(frequency, prototype, &zTransformCoefficients);
		}
		inline DspFloatType Tick(DspFloatType x0)
		{
            Undenormal nerdpowers;

			DspFloatType x1 = x[0];
			DspFloatType x2 = x[1];
			DspFloatType y1 = y[0];
			DspFloatType y2 = y[1];

			DspFloatType y0 = (x0 * (zTransformCoefficients.b[0])
				+ x1 * zTransformCoefficients.b[1]
				+ x2 * zTransformCoefficients.b[2]
				- (
					y1* zTransformCoefficients.a[1]
					+ y2* zTransformCoefficients.a[2]
					));
			y[0] = y0;
			y[1] = y1;
			x[0] = x0;
			x[1] = x1;

			return y0;
		}
		inline DspFloatType TickR(DspFloatType x0)
		{
			DspFloatType x1 = xR[0];
			DspFloatType x2 = xR[1];
			DspFloatType y1 = yR[0];
			DspFloatType y2 = yR[1];

			DspFloatType y0 = Undenormalize(x0 * (zTransformCoefficients.b[0])
				+ x1 * zTransformCoefficients.b[1]
				+ x2 * zTransformCoefficients.b[2]
				- (
					y1* zTransformCoefficients.a[1]
					+ y2* zTransformCoefficients.a[2]
					));
			yR[0] = y0;
			yR[1] = y1;
			xR[0] = x0;
			xR[1] = x1;

			return y0;
		}

		DspFloatType GetFrequencyResponse(DspFloatType frequency);	
		void BilinearTransform(DspFloatType frequency, const FilterCoefficients2& prototype, FilterCoefficients2* result);

	};

    class  LowPassFilter: public AudioFilter2 {
    private:
        static FilterCoefficients2 LOWPASS_PROTOTYPE;

    public:
        LowPassFilter();
        LowPassFilter(DspFloatType minFrequency, DspFloatType maxFrequency, DspFloatType disabledFrequency = -1);

    };

    FilterCoefficients2 LowPassFilter::LOWPASS_PROTOTYPE = FilterCoefficients2(
	0.8291449788086549, 0, 0,
	0.8484582463996709, 1.156251050939778,1);

    LowPassFilter::LowPassFilter()
    : AudioFilter2(LOWPASS_PROTOTYPE)
    {

    }

    LowPassFilter::LowPassFilter(DspFloatType minFrequency, DspFloatType maxFrequency, DspFloatType disabledFrequency)
    : AudioFilter2(LOWPASS_PROTOTYPE, minFrequency,maxFrequency,disabledFrequency)
    {

    }

    void AudioFilter2::Reset()
    {
        memset(x, 0, sizeof(x));
        memset(y, 0, sizeof(y));
        memset(xR, 0, sizeof(xR));
        memset(yR, 0, sizeof(yR));
    }

    void AudioFilter2::Disable()
    {
        // set to identity IIF.
        this->zTransformCoefficients.Disable();

    }

    static constexpr DspFloatType PI = 3.14159265358979323846;
    static constexpr DspFloatType TWO_PI = PI*2;

    void AudioFilter2::BilinearTransform(DspFloatType frequency, const FilterCoefficients2& prototype, FilterCoefficients2* result)
    {
        DspFloatType w0 = frequency * TWO_PI;
        DspFloatType k = 1 / std::tan(w0 * T * 0.5);
        DspFloatType k2 = k * k;

        DspFloatType b0 = prototype.b[0] - prototype.b[1] * k + prototype.b[2] * k2;
        DspFloatType b1 = 2 * prototype.b[0] - 2 * prototype.b[2] * k2;
        DspFloatType b2 = prototype.b[0] + prototype.b[1] * k + prototype.b[2] * k2;

        DspFloatType a0 = prototype.a[0] - prototype.a[1] * k + prototype.a[2] * k2;
        DspFloatType a1 = 2 * prototype.a[0] - 2 * prototype.a[2] * k2;
        DspFloatType a2 = prototype.a[0] + prototype.a[1] * k + prototype.a[2] * k2;

        // causitive form, and normalize.
        DspFloatType scale = 1.0 / a2;

        result->a[0] = a2 * scale;
        result->a[1] = a1 * scale;
        result->a[2] = a0 * scale;
        result->b[0] = b2 * scale;
        result->b[1] = b1 * scale;
        result->b[2] = b0 * scale;

    }

    DspFloatType AudioFilter2::GetFrequencyResponse(DspFloatType frequency)
    {
        DspFloatType w0 = frequency *T *TWO_PI;

        std::complex<DspFloatType> ejw = std::exp(std::complex<DspFloatType>(0.0, w0));
        std::complex<DspFloatType> ejw2 = ejw * ejw;

        std::complex<DspFloatType> result =
            (
                zTransformCoefficients.b[0]
                + zTransformCoefficients.b[1] * ejw
                + zTransformCoefficients.b[2] * ejw2
                ) 
            / (
                zTransformCoefficients.a[0]
                + zTransformCoefficients.a[1] * ejw
                + zTransformCoefficients.a[2] * ejw2

                    );

        return std::abs(result);
    }
    
    class InterpolatingDelay {
    public:
        InterpolatingDelay();
        InterpolatingDelay(uint32_t maxDelay);
        void SetMaxDelay(uint32_t maxDelay);
        void Clear();

        void Put(DspFloatType value)
        {
            delayIndex = (delayIndex-1) & indexMask;
            delayLine[delayIndex] = value;
        }
        DspFloatType Get(uint32_t index) const {
            return delayLine[(delayIndex + index) & indexMask];
        }
        DspFloatType Get(DspFloatType index) const {
            uint32_t iIndex = (uint32_t)index;
            DspFloatType frac = index-iIndex;
            DspFloatType v0 = Get(iIndex);
            DspFloatType v1 = Get(iIndex+1);
            return (v0*(1-frac)+v1*frac);
        }
        DspFloatType Get(DspFloatType index) const {
            uint32_t iIndex = (uint32_t)index;
            DspFloatType frac = index-iIndex;
            DspFloatType v0 = Get(iIndex);
            DspFloatType v1 = Get(iIndex+1);
            return (DspFloatType)(v0*(1-frac)+v1*frac);
        }

    private:
        uint32_t delayIndex = 0;
        uint32_t indexMask = 0;
        std::vector<DspFloatType> delayLine;

    };

    InterpolatingDelay::InterpolatingDelay()
    {
    }
    InterpolatingDelay::InterpolatingDelay(uint32_t maxDelay)
    {
        SetMaxDelay(maxDelay);
    }

    void InterpolatingDelay::SetMaxDelay(uint32_t maxDelay)
    {
        uint32_t size = NextPowerOfTwo(maxDelay + 2); // +one for the interpolator, +one to prevent wrapping.
        delayLine.resize(size);
        indexMask = size - 1;
        Clear();
    }

    void InterpolatingDelay::Clear()
    {
        delayIndex = 0;
        for (size_t i = 0; i < delayLine.size(); ++i)
        {
            delayLine[i] = 0;
        }
    }

    // Emulation of a famous Chorus pedal.
    // it's just kind of weak and tweaking it a bit
    class Ce2Chorus {
    public:
        Ce2Chorus();
        Ce2Chorus(DspFloatType sampleRate);

        void SetSampleRate(DspFloatType sampleRate);

        void SetRate(DspFloatType value); // [0..1]
        void SetDepth(DspFloatType depth); // [..1]
        void SetFeedback(DspFloatType f) { feedback = f; }
        void SetMix(DspFloatType m) { mix = m; }
        DspFloatType Tick(DspFloatType value);

        void Clear();
        
    private:
        uint32_t bucketBrigadeIndex;
        static constexpr size_t BUCKET_BRIGADE_LENGTH = 1024;
        static constexpr DspFloatType BUCKET_BRIGADE_SCALE = 1.0/BUCKET_BRIGADE_LENGTH;
        DspFloatType bucketBrigadeDelays[BUCKET_BRIGADE_LENGTH];
        DspFloatType bucketBrigadeTotal;
        DspFloatType bbX = 0;

        void ClearBucketBrigade();
        DspFloatType TickBucketBrigade(DspFloatType value);

        DspFloatType TickLfo();
        DspFloatType sampleRate = 44100;
        DspFloatType rate = 0.2;
        DspFloatType depth = 0.15f;
        DspFloatType depthFactor = 0;
        DspFloatType feedback = 0.5;
        DspFloatType rateFactor = 0;
        DspFloatType lfoValue = 0;
        DspFloatType mix = 0.5;

        DspFloatType lfoDx = 0;
        DspFloatType lfoSign = 1;

        InterpolatingDelay delayLine;
        LowPassFilter lfoLowpassFilter;
        
    };


    constexpr DspFloatType LFO_V0  = 4.5; // volts.
    constexpr DspFloatType LFO_MIN = 0.01;    
    constexpr DspFloatType LFO_MAX = 6.5-LFO_V0;

    // you can change this but it will sound warped
    constexpr DspFloatType BUCKET_BRIGADE_V0_DELAY = 0.005; // s. total delay of the bucket brigade at Vlfo = LFO_V0.
    constexpr DspFloatType BUCKET_BRIGADE_V0_RATE = 1024/BUCKET_BRIGADE_V0_DELAY; // The bucket brigade clock rate at LFO_V0


    Ce2Chorus::Ce2Chorus() { }
    Ce2Chorus::Ce2Chorus(DspFloatType sampleRate)
    {
        SetSampleRate(sampleRate);
    }

    void Ce2Chorus::SetSampleRate(DspFloatType sampleRate)
    {
        const DspFloatType MAX_DELAY_MS = 50;
        this->sampleRate = sampleRate;

        uint32_t maxDelay = sampleRate*MAX_DELAY_MS/1000;
        delayLine.SetMaxDelay(maxDelay);

        lfoLowpassFilter.SetSampleRate(sampleRate);
        lfoLowpassFilter.SetCutoffFrequency(76.0); 

        //antiAliasingLowpassFilter.Design(sampleRate,0.5,5000,-25,20000);

        SetRate(this->rate);
        SetDepth(this->depth);
        Clear();

    }

    inline void Ce2Chorus::ClearBucketBrigade()
    {
        auto averageDelay = BUCKET_BRIGADE_V0_DELAY*BUCKET_BRIGADE_SCALE;
        for (size_t i = 0; i < BUCKET_BRIGADE_LENGTH; ++i)
        {
            bucketBrigadeDelays[i] = averageDelay;
        }
        bucketBrigadeTotal = BUCKET_BRIGADE_V0_DELAY;
        bucketBrigadeIndex = 0;
        bbX = 0;

    }
    inline DspFloatType Ce2Chorus::TickBucketBrigade(DspFloatType voltage) {

        // guard against out-of-range voltage swings because of the lfo filter.
        if (voltage < 0.1f) voltage = 0.1f;
        if (voltage > 10) voltage = 10;

        // assume that clock frequency is linearly proportional to voltage.

        DspFloatType fBB = BUCKET_BRIGADE_V0_RATE*voltage/LFO_V0;
        if (fBB < 1) fBB = 1; 

        DspFloatType bbDelay = 1/fBB;

        DspFloatType clocksThisSample = fBB/sampleRate + bbX;
        int iClocksThisSample = (int)clocksThisSample;
        bbX = clocksThisSample-iClocksThisSample;

        DspFloatType t = bbDelay;
        for (int x = 0; x < iClocksThisSample; ++x)
        {
            DspFloatType dx = (DspFloatType)(t-bucketBrigadeDelays[bucketBrigadeIndex]);
            bucketBrigadeDelays[bucketBrigadeIndex] = t;
            bucketBrigadeTotal += dx;
            ++bucketBrigadeIndex;
            if (bucketBrigadeIndex >= BUCKET_BRIGADE_LENGTH)
            {
                bucketBrigadeIndex = 0;
            }
        }
        return bucketBrigadeTotal;

    }

    inline DspFloatType Ce2Chorus::TickLfo()
    {
        lfoValue += lfoDx;
        // one branch version of lfo update (-1 compare, -1 conditional brach,  +1 multiply)
        if (lfoValue >= 1)
        {
            lfoValue = lfoValue-2;
            lfoSign = -lfoSign;
        }
        DspFloatType value =  lfoValue*lfoSign;
        // 76hz low pass (to match Ce2 analysis)
        value = lfoLowpassFilter.Tick(value);
        
        // The output of the LFO in VOLTS.
        value = value*depthFactor+LFO_V0;
        value = TickBucketBrigade(value);

        // calculate the total delay in a 1024-entry bucket brigade delay (to provide the correct shape)

        return value;
    }


    void Ce2Chorus::SetRate(DspFloatType rate)
    {
        this->rate = rate;

        DspFloatType rateHz = 0.1*(1-rate) + 3.25*rate;
        this->lfoDx = 4*rateHz/(sampleRate); // *2 for 1/2 duty cycle cycle, *2 for [-1..1].

    }
    void Ce2Chorus::SetDepth(DspFloatType depth)
    {
        this->depth = depth;
        this->depthFactor = LFO_MIN*(1-depth)+LFO_MAX*depth;
    }

    DspFloatType Ce2Chorus::Tick(DspFloatType value)
    {
        DspFloatType delaySec = TickLfo();
        DspFloatType delayValue = delayLine.Get(delaySec*sampleRate);
        delayLine.Put( value + 0.9*feedback*delayValue);
        return mix*delayValue+(1.0-mix)*value;
    }

    void Ce2Chorus::Clear()
    {
        delayLine.Clear();

        lfoValue = 0;
        lfoSign = 1;
        ClearBucketBrigade();
    }
    
};
