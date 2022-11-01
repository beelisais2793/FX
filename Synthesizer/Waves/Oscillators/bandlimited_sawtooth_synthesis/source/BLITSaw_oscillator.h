/*
 * BLITSaw_oscillator.h
 *
 * Copyright (c) 2014, fukuroda (https://github.com/fukuroder)
 * Released under the MIT license
 */

#pragma once
#include <array>
#include "pluginterfaces/vst/ivstevents.h"

using namespace Steinberg;
using namespace Steinberg::Vst;
namespace MyVst {
	//
	class BLITSaw_oscillator_note
	{
	public:
		BLITSaw_oscillator_note();

		// ADSR
		enum ADSR
		{
			//
			Off,

			//
			On,
		};

		//
		ADSR envelope;

		//
		double t;

		//
		int32 sampleOffset;

		//
		double value;

		//
		int n;

		//
		double dt;

		//
		int32 note_id;

		//
		double velocity;

	protected:

		//
		static const int _note_no_center = 69;
	};

	//
	class BLITSaw_oscillator
	{
	public:
		// constructor
		BLITSaw_oscillator();

		//
		void setLeak(double value);

		//
		void trigger(const NoteOnEvent& noteOn, int32 sampleOffset, double srate);

		//
		void release(const NoteOffEvent& noteOff);

		//
		bool is_silent();

		//
		double render();

		//
		void next();

	protected:

		//
		static const int _note_no_center = 69;

		//
		std::array<BLITSaw_oscillator_note, 8> _notes;

		//
		std::array<double, (1 << 10) + 1> _sinTable;

		//
		double _Leak;

		//
		double LinearInterpolatedSin(double x);

		//
		double BLIT(double t, int n);
	};
} //  namespace
