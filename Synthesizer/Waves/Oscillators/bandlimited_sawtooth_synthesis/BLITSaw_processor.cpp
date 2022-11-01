/*
 * BLITSaw_processor.cpp
 *
 * Copyright (c) 2014, fukuroda (https://github.com/fukuroder)
 * Released under the MIT license
 */

#include "BLITSaw_processor.h"
#include "BLITSaw_guids.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include <algorithm>

namespace MyVst {
	//
	BLITSaw_processor::BLITSaw_processor()
	{
		setControllerClass(BLITSawControllerID);
	}

	//
	FUnknown* BLITSaw_processor::create(void* context)
	{
		return (IAudioProcessor*)new BLITSaw_processor();
	}

	//
	tresult PLUGIN_API BLITSaw_processor::initialize(FUnknown* context)
	{
		// base class initialization 
		tresult result = AudioEffect::initialize(context);
		if (result != kResultOk)
		{
			return result;
		}

		// set bus
		addAudioOutput(STR16("Stereo Out"), SpeakerArr::kStereo);

		return kResultOk;
	}

	//
	tresult PLUGIN_API BLITSaw_processor::setBusArrangements(
		SpeakerArrangement* inputs,
		int32 numIns,
		SpeakerArrangement* outputs,
		int32 numOuts
		){
		if (numIns == 0 && numOuts == 1 && outputs[0] == SpeakerArr::kStereo)
		{
			return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
		}
		return kResultFalse;
	}

	//
	tresult PLUGIN_API BLITSaw_processor::process(ProcessData& data)
	{
		//-------------------
		// update parameters
		//-------------------
		if (data.inputParameterChanges)
		{
			int32 numParamsChanged = data.inputParameterChanges->getParameterCount();
			for (int32 ii = 0; ii < numParamsChanged; ii++)
			{
				IParamValueQueue* paramQueue = data.inputParameterChanges->getParameterData(ii);
				if (paramQueue)
				{
					int32 offsetSamples;
					double value;
					// 
					if (paramQueue->getPoint(paramQueue->getPointCount() - 1, offsetSamples, value) == kResultTrue)
					{
						ParamID id = paramQueue->getParameterId();
						if (id == Leak)
						{
							// -> [0.99, 1.0]
							double Leak = 0.99 + 0.01 * value;
							blit.setLeak(Leak);
						}
					}
				}
			}
		}

		//----------------
		// process events
		//----------------
		if (data.inputEvents)
		{
			int nEventCount = data.inputEvents->getEventCount();

			for (int ii = 0; ii < nEventCount; ii++)
			{
				Event e;
				tresult result = data.inputEvents->getEvent(ii, e);
				if (result != kResultOk)continue;

				if (e.type == Event::kNoteOnEvent)
				{
					blit.trigger(e.noteOn, e.sampleOffset, processSetup.sampleRate);
				}
				else if (e.type == Event::kNoteOffEvent)
				{
					blit.release(e.noteOff);
				}
			}
		}

		if (blit.is_silent())
		{
			return kResultOk;
		}

		//
		if (data.numInputs == 0 && data.numOutputs == 1 && data.outputs[0].numChannels == 2)
		{
			Sample32** out = data.outputs[0].channelBuffers32;
			const int32 sampleFrames = data.numSamples;
			for (int ii = 0; ii < sampleFrames; ii++)
			{
				out[0][ii] = out[1][ii] = blit.render();
				blit.next();
			}
		}
		return kResultOk;
	}
} // namespace
