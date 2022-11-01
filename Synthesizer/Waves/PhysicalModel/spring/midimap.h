

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

#include <stdint.h>
#include "utilities.h"

// A class inherits from this the method CCProcess which maps a enumerated CC function which is then called with the CC value
class CCMIDIMapable
{
public:
	// Pass the enumerated CC function in the mapable class and it processes the CC value
	virtual void CCProcess(uint8_t ccFuncNumber, uint8_t value) {}
};

// control change midi map
// maps CC numbers to enumerated CC functions in a class that inherits from CCMIDIMapable
class CCMIDIMap
{
public:
	// set all maps to NULL
	void Init() { for (uint8_t i = 0; i < 127; i++) map[i].mapobj = NULL; }
	// Add a map with the enumerated CC function number, the CC number to map to and the mapable class
	void Add(uint8_t ccFuncNumber, uint8_t cc, CCMIDIMapable *mapable) 
	{
		if (cc >= 127)
		{
			return;
		}
		map[cc].funcNumber = ccFuncNumber;
		map[cc].mapobj = mapable; 
	}
	
	// called with every CC event to see if its mapped and send the value to the enumerated function in the mapable class. 
	void Invoke(uint8_t cc, uint8_t value)
	{
		if (cc >= 127 || map[cc].mapobj == NULL) 
		{
			return;
		}
		
		// call the CCProcess that overrode the CCMIDIMapable
		map[cc].mapobj->CCProcess(map[cc].funcNumber, value);
	}
	
private:
	typedef struct
	{
		uint8_t funcNumber; // enumerated CC func in mapable class
		CCMIDIMapable *mapobj;	// mapable instantiation
	}CCMap;
	
	CCMap map[127];
	
};

// program change midi map
class PCMIDIMap
{
public:
	typedef void(*PCFunc)(void);
	void Init() { for (uint8_t i = 0; i < 127; i++) map[i] = NULL; }
	void Add(uint8_t pc, PCFunc func) {if (pc >= 127) return; map[pc] = func; }
	void Invoke(uint8_t pc)
	{
		if (pc >= 127 || map[pc] == NULL) 
		{
			return;
		}
		
		map[pc]();
	}
	
private:
	
	PCFunc map[127];
	
};


typedef enum
{
	C_ROOT_NOTE = 0,
	CS_ROOT_NOTE,	
	D_ROOT_NOTE,	
	DS_ROOT_NOTE,
	E_ROOT_NOTE,
	F_ROOT_NOTE,
	FS_ROOT_NOTE,
	G_ROOT_NOTE,
	GS_ROOT_NOTE,
	A_ROOT_NOTE,
	AS_ROOT_NOTE,
	B_ROOT_NOTE,
}MIDI_ROOT_NOTE;


// add 12 for upper octaves
// major pent: root, 2nd, 3rd, 5th, and 6th intervals
// C	C#	D	D#	E	F	F#	G	G#	A	A#	B
// 0    1   2   3   4   5   6   7   8   9   10  11
// 0    0   2   2   5   5   6   6   9   9   12  12 <- one way to do it, but
// 0    2   5   6   9   12  14  17  ... <- root is C, this is a better way


class MIDINoteMap
{
public:
	typedef enum
	{
		NOTE_MAP_NONE = 0,
		NOTE_MAP_MAJOR_PENTATONIC
	}NOTE_MAP_TYPE;
	
	MIDINoteMap(NOTE_MAP_TYPE nmt, MIDI_ROOT_NOTE rootNote) { type = nmt; root = rootNote; }
	
	// TODO switch in note map types...
	
	uint8_t Process(uint8_t noteIn)
	{
		if (type == NOTE_MAP_NONE)
		{
			return noteIn;
		}
		
		uint8_t oct = noteIn / 12; //truncation towards zero
		uint8_t n = noteIn - (oct * 12);
	
		uint8_t noteOut = root + (oct * 12) + MajorPentNoteMap[n];
		log("noteIn: %d, NoteOut: %d", noteIn, noteOut);
		return noteOut;
	}

private:
	NOTE_MAP_TYPE type;
	uint8_t root;
	uint8_t MajorPentNoteMap[5] = { 0, 2, 5, 6, 9 }; 
};


// maps cc to a scale map
class CCMIDINoteMap
{
public:
	void Init() { for (uint8_t i = 0; i < 127; i++) map[i] = NULL; }
	void Add(uint8_t cc, MIDINoteMap *noteMap) {if (cc >= 127) return; map[cc] = noteMap; }
	void Invoke(uint8_t cc, uint8_t value)
	{
		if (cc >= 127) 
		{
			return;
		}
		
		if (cc == setCC && value == 0)
		{
			setCC = 0; // turn map off
		}
		else
		{
			setCC = cc;			
		}
	}
	
	uint8_t Process(uint8_t  NoteIn)
	{
		if (map[setCC] == NULL)
		{
			return NoteIn;
		}
		
		return map[setCC]->Process(NoteIn);
	}
	
private:
	
	MIDINoteMap *map[127];
	uint8_t setCC;
	
};

void SetAlesisV125MIDIMap(CCMIDIMap *ccmap, CCMIDINoteMap *noteMap, CCMIDIMapable *voice, CCMIDIMapable *filt);

