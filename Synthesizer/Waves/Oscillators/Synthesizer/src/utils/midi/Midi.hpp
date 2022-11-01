#include <RtMidi.h>
#include <thread>
#include <cmath>
#include <vector>

class Midi 
{
private:
    RtMidiIn *midiin;
    std::thread connectThread;
    bool active;

    void Loop();

public:
    Midi();
    ~Midi();

    void (*MidiPress)(int note, int velocity);
    void (*MidiRelease)(int note, int velocity);
    void (*MidiControl)(int cc, int value);

    static double NoteToFreq(int note);
    static int NoteToScale(int note, int* scale, int size);
};