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
#pragma once

#include "daisy_pod.h"
#include "daisysp.h"

#include "midimap.h"


using namespace daisy;
using namespace daisysp;


class NullFilter
{
public:
	void Init() {}
	virtual void SetRes(float r) {}
	virtual void SetFreq(float f) {}
	virtual float Process(float in) { return in; }	
	
};



class SVFilter: public NullFilter
{
public:
	void Init(float sampleRate) {filter.Init(sampleRate); }
	void SetRes(float r) { filter.SetRes(r); }
	void SetFreq(float f) { filter.SetFreq(f); }
	float Process(float in) { filter.Process(in);  return filter.Low(); }	
	
private:
	Svf	filter;	
};



class MoogFilter : public NullFilter
{
public:
	void Init(float sampleRate) {filter.Init(sampleRate); }
	void SetRes(float r) { filter.SetRes(r); }
	void SetFreq(float f) { filter.SetFreq(f); }
	float Process(float in) { return filter.Process(in) * 4;  }	// moog filter seems to have a gain issue
	
private:
	MoogLadder	filter;	
};



// a container for filters
class Filters : public CCMIDIMapable
{
public:
	Filters() {}
		
	void Init(float sampleRate); 
	
	// 1/-1 rotates thru filters, 0 does nothing
	void Select(int8_t sel);
	
	float Process(float in);
	
	void SetCC0(uint8_t value);
	void SetCC1(uint8_t value);
	
	// adjusts filter parms or selects specific filter via MIDI map
	void CCProcess(uint8_t ccFuncNumber, uint8_t value);

	

private:
	float sampleRate;	
	uint8_t currentFilterSelector;
	
	typedef enum
	{
		NO_FILTER,
		SV_FILTER,
		MOOG_FILTER
	}FILTER_TYPE;
	
	#define NUM_FILTERS 3
	
	void ChangeFilter(uint8_t sel);

	
	NullFilter	nfilter;
	SVFilter	sfilter;
	MoogFilter	mfilter;
	
	NullFilter *pfilter;
	
	void SetFreqCC(uint8_t value);
	void SetResCC(uint8_t value);
};

