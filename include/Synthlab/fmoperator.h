#pragma once

#include "synthbase.h"
//#include "guiconstants.h"
#include "bleptables.h"
#include "synthfunctions.h"
#include "synthlabwtsource.h"
#include "fmocore.h"

// -----------------------------
//	--- SynthLab SDK File --- // 
//  ----------------------------
/**
\file   fmoperator.h
\author Will Pirkle
\brief  See also Designing Software Synthesizers in C++ 2nd Ed. by Will Pirkle
\date   20-April-2021
- http://www.willpirkle.com
*/
// -----------------------------------------------------------------------------
namespace SynthLab
{
	/**
	\class FMOperator
	\ingroup SynthModules
	\brief
	FMOperator module.
	- loads and supports up to NUM_MODULE_CORES (4) cores
	- it is mainly a wrapper/container for the module cores that do the acutal processing/rendering.

	The cores include (zero-indexed):
	0. FMOCoreFM implements the 8 Yamaha DX-100 algorithms. 
	1. --- EMPTY ---
	2. --- EMPTY ---
	3. --- EMPTY ---

	Base Class: SynthModule
	- Overrides the five (5) common functions plus a special getParameters() method to
	return a shared pointer to the parameters structure.

	Databases: WavetableDatabase
	- registers the sinusoidal wavetable in the database
	- recalls the sinusoidal table from the database during the update() function
	- can add more wavetables as an option

	GUI Parameters: FMOperatorParameters
	- getParameters() function allows direct access to std::shared_ptr<FMOperatorParameters>

	std::shared_ptr<FMOperatorParameters> getParameters()

	- call the getParameters() function
	- set the parameters in the FMOperatorParameters structure with new values, typically from a GUI
	To apply these new parameters either:
	- (a) call the module's update() function OR
	- (b) call the render() function which in turn calls the update() method.

	Ordinarily, this operation happens just prior to calling the render() function so that is the preferred
	method of operation to avoid multiple calls to the update() function, which is usually the most CPU intensive
	function of the SynthModule.

	Access to Modulators
	- std::shared_ptr<Modulators> getModulationInput()
	- std::shared_ptr<Modulators> getModulationOutput()

	Access to audio buffers (I/O)
	- std::shared_ptr<AudioBuffer> getAudioBuffers()

	Reads:
	- Modulation Input values (modulators)
	- AudioBuffer Input samples for PhaseModulation (FM Synthesis)

	Writes:
	- AudioBuffer Output samples

	Construction:

	(1) For use within a synth project, the constructor
	is specialized to use shared recources for:
	- MidiInputData
	- FMOperatorParameters
	- WavetableDatabase

	The owning object (SynthVoice for the SynthLab projects) must pass these valid pointers
	to the object at construction time. Typically the engine or voice will be the primary synthesizers
	of these resources. See the 2nd Edition Synth Book for more information.

	(2) Standalone:

	To use in standalone mode, call the constructor with the shared resoure pointers as null:

	FMOperator(nullptr, nullptr, nullptr, 64);

	In standalone mode, the object creates and maintains these resources:
	- MidiInputData: this has no read access so you cannot access its data. Ordinarily, you
	pass this shared pointer into the object so that it has access to the current MIDI input data.
	The object does not write data into this resource, so it is functionally non-opeational.

	- WavetableDatabase: this has no read access so you cannot access its data. Ordinarily, you
	pass this shared pointer into the object so that it can share these resources with other like-objects.
	However, in standalone operation the object will create the database and then register its wavetables
	with the object and use this local database for rendering operations. The code is the same for all operations
	so the object does not know if it is using a local database or a shared one.

	- FMOperatorParameters: in standalone mode only, these are synthesized locally on the object,
	and then the owning object may obtain a shared pointer to them to read/write the parameters directly.

	Render:
	- renders into its own AudioBuffers object; see SynthModule::getAudioBuffers()
	- renders stereo by default. For wavetables this really means dual-mono where the
	left buffer is copied to the right.

	\author Will Pirkle http://www.willpirkle.com
	\remark This object is included and described in further detail in
	Designing Software Synthesizer Plugins in C++ 2nd Ed. by Will Pirkle
	\version Revision : 1.0
	\date Date : 2021 / 04 / 26
	*/
	class FMOperator : public SynthModule
	{
	public:
		/** One and only specialized constructor; pointers may be null for stanalone */
		FMOperator(std::shared_ptr<MidiInputData> _midiInputData,
			std::shared_ptr<FMOperatorParameters> _parameters,
			std::shared_ptr<WavetableDatabase> _waveTableDatabase,
			uint32_t blockSize = 32);

		/** Destructor is empty: all resources are smart pointers */
		virtual ~FMOperator() {}/* D-TOR */

		/** SynthModule Overrides */
		virtual bool reset(double _sampleRate) override;
		virtual bool update() override;
		virtual bool render(uint32_t samplesToProcess = 1) override;
		virtual bool doNoteOn(MIDINoteEvent& noteEvent) override;
		virtual bool doNoteOff(MIDINoteEvent& noteEvent) override;

		/** For standalone operation only; not used in SynthLab synth projects */
		std::shared_ptr<FMOperatorParameters> getParameters() { return parameters; }

	protected:
		/** For standalone operation only; not used in SynthLab synth projects */
		std::shared_ptr<FMOperatorParameters> parameters = nullptr;

		/** For standalone operation only; not used in SynthLab synth projects */
		std::shared_ptr<WavetableDatabase> waveTableDatabase = nullptr;
	};


}
