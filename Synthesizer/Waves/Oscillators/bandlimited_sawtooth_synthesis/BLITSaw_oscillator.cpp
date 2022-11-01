/*
 * BLITSaw_oscillator.cpp
 *
 * Copyright (c) 2014, fukuroda (https://github.com/fukuroder)
 * Released under the MIT license
 */

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include "BLITSaw_oscillator.h"

namespace MyVst {

	// constructor
	BLITSaw_oscillator_note::BLITSaw_oscillator_note()
		:envelope(Off)
		, t(0.0)
		, sampleOffset(0)
		, value(0.0)
		, n(0)
		, dt(0.0)
	{
	}

	//
	void BLITSaw_oscillator::trigger(const NoteOnEvent& noteOn, int32 sampleOffset, double srate)
	{
		//
		auto available_note = std::find_if(
			_notes.begin(),
			_notes.end(),
			[](const BLITSaw_oscillator_note& n){return n.envelope == BLITSaw_oscillator_note::Off; });

		if (available_note != _notes.end()){
			available_note->note_id = noteOn.noteId;
			available_note->velocity = noteOn.velocity;
			available_note->envelope = BLITSaw_oscillator_note::On;

			//
			double freq = 440.0*(::pow(2.0, (noteOn.pitch - _note_no_center) / 12.0));
			available_note->n = static_cast<int>(srate / 2.0 / freq);
			available_note->dt = freq / srate;
			available_note->value = 0.0;
			available_note->t = 0.5;
			available_note->sampleOffset = sampleOffset;
		}
	}

	//
	void BLITSaw_oscillator::release(const NoteOffEvent& noteOff)
	{
		const int32 note_id = noteOff.noteId;
		auto target_note = std::find_if(
			_notes.begin(),
			_notes.end(),
			[note_id](const BLITSaw_oscillator_note& n){return n.note_id == note_id; });

		if (target_note != _notes.end()){
			//
			target_note->envelope = BLITSaw_oscillator_note::Off;
		}
	}

	bool BLITSaw_oscillator::is_silent()
	{
		return std::all_of(
			_notes.begin(),
			_notes.end(),
			[](const BLITSaw_oscillator_note& n){return n.envelope == BLITSaw_oscillator_note::Off; });
	}

	// constructor
	BLITSaw_oscillator::BLITSaw_oscillator()
	{
		// sine wave table
		for (size_t ii = 0; ii < _sinTable.size(); ii++)
		{
			_sinTable[ii] = sin(2.0*M_PI * ii / (_sinTable.size() - 1));
		}

		_Leak = 0.995;
	}

	//
	void BLITSaw_oscillator::setLeak(double value)
	{
		_Leak = value;
	}

	//
	double BLITSaw_oscillator::LinearInterpolatedSin(double x)
	{
		//
		double pos = (_sinTable.size() - 1) * x;

		//
		unsigned int idx_A = static_cast<int>(pos);

		//
		double s = pos - idx_A;

		//
		return (1.0 - s) * _sinTable[idx_A] + s*_sinTable[idx_A + 1];
	}

	//
	double BLITSaw_oscillator::BLIT(double t, int n)
	{
		//
		double den = LinearInterpolatedSin(0.5*t);

		if (den < 1.0e-12)
		{
			return 2.0*(2 * n + 1);
		}

		double num = LinearInterpolatedSin(::fmod((n + 0.5)*t, 1.0));

		return 2.0*num / den;
	}

	//
	double BLITSaw_oscillator::render()
	{
		double value = 0.0;
		for (auto &note : _notes)
		{
			if (note.envelope == BLITSaw_oscillator_note::On){
				// add
				value += note.value * note.velocity;
			}
		}
		return value;
	}

	//
	void BLITSaw_oscillator::next()
	{
		for (auto& note : _notes)
		{
			if (note.envelope == BLITSaw_oscillator_note::On){

				if (note.sampleOffset > 0) {
					note.sampleOffset--;
					return;
				}

				// add
				note.t += note.dt;
				if (1.0 <= note.t) {
					note.t -= 1.0;
				}

				note.value = note.value * _Leak + (BLIT(note.t, note.n) - 2.0) * note.dt;
			}
		}
	}

} // namespace