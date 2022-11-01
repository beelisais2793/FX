#include "utils/gpio/gpio.hpp"
#include "utils/audio/Audio.hpp"
#include "utils/midi/Midi.hpp"
#include "components/envelopes/ADSR.hpp"
#include "components/filters/LPF.hpp"
#include "components/filters/HPF.hpp"
#include "components/generators/Oscillator.hpp"
#include "components/effects/Delay.hpp"
#include "components/effects/Flanger.hpp"
#include "components/effects/Chorus.hpp"
#include "components/effects/Phaser.hpp"
#include "components/effects/Reverb.hpp"
#include <algorithm>

GPIO gpio;


void AudioCallback(Buffer& buffer);

Oscillator lfo;
Oscillator osc1;
Oscillator osc2;
ADSR env{ 0, 0.6, 1, 0.3 };
ADSR env2{ 0, 0.3, 0.2, 0.3 };
ADSR env3{ 0, 1.8, 0, 0 };

LPF lpf;
LPF lpf2;
Delay delay1;
Delay delay2;
Flanger flanger;
Chorus chorus1;
Chorus chorus2;
Phaser phaser1;
Phaser phaser2;
Reverb reverb1;
Reverb reverb2;


Oscillator kick;
ADSR kenv1{ 0, 0.09, 0, 0 };
ADSR kenv2{ 0, 0.2, 0, 0 };

int a = 0;
void MidiPress(int note, int velocity) 
{
    a++;
    osc1.Frequency(Midi::NoteToFreq(note));
    osc1.ResetPhase();
    osc2.ResetPhase();
    env.Trigger();
    env.Gate(true);
    env2.Trigger();
    env3.Trigger();
    env2.Gate(true);
}

void MidiRelease(int note, int velocity) 
{
    a--;
    if (a == 0)env.Gate(false);
    if (a == 0)env2.Gate(false);
}


int main(void) 
{
    Midi midi;
    midi.MidiPress = MidiPress;
    midi.MidiRelease = MidiRelease;
    
    reverb1.Offset(0.0032);
    reverb2.Offset(0.0038);

    osc1.Wavetable(new Wavetables::Basic);
    lfo.Wavetable(new Wavetables::Basic);
    lfo.Frequency(10);
    osc2.Wavetable(new Wavetables::Basic);
    

    Audio::Start();
    Audio::SetCallback(AudioCallback);
   
    gpio.Start();

    Audio::Clean();
    return 0;
}


MonoChannel master1 = []() -> Sample {
    return 0.5 * osc1 >> env;
};

Channel master = []() -> const Stereo {
    const Stereo mix = 0.5 *
        gpio[25] * osc2
        .Detune((gpio[4] - 0.5) * 2)
        .Frequency(osc1.Frequency())
        .WTP(gpio[5])
        +
        gpio[24] * osc1
        //.PhaseDistort([](double a) { return std::pow(std::abs(std::min((4.0 * gpio[6] + 1.0), 2.0) * (a - (1.0 / min((4 * gpio[6] + 1), 2)))), (4 * gpio[6] + 1)); })
        .Detune((gpio[0] - 0.5) * 2 + lfo * gpio[8] * 0.1)
        .Sync(env3 * gpio[2] * 10 + 1)
        .FM(std::pow(gpio[3], 2) * 20000.0 * osc2.GetSample())
        .WTP(gpio[1])
        //>> lpf
        //.Cutoff(20000 *
        //    env2
        //    .Attack(std::pow(gpio[20], 2) * 2)
        //    .Decay(std::pow(gpio[21], 2) * 2)
        //    .Sustain(gpio[22])
        //    .Release(std::pow(gpio[23], 2) * 2))
        >> env
        .Attack(std::pow(gpio[16], 2) * 2)
        .Decay(std::pow(gpio[17], 2) * 2)
        .Sustain(gpio[18])
        .Release(std::pow(gpio[19], 2) * 2)
        //>> lpf2.Cutoff(20000)
        >> StereoEffect{
            chorus1
            .Intensity(0)
            .Frequency(0.20)
            .Feedback(0.1),
            chorus2
            .Intensity(0)
            .Frequency(0.30)
            .Feedback(0.1) }
        >> StereoEffect{
            delay1
           .Mix(gpio[7])
           .Time(0.300)
           .Feedback(0.4),
           delay2
           .Mix(gpio[7])
           .Time(0.320)
           .Feedback(0.4) };
    return mix;
};

bool trig = false;
void AudioCallback(Buffer& buffer)
{
    //if (gpio[31] != 1)
    //{
    //    int note = Midi::NoteToScale(gpio[31] * 64 + 24, new int[7]{ 0, 2, 3, 5, 7, 8, 10 }, 7);
    //    if (!trig) MidiPress(note, 1);
    //    osc1.Frequency(Midi::NoteToFreq(note));
    //    trig = true;
    //}
    //else
    //{
    //    if (trig) MidiRelease(0, 1);
    //    trig = false;
    //}    
    //for (int i = 0; i < BUFFER_SIZE * CHANNELS;) {
    //    Sample a = 0.5 * osc1 >> env;
    //    buffer[i++] = a;
    //    buffer[i++] = a;
    //}
    
    FillBuffer(buffer, master1);
}