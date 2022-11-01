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
#include "filter.h"

using namespace daisy;
using namespace daisysp;


void Filters::Init(float sr) 
{ 
	sampleRate = sr;
	currentFilterSelector = NO_FILTER; 
	
	pfilter = &nfilter;
	
	sfilter.Init(sampleRate);
	mfilter.Init(sampleRate);
}


void Filters::ChangeFilter(uint8_t sel)
{
	currentFilterSelector = sel;
	
	switch (currentFilterSelector)
	{
	case SV_FILTER:
		log("SV filter");
		pfilter = &sfilter;
		break;
		
	case MOOG_FILTER:
		log("Moog filter");
		pfilter = &mfilter;
		break;

	case NO_FILTER:
		log("No filter");
		pfilter = &nfilter;
		break;

	default:
		log("Unknown filter");
		break;
	}
}	
void Filters::Select(int8_t sel)
{
	if (sel == 0)
	{
		return;
	}
	
	int8_t s = currentFilterSelector;
	
	s += sel;
	if (s < 0)
	{
		s = NUM_FILTERS - 1;
	}
	
	if (s > NUM_FILTERS - 1)
	{
		s = 0;
	}
	
	
	currentFilterSelector = s;
	
	ChangeFilter(currentFilterSelector);
	
}


void Filters::SetFreqCC(uint8_t value)
{
	//f must be between 0.0 and sample_rate / 3
	float f = (float)value / 127.0f * sampleRate / 3.0f;
	pfilter->SetFreq(f);
}


void Filters::SetResCC(uint8_t value)
{	
	pfilter->SetRes((float)value / 127.0f);
}


float Filters::Process(float in)
{	
	return pfilter->Process(in);
}


void Filters::SetCC0(uint8_t value)
{	
	SetFreqCC(value);
}


void Filters::SetCC1(uint8_t value)
{	
	SetResCC(value);
}

void Filters::CCProcess(uint8_t ccFuncNumber, uint8_t value)
{
	switch (ccFuncNumber)
	{
	case 0:
		SetFreqCC(value);
		break;
		
	case 1:
		SetResCC(value);
		break;
		
	case 10:
		ChangeFilter(NO_FILTER);
		break;
		
	case 11:
		ChangeFilter(SV_FILTER);
		break;
		
	case 12:
		ChangeFilter(MOOG_FILTER);
		break;


		
	default:
		break;
		
	}
}


