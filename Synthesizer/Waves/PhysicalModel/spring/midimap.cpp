
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
#include <stdint.h>
#include "midimap.h"


MIDINoteMap DMajorPentNoteMap(MIDINoteMap::NOTE_MAP_MAJOR_PENTATONIC, D_ROOT_NOTE);

void SetFCB1010MIDIMap(CCMIDINoteMap *noteMap, CCMIDIMapable *voice, CCMIDIMapable *filt)
{
	
	// via the FCB-UNO Control Center
	// page 00, maps notes to pentatonic major scale
	// CC50 - CC56 : A - G major pentatonic
	// page 01, maps notes to pentatonic major scale
	// CC60 - CC66 :  A - G minor pentatonic
	
}


void SetAlesisV125MIDIMap(CCMIDIMap *ccmap, CCMIDINoteMap *noteMap, CCMIDIMapable *voice, CCMIDIMapable *filt)
{
	// buttons
	
	// 48 voice select
	ccmap->Add(10, 48, voice); // voice 1
	ccmap->Add(11, 49, voice);
	ccmap->Add(12, 50, voice);
	ccmap->Add(13, 51, voice);
	ccmap->Add(14, 52, voice);
	
	// 56 filter select
	ccmap->Add(10, 56, filt); // no filt
	ccmap->Add(11, 57, filt); // filt 1
	ccmap->Add(12, 58, filt); // filt 2
	
	// 64 note map select
	noteMap->Add(64, &DMajorPentNoteMap); //button CC 50 to D major pentatonic
	
	// knobs
	ccmap->Add(0, 20, filt); // freq to CC 20
	ccmap->Add(1, 21, filt); // resonance to CC 21
	
	// voice parameter change
	ccmap->Add(0, 22, voice); //voice 0 to CC 22
	ccmap->Add(1, 23, voice); 
	ccmap->Add(2, 24, voice); 
	ccmap->Add(3, 25, voice); 
}
