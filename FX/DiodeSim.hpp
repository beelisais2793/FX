#pragma once

namespace FX::Distortion::Diode
{
	typedef enum
	{
		//for a non-inverting topology
		SAT_NON_INVERTING = 0,
		SAT_INVERTING,
		SAT_NOISE_SUPPRESSOR,
		SAT_NUM
	}ESaturationType;

	//various typical diode types
	typedef enum
	{
		DIO_SCHOTTKY = 0,
		DIO_JUNCTION,
		DIO_ZENER_2V4,
		DIO_ZENER_3V0,
		DIO_ZENER_3V9,
		DIO_LED,
		DIO_NUM
	}EDiodeType;

	typedef enum
	{
		//pots with two pins shorted
		POT_LINEAR = 0,
		POT_LOG,
		//pot with three wires
		POT_LIN_SPLIT,
		POT_NUM
	}EPotType;

	//maximum output voltage
	#define SATURATION_MAX_VOLTAGE 3.8f
	#define MAX(a,b) (a>b?a:b)
	#define MIN(a,b) (a<b?a:b)
	#define CLAMP(x,min,max) ( MIN(MAX(min, x), max) )

	//diagram of a typical 3-segment pot
	DspFloatType logPot(const DspFloatType fPos)
	{
		if(fPos < .05f)
			return 0;
		if(fPos < .25f)
			return (fPos - .05f) * .04f/.2f;
		if(fPos < 2.0f/3.0f)
			return .04f + (fPos - .25f) * (.28f-.04f) / (2.0f/3.0f - .25f);
		if(fPos > .95)
			return 1;
		return .28f + (fPos - 2.0f/3.0f) * (1.0f - .28f) / (0.95f - 2.0f/3.0f);
	}

	void
	saturation(const DspFloatType *pIn, DspFloatType *pOut, const uint nSamples,
			const ESaturationType satType, const uint nDiodes,
			const EDiodeType* pDiodeTypes, const EPotType potType,
			DspFloatType R1, DspFloatType R2, const DspFloatType R3,
			const DspFloatType fGainSetting);

	void
	saturationSigned(const DspFloatType *pIn, DspFloatType *pOut, const uint nSamples,
			const ESaturationType satType, const uint nDiodes,
			const EDiodeType* pDiodeTypes, const EPotType potType,
			DspFloatType ground, DspFloatType pot, const DspFloatType series,
			const DspFloatType fGainSetting);

	inline DspFloatType
	diodeVoltageCached(const DspFloatType fLogC1, const DspFloatType fOverC2, const DspFloatType fLogI)
	{
		const DspFloatType fDiff = fLogI - fLogC1;
		return fDiff > 0 ? fDiff*fOverC2 : 0;
	}

	inline DspFloatType
	diodeCurrent(const DspFloatType C1, const DspFloatType C2, const DspFloatType Ud)
	{
		if(Ud < 0.0f)
			return 0;
		return C1 * expf(Ud * C2);
	}

	//diode params[0] -> C1
	//diode params[1] -> C2
	//cached params[0] -> log(C1)
	//cached params[1] -> C2^(-1)
	inline DspFloatType
	feedbackVoltageCached(const DspFloatType Id, const DspFloatType R3, DspFloatType* const aafDiodeParams[2], DspFloatType* const aafCachedParams[2], const uint nDiodes)
	{
		DspFloatType fLogId = logf(Id);

		DspFloatType Ud = 0;
		uint uiDiode = 0;
		for(; uiDiode < nDiodes; uiDiode++)
			if(Id > aafDiodeParams[0][uiDiode])
				Ud += diodeVoltageCached(aafCachedParams[0][uiDiode], aafCachedParams[1][uiDiode], fLogId);

		return Ud + R3 * Id;
	}

	inline DspFloatType
	feedbackCurrentCached(const DspFloatType Id, const DspFloatType fOverR2, const DspFloatType R3, DspFloatType* const aafDiodeParams[2], DspFloatType* const aafCachedParams[2], const uint nDiodes)
	{
		return feedbackVoltageCached(Id, R3, aafDiodeParams, aafCachedParams, nDiodes) * fOverR2 + Id;
	}

	inline DspFloatType
	feedbackCurrentDerivativeCached(const DspFloatType Id, const DspFloatType fOverR2, const DspFloatType R3, DspFloatType* const aafDiodeParams[2], DspFloatType* const aafCachedParams[2], const uint nDiodes)
	{
		DspFloatType dIf = 1.0f;
		DspFloatType fOverId = 1.0f / Id;

		uint uiDiode = 0;
		for(; uiDiode < nDiodes; uiDiode++)
			if(Id > aafDiodeParams[0][uiDiode])
				dIf += fOverId * aafCachedParams[1][uiDiode] * fOverR2;

		return dIf;
	}

	//R1, R2, C1, C2
	//resistance to ground / virtual ground, defining current
	//feedback resistance, defining gain at zero signal
	//resistance for diodes
	//constants for diode, defining saturation


	//parameters for diode power law
	//Id = C1 * expf(C2 * Ud)
	DspFloatType g_acfDiodes[DIO_NUM][2] = {
		{0.000000001f, 23.0258509299f},
		{8.37391804372927E-011f, 10.0589869527f},
		{1.87381742286039E-008f, 41.865183509f},
		{4.53158363760083E-008f, 7.1955784156f},
		{3.98107170553499E-008f, 4.605170186f},
		{0.00000001f, 3.8376418217f}
	};

	//a minimum value for R1, R2
	#define R_MIN 3.0f

	void
	saturation(const DspFloatType *pIn, DspFloatType *pOut, const uint nSamples,
			const ESaturationType satType, const uint nDiodes,
			const EDiodeType* const pDiodeTypes, const EPotType potType,
			DspFloatType R1, DspFloatType R2, const DspFloatType R3,
			const DspFloatType fGainSetting)
	{
	#ifdef SPAM_OUTPUT
		puts("running saturation");
	#endif

	#ifdef _MSC_VER
		DspFloatType* afDiodeC1 = (DspFloatType*)calloc(nDiodes, sizeof(DspFloatType));
		DspFloatType* afDiodeC2 = (DspFloatType*)calloc(nDiodes, sizeof(DspFloatType));
	#else
		DspFloatType afDiodeC1[nDiodes];
		DspFloatType afDiodeC2[nDiodes];
	#endif
		
		DspFloatType* const aafDiodeParams[2]={afDiodeC1, afDiodeC2};
		uint uiDiode = 0;
		for(; uiDiode < nDiodes; uiDiode++)
		{
	#ifdef SPAM_OUTPUT
			printf("diode %d is type %d\n", uiDiode, pDiodeTypes[uiDiode]);
	#endif

			aafDiodeParams[0][uiDiode] = g_acfDiodes[pDiodeTypes[uiDiode]][0];
			aafDiodeParams[1][uiDiode] = g_acfDiodes[pDiodeTypes[uiDiode]][1];
		}

	#ifdef SPAM_OUTPUT
		printf("R1 = %f\n", R1);
	#endif

	#ifdef SPAM_OUTPUT
		printf("R1 = %f\n", R1);
	#endif

		switch(potType)
		{
		case POT_LINEAR:
	#ifdef SPAM_OUTPUT
			puts("pot is linear.");
	#endif

			R2 *= fGainSetting;
			break;
		case POT_LOG:
		{	//simulate a pot with 60 dB wipe
	#ifdef SPAM_OUTPUT
			puts("pot is log.");
	#endif

			DspFloatType fdBAttenuation = -60.0f * (1.0f - fGainSetting);
			R2 *= powf(10.0f, fdBAttenuation/20.0f);
			break;
		}
		//pot with three wires
		case POT_LIN_SPLIT:
	#ifdef SPAM_OUTPUT
			puts("pot is split.");
	#endif

			R1 += R2 * (1.0f-fGainSetting);

			R2 *= fGainSetting;
			break;
		default:
			break;
		}

		R2 = fmaxf(R_MIN, R2);
		R1 = fmaxf(R_MIN, R1);

		//minimum feedback current for which to solve
		//-140dB is plenty, well below A/D noise
		//DspFloatType fMinFeedbackCurrent = 1e-7f * SATURATION_MAX_VOLTAGE / R1;
		DspFloatType fMinFeedbackCurrent = 1e-8f * SATURATION_MAX_VOLTAGE / R1;

		//uint iters = 0;

		//calculate cached inverse resistor values
		const DspFloatType fOverR1 = 1.0f / R1;
		const DspFloatType fOverR2 = 1.0f / R2;
		//calculate cached diode values

	#ifdef _MSC_VER
		DspFloatType* afCached1 = (DspFloatType*)calloc(nDiodes, sizeof(DspFloatType));
		DspFloatType* afCached2 = (DspFloatType*)calloc(nDiodes, sizeof(DspFloatType));
	#else
		DspFloatType afCached1[nDiodes];
		DspFloatType afCached2[nDiodes];
	#endif


		DspFloatType* const aafCachedParams[2]={afCached1, afCached2};
		uiDiode = 0;
		for(; uiDiode < nDiodes; uiDiode++)
		{
			aafCachedParams[0][uiDiode] = logf(aafDiodeParams[0][uiDiode]);
			aafCachedParams[1][uiDiode] = 1.0f/aafDiodeParams[1][uiDiode];
		}

		uint uiSample = 0;
		for(; uiSample < nSamples; uiSample++)
		{
			DspFloatType Ucurr = pIn[uiSample];
	#ifdef SPAM_OUTPUT
			printf("processing voltage %f\n", Ucurr);
	#endif

			//inverting saturation is the simplest to calculate
	#ifdef SPAM_OUTPUT
			printf("calculating inverting saturation. R1=%f R2=%f R3=%f Uin=%f\n", R1, R2, R3, Ucurr);
	#endif
			DspFloatType If = Ucurr * fOverR1;

	#ifdef SPAM_OUTPUT
			printf("number of diodes is %d; If=%.10e\n", nDiodes, If);
	#endif

	#ifdef SPAM_OUTPUT
			puts("finding root of equation");
	#endif
			//current through diode must be somewhere between zero and total current
			//epsilon to ensure good approximation
			DspFloatType fIdMax = fminf(If, diodeCurrent(aafDiodeParams[0][0], aafDiodeParams[1][0], R2 * If));
			//find the diode current by -100dB of the total feedback current
			//limit accuracy for low signal level to absolute minimum current
			//const DspFloatType fEpsilon = fmaxf(If * 1e-5f, fMinFeedbackCurrent);
			const DspFloatType fEpsilon = fmaxf(If * 1e-6f, fMinFeedbackCurrent);

			//minimum bound diode current
			DspFloatType fIdMin = 0;

			//find root of monotonic ascending function, must be in [fIdMin : fIdMax]
			//Newton with upper and lower bounds, defaults to bisection if convergence is bad

			//feedback current for lower and upper bound
			DspFloatType fIfMin = 0;
			DspFloatType fIfMax = feedbackCurrentCached(fIdMax, fOverR2, R3, aafDiodeParams, aafCachedParams, nDiodes);

			//best values of search
			DspFloatType Id_best = fIdMax;
			DspFloatType If_best = fIfMax;

			while(fabsf(If_best - If) > fEpsilon )
			{
				//calculate guess for Id
				DspFloatType Id_current;

				//start point is the bound closer to the objective
				//use newtons method, first order
				if(fabsf(fIfMax - If) > fabsf(fIfMin - If) )
				{
					const DspFloatType fDMin = feedbackCurrentDerivativeCached(fIdMin, fOverR2, R3, aafDiodeParams, aafCachedParams,nDiodes);
					Id_current = fIdMin + (If - fIfMin) / fDMin;
				}
				else
				{
					const DspFloatType fDMax = feedbackCurrentDerivativeCached(fIdMax, fOverR2, R3, aafDiodeParams, aafCachedParams,nDiodes);
					Id_current = fIdMax - (fIfMax - If) / fDMax;
				}

				//if out of bounds, default to bisection
				if(Id_current <= fIdMin || Id_current >= fIdMax)
					Id_current = 0.5f * (fIdMin + fIdMax);

				//calculate target function
				const DspFloatType If_current = feedbackCurrentCached(Id_current, fOverR2, R3, aafDiodeParams, aafCachedParams, nDiodes);

				//check if the current value is new lower or new upper bound
				if(If_current - If < 0)
				{
					fIfMin = If_current;
					fIdMin = Id_current;
				}
				else
				{
					fIfMax = If_current;
					fIdMax = Id_current;
				}

				//break when numerically exhausted
				if(If_best == If_current || Id_best == Id_current)
					break;

				//write output
				Id_best = Id_current;
				If_best = If_current;

				//iters += 1;
			}

			DspFloatType Uout = 0;
			//check which branch carries most current
			if(Id_best < If*0.5f)
			//resistance dominates -> low voltages, linear region
				Uout = - (If - Id_best) * R2;
			else
			//diodes dominate -> high voltages, nonlinear region
				Uout = -feedbackVoltageCached(Id_best, R3, aafDiodeParams, aafCachedParams, nDiodes);

	#ifdef SPAM_OUTPUT
			printf("Id = %.10e; (If - Id) = %.10e Uo = %.6f\n", Id_best, If-Id_best, Uout);
	#endif

			//check which one is actually used and alter value to that
			switch(satType)
			{
			case SAT_NON_INVERTING:
	#ifdef SPAM_OUTPUT
				puts("saturation is non-inverting.");
	#endif
				Uout = Ucurr - Uout;
				break;
			case SAT_INVERTING:
	#ifdef SPAM_OUTPUT
				puts("saturation is inverting.");
	#endif
				//do nothing
				break;
			case SAT_NOISE_SUPPRESSOR:
	#ifdef SPAM_OUTPUT
				puts("saturation is a noise suppressor.");
				printf("R1 = %f\n", R1);
	#endif
				//calculate voltage divider for input and inverting saturation
				Uout = (Uout * R1 + Ucurr * R2) / (R1 + R2);
	#ifdef SPAM_CURVES
				printf("%f\t%f\n", Ucurr, Uout);
	#endif
				break;
			default:
				break;
			}

			//clamp to maximum permissible voltage
			pOut[uiSample] = CLAMP(Uout, -SATURATION_MAX_VOLTAGE, SATURATION_MAX_VOLTAGE);
		}
		//printf("%.10f\t%.10f\n", pIn[nSamples - 1], pOut[nSamples - 1]);
		//printf("Uin = %.10f UOut = %.10f iters = %.2f\n", pIn[nSamples - 1], pOut[nSamples - 1], (DspFloatType)iters / (DspFloatType)nSamples);

	#ifdef SPAM_CURVES
		puts("");
	#endif


	#ifdef SPAM_CURVES
		if(satType == SAT_NOISE_SUPPRESSOR)
				exit(0);
	#endif

	#ifdef _MSC_VER
		free(afCached1);
		free(afCached2);
		free(afDiodeC1);
		free(afDiodeC2);
	#endif
	}


	void saturationSigned(const DspFloatType *pIn, DspFloatType *pOut, const uint nSamples,
			const ESaturationType satType, const uint nDiodes,
			const EDiodeType* pDiodeTypes, const EPotType potType,
			DspFloatType ground, DspFloatType pot, const DspFloatType series,
			const DspFloatType fGainSetting)
	{
		uint i = 0;

		DspFloatType* afTempIn = (DspFloatType*) calloc(nSamples, sizeof(DspFloatType));

		for(; i < nSamples; i++)
			afTempIn[i] = fabsf(pIn[i]);

		saturation(afTempIn, pOut, nSamples,
					satType, nDiodes, pDiodeTypes, potType,
					ground, pot, series,
					fGainSetting);

		i = 0;
		for(; i < nSamples; i++)
		//saturation calculated using absolute voltages
		//copy sign of input and saturation curve to output
		//multiplication for inverting/non-inverting circuit
			pOut[i] = copysignf(pOut[i], pIn[i] * pOut[i]);


		free(afTempIn);
	}
}