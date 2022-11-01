#pragma once

#include "BogAudioDSP.hpp"


namespace DSP::BogAudio
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Equalizer
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct Equalizer : Filter {
		static constexpr double gainDb = 12.0f;
		static constexpr double cutDb = -36.0f;

		Amplifier _lowAmp;
		Amplifier _midAmp;
		Amplifier _highAmp;
		FourPoleButtworthLowpassFilter _lowFilter;
		TwoPoleButtworthBandpassFilter _midFilter;
		FourPoleButtworthHighpassFilter _highFilter;

		void setParams(
			double sampleRate,
			double lowDb,
			double midDb,
			double highDb
		);
		enum {
			PORT_LOWDB,
			PORT_MIDDB,
			PORT_HIGHDB,
		};
		void setPort(int port, double v) {
			switch(port) {
				case PORT_LOWDB: setParams(sampleRate,v,_midAmp,_highAmp); break;
				case PORT_MIDDB: setParams(sampleRate,_lowAmp,v,_highAmp); break;
				case PORT_HIGHDB: setParams(sampleRate,_lowAmp,_midAmp,v); break;
			}
		}
		double next(double sample) override;
	};

	void Equalizer::setParams(
		double sampleRate,
		double lowDb,
		double midDb,
		double highDb
	) {
		assert(lowDb >= cutDb && lowDb <= gainDb);
		assert(midDb >= cutDb && midDb <= gainDb);
		assert(highDb >= cutDb && highDb <= gainDb);

		_lowAmp.setLevel(lowDb);
		_lowFilter.setParams(sampleRate, 100.0f, 0.0f);

		_midAmp.setLevel(midDb);
		_midFilter.setParams(sampleRate, 350.0f, 0.55f, MultimodeFilter::PITCH_BANDWIDTH_MODE);

		_highAmp.setLevel(highDb);
		_highFilter.setParams(sampleRate, 1000.0f, 0.0f);
	}

	double Equalizer::next(double sample) {
		double low = _lowAmp.next(_lowFilter.next(sample));
		double mid = _midAmp.next(_midFilter.next(sample));
		double high = _highAmp.next(_highFilter.next(sample));
		return low + mid + high;
	}
}