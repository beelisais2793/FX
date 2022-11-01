#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class metronome
    {
    public:
        metronome ();
        ~metronome ();
        void cleanup();
        void metronomeout (DspFloatType * tickout);
        void set_tempo (int bpm);
        void set_meter (int counts);
        int markctr;

    private:
        int tick_interval;
        int tickctr;
        int meter;
        int tickper;
        int ticktype;

        class AnalogFilter *dulltick,*sharptick, *hpf;

    };
}