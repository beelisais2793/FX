
#pragma once

#include "BogAudioDSP.hpp"

namespace DSP::BogAudio
{
	struct PositiveZeroCrossing {
		const float positiveThreshold = 0.01f;
		const float negativeThreshold = -positiveThreshold;
		const int zeroesForReset = 20;

		enum State {
			NEGATIVE_STATE,
			POSITIVE_STATE,
			COUNT_ZEROES_STATE
		};

		State _state;
		bool _triggerable;
		int _zeroCount = 0;

		PositiveZeroCrossing(bool triggerable = true)
		: _triggerable(triggerable)
		{
			reset();
		}

		bool next(float sample);
		void reset();
	};

    	bool PositiveZeroCrossing::next(float sample) {
		switch (_state) {
			case NEGATIVE_STATE: {
				if (sample > positiveThreshold) {
					_state = POSITIVE_STATE;
					return true;
				}
				break;
			}
			case POSITIVE_STATE: {
				if (sample < negativeThreshold) {
					_state = NEGATIVE_STATE;
				}
				else if (sample < positiveThreshold && _triggerable) {
					_state = COUNT_ZEROES_STATE;
					_zeroCount = 1;
				}
				break;
			}
			case COUNT_ZEROES_STATE: {
				if (sample >= negativeThreshold) {
					if (++_zeroCount >= zeroesForReset) {
						_state = NEGATIVE_STATE;
					}
				}
				else {
					_state = NEGATIVE_STATE;
				}
				break;
			}
		}
		return false;
	}

	void PositiveZeroCrossing::reset() {
		_state = NEGATIVE_STATE;
	}


}