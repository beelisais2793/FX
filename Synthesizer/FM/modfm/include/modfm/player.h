#pragma once

#include <mutex>
#include <vector>

#include "envgen.h"
#include "oscillator.h"

class GUI;

class Generator {
public:
  explicit Generator(int sample_frequency);

  void Perform(const GeneratorPatch &patch, std::complex<float> *out_buffer,
               float base_freq, size_t frames_per_buffer);

  void NoteOn(const GeneratorPatch &patch, unsigned long ts, uint8_t velocity,
              uint8_t note);

  void NoteOff(const GeneratorPatch &patch, uint8_t note);

  bool Playing() const;

  void Stop();

private:
  const int sample_frequency_;
  EnvelopeGenerator e_a_;
  EnvelopeGenerator e_k_;
  Oscillator o_;
};

class Player {
public:
  Player(Patch *gennum, int num_voices, int sample_frequency);

  bool Perform(const void *in_buffer, void *out_buffer,
               size_t frames_per_buffer);

  void NoteOn(unsigned long ts, uint8_t velocity, uint8_t note);

  void NoteOff(uint8_t note);

private:
  struct Voice {
    std::vector<std::unique_ptr<Generator>> generators_;
    int32_t on_time;
    uint8_t note;
    float velocity;
    float base_freq;

    bool Playing() const;
  };
  Voice *NewVoice();
  Voice *VoiceFor(uint8_t note);

  std::mutex voices_mutex_;

  Patch *patch_;
  const int num_voices_ = 8;
  const int sample_frequency_;
  // Track free voices.
  // Could probably structure this as a ring buffer in order of note-on instead
  // of using timestamps.
  std::vector<Voice> voices_;
};
