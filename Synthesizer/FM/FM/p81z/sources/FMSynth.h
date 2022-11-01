#pragma once
#include "FMArrangement.h"
#include "FMPatch.h"
#include "MIDI.h"
#include "TX81Z/TX81Z_output_filter.h"
#include <pl_list.hpp>
#include <memory>

class FMSynth {
public:
    FMSynth();
    void setup(float sampleRate);
    void clear();
    void adjustBuffer(unsigned size);
    void generate(float *outputs[2], unsigned count);
    void handleMidi(const uint8_t *msg, unsigned length);

private:
    void handleChannelMidi(uint32_t msg);
    void resetAllControllers(unsigned channel);
    void updatePitchBend(unsigned channel);
    void setPolyphony(int value);
    void noteOn(unsigned channel, unsigned note, unsigned vel);
    void noteOff(unsigned channel, unsigned note, unsigned vel);
    void allNotesOff(unsigned channel);
    void allSoundOff(unsigned channel);
    void allSoundOffAllChannels();
    struct Voice;
    Voice &allocNewVoice();
    Voice *findVoiceKeyedOn(unsigned channel, unsigned note);
    void generateVoiceAdding(Voice &voice, float *outputL, float *outputR, unsigned count);

public:
    struct Settings {
        FMArrangement::Settings arr;
    };

public:
    void loadSettings(const Settings &s);
    void loadSettingsFromPatch(const FMPatch &patch);
    static Settings createInitialSettings();

private:
    enum { kPolyphonyLimit = 64 };
    unsigned polyphony_ = 32;

    std::unique_ptr<float[]> buffer_;

    TX81Z::OutputFilter outputFilter_[2];

    struct Voice {
        unsigned note = 0;
        unsigned channel = 0;
        unsigned release = 0;
        FMArrangement arr;
        void clear() { arr.clear(); }
        bool hasReleased() const { return arr.hasReleased(); }
        bool hasFinished() const { return arr.hasFinished(); }
    };

    struct Controllers {
        RpnIdentifier rpnIdentifier = {};
        float pitchBend = 0;
        float pitchBendSensitivity = 0;
        unsigned velocityPrefix = 0;
    };

    Controllers fControllers[16];

    pl_list<Voice *> voicesUsed_{kPolyphonyLimit};
    pl_list<Voice *> voicesFree_{kPolyphonyLimit};

    std::array<Voice, kPolyphonyLimit> voicesReserved_;
    Settings settings_ = createInitialSettings();
};
