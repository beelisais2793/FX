#pragma once

#include "BogAudioDSP.hpp"


namespace DSP::BogAudio
{
	////////////////////////////////////////////////////////////////////////////////////////
	// Envelope
	////////////////////////////////////////////////////////////////////////////////////////
	struct EnvelopeGenerator : Generator {
		double _sampleRate = -1.0f;
		double _sampleTime;

		EnvelopeGenerator(double sampleRate = 1000.0f) : Generator() {
			setSampleRate(std::max(1.0f, sampleRate));
		}

		void setSampleRate(double sampleRate);
		virtual void _sampleRateChanged() {}
	};


    
	struct ADSR : EnvelopeGenerator {
		enum Stage {
			STOPPED_STAGE,
			ATTACK_STAGE,
			DECAY_STAGE,
			SUSTAIN_STAGE,
			RELEASE_STAGE
		};

		Stage _stage = STOPPED_STAGE;
		bool _gated = false;
		double _attack = 0.0f;
		double _decay = 0.0f;
		double _sustain = 1.0f;
		double _release = 0.0f;
		double _attackShape;
		double _decayShape;
		double _releaseShape;
		double _stageProgress = 0.0f;
		double _releaseLevel = 0.0f;
		double _envelope = 0.0f;

		ADSR(bool linear = false, double sampleRate = 1000.0f) : EnvelopeGenerator(sampleRate) {
			setLinearShape(linear);
		}

		void reset();
		void setGate(bool high);
		void setAttack(double seconds);
		void setDecay(double seconds);
		void setSustain(double level);
		void setRelease(double seconds);
		void setLinearShape(bool linear);
		void setShapes(double attackShape, double decayShape, double releaseShape);
		bool isStage(Stage stage) { return _stage == stage; }
		void retrigger();

		enum {
			PORT_RESET,
			PORT_GATE,
			PORT_ATTACK,
			PORT_DECAY,
			PORT_SUSTAIN,
			PORT_RELEASE,
			PORT_LINEARSHAPE,
			PORT_ATTACKSHAPE,
			PORT_DECAYSHAPE,
			PORT_RELEASESHAPE,
			PORT_RETRUGGER,
		};
		void setPort(int port, double v) {
			switch(port) {
				case PORT_RESET: reset(); break;
				case PORT_GATE: setGate((bool)v); break;
				case PORT_ATTACK: setAttack(v); break;
				case PORT_DECAY: setDecay(v); break;
				case PORT_SUSTAIN: setSustain(v); break;
				case PORT_RELEASE: setRelease(v); break;
				case PORT_LINEARSHAPE: setLinearShape((bool)v); break;
				case PORT_ATTACKSHAPE: setShapes(v,_decayShape,_releaseShape); break;
				case PORT_DECAYSHAPE: setShapes(_attackShape,v,_releaseShape); break;
				case PORT_RELEASESHAPE: setShapes(_attackShape,_decayShape,v); break;

			}
		}
		double _next() override;
	};


	void EnvelopeGenerator::setSampleRate(double sampleRate) {
		assert(sampleRate >= 1.0f);
		if (_sampleRate != sampleRate) {
			_sampleRate = sampleRate;
			_sampleTime = 1.0f / sampleRate;
			_sampleRateChanged();
		}
	}


	void ADSR::reset() {
		_stage = STOPPED_STAGE;
		_gated = false;
		_envelope = 0.0f;
	}

	void ADSR::setGate(bool high) {
		_gated = high;
	}

	void ADSR::setAttack(double seconds) {
		assert(_attack >= 0.0f);
		_attack = std::max(seconds, 0.001f);
	}

	void ADSR::setDecay(double seconds) {
		assert(_decay >= 0.0f);
		_decay = std::max(seconds, 0.001f);
	}

	void ADSR::setSustain(double level) {
		assert(_sustain >= 0.0f);
		assert(_sustain <= 1.0f);
		_sustain = level;
	}

	void ADSR::setRelease(double seconds) {
		assert(_release >= 0.0f);
		_release = std::max(seconds, 0.001f);
	}

	void ADSR::setLinearShape(bool linear) {
		if (linear) {
			setShapes(1.0f, 1.0f, 1.0f);
		}
		else {
			setShapes(0.5f, 2.0f, 2.0f);
		}
	}

	void ADSR::setShapes(double attackShape, double decayShape, double releaseShape) {
		assert(attackShape >= 0.1f && attackShape <= 10.0f);
		assert(decayShape >= 0.0f && decayShape <= 10.0f);
		assert(releaseShape >= 0.0f && releaseShape <= 10.0f);
		_attackShape = attackShape;
		_decayShape = decayShape;
		_releaseShape = releaseShape;
	}

	void ADSR::retrigger() {
		switch (_stage) {
			case STOPPED_STAGE: {
				_stage = ATTACK_STAGE;
				_stageProgress = 0.0f;
				break;
			}
			default: {
				_stage = ATTACK_STAGE;
				double e = powf(_envelope, 1.0f / _attackShape);
				_stageProgress = e * _attack;
			}
		}
	}

	double ADSR::_next() {
		if (_gated) {
			switch (_stage) {
				case STOPPED_STAGE: {
					_stage = ATTACK_STAGE;
					_stageProgress = 0.0f;
					break;
				}
				case ATTACK_STAGE: {
					if (_envelope >= 1.0) {
						_stage = DECAY_STAGE;
						_stageProgress = 0.0f;
					}
					break;
				}
				case DECAY_STAGE: {
					if (_stageProgress >= _decay) {
						_stage = SUSTAIN_STAGE;
						_stageProgress = 0.0f;
					}
					break;
				}
				case SUSTAIN_STAGE: {
					break;
				}
				case RELEASE_STAGE: {
					_stage = ATTACK_STAGE;
					_stageProgress = _attack * powf(_envelope, _releaseShape);
					break;
				}
			}
		}
		else {
			switch (_stage) {
				case STOPPED_STAGE: {
					break;
				}
				case ATTACK_STAGE:
				case DECAY_STAGE:
				case SUSTAIN_STAGE: {
					_stage = RELEASE_STAGE;
					_stageProgress = 0.0f;
					_releaseLevel = _envelope;
					break;
				}
				case RELEASE_STAGE: {
					if (_stageProgress >= _release) {
						_stage = STOPPED_STAGE;
					}
					break;
				}
			}
		}

		switch (_stage) {
			case STOPPED_STAGE: {
				_envelope = 0.0f;
				break;
			}
			case ATTACK_STAGE: {
				_stageProgress += _sampleTime;
				_envelope = std::min(1.0f, _stageProgress / _attack);
				_envelope = powf(_envelope, _attackShape);
				break;
			}
			case DECAY_STAGE: {
				_stageProgress += _sampleTime;
				_envelope = std::min(1.0f, _stageProgress / _decay);
				_envelope = powf(1.0f - _envelope, _decayShape);
				_envelope *= 1.0f - _sustain;
				_envelope += _sustain;
				break;
			}
			case SUSTAIN_STAGE: {
				_envelope = _sustain;
				break;
			}
			case RELEASE_STAGE: {
				_stageProgress += _sampleTime;
				_envelope = std::min(1.0f, _stageProgress / _release);
				_envelope = powf(1.0f - _envelope, _releaseShape);
				_envelope *= _releaseLevel;
				break;
			}
		}

		return _envelope;
	}
}