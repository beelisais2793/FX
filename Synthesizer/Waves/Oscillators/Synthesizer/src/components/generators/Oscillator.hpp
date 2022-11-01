#pragma once
#include "../../utils/audio/Audio.hpp"
#include "../Generator.hpp"
#include "Wavetables.hpp"
#include <memory>

typedef double (*PhaseFunction)(double);

class Oscillator : public Generator
{
private:
    PhaseFunction phaseDistort = [](double a) { return a; };
    double phase = 0;
    Sample sample = 0;

    double fm = 0;
    double am = 1;
    double detune = 0;

    double frequency = 200; // Hz

    Wavetables::Wavetable* wavetable = new Wavetables::Sine;
public:

    ~Oscillator() { delete wavetable; }

    Oscillator& Frequency(double f);
    double Frequency();

    Sample NextSample() override;
    Sample GetSample() override;

    double Phase();
    void ResetPhase();


    Oscillator& Wavetable(Wavetables::Wavetable*); // Frequency modulate
    Oscillator& FM(Sample); // Frequency modulate
    Oscillator& Detune(double); // Frequency modulate
    Oscillator& AM(Sample); // Amplitude modulate
    Oscillator& WTP(double); // Wavetable position
    Oscillator& PhaseDistort(PhaseFunction);
    Oscillator& Sync(double); // Hard sync
};