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

void NullVoice::Init(float SR)
{
	sampleRate = SR;
	polyphony = 0;
	Panic();
}

void NullVoice::Panic()
{
	//log("Null voice Panic");
	
	for (uint8_t i = 0; i < MAX_POLYPHONY; i++)
	{
		notes[i].amplitude = 0.0;
		notes[i].midiNote = 0;
	}
}


void Voices::Init(DaisyPod *pod, float SR) 
{ 
	phw = pod; 
	sampleRate = SR; 
	currentVoiceSelector = 0; 
	oscVoice.Init(sampleRate);
	springVoice.Init(sampleRate);
	malletVoice.Init(sampleRate);
	hihatVoice.Init(sampleRate);
	noiseVoice.Init(sampleRate);
	
	pvoice = &oscVoice;
}

void Voices::Panic(void)
{
	pvoice->Panic();
}

void Voices::ChangeVoice(uint8_t sel)
{
	
	if (sel != currentVoiceSelector)
	{
		Panic();
	}
	
	currentVoiceSelector = sel;
	
	switch (sel)
	{
	case SYNTH_VOICE:
		log("Synth voice");
		pvoice = &oscVoice;
		break;
		
	case SPRING_VOICE:
		log("Spring voice");
		pvoice = &springVoice;
		break;
		
	case MALLET_VOICE:
		log("Mallet voice");
		pvoice = &malletVoice;
		break;
		
	case HIHAT_VOICE:
		log("HiHat voice");
		pvoice = &hihatVoice;
		break;
		
	case NOISE_VOICE:
		log("Noise voice");
		pvoice = &noiseVoice;
		break;
		
	default:
		log("Unused voice");
		break;
	}
}	

// button or knob selector
void Voices::Select(int8_t sel)
{
	if (sel == 0)
	{
		return;
	}
	
	int8_t s = currentVoiceSelector;
	
	s += sel;
	if (s < 0)
	{
		s = NUM_VOICES - 1;
	}
	
	if (s > NUM_VOICES - 1)
	{
		s = 0;
	}
	
	if (s != currentVoiceSelector)
	{
		Panic();
	}
	
	currentVoiceSelector = s;
	
	ChangeVoice(currentVoiceSelector);
	
	
}

void Voices::UpdateBackGround(void)
{
	switch (currentVoiceSelector)
	{
	case SYNTH_VOICE:
		phw->led1.Set(0.0, 1.0, 0.0);
		break;
			
	case SPRING_VOICE:
		phw->led1.Set(0.0, 0.0, 1.0);
		break;
			
	case MALLET_VOICE:
		phw->led1.Set(1.0, 0.0, 0.0);
		break;
		
	case HIHAT_VOICE:
		phw->led1.Set(1.0, 1.0, 0.0);
		break;
		
	case NOISE_VOICE:
		phw->led1.Set(0.0, 1.0, 1.0);
		break;
	
	default:
		phw->led1.Set(0.0, 0.0, 0.0);
		break;
	}
		
	phw->UpdateLeds();
}

float Voices::Process(void)
{	
	return pvoice->Process();
}


void Voices::NoteOn(NoteOnEvent *p)
{
	pvoice->NoteOn(p);
}

void Voices::NoteOff(NoteOffEvent *p)
{
	pvoice->NoteOff(p);
}

	
void Voices::SetCC0(uint8_t value)
{	
	pvoice->SetCC0(value);
}


void Voices::SetCC1(uint8_t value)
{	
	pvoice->SetCC1(value);
}


void Voices::SetCC2(uint8_t value)
{	
	pvoice->SetCC2(value);	
}


void Voices::SetCC3(uint8_t value)
{	
	pvoice->SetCC3(value);
}


// CCMIDIMap map parms or selector
void Voices::CCProcess(uint8_t ccFuncNumber, uint8_t value)
{
	switch (ccFuncNumber)
	{
	case 0:
		pvoice->SetCC0(value);
		break;
		
	case 1:
		pvoice->SetCC1(value);
		break;
		
	case 2:
		pvoice->SetCC2(value);
		break;
	case 3:
		pvoice->SetCC3(value);
		break;
		
	case 10:
		ChangeVoice(SYNTH_VOICE);
		break;
		
	case 11:
		ChangeVoice(SPRING_VOICE);
		break;
		
	case 12:
		ChangeVoice(MALLET_VOICE);
		break;
		
	case 13:
		ChangeVoice(HIHAT_VOICE);
		break;

	case 14:
		ChangeVoice(NOISE_VOICE);
		break;

	default:
		break;
		
	}
}
	