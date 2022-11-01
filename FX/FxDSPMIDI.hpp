#pragma once

namespace DSPFX
{

    struct FxMidi
    {
        static float midiNoteToFrequency(unsigned note) {
            return ::midiNoteToFrequency(note);
        }
        static unsigned frequencyToMidiNote(float f) {
            return ::frequencyToMidiNote(f);
        }
    };
}
