//
//  LadderFilter.h
//  ladderFIlter
//
//  Created by Rares on 07/10/2020.
//

#pragma once


#include <math.h>

namespace Analog::Filters::LadderFilter
{
	static const DspFloatType TEMP = 30; 										///--> temperature in degrees Celsius
	static const DspFloatType THERMAL_VOLT = 8.617f*1e-5*(273.15f + TEMP); 	///--> thermal voltage, Vt
	static const DspFloatType OVER_TWO_THERMAL_VOLT = 1/(THERMAL_VOLT * 2.f); 	///--> precompute division to 2 * Vt
	static const int NUMBER_OF_FILTERS = 4; 							///--> number of filter stages

	/*!
	* \class LadderFilter
	*
	* \brief Physically-motivated model of a Moog lowpass ladder filter
	*
	* \author Rares
	*/
	template<class T>
	class LadderFilter : public FilterProcessor {
		
	public:
		/*! constructor for LadderFilter class
		\param osFactor Oversampling factor
		*/
		LadderFilter(int osFactor);
		
		/*! destructor for LadderFilter class
		*/
		virtual ~LadderFilter();
		
		/*! set sample rate for filter
		\param sampleRate plugin host sample rate, in Hz
		*/
		void setSampleRate(T sampleRate);
		
		/*! set cutoff frequency for filter by calculating the bias control current
		\param cutFreq desired cutoff frequency
		*/
		void setCutoffFreq(T cutFreq);
		
		/*! set resonance factor for filter
		\param resonance desired resonance factor
		*/
		void setResonance(T resonance);
		
		/*! set gain level for input signal
		\param dbLevel desired gain level for input signal in dB
		*/
		void setGain(T dbLevel);
		
		enum
        {
            PORT_CUTOFF,
            PORT_RESONANCE,
			PORT_GAIN,
        };
        void setPort(int port, DspFloatType v)
        {
            switch (port)
            {
            case PORT_CUTOFF:
                setCutoffFreq(v);
                break;
            case PORT_RESONANCE:
                setResonance(v);
                break;
			case PORT_GAIN:
				setGain(v);
				break;
            }
        }
		/*! process loop for filter
		\param inputSample sample from input signal
		\return filtered output sample
		*/
		T process(T inputSample);
		
		DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
			return A*process(I);
		}
	private:
		
		// variables
		T mSampleRate{44100.f}; 			///--> filter sample rate
		T mResonance{0.f};					///--> resonance factor
		T mCutoffFreq{0.f};					///--> cutoff frequency
		T mBiasControl{0.f};				///--> bias control factor, which represents the control current source in the analog circuit
		T mGain{1.f};						///--> input signal gain level (converted from dB)
		int mOversampleFactor{1};			///--> oversampling factor
		T mvVolt[NUMBER_OF_FILTERS];		///--> array of voltage output at each filter stage at current time step, V[n]
		T mvVolt_1[NUMBER_OF_FILTERS];		///--> array of voltage output at each filter stage at previous time step, V[n-1]
		T mvDiffVolt[NUMBER_OF_FILTERS];	///--> array of voltage output differential at each filter stage at current time step, dV[n]
		T mvDiffVolt_1[NUMBER_OF_FILTERS];	///--> array of voltage output differential at each filter stage at previous time step, dV[n-1]
	};

	template <class T>
	LadderFilter<T>::LadderFilter(int osFactor) : FilterProcessor()
	{
		// initialize voltage arrays
		for(int index = 0; index < NUMBER_OF_FILTERS; ++index)
		{
			mvVolt[index] = 0.f;
			mvVolt_1[index] = 0.f;
			mvDiffVolt[index] = 0.f;
			mvDiffVolt_1[index] = 0.f;
		}
		
		// define oversampling factor
		mOversampleFactor = osFactor;
	}

	template <class T>
	LadderFilter<T>::~LadderFilter()
	{
		
	}

	template <class T>
	void LadderFilter<T>::setSampleRate(T sampleRate)
	{
		mSampleRate = mOversampleFactor*sampleRate;
	}

	template <class T>
	void LadderFilter<T>::setCutoffFreq(T cutFreq)
	{

		mCutoffFreq = cutFreq;
		T var = M_PI * mCutoffFreq / mSampleRate;
		
		// mBiasControl = I_ctl / (2*C)
		// where I_ctl - control current intensity
		// and C - capacitor value used at the output of each filter stage
		mBiasControl = 4.f * M_PI * THERMAL_VOLT * mCutoffFreq * (1.f - var)/(1.f + var);
		
	}

	template <class T>
	void LadderFilter<T>::setResonance(T resonance)
	{
		mResonance = resonance;
	}

	template <class T>
	void LadderFilter<T>::setGain(T dbLevel)
	{
		mGain = pow(10.0,dbLevel/20.0);
	}

	template <class T>
	T LadderFilter<T>::process(T inputSample)
	{
		for(int i = 0; i < mOversampleFactor; ++i) // repeat sample processing per oversampling factor
		{
			for(int filterIndex = 0; filterIndex < NUMBER_OF_FILTERS; ++filterIndex) // go through filter stages
			{
				// compute voltage differential at current time step
				if(filterIndex == 0) // for the first filter
				{
					mvDiffVolt[filterIndex] = -mBiasControl * (tanh(mvVolt_1[filterIndex] * OVER_TWO_THERMAL_VOLT) + tanh((mGain * inputSample + mResonance * mvVolt_1[NUMBER_OF_FILTERS-1]) * OVER_TWO_THERMAL_VOLT));
				}
				else // for the rest of the filters
				{
					mvDiffVolt[filterIndex] = mBiasControl * (tanh(mvVolt[filterIndex-1] * OVER_TWO_THERMAL_VOLT) - tanh(mvVolt_1[filterIndex] * OVER_TWO_THERMAL_VOLT));
				}
				
				// compute voltage output for current filter at current time step
				mvVolt[filterIndex] += (mvDiffVolt[filterIndex] +  mvDiffVolt_1[filterIndex])/(2*mSampleRate);
				
				// update voltages from previous time step
				mvVolt_1[filterIndex] = mvVolt[filterIndex];
				mvDiffVolt_1[filterIndex] = mvDiffVolt[filterIndex];
			}
		}
		
		// return the voltage output of the last filter
		return mvVolt[NUMBER_OF_FILTERS-1];
	}
}
