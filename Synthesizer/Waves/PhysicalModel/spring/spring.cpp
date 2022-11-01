
/*
 This file is part of Virtual Robot's Pine Synthesizer. 

Copyright <2022> <Brian Gunnison>

Permission is hereby granted, free of charge, to any person obtaining a copy of this 
software and associated documentation files (the "Software"), to deal in the Software 
without restriction, including without limitation the rights to use, copy, modify, 
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
permit persons to whom the Software is furnished to do so, subject to the following 
conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include <string.h>
#include "daisy_pod.h"
#include "daisysp.h"
#include "utilities.h"
#include "voice.h"
#include "filter.h"
#include "midimap.h"

using namespace daisy;
using namespace daisysp;

#define MIDI_CHANNEL 0
#define AUDIO_BLOCK_SIZE 48

DaisyPod   hw;
CpuLoadMeter loadMeter;
Voices  voice;
Filters filt;
Parameter volumePot;
CCMIDIMap ccmap;
PCMIDIMap pcmap;
CCMIDINoteMap noteMap;

float sampleRate;
float finalGain;

uint32_t noteCounter = 0;
uint32_t outClipIndicator = 0;

uint8_t currentCpuLoad = 0;



void logMidiEvent(MidiEvent *m)
{	
	if (m->type == NoteOn)
	{
		NoteOnEvent p = m->AsNoteOn();
		log("%d - Note on:\t%d\t%d\t%d", noteCounter, p.channel, p.note, p.velocity);	
		noteCounter++;
	}
	
	if (m->type == NoteOff)
	{
		NoteOffEvent p = m->AsNoteOff();
		log("%d - Note off:\t%d\t%d\t%d", noteCounter, p.channel, p.note, p.velocity);	
		noteCounter--;
	}
	
	if (m->type == ControlChange)
	{
		ControlChangeEvent p = m->AsControlChange();
		 
		log("%d - CC:\t%d\t%d\t%d", noteCounter, p.channel, p.control_number, p.value);	
	}
	
	if (m->type == ProgramChange)
	{
		ProgramChangeEvent  p = m->AsProgramChange();
		 
		log("%d - PG:\t%d\t%d\t%d", noteCounter, p.channel, p.program);	
	}
	
	

	noteCounter++;
}


void UpdateControls()
{
	hw.ProcessAnalogControls();
	hw.ProcessDigitalControls();

	// returns 0 - max
	finalGain = volumePot.Process();
	
	//Returns + 1 if the encoder was turned clockwise, -1 if it was turned counter - clockwise, or 0 if it was not just turned.
	//voice.Select(hw.encoder.Increment());	}
	
	// use button1 for voice select
	voice.Select(hw.button1.RisingEdge());
	filt.Select(hw.button2.RisingEdge());
}


void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
	AudioHandle::InterleavingOutputBuffer out,
	size_t                                size)
{
	float sig = 0;
	loadMeter.OnBlockStart();
	
	UpdateControls();

	for (size_t i = 0; i < size; i += 2)
	{
		sig = filt.Process(voice.Process()) * finalGain; 
		
		if (sig > 1.0)
		{
			outClipIndicator++;
		}
		
		out[i] = out[i + 1] = sig;
	}
	
	loadMeter.OnBlockEnd();
	
	currentCpuLoad = (uint8_t)(loadMeter.GetAvgCpuLoad() * 100);

}

// Typical Switch case for Message Type.
void HandleMidiMessage(MidiEvent m)
{
	//logMidiEvent(&m);
	//if (m.channel != MIDI_CHANNEL)
	//{
	//	return;
	//}
	
	switch (m.type)
	{
	case NoteOn:
		{
			//logMidiNote(&m);
			NoteOnEvent p = m.AsNoteOn();
			RotateLED(2);
			p = m.AsNoteOn();
			p.note = noteMap.Process(p.note);
			voice.NoteOn(&p); 					
		}
		break;

	case NoteOff:
		{
			NoteOffEvent p = m.AsNoteOff();
			p.note = noteMap.Process(p.note);
			voice.NoteOff(&p);
			break;
		}

	case ControlChange:
		{
			ControlChangeEvent p = m.AsControlChange();
			//log("CC: %d, v: %d", p.control_number, p.value);
			ccmap.Invoke(p.control_number, p.value);
			noteMap.Invoke(p.control_number, p.value);		
		}
	default: break;
	}
}

void SetCCFinalGain(uint8_t value)
{
	finalGain = float(value) / 127.0f;
}

// Main -- Init, and Midi Handling
int main(void)
{
	// Init
	hw.Init();
	hw.SetAudioBlockSize(AUDIO_BLOCK_SIZE);
	ComInit(&hw);

	//log("Init start");
	
	sampleRate = hw.AudioSampleRate();
	voice.Init(&hw, sampleRate);
	
	filt.Init(sampleRate);
	
	loadMeter.Init(sampleRate, AUDIO_BLOCK_SIZE);
	
	// allow clipping and adjust for low volume voices
	volumePot.Init(hw.knob1, 0, 16, Parameter::LOGARITHMIC);
	
	ccmap.Init();
	pcmap.Init();
	noteMap.Init();
	
	//ccmap.Add(7, SetCCFinalGain);
	SetAlesisV125MIDIMap(&ccmap, &noteMap, &voice, &filt);
	
	finalGain = 1.0;
	
	log("Init end");

	// Start stuff.
	hw.StartAdc();
	hw.StartAudio(AudioCallback);
	hw.midi.StartReceive();

	uint8_t cpuLoad = 0;
	
	uint32_t now = 0;
	for (;;)
	{
		
		uint32_t tp = System::GetNow() - now;
		if (tp > 5000)
		{
			now = System::GetNow();
			loadMeter.Reset();
			cpuLoad = currentCpuLoad;
			log("Ave CPU load Peak: %d", cpuLoad);
		}
		
		hw.midi.Listen();
		// Handle MIDI Events
		while (hw.midi.HasEvents())
		{
			HandleMidiMessage(hw.midi.PopEvent());
		}
				
		voice.UpdateBackGround();
		if (outClipIndicator > 0)
		{
			hw.seed.SetLed(true);
			outClipIndicator--;
		}
		else
		{
			hw.seed.SetLed(false);
		}
		
	}
}
