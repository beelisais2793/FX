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

void HiHatVoice::Init(float SR) 
{
	NullVoice::Init(SR);
	polyphony = HIHAT_VOICE_POLYPHONY;
	
	for (uint8_t i = 0; i < polyphony; i++)
	{
		hihat[i].Init(sampleRate);
	}	
	
	Panic();
}

void HiHatVoice::Panic() 
{
	NullVoice::Panic();
	
	//for (uint8_t i = 0; i < polyphony; i++)
	//{
	//	hihat[i].Reset();
	//}
}


void HiHatVoice::StartNote(uint8_t i, NoteOnEvent *p)
{
	//log("n: %d, m: %d", i, p->note);
	notes[i].midiNote = p->note;
	notes[i].amplitude = (float)p->velocity / 127.0f;
			
	hihat[i].SetFreq(mtof(p->note));
	hihat[i].Trig();
	
}

void HiHatVoice::NoteOn(NoteOnEvent *p)
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


void HiHatVoice::NoteOff(NoteOffEvent *p)
{
	for (uint8_t i = 0; i < polyphony; i++)
	{
		if (notes[i].midiNote == p->note)
		{
			notes[i].midiNote = 0;	
		}
	}
}


float HiHatVoice::Process(void) 
{
	float sig = 0.0;
	for (uint8_t i = 0; i < polyphony; i++)
	{
		sig += hihat[i].Process() * notes[i].amplitude;
	}
	
	return sig / polyphony;
}
	
void HiHatVoice::SetCC0(uint8_t value)
{
	SetDecayCC(value);
}

void HiHatVoice::SetCC1(uint8_t value)
{
	SetToneCC(value);
}

void HiHatVoice::SetCC2(uint8_t value)
{
	SetAccentCC(value);
}

void HiHatVoice::SetCC3(uint8_t value)
{
	SetNoisinessCC(value);
}



void HiHatVoice::SetDecayCC(uint8_t value)
{
	float set = (float)value / 127.0;
	if (set == decay)
	{
		return;
	}
	
	decay = set;
	for (uint8_t i = 0; i < polyphony; i++)
	{
		hihat[i].SetDecay(decay);
	}
}


void HiHatVoice::SetToneCC(uint8_t value)
{
	float set = (float)value / 127.0;
	if (set == tone)
	{
		return;
	}
	
	tone = set;
	for (uint8_t i = 0; i < polyphony; i++)
	{
		hihat[i].SetTone(tone);
	}
}


void HiHatVoice::SetAccentCC(uint8_t value)
{
	float set = (float)value / 127.0;
	if (set == accent)
	{
		return;
	}
	
	accent = set;
	for (uint8_t i = 0; i < polyphony; i++)
	{
		hihat[i].SetAccent(accent);
	}
}


void HiHatVoice::SetNoisinessCC(uint8_t value)
{
	float set = (float)value / 127.0;
	if (set == noisiness)
	{
		return;
	}
	
	noisiness = set;
	for (uint8_t i = 0; i < polyphony; i++)
	{
		hihat[i].SetNoisiness(noisiness);
	}
}


