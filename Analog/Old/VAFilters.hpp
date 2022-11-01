#pragma once
#include <cmath>
#include <vector>
#include <cstring>
#include <cstdint>

    constexpr double kTwoPi = 2.0*M_PI;

    const double GUI_Q_MIN = 1.0;
	const double GUI_Q_MAX = 10.0;
	const double SVF_Q_SLOPE = (25.0 - 0.707) / (GUI_Q_MAX - GUI_Q_MIN);
	const double KORG35_Q_SLOPE = (2.0 - 0.707) / (GUI_Q_MAX - GUI_Q_MIN);
	const double MOOG_Q_SLOPE = (4.0 - 0.0) / (GUI_Q_MAX - GUI_Q_MIN);
	const double DIODE_Q_SLOPE = (17.0 - 0.0) / (GUI_Q_MAX - GUI_Q_MIN);
	const double HSYNC_MOD_SLOPE = 3.0;

    enum { LPF1, LPF2, LPF3, LPF4, HPF1, HPF2, HPF3, HPF4, BPF2, BPF4, BSF2, BSF4, 
		APF1, APF2, ANM_LPF1, ANM_LPF2, ANM_LPF3, ANM_LPF4, NUM_FILTER_OUTPUTS };

    inline double semitonesBetweenFreqs(double startFrequency, double endFrequency)
	{
		return log2(endFrequency / startFrequency)*12.0;
	}

	inline void boundValue(double& value, double minValue, double maxValue)
	{
		const double t = value < minValue ? minValue : value;
		value = t > maxValue ? maxValue : t;
	}
	inline void mapDoubleValue(double& value, double min, double minMap, double slope)
	{
		// --- bound to limits
		value = minMap + slope * (value - min);
	}

	inline void mapDoubleValue(double& value, double min, double max, double minMap, double maxMap)
	{
		// --- bound to limits
		boundValue(value, min, max);
		double mapped = ((value - min) / (max - min)) * (maxMap - minMap) + minMap;
		value = mapped;
	}	
    struct FilterOutput
	{
        double filter[NUM_FILTER_OUTPUTS];
		FilterOutput() { clearData(); }		
		void clearData()
		{
			memset(&filter[0], 0, sizeof(double) * NUM_FILTER_OUTPUTS);
		}
	};

   class IFilterBase
	{
		virtual bool reset(double _sampleRate) = 0;
		virtual bool update() = 0;
		virtual FilterOutput* process(double xn) = 0;
		virtual void setFilterParams(double _fc, double _Q) = 0;
	};

    
	struct VA1Coeffs
	{
		// --- filter coefficients
		double alpha = 0.0;			///< alpha is (wcT/2)
		double beta = 1.0;			///< beta value, not used
	};

	
    //@{
	/**
	\ingroup Constants-Enums
	Constants for the synth filter objects and cores
	*/
	const double freqModLow = 20.0;
	const double freqModHigh = 18000.0; // -- this is reduced from 20480.0 only for self oscillation at upper frequency range
	const double freqModSemitoneRange = semitonesBetweenFreqs(freqModLow, freqModHigh);
	const uint32_t FILTER_AUDIO_INPUTS = 2;
	const uint32_t FILTER_AUDIO_OUTPUTS = 2;
	enum class FilterModel { kFirstOrder, kSVF, kKorg35, kMoog, kDiode };
	enum { FLT1, FLT2, FLT3, FLT4 };
	const int MOOG_SUBFILTERS = 4;
	const int DIODE_SUBFILTERS = 4;
	const int KORG_SUBFILTERS = 3;
	enum class VAFilterAlgorithm {
		kBypassFilter, kLPF1, kHPF1, kAPF1, kSVF_LP, kSVF_HP, kSVF_BP, kSVF_BS, kKorg35_LP, kKorg35_HP, kMoog_LP1, kMoog_LP2, kMoog_LP3, kMoog_LP4, kDiode_LP4
	};
	enum class BQFilterAlgorithm {
		kBypassFilter, k1PLPF, k1PHPF, kLPF2, kHPF2
	};
	//@}


	/**
	\struct FilterParameters
	\ingroup SynthStructures
	\brief
	Custom parameter structure for the any of the synth filters. This structure is designed to take care
	of both VA and biquad filter parameter requirements. Notable members:
	- filterIndex the selected index from a GUI control that the user toggles
	- modKnobValue[4] the four mod knob values on the range [0, 1]
	\author Will Pirkle http://www.willpirkle.com
	\remark This object is included in Designing Software Synthesizer Plugins in C++ 2nd Ed. by Will Pirkle
	\version Revision : 1.0
	\date Date : 2021 / 05 / 02
	*/
	struct FilterParameters
	{
		// --- use with strongly typed enums
		int32_t filterIndex = 0;	///< filter index in GUI control

		double fc = 1000.0;					///< parameter fc
		double Q = 1.0;						///< parameter Q
		double filterOutputGain_dB = 0.0;	///< parameter output gain in dB
		double filterDrive = 1.0;			///< parameter drive (distortion)
		double bassGainComp = 0.0;			///< 0.0 = no bass compensation, 1.0 = restore all bass 
		bool analogFGN = true;				///< use analog FGN filters; adds to CPU load

		// --- key tracking
		bool enableKeyTrack = false;		///< key track flag
		double keyTrackSemis = 0.0;			///< key tracking ratio in semitones

		// --- Mod Knobs and core support
		double modKnobValue[4] = { 0.5, 0.0, 0.0, 0.0 }; ///< mod knobs
		uint32_t moduleIndex = 0;	///< module identifier
	};


	class VA1Filter : public IFilterBase
	{
	public:
		// --- constructor/destructor
		VA1Filter();
		virtual ~VA1Filter() {}

		// --- these match SynthModule names
		virtual bool reset(double _sampleRate);
		virtual bool update();
		virtual FilterOutput* process(double xn); 
		virtual void setFilterParams(double _fc, double _Q);

		// --- set coeffs directly, bypassing coeff calculation
		void setAlpha(double _alpha) { coeffs.alpha = _alpha; }
		void setBeta(double _beta) { coeffs.beta = _beta; }
		void setCoeffs(VA1Coeffs& _coeffs) {
			coeffs = _coeffs;
		}

		void copyCoeffs(VA1Filter& destination) { 
			destination.setCoeffs(coeffs);
		}
		
		// --- added for MOOG & K35, need access to this output value, scaled by beta
		double getFBOutput() { return coeffs.beta * sn; }

	protected:
		FilterOutput output;
		double sampleRate = 44100.0;				///< current sample rate
		double halfSamplePeriod = 1.0;
		double fc = 0.0;

		// --- state storage
		double sn = 0.0;						///< state variables

		// --- filter coefficients
		VA1Coeffs coeffs;
	};

	struct VASVFCoeffs
	{
		// --- filter coefficients
		double alpha = 0.0;			///< alpha is (wcT/2)
		double rho = 1.0;			///< beta value, not used
		double sigma = 1.0;			///< beta value, not used
		double alpha0 = 1.0;			///< beta value, not used
	};

	// --- makes all filter outputs: LPF1, LPF1A, HPF1, APF1
	class VASVFilter : public IFilterBase
	{
	public:
		// --- constructor/destructor
		VASVFilter();
		virtual ~VASVFilter() {}

		// --- these match SynthModule names
		virtual bool reset(double _sampleRate);
		virtual bool update();
		virtual FilterOutput* process(double xn); 
		virtual void setFilterParams(double _fc, double _Q);

		// --- set coeffs directly, bypassing coeff calculation
		void setCoeffs(VASVFCoeffs& _coeffs) {
			coeffs = _coeffs;
		}

		void copyCoeffs(VASVFilter& destination) {
			destination.setCoeffs(coeffs);
		}

	protected:
		FilterOutput output;
		double sampleRate = 44100.0;				///< current sample rate
		double halfSamplePeriod = 1.0;
		double fc = 0.0;
		double Q = 0.0;

		// --- state storage
		double integrator_z[2];						///< state variables

		// --- filter coefficients
		VASVFCoeffs coeffs;
	};

	struct VAKorg35Coeffs
	{
		// --- filter coefficients
		double K = 1.0;			///< beta value, not used
		double alpha = 0.0;			///< alpha is (wcT/2)
		double alpha0 = 1.0;			///< beta value, not used
		double g = 1.0;			///< beta value, not used
	};

	// --- makes both LPF and HPF (double filter)
	class VAKorg35Filter : public IFilterBase
	{
	public:
		// --- constructor/destructor
		VAKorg35Filter();
		virtual ~VAKorg35Filter() {}

		// --- these match SynthModule names
		virtual bool reset(double _sampleRate);
		virtual bool update();
		virtual FilterOutput* process(double xn); 
		virtual void setFilterParams(double _fc, double _Q);

		// --- set coeffs directly, bypassing coeff calculation
		void setCoeffs(VAKorg35Coeffs& _coeffs) {
			coeffs = _coeffs;

			// --- three sync-tuned filters
			for (uint32_t i = 0; i < KORG_SUBFILTERS; i++)
			{
				lpfVAFilters[i].setAlpha(coeffs.alpha);
				hpfVAFilters[i].setAlpha(coeffs.alpha);
			}

			// --- set filter beta values
			double deno = 1.0 + coeffs.g;

			lpfVAFilters[FLT2].setBeta((coeffs.K * (1.0 - coeffs.alpha)) / deno);
			lpfVAFilters[FLT3].setBeta(-1.0 / deno);

			hpfVAFilters[FLT2].setBeta(-coeffs.alpha / deno);
			hpfVAFilters[FLT3].setBeta(1.0 / deno);
		//	hpfVAFilters[FLT3].setBeta(lpfVAFilters[FLT3].getBeta);
		}

		void copyCoeffs(VAKorg35Filter& destination) {
			destination.setCoeffs(coeffs);
		}

	protected:
		FilterOutput output;
		VA1Filter lpfVAFilters[KORG_SUBFILTERS];
		VA1Filter hpfVAFilters[KORG_SUBFILTERS];
		double sampleRate = 44100.0;				///< current sample rate
		double halfSamplePeriod = 1.0;
		double fc = 0.0;

		// --- filter coefficients
		VAKorg35Coeffs coeffs;

		//double K = 0.0;
		//double alpha = 0.0;			///< alpha is (wcT/2)
		//double alpha0 = 0.0;		///< input scalar, correct delay-free loop
	};

	struct VAMoogCoeffs
	{
		// --- filter coefficients
		double K = 1.0;			///< beta value, not used
		double alpha = 0.0;			///< alpha is (wcT/2)
		double alpha0 = 1.0;			///< beta value, not used
		double sigma = 1.0;			///< beta value, not used
		double bassComp = 1.0;			///< beta value, not used
		double g = 1.0;			///< beta value, not used

		// --- these are to minimize repeat calculations for left/right pairs
		double subFilterBeta[MOOG_SUBFILTERS] = { 0.0, 0.0, 0.0, 0.0 };
	};

	// --- makes both LPF and HPF (double filter)
	class VAMoogFilter : public IFilterBase
	{
	public:
		// --- constructor/destructor
		VAMoogFilter();
		virtual ~VAMoogFilter() {}

		// --- these match SynthModule names
		virtual bool reset(double _sampleRate);
		virtual bool update();
		virtual FilterOutput* process(double xn); 
		virtual void setFilterParams(double _fc, double _Q);

		// --- set coeffs directly, bypassing coeff calculation
		void setCoeffs(const VAMoogCoeffs& _coeffs) {
			coeffs = _coeffs;

			// --- four sync-tuned filters
			for (uint32_t i = 0; i < MOOG_SUBFILTERS; i++)
			{
				// --- set alpha directly
				subFilter[i].setAlpha(coeffs.alpha);
				subFilterFGN[i].setAlpha(coeffs.alpha);

				// --- set beta directly
				subFilter[i].setBeta(coeffs.subFilterBeta[i]);
				subFilterFGN[i].setBeta(coeffs.subFilterBeta[i]);
			}
		}

		void copyCoeffs(VAMoogFilter& destination) {
			destination.setCoeffs(coeffs);
		}

	protected:
		FilterOutput output;
		VA1Filter subFilter[MOOG_SUBFILTERS];
		VA1Filter subFilterFGN[MOOG_SUBFILTERS];
		double sampleRate = 44100.0;				///< current sample rate
		double halfSamplePeriod = 1.0;
		double fc = 0.0;

		// --- filter coefficients
		VAMoogCoeffs coeffs;
	};

	struct DiodeVA1Coeffs
	{
		// --- filter coefficients
		double alpha0 = 0.0;		///< input scalar, correct delay-free loop
		double alpha = 0.0;			///< alpha is (wcT/2)
		double beta = 1.0;			///< beta value, not used
		double gamma = 1.0;			///< beta value, not used
		double delta = 1.0;			///< beta value, not used
		double epsilon = 1.0;		///< beta value, not used
	};

	class VADiodeSubFilter : public IFilterBase
	{
	public:
		// --- constructor/destructor
		VADiodeSubFilter();
		virtual ~VADiodeSubFilter() {}

		// --- these match SynthModule names
		virtual bool reset(double _sampleRate);
		virtual bool update();
		virtual FilterOutput* process(double xn); 
		virtual void setFilterParams(double _fc, double _Q);

		void setCoeffs(const DiodeVA1Coeffs& _coeffs) { coeffs = _coeffs; }
		void copyCoeffs(VADiodeSubFilter& destination) {
			destination.setCoeffs(coeffs);
		}

		void setFBInput(double _feedbackIn) { feedbackIn = _feedbackIn; }
		double getFBOutput() { return coeffs.beta * (sn + feedbackIn*coeffs.delta); }

	protected:
		DiodeVA1Coeffs coeffs;
		FilterOutput output;
		double sampleRate = 44100.0;				///< current sample rate
		double halfSamplePeriod = 1.0;
		double fc = 0.0;

		// --- state storage
		double sn = 0.0;						///< state variables
		double feedbackIn = 0.0;
	};
	
	struct VADiodeCoeffs
	{
		// --- filter coefficients

		double alpha0 = 0.0;		///< input scalar, correct delay-free loop
		double gamma = 0.0;		///< input scalar, correct delay-free loop
		double beta[4] = { 0.0, 0.0, 0.0, 0.0 };
		DiodeVA1Coeffs diodeCoeffs[4];
		double bassComp = 0.0;		// --- increase for MORE bass
		double alpha1 = 1.0;		// --- FGN amp correction
		double K = 1.0;		// --- 
	};

	// --- makes both LPF and HPF (double filter)
	class VADiodeFilter
	{
	public:
		// --- constructor/destructor
		VADiodeFilter();
		virtual ~VADiodeFilter() {}

		// --- these match SynthModule names
		virtual bool reset(double _sampleRate);
		virtual bool update();
		virtual FilterOutput* process(double xn); 
		virtual void setFilterParams(double _fc, double _Q);
	
		void setCoeffs(const VADiodeCoeffs& _coeffs) {
			coeffs = _coeffs; 

			// --- update subfilter coeffs
			for (uint32_t i = 0; i < DIODE_SUBFILTERS; i++)
			{
				subFilter[i].setCoeffs(coeffs.diodeCoeffs[i]);
				subFilterFGN[i].setCoeffs(coeffs.diodeCoeffs[i]);
			}
		}

		void copyCoeffs(VADiodeFilter& destination) {
			destination.setCoeffs(coeffs);
		}

	protected:
		FilterOutput output;
		VADiodeSubFilter subFilter[DIODE_SUBFILTERS];
		VADiodeSubFilter subFilterFGN[DIODE_SUBFILTERS];

		double sampleRate = 44100.0;				///< current sample rate
		double halfSamplePeriod = 1.0;
		double fc = 0.0;

		// --- filter coefficients
		VADiodeCoeffs coeffs;
	};



// -----------------------------
//	--- SynthLab SDK File --- // 
//  ----------------------------
/**
\file   vafilters.cpp
\author Will Pirkle
\brief  See also Designing Software Synthesizers in C++ 2nd Ed. by Will Pirkle
\date   20-April-2021
- http://www.willpirkle.com
*/
// -----------------------------------------------------------------------------
	VA1Filter::VA1Filter(){ }

	/** reset members to initialized state */
	bool VA1Filter::reset(double _sampleRate)
	{
		sampleRate = _sampleRate;
		halfSamplePeriod = 1.0 / (2.0 * sampleRate);
		sn = 0.0;
		output.clearData();
		return true;
	}

	void VA1Filter::setFilterParams(double _fc, double _Q)
	{
		if (fc != _fc)
		{
			fc = _fc;
			update();
		}
	}

	bool VA1Filter::update()
	{
		double g = tan(kTwoPi*fc*halfSamplePeriod); // (2.0 / T)*tan(wd*T / 2.0);
		coeffs.alpha = g / (1.0 + g);

		return true;
	}

	FilterOutput* VA1Filter::process(double xn)
	{
		// --- create vn node
		double vn = (xn - sn)*coeffs.alpha;

		// --- form LP output
		output.filter[LPF1] = ((xn - sn)*coeffs.alpha) + sn;

		// --- form the HPF = INPUT = LPF
		output.filter[HPF1] = xn - output.filter[LPF1];

		// --- form the APF = LPF - HPF
		output.filter[APF1] = output.filter[LPF1] - output.filter[HPF1];

		// --- anm output
		output.filter[ANM_LPF1] = output.filter[LPF1] + coeffs.alpha*output.filter[HPF1];

		// --- update memory
		sn = vn + output.filter[LPF1];

		return &output;
	}


	VASVFilter::VASVFilter() {}

	/** reset members to initialized state */
	bool VASVFilter::reset(double _sampleRate)
	{
		sampleRate = _sampleRate;
		halfSamplePeriod = 1.0 / (2.0 * sampleRate);
		integrator_z[0] = 0.0;
		integrator_z[1] = 0.0;
		output.clearData();

		return true;
	}

	void VASVFilter::setFilterParams(double _fc, double _Q)
	{
		// --- use mapping function for Q -> K
		mapDoubleValue(_Q, 1.0, 0.707, SVF_Q_SLOPE);

		if (fc != _fc || Q != _Q)
		{
			fc = _fc;
			Q = _Q;
			update();
		}
	}

	bool VASVFilter::update()
	{
		// --- prewarp the cutoff- these are bilinear-transform filters
		coeffs.alpha = tan(kTwoPi*fc*halfSamplePeriod);

		// --- note R is the traditional analog damping factor zeta
		double R = 1.0 / (2.0*Q);
		coeffs.alpha0 = 1.0 / (1.0 + 2.0*R*coeffs.alpha + coeffs.alpha*coeffs.alpha);

		// --- for max Q, go to self-oscillation (HOMEWORK!)
		if (Q > 24.9)
			coeffs.rho = coeffs.alpha;
		else
			coeffs.rho = 2.0*R + coeffs.alpha;

		// --- sigma for analog matching version
		double theta_c = (sampleRate / 2.0) / fc;
		coeffs.sigma = 1.0 / (coeffs.alpha*theta_c*theta_c);
	
		return true;
	}

	FilterOutput* VASVFilter::process(double xn)
	{
		// --- form the HP output first
		output.filter[HPF2] = coeffs.alpha0*(xn - coeffs.rho*integrator_z[0] - integrator_z[1]);

		// --- BPF Out
		output.filter[BPF2] = coeffs.alpha*output.filter[HPF2] + integrator_z[0];

		// --- LPF Out
		output.filter[LPF2] = coeffs.alpha*output.filter[BPF2] + integrator_z[1];

		// --- BSF Out
		output.filter[BSF2] = output.filter[HPF2] + output.filter[LPF2];

		// --- finite gain at Nyquist; slight error at VHF
		double sn = integrator_z[0];

		output.filter[ANM_LPF2] = output.filter[LPF2] + coeffs.sigma*(sn);

		// update memory
		integrator_z[0] = coeffs.alpha*output.filter[HPF2] + output.filter[BPF2];
		integrator_z[1] = coeffs.alpha*output.filter[BPF2] + output.filter[LPF2];

		return &output;
	}


	VAKorg35Filter::VAKorg35Filter() {}

	/** reset members to initialized state */
	bool VAKorg35Filter::reset(double _sampleRate)
	{
		sampleRate = _sampleRate;
		halfSamplePeriod = 1.0 / (2.0 * sampleRate);

		for (uint32_t i = 0; i < KORG_SUBFILTERS; i++)
		{
			lpfVAFilters[i].reset(_sampleRate);
			hpfVAFilters[i].reset(_sampleRate);
		}

		output.clearData();
		return true;
	}

	void VAKorg35Filter::setFilterParams(double _fc, double _Q)
	{
		// --- use mapping function for Q -> K
		mapDoubleValue(_Q, 1.0, 0.707, KORG35_Q_SLOPE);

		if (fc != _fc || coeffs.K != _Q)
		{
			fc = _fc;
			coeffs.K = _Q;
			update();
		}
	}

	bool VAKorg35Filter::update()
	{
		coeffs.g = tan(kTwoPi*fc*halfSamplePeriod); // (2.0 / T)*tan(wd*T / 2.0);
		coeffs.alpha = coeffs.g / (1.0 + coeffs.g);

		// --- alpha0 same for LPF, HPF
		coeffs.alpha0 = 1.0 / (1.0 - coeffs.K*coeffs.alpha + coeffs.K*coeffs.alpha*coeffs.alpha);

		// --- three sync-tuned filters
		for (uint32_t i = 0; i < KORG_SUBFILTERS; i++)
		{
			lpfVAFilters[i].setAlpha(coeffs.alpha);
			hpfVAFilters[i].setAlpha(coeffs.alpha);
		}

		// --- set filter beta values
		lpfVAFilters[FLT2].setBeta((coeffs.K * (1.0 - coeffs.alpha)) / (1.0 + coeffs.g));
		lpfVAFilters[FLT3].setBeta(-1.0 / (1.0 + coeffs.g));

		hpfVAFilters[FLT2].setBeta(-coeffs.alpha / (1.0 + coeffs.g));
		hpfVAFilters[FLT3].setBeta(1.0 / (1.0 + coeffs.g));

		return true;
	}

	FilterOutput* VAKorg35Filter::process(double xn)
	{
		// --- lowpass
		// --- process input through LPF1
		FilterOutput* tempOut;
		tempOut = lpfVAFilters[FLT1].process(xn);

		// --- form S35
		double S35 = lpfVAFilters[FLT2].getFBOutput() + lpfVAFilters[FLT3].getFBOutput();

		// --- calculate u
		double u = coeffs.alpha0*(tempOut->filter[ANM_LPF1] + S35);

		// --- feed it to LPF2
		tempOut = lpfVAFilters[FLT2].process(u);

		// --- output = LPF*K
		output.filter[LPF2] = tempOut->filter[LPF1] * coeffs.K;
		output.filter[ANM_LPF2] = tempOut->filter[ANM_LPF1] * coeffs.K;

		// --- feed output to HPF, no need to gather it's output
		lpfVAFilters[FLT3].process(output.filter[LPF2]);

		// --- HIGHPASS: 
		// --- process input through HPF1
		tempOut = hpfVAFilters[FLT1].process(xn);

		// --- then: form feedback and feed forward values (read before write)
		S35 = hpfVAFilters[FLT2].getFBOutput() + hpfVAFilters[FLT3].getFBOutput();

		// --- calculate u
		u = coeffs.alpha0*(tempOut->filter[HPF1] + S35);

		//---  form HPF output
		output.filter[HPF2] = coeffs.K*u;

		// --- process through feedback path
		tempOut = hpfVAFilters[FLT2].process(output.filter[HPF2]);

		// --- continue to LPF, no need to gather it's output
		hpfVAFilters[FLT3].process(tempOut->filter[HPF1]);

		// auto-normalize
		if (coeffs.K > 0)
		{
			output.filter[ANM_LPF2] *= 1.0 / coeffs.K;
			output.filter[LPF2] *= 1.0 / coeffs.K;
			output.filter[HPF2] *= 1.0 / coeffs.K;
		}

		return &output;
	}

	VAMoogFilter::VAMoogFilter()
	{
	}

	/** reset members to initialized state */
	bool VAMoogFilter::reset(double _sampleRate)
	{
		sampleRate = _sampleRate;
		halfSamplePeriod = 1.0 / (2.0 * sampleRate);

		for (uint32_t i = 0; i < MOOG_SUBFILTERS; i++)
		{
			subFilter[i].reset(_sampleRate);
			subFilterFGN[i].reset(_sampleRate);
		}

		output.clearData();
		return true;
	}

	void VAMoogFilter::setFilterParams(double _fc, double _Q)
	{
		// --- use mapping function for Q -> K
		mapDoubleValue(_Q, 1.0, 0.0, MOOG_Q_SLOPE);

		if (fc != _fc || coeffs.K != _Q)
		{
			fc = _fc;
			coeffs.K = _Q;
			update();
		}
	}

	bool VAMoogFilter::update()
	{
		coeffs.g = tan(kTwoPi*fc*halfSamplePeriod); // (2.0 / T)*tan(wd*T / 2.0);
		coeffs.alpha = coeffs.g / (1.0 + coeffs.g);

		// --- alpha0 
		coeffs.alpha0 = 1.0 / (1.0 + coeffs.K*coeffs.alpha*coeffs.alpha*coeffs.alpha*coeffs.alpha);
		double kernel = 1.0 / (1.0 + coeffs.g);

		// --- pre-calculate for distributing to subfilters
		coeffs.subFilterBeta[FLT4] = kernel;
		coeffs.subFilterBeta[FLT3] = coeffs.alpha * coeffs.subFilterBeta[FLT4];
		coeffs.subFilterBeta[FLT2] = coeffs.alpha * coeffs.subFilterBeta[FLT3];
		coeffs.subFilterBeta[FLT1] = coeffs.alpha * coeffs.subFilterBeta[FLT2];

		// --- four sync-tuned filters
		for (uint32_t i = 0; i < MOOG_SUBFILTERS; i++)
		{
			// --- set alpha - no calculation required
			subFilter[i].setAlpha(coeffs.alpha);
			subFilterFGN[i].setAlpha(coeffs.alpha);

			// --- set beta - no calculation required
			subFilter[i].setBeta(coeffs.subFilterBeta[i]);
			subFilterFGN[i].setBeta(coeffs.subFilterBeta[i]);
		}

		return true;
	}

	FilterOutput* VAMoogFilter::process(double xn)
	{
		// --- 4th order MOOG:
		double sigma = 0.0;

		for (uint32_t i = 0; i < MOOG_SUBFILTERS; i++)
			sigma += subFilter[i].getFBOutput();

		// --- gain comp 
		xn *= 1.0 + coeffs.bassComp*coeffs.K; // --- bassComp is hard coded

		// --- now figure out u(n) = alpha0*[x(n) - K*sigma]
		double u = coeffs.alpha0*(xn - coeffs.K * sigma);

		// --- send u -> LPF1 and then cascade the outputs to form y(n)
		FilterOutput* subFltOut[4];
		FilterOutput* subFltOutFGN[4];

		subFltOut[FLT1] = subFilter[FLT1].process(u);
		subFltOut[FLT2] = subFilter[FLT2].process(subFltOut[FLT1]->filter[LPF1]);
		subFltOut[FLT3] = subFilter[FLT3].process(subFltOut[FLT2]->filter[LPF1]);
		subFltOut[FLT4] = subFilter[FLT4].process(subFltOut[FLT3]->filter[LPF1]);

		// --- optional outputs 1,2,3
		output.filter[LPF1] = subFltOut[FLT1]->filter[LPF1];
		output.filter[LPF2] = subFltOut[FLT2]->filter[LPF1];
		output.filter[LPF3] = subFltOut[FLT3]->filter[LPF1];

		// --- MOOG LP4 output
		output.filter[LPF4] = subFltOut[FLT4]->filter[LPF1];

		// --- OPTIONAL: analog nyquist matched version
		subFltOutFGN[FLT1] = subFilterFGN[FLT1].process(u);
		subFltOutFGN[FLT2] = subFilterFGN[FLT2].process(subFltOutFGN[FLT1]->filter[ANM_LPF1]);
		subFltOutFGN[FLT3] = subFilterFGN[FLT3].process(subFltOutFGN[FLT2]->filter[ANM_LPF1]);
		subFltOutFGN[FLT4] = subFilterFGN[FLT4].process(subFltOutFGN[FLT3]->filter[ANM_LPF1]);

		// --- optional outputs 1,2,3
		output.filter[ANM_LPF1] = subFltOutFGN[FLT1]->filter[ANM_LPF1];
		output.filter[ANM_LPF2] = subFltOutFGN[FLT2]->filter[ANM_LPF1];
		output.filter[ANM_LPF3] = subFltOutFGN[FLT3]->filter[ANM_LPF1];

		// --- MOOG LP4 output
		output.filter[ANM_LPF4] = subFltOutFGN[FLT4]->filter[ANM_LPF1];

		return &output;
	}


	VADiodeSubFilter::VADiodeSubFilter()
	{
	}

	/** reset members to initialized state */
	bool VADiodeSubFilter::reset(double _sampleRate)
	{
		sampleRate = _sampleRate;
		halfSamplePeriod = 1.0 / (2.0 * sampleRate);
		sn = 0.0;
		feedbackIn = 0.0;
		output.clearData();

		return true;
	}

	void VADiodeSubFilter::setFilterParams(double _fc, double _Q)
	{
		if (fc != _fc)
		{
			fc = _fc;
			update();
		}
	}

	bool VADiodeSubFilter::update()
	{
		double g = tan(kTwoPi*fc*halfSamplePeriod);
		coeffs.alpha = g / (1.0 + g);

		return true;
	}

	FilterOutput* VADiodeSubFilter::process(double xn)
	{
		// --- specific to diode
		xn = (xn * coeffs.gamma) + feedbackIn + (coeffs.epsilon * getFBOutput());

		// --- v(n)
		double vn = ((coeffs.alpha0 * xn) - sn) * coeffs.alpha;

		// --- form LP output
		output.filter[LPF1] = vn + sn;

		// --- update memory
		sn = vn + output.filter[LPF1];

		// --- do the HPF
		output.filter[HPF1] = xn - output.filter[LPF1];

		// --- will this work?
		output.filter[ANM_LPF1] = output.filter[LPF1] + coeffs.alpha * output.filter[HPF1];

		return &output;
	}

	VADiodeFilter::VADiodeFilter()
	{
		coeffs.diodeCoeffs[FLT1].alpha0 = 1.0;
		coeffs.diodeCoeffs[FLT2].alpha0 = 0.5;
		coeffs.diodeCoeffs[FLT3].alpha0 = 0.5;
		coeffs.diodeCoeffs[FLT4].alpha0 = 0.5;

		coeffs.diodeCoeffs[FLT4].delta = 0.0;
		coeffs.diodeCoeffs[FLT4].epsilon = 0.0;
		coeffs.beta[3] = 1.0;
	}


	/** reset members to initialized state */
	bool VADiodeFilter::reset(double _sampleRate)
	{
		sampleRate = _sampleRate;
		halfSamplePeriod = 1.0 / (2.0 * sampleRate);

		for (uint32_t i = 0; i < MOOG_SUBFILTERS; i++)
		{
			subFilter[i].reset(_sampleRate);
			subFilterFGN[i].reset(_sampleRate);
		}

		output.clearData();
		return true;
	}

	void VADiodeFilter::setFilterParams(double _fc, double _Q)
	{
		// --- use mapping function for Q -> K
		mapDoubleValue(_Q, 1.0, 0.0, DIODE_Q_SLOPE);

		if (fc != _fc || coeffs.K != _Q)
		{
			fc = _fc;
			coeffs.K = _Q;
			update();
		}
	}

	const double fgn_a = 1.005381;
	const double fgn_b = 0.8783896;
	const double fgn_c = 1.113067;
	const double fgn_d = -0.2110344;

	bool VADiodeFilter::update()
	{
		double g = tan(kTwoPi*fc*halfSamplePeriod);

		double G4 = 0.5*g / (1.0 + g);
		double G3 = 0.5*g / (1.0 + g - 0.5*g*G4);
		double G2 = 0.5*g / (1.0 + g - 0.5*g*G3);
		double G1 = g / (1.0 + g - g*G2);

		// --- our coeffs
		coeffs.gamma = G4*G3*G2*G1;
		coeffs.alpha0 = 1.0 / (1.0 + coeffs.K * coeffs.gamma);

		coeffs.beta[0] = G4*G3*G2;
		coeffs.beta[1] = G4*G3;
		coeffs.beta[2] = G4;

		// --- analog 
		double f = kTwoPi*fc / sampleRate;
		double e = pow((f / fgn_c), fgn_b);
		coeffs.alpha1 = fgn_d + (fgn_a - fgn_d) / (1.0 + e);

		// --- diode sub-filter coeffs
		// 
		// -- alphas
		double alpha = g / (1.0 + g);

		coeffs.diodeCoeffs[FLT1].alpha = alpha;
		coeffs.diodeCoeffs[FLT2].alpha = alpha;
		coeffs.diodeCoeffs[FLT3].alpha = alpha;
		coeffs.diodeCoeffs[FLT4].alpha = alpha;

		// --- betas
		coeffs.diodeCoeffs[FLT1].beta = 1.0 / (1.0 + g - g*G2);
		coeffs.diodeCoeffs[FLT2].beta = 1.0 / (1.0 + g - 0.5*g*G3);
		coeffs.diodeCoeffs[FLT3].beta = 1.0 / (1.0 + g - 0.5*g*G4);
		coeffs.diodeCoeffs[FLT4].beta = 1.0 / (1.0 + g);

		// --- gammas
		coeffs.diodeCoeffs[FLT1].gamma = 1.0 + G1*G2;
		coeffs.diodeCoeffs[FLT2].gamma = 1.0 + G2*G3;
		coeffs.diodeCoeffs[FLT3].gamma = 1.0 + G3*G4;
		coeffs.diodeCoeffs[FLT4].gamma = 1.0;

		// --- deltas
		coeffs.diodeCoeffs[FLT1].delta = g;
		coeffs.diodeCoeffs[FLT2].delta = 0.5*g;
		coeffs.diodeCoeffs[FLT3].delta = 0.5*g;

		// set epsilons
		coeffs.diodeCoeffs[FLT1].epsilon = G2;
		coeffs.diodeCoeffs[FLT2].epsilon = G3;
		coeffs.diodeCoeffs[FLT3].epsilon = G4;

		// --- update subfilter coeffs
		for (uint32_t i = 0; i < DIODE_SUBFILTERS; i++)
		{
			subFilter[i].setCoeffs(coeffs.diodeCoeffs[i]);
			subFilterFGN[i].setCoeffs(coeffs.diodeCoeffs[i]);
		}

		return true;
	}

	FilterOutput* VADiodeFilter::process(double xn) 
	{
		// --- must be processed in this order, with function calls in args
		subFilter[FLT4].setFBInput(0.0);
		subFilterFGN[FLT4].setFBInput(0.0);

		subFilter[FLT3].setFBInput(subFilter[FLT4].getFBOutput());
		subFilterFGN[FLT3].setFBInput(subFilter[FLT4].getFBOutput());

		subFilter[FLT2].setFBInput(subFilter[FLT3].getFBOutput());
		subFilterFGN[FLT2].setFBInput(subFilter[FLT3].getFBOutput());

		subFilter[FLT1].setFBInput(subFilter[FLT2].getFBOutput());
		subFilterFGN[FLT1].setFBInput(subFilter[FLT2].getFBOutput());

		// form input
		double sigma = coeffs.beta[0] * subFilter[FLT1].getFBOutput() +
			coeffs.beta[1] * subFilter[FLT2].getFBOutput() +
			coeffs.beta[2] * subFilter[FLT3].getFBOutput() +
			coeffs.beta[3] * subFilter[FLT4].getFBOutput();

		// --- gain comp 
		xn *= 1.0 + (coeffs.bassComp * coeffs.K); // --- bassComp is hard coded

		// form input
		double u = (xn - (coeffs.K * sigma)) * coeffs.alpha0;

		// cascade of four filters
		FilterOutput* subFltOut[4];
		FilterOutput* subFltOutFGN[4];

		subFltOut[FLT1] = subFilter[FLT1].process(u);
		subFltOut[FLT2] = subFilter[FLT2].process(subFltOut[FLT1]->filter[LPF1]);
		subFltOut[FLT3] = subFilter[FLT3].process(subFltOut[FLT2]->filter[LPF1]);
		subFltOut[FLT4] = subFilter[FLT4].process(subFltOut[FLT3]->filter[LPF1]);

		subFltOutFGN[FLT1] = subFilterFGN[FLT1].process(u);
		subFltOutFGN[FLT2] = subFilterFGN[FLT2].process(subFltOutFGN[FLT1]->filter[ANM_LPF1]);
		subFltOutFGN[FLT3] = subFilterFGN[FLT3].process(subFltOutFGN[FLT2]->filter[ANM_LPF1]);
		subFltOutFGN[FLT4] = subFilterFGN[FLT4].process(subFltOutFGN[FLT3]->filter[ANM_LPF1]);

		// --- output is last filter's LPF1 out
		output.filter[LPF4] = subFltOut[FLT4]->filter[LPF1];
		output.filter[ANM_LPF4] = coeffs.alpha1 * subFltOutFGN[FLT4]->filter[ANM_LPF1];

		return &output;
	}
