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


void NoiseFilter::Init(float SR, int32_t seed)
{
	sampleRate = SR;
	wn.Init(seed);
	hpFilter.Init(sampleRate);
	hpFilter.SetRes(0.5);
	
	for (uint8_t i = 0; i < NUM_FILTERS; i++)
	{
		filter[i].Init(sampleRate);
	}
	
	SetResonance(0.8);
}


float NoiseFilter::Process()
{
	float w = wn.Process();
	hpFilter.Process(w * resGain); // as the resonance goes up the gain must go down
	
	filter[0].Process(hpFilter.High());
	float o1 = filter[0].Band();
	filter[1].Process(o1); 
	float o2 = filter[1].Band();
	filter[2].Process(o2); 
	float o = filter[2].Band();
	if (isnan(o))
	{
		for (uint8_t i = 0; i < NUM_FILTERS; i++)
		{
			filter[i].Init(sampleRate);
		}
		return 0.0;
	}
	return o;
}
	
void NoiseFilter::SetFreq(float freq)
{
	hpFilter.SetFreq(freq / 2); // an octave below to prevent rumble
	
	for (uint8_t i = 0; i < NUM_FILTERS; i++)
	{
		filter[i].SetFreq(freq);
	}
}

void NoiseFilter::SetResonance(float res)
{
	float r = fclamp(res, 0.3, 1.0);
	resGain = 3.0 / r;
	for (uint8_t i = 0; i < NUM_FILTERS; i++)
	{
		filter[i].SetRes(r);
	}
}

void NoiseFilter::SetDrive(float drive)
{
	for (uint8_t i = 0; i < NUM_FILTERS; i++)
	{
		filter[i].SetDrive(drive);
	}
}



void NoiseVoice::Init(float SR) 
{
	NullVoice::Init(SR);
	polyphony = NOISE_VOICE_POLYPHONY;
	ADSROn = true;
	ADSRAttack = ADSR_ATTACK_DEFAULT;
	ADSRDecay = ADSR_DECAY_DEFAULT;
	ADSRSustain = 1.0;
	ADSRRelease = ADSR_RELEASE_DEFAULT;
	
	resonance = 0.8;
	drive = 0.5;
	
	int32_t seed = 7;
	for (uint8_t i = 0; i < polyphony; i++)
	{
		noise[i].Init(SR, seed + (i * seed)); // i is seed
		noise[i].SetAmp(0);
		noise[i].SetDrive(drive);
		noise[i].SetResonance(resonance);
		
		adsr[i].Init(sampleRate);
		adsr[i].SetAttackTime(ADSRAttack);
		adsr[i].SetDecayTime(ADSRDecay);
		adsr[i].SetSustainLevel(ADSRSustain);
		adsr[i].SetReleaseTime(ADSRRelease);
	}	
	
	Panic();
}

void NoiseVoice::Panic() 
{
	NullVoice::Panic();
	
	for (uint8_t i = 0; i < polyphony; i++)
	{
		noise[i].SetAmp(0);
		adsr[i].Process(false);	
	}
}

void NoiseVoice::StartNote(uint8_t i, NoteOnEvent *p)
{
	//log("n: %d, m: %d", i, p->note);
	notes[i].midiNote = p->note;
	notes[i].amplitude = (float)p->velocity / 127.0f;
			
	noise[i].SetFreq(mtof(p->note));
	noise[i].SetAmp(notes[i].amplitude);
	adsr[i].Retrigger(false); // set attack mode
	
}


void NoiseVoice::NoteOn(NoteOnEvent *p)
{
	for (uint8_t i = 0; i < polyphony; i++)
	{
		if (notes[i].midiNote == p->note)
		{
			// if we are playing the same note
			StartNote(i, p);
			return;
		}
		
		// start new note if unused and adsr is done 
		if (notes[i].midiNote == 0 && adsr[i].IsRunning() == false)
		{
			StartNote(i, p);
			return;	
		}
	}
}


void NoiseVoice::NoteOff(NoteOffEvent *p)
{
	for (uint8_t i = 0; i < polyphony; i++)
	{
		if (notes[i].midiNote == p->note)
		{
			notes[i].midiNote = 0;	
			
			if (ADSROn == false)
			{
				noise[i].SetAmp(0.0);
			}
		}
	}
}


float NoiseVoice::Process(void) 
{
	float sig = 0.0;
	for (uint8_t i = 0; i < polyphony; i++)
	{
		bool attack = true;
		if (notes[i].midiNote == 0)
		{
			attack = false; // release
		}
				
		float ADSRLevel = 1.0;
		if (ADSROn == true)
		{
			ADSRLevel = adsr[i].Process(attack);
		}
		sig += noise[i].Process() * ADSRLevel;
	}
	
	return sig / polyphony;
}

void NoiseVoice::SetCC0(uint8_t value)
{
	SetResonanceCC(value);
}
	
void NoiseVoice::SetCC1(uint8_t value)
{
	SetDriveCC(value);
}

void NoiseVoice::SetCC2(uint8_t value)
{
	SetADSRAttackCC(value);
}

void NoiseVoice::SetCC3(uint8_t value)
{
	SetADSRReleaseCC(value);
}


//void NoiseVoice::SetCC2(uint8_t value)
//{
//	SetADSRSustainCC(value);
//}


void NoiseVoice::SetResonanceCC(uint8_t value)
{
	float a = GetCCMinMax(value, 0.0, 1.0);
	if (a == resonance)
	{
		return;
	}
	
	resonance = a;
	
	for (uint8_t i = 0; i < polyphony; i++)
	{
		noise[i].SetResonance(resonance); 
	}
}


void NoiseVoice::SetDriveCC(uint8_t value)
{
	float a = GetCCMinMax(value, 0.0, 1.0);
	if (a == drive)
	{
		return;
	}
	
	drive = a;
	
	for (uint8_t i = 0; i < polyphony; i++)
	{
		noise[i].SetDrive(drive); 
	}
}

void NoiseVoice::SetADSRAttackCC(uint8_t value)
{
	float a = GetCCMinMax(value, ADSR_ATTACK_MIN, ADSR_ATTACK_MAX);
	if (a == ADSRAttack)
	{
		return;
	}
	ADSRAttack = a;
	//log("Attack: %d msec", (uint32_t)(ADSRAttack * 1000));
	
	for (uint8_t i = 0; i < polyphony; i++)
	{
		adsr[i].SetAttackTime(ADSRAttack); 
	}
}


void NoiseVoice::SetADSRDecayCC(uint8_t value)
{
	float d = GetCCMinMax(value, ADSR_DECAY_MIN, ADSR_DECAY_MAX);
	if (d == ADSRDecay)
	{
		return;
	}
	ADSRDecay = d;
	//log("Decay: %d", (uint32_t)(ADSRDecay * 1000));
	
	for (uint8_t i = 0; i < polyphony; i++)
	{
		adsr[i].SetDecayTime(ADSRDecay); 
	}
}


void NoiseVoice::SetADSRSustainCC(uint8_t value)
{
	if (value == 127)
	{
		ADSROn = false;
		log("ADSR off");
	}
	else
	{
		ADSROn = true;
	}
	
	float s = value / 127.0;
	
	if (s == ADSRSustain)
	{
		return;
	}
	ADSRSustain = s;
	//log("Sustain: %d", (uint32_t)(ADSRSustain * 1000));

	for (uint8_t i = 0; i < polyphony; i++)
	{
		adsr[i].SetSustainLevel(ADSRSustain); 
	}
}

void NoiseVoice::SetADSRReleaseCC(uint8_t value)
{
	float r = GetCCMinMax(value, ADSR_RELEASE_MIN, ADSR_RELEASE_MAX);
	if (r == ADSRRelease)
	{
		return;
	}
	ADSRRelease = r;
	//log("Release: %d", (uint32_t)(ADSRRelease * 1000));

	for (uint8_t i = 0; i < polyphony; i++)
	{
		adsr[i].SetReleaseTime(ADSRRelease); 
	}
}

