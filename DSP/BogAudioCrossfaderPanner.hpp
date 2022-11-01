
#pragma once

#include "BogAudioDSP.hpp"

namespace DSP::BogAudio
{


    struct CrossFader : public Parameter2Processor {
		double _mix = 2.0f;
		double _curve = 1.0f;
		bool _linear = true;
		double _aMix;
		double _bMix;
		Amplifier _aAmp;
		Amplifier _bAmp;

		CrossFader() : Parameter2Processor() {
			setParams(0.0f);
		}

		void setParams(
			double mix, // -1 to 1, 0 for equal output of both inputs.
			double curve = 1.0f, // -1 to 1: at -1, A will cut fully as mix goes to 0; at 0, A cuts over full mix; at 1, A cuts from 0 to 1.  B symmetric.
			bool linear = true// cut is linear in amplitude if true; linear in decibels otherwise.
		);
		double next(double a, double b);
		enum {
			PORT_MIX,
			PORT_CURVE,
			PORT_LINEAR
		};
		void setPort(int port, double v) {
			switch(port) {
				case PORT_MIX: setParams(v); break;
				case PORT_CURVE: setParams(_mix,v); break;
				case PORT_LINEAR: setParams(_mix,_curve,(bool)v); break;
			}
		}
		double Tick(double a, double b) {			
			return next(a,b);
		}
	};

	struct Panner : public StereoSplitterProcessor {
		double _pan = 2.0f;
		double _lLevel = 0.0f;
		double _rLevel = 0.0f;
		const Table& _sineTable;

		Panner() : 
		StereoSplitterProcessor(),
		_sineTable(StaticSineTable::table()) {
			setPan(0.0f);
		}

		void setPan(double pan); // -1.0 full left, 0.0 even, 1.0f full right.
		void next(double sample, double& l, double& r);

		enum {
			PORT_PAN,			
		};
		void setPort(int port, double v) {
			switch(port) {
				case PORT_PAN: setPan(v); break;
			}
		}
		double Tick(double in, double &L, double &R) {
			double l=0;
			double r=0;
			next(in,l,r);
			L = l;
			R = r;
			return 0.5*(l+r);
		}
	};

    void CrossFader::setParams(double mix, double curve, bool linear) {
		assert(mix >= -1.0f && mix <= 1.0f);
		assert(curve >= -1.0f && curve <= 1.0f);
		if (_mix != mix || _curve != curve || _linear != linear) {
			_mix = mix;
			_curve = curve;
			_linear = linear;

			double aMax, aMin;
			double bMax, bMin;
			if (_curve < 0.0f) {
				aMax = 0.0f;
				aMin = _curve + 2.0f;
				bMax = 2.0f;
				bMin = 0.0f - _curve;
			}
			else {
				aMax = _curve;
				aMin = 2.0f;
				bMax = 2.0f - _curve;
				bMin = 0.0f;
			}

			double m = _mix + 1.0f;
			if (m < aMax) {
				_aMix = 1.0f;
			}
			else if (m > aMin) {
				_aMix = 0.0f;
			}
			else {
				_aMix = 1.0f - ((m - aMax) / (aMin - aMax));
			}

			if (m > bMax) {
				_bMix = 1.0f;
			}
			else if (m < bMin) {
				_bMix = 0.0f;
			}
			else {
				_bMix = (m - bMin) / (bMax - bMin);
			}

			if (!_linear) {
				_aAmp.setLevel((1.0f - _aMix) * Amplifier::minDecibels);
				_bAmp.setLevel((1.0f - _bMix) * Amplifier::minDecibels);
			}
		}
	}

	double CrossFader::next(double a, double b) {
		if (_linear) {
			return _aMix * a + _bMix * b;
		}
		return _aAmp.next(a) + _bAmp.next(b);
	}


	void Panner::setPan(double pan) {
		assert(pan >= -1.0f);
		assert(pan <= 1.0f);
		if (_pan != pan) {
			_pan = pan;
			_lLevel = _sineTable.value(((1.0f + _pan) / 8.0f + 0.25f) * _sineTable.length());
			_rLevel = _sineTable.value(((1.0f + _pan) / 8.0f) * _sineTable.length());
		}
	}

	void Panner::next(double sample, double& l, double& r) {
		l = _lLevel * sample;
		r = _rLevel * sample;
	}


}