#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class RecChord
    {

    public:
        RecChord ();
        ~RecChord ();
        void MiraChord ();
        void IniciaChords ();
        void Vamos (int voz, int interval);
        void cleanup ();

        int cc;
        int fundi;
        int bass;
        int ctipo;

        char NombreAcorde[20];

        struct TipoCh {
            int tipo;
            int num;
            char Nom[16];
        } ChN[34];

        struct NomChord {
            char Nom[3];
        } NC[13];

        struct NomChordExtended {
            char Nom[3];
            int note;
        } NCE[25];




    private:

        int plus (int nota);

        int NumChord3;
        int NumChord4;
        int NumChord5;

        int Ch[34][13];


        struct Ch3 {
            int tipo;
            int fund;
            int di1;
            int di2;
            char Nom[10];
        } Chord3[17];

        struct Ch4 {
            int tipo;
            int fund;
            int di1;
            int di2;
            int di3;
            char Nom[10];
        } Chord4[62];

        struct Ch5 {
            int tipo;
            int fund;
            int di1;
            int di2;
            int di3;
            int di4;
            char Nom[16];
        } Chord5[67];
    };

}
