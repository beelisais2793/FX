%{
#include "MidiFileIn.h"
%}

namespace stk
{
    class MidiFileIn : public Stk
    {
    public:

        MidiFileIn( std::string fileName );
        ~MidiFileIn();

        int getFileFormat() const;
        unsigned int getNumberOfTracks() const;
        int getDivision() const;
        void rewindTrack( unsigned int track = 0 );
        double getTickSeconds( unsigned int track = 0 );
        unsigned long getNextEvent( std::vector<unsigned char> *event, unsigned int track = 0 );
        unsigned long getNextMidiEvent( std::vector<unsigned char> *midiEvent, unsigned int track = 0 );
    };
}