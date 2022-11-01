#pragma once

template<int N> using Scale = std::array<int,N>;
typedef int SemiTone;

template<int N>
struct Chord
{
    std::array<int,N> chord;

    Chord(std::initializer_list<int> & c) {
        std::copy(c.begin(),c.end(),chord.begin());
    }

    void setSemiTone(int n, int semi) {
        chord[n] = semi % 12;
    }
    
    void noteOn(int root, int channel, int velocity = 127)
    {
        for(int i = 0; i < N; i++)
            SendMidiMessage(0x90+channel, root + chord[i], velocity);    
    }
    void noteOff(int root, int channel)
    {
        for(int i = 0; i < N; i++)
            SendMidiMessage(0x80+channel, root + chord[i], 0);    
    }
};

template<int N>
struct ChordMaker
{
    std::array< Chord<N>, 12 > table;

    void setChord(size_t semi, Chord<N> & chord) {
        table[semi] = chord.chord;
    }
    void noteOn(int root, int channel, int velocity=127)
    {
        table[root % 12].noteOn(root,channel,velocity);
    }
    void noteOff(int root, int channel) {
        table[root % 12].noteOff(root,channel);
    }
};

