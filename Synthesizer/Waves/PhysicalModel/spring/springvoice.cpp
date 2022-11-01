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
#include "daisy_pod.h"
#include "daisysp.h"

#include "utilities.h"
#include "voice.h"

using namespace daisy;
using namespace daisysp;

void SpringVoice::Init(float SR) 
{
	NullVoice::Init(SR);
	polyphony = SPRING_VOICE_POLYPHONY;
	
	for (uint8_t i = 0; i < polyphony; i++)
	{
		spring[i].Init(sampleRate);
	}	
	
	Panic();
}

void SpringVoice::Panic() 
{
	NullVoice::Panic();
	
	for (uint8_t i = 0; i < polyphony; i++)
	{
		spring[i].Reset();
	}
}


void SpringVoice::StartNote(uint8_t i, NoteOnEvent *p)
{
	//log("n: %d, m: %d", i, p->note);
	notes[i].midiNote = p->note;
	notes[i].amplitude = (float)p->velocity / 127.0f;
			
	spring[i].SetFreq(mtof(p->note));
	spring[i].Trig();
	
}

void SpringVoice::NoteOn(NoteOnEvent *p)
{
	for (uint8_t i = 0; i < polyphony; i++)
	{
		
		if (notes[i].midiNote == p->note)
		{
			// if we are playing the same note
			StartNote(i, p);
			return;
		}
		
		// start new note if unused and (todo) adsr is done 
		if (notes[i].midiNote == 0 )
		{
			StartNote(i, p);
			return;	
		}
	}
}


void SpringVoice::NoteOff(NoteOffEvent *p)
{
	for (uint8_t i = 0; i < polyphony; i++)
	{
		if (notes[i].midiNote == p->note)
		{
			notes[i].midiNote = 0;	
		}
	}
}


float SpringVoice::Process(void) 
{
	float sig = 0.0;
	for (uint8_t i = 0; i < polyphony; i++)
	{
		sig += spring[i].Process() * notes[i].amplitude;
	}
	
	return sig / polyphony;
}
	
void SpringVoice::SetCC0(uint8_t value)
{
	SetDampingCC(value);
}

void SpringVoice::SetCC1(uint8_t value)
{
	SetStructureCC(value);
}

void SpringVoice::SetCC2(uint8_t value)
{
	SetBrightnessCC(value);
}

void SpringVoice::SetCC3(uint8_t value)
{
	SetAccentCC(value);
}



void SpringVoice::SetDampingCC(uint8_t value)
{
	float set = (float)value / 127.0;
	if (set == damping)
	{
		return;
	}
	
	damping = set;
	for (uint8_t i = 0; i < polyphony; i++)
	{
		spring[i].SetDamping(damping);
	}
}


void SpringVoice::SetStructureCC(uint8_t value)
{
	float set = (float)value / 127.0;
	if (set == structure)
	{
		return;
	}
	
	structure = set;
	for (uint8_t i = 0; i < polyphony; i++)
	{
		spring[i].SetStructure(structure);
	}
}


void SpringVoice::SetBrightnessCC(uint8_t value)
{
	float set = (float)value / 127.0;
	if (set == brightness)
	{
		return;
	}
	
	brightness = set;
	for (uint8_t i = 0; i < polyphony; i++)
	{
		spring[i].SetBrightness(brightness);
	}
}


void SpringVoice::SetAccentCC(uint8_t value)
{
	float set = (float)value / 127.0;
	if (set == accent)
	{
		return;
	}
	
	accent = set;
	for (uint8_t i = 0; i < polyphony; i++)
	{
		spring[i].SetAccent(accent);
	}
}


