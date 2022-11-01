#include "FMSynth.h"
#include "TX81Z/TX81Z_extra.h"
#include <cstdio>
#include <cstring>

///
#if 0
#   define TRACE_ALLOC(fmt, ...) fprintf(stderr, "[Voices] " fmt "\n", ##__VA_ARGS__);
#else
#   define TRACE_ALLOC(fmt, ...)
#endif

///
FMSynth::FMSynth()
{
    TX81Z::initWavetables();

    for (Voice &voice : voicesReserved_) {
        voicesFree_.push_back(&voice);
        voice.arr.useSettings(&settings_.arr);
    }
}

void FMSynth::setup(float sampleRate)
{
    for (Voice &voice : voicesReserved_)
        voice.arr.setup(sampleRate);
    for (TX81Z::OutputFilter &filter : outputFilter_)
        filter.setup(sampleRate);
}

void FMSynth::clear()
{
    voicesUsed_.clear();
    for (Voice &voice : voicesReserved_) {
        voicesFree_.push_back(&voice);
        voice.arr.clear();
    }
    for (TX81Z::OutputFilter &filter : outputFilter_)
        filter.clear();
}

void FMSynth::adjustBuffer(unsigned size)
{
    for (Voice &voice : voicesReserved_)
        voice.arr.adjustBuffer(size);
    buffer_.reset(new float[size]);
}

void FMSynth::handleMidi(const uint8_t *msg, unsigned length)
{
    uint32_t channelMsg = 0;

    switch (length) {
    case 3:
        channelMsg |= msg[2] << 16;
        // fall through
    case 2:
        channelMsg |= msg[1] << 8;
        // fall through
    case 1:
        channelMsg |= msg[0];
        handleChannelMidi(channelMsg);
        break;
    }
}

void FMSynth::handleChannelMidi(uint32_t msg)
{
    const unsigned status = msg & 0xff;
    const unsigned channel = status & 0x0f;
    const unsigned d1 = (msg >> 8) & 0x7f;
    const unsigned d2 = (msg >> 16) & 0x7f;
    Controllers &ctl = fControllers[channel];

    switch (status & 0xf0) {
    case kStatusNoteOn:
        if (d2 != 0) {
            noteOn(channel, d1, d2);
            break;
        }
        // fall through
    case kStatusNoteOff:
        noteOff(channel, d1, d2);
        break;
    case kStatusControllerChange:
        switch (d1) {
        case kCcDataMsb: {
            RpnIdentifier id = ctl.rpnIdentifier;
            if (id.registered && id.lsb == 0 && id.msb == 0) {
                ctl.pitchBendSensitivity = d2;
                updatePitchBend(channel);
            }
            break;
        }
        case kCcVelocityPrefix:
            ctl.velocityPrefix = d2;
            break;
        case kCcNrpnLsb:
        case kCcRpnLsb:
            ctl.rpnIdentifier.lsb = d2;
            ctl.rpnIdentifier.registered = d1 == kCcRpnLsb;
            break;
        case kCcNrpnMsb:
        case kCcRpnMsb:
            ctl.rpnIdentifier.msb = d2;
            ctl.rpnIdentifier.registered = d1 == kCcRpnMsb;
            break;
        case kCcSoundOff:
            allSoundOff(channel);
            break;
        case kCcResetControllers:
            resetAllControllers(channel);
            break;
        case kCcNotesOff:
        case kCcOmniOff:
        case kCcOmniOn:
        case kCcMonoOn:
        case kCcPolyOn:
            allNotesOff(channel);
            break;
        }
        break;
    case kStatusPitchBend:
        ctl.pitchBend = ((int)(d1 | (d2 << 7)) - 8192) * (1.0f / 8191.0f);
        updatePitchBend(channel);
        break;
    }
}

void FMSynth::generate(float *outputs[2], unsigned count)
{
    float *outL = outputs[0];
    float *outR = outputs[1];
    std::memset(outL, 0, count * sizeof(float));
    std::memset(outR, 0, count * sizeof(float));

    for (auto it = voicesUsed_.begin(), end = voicesUsed_.end(); it != end;) {
        Voice &voice = *it->value;
        const Controllers &ctl = fControllers[voice.channel];

        voice.arr.pitchBend(ctl.pitchBend * ctl.pitchBendSensitivity);
        generateVoiceAdding(voice, outL, outR, count);

        if (!voice.hasFinished())
            ++it;
        else {
            TRACE_ALLOC("Finish %u note=%s", voice.id, MidiNoteName[voice.note]);
            voicesUsed_.erase(it++);
            voicesFree_.push_back(&voice);
        }
    }

    outputFilter_[0].run(outL, outL, count);
    outputFilter_[1].run(outR, outR, count);
}

void FMSynth::resetAllControllers(unsigned channel)
{
    Controllers &ctl = fControllers[channel];
    ctl.pitchBend = 0.0;
    ctl.pitchBendSensitivity = 2.0;
    ctl.velocityPrefix = 0;
    ctl.rpnIdentifier.registered = 1;
    ctl.rpnIdentifier.msb = 0;
    ctl.rpnIdentifier.lsb = 0;
    updatePitchBend(channel);
}

void FMSynth::updatePitchBend(unsigned channel)
{
    Controllers &ctl = fControllers[channel];
    float bend = ctl.pitchBend * ctl.pitchBendSensitivity;

    for (pl_cell<Voice *> &cell : voicesUsed_) {
        Voice &voice = *cell.value;
        if (voice.channel == channel)
            voice.arr.pitchBend(bend);
    }
}

void FMSynth::setPolyphony(int value)
{
    value = (value < 1) ? 1 : value;
    value = (value > kPolyphonyLimit) ? kPolyphonyLimit : value;

    if (polyphony_ == (unsigned)value)
        return;

    polyphony_ = value;
    allSoundOffAllChannels();
}

void FMSynth::noteOn(unsigned channel, unsigned note, unsigned vel)
{
    Voice &voice = allocNewVoice();
    TRACE_ALLOC("Play %u note=%s", voice.id, MidiNoteName[note]);

    Controllers &ctl = fControllers[channel];

    voice.channel = channel;
    voice.note = note;
    voice.arr.noteOn(note, ((vel << 7) | ctl.velocityPrefix) * (1.0 / 16383.0));
    voice.release = 0;
}

void FMSynth::noteOff(unsigned channel, unsigned note, unsigned vel)
{
    (void)vel;

    Voice *voice = findVoiceKeyedOn(channel, note);
    if (!voice)
        return;

    voice->arr.noteOff();
}

void FMSynth::allNotesOff(unsigned channel)
{
    for (pl_cell<Voice *> &cell : voicesUsed_) {
        Voice &voice = *cell.value;
        if (voice.channel == channel && !voice.hasReleased())
            voice.arr.noteOff();
    }
}

void FMSynth::allSoundOff(unsigned channel)
{
    auto it = voicesUsed_.begin();
    while (!it.is_end()) {
        Voice &voice = *it->value;
        if (voice.channel != channel) {
            ++it;
            continue;
        }
        voice.clear();
        voicesUsed_.erase(it++);
        voicesFree_.push_back(&voice);
    }
}

void FMSynth::allSoundOffAllChannels()
{
    while (!voicesUsed_.empty()) {
        pl_cell<Voice *> &cell = voicesUsed_.front();
        Voice &voice = *cell.value;
        voice.clear();
        voicesUsed_.pop_front();
        voicesFree_.push_back(&voice);
    }
}

FMSynth::Voice &FMSynth::allocNewVoice()
{
    Voice *voice;

    if (voicesUsed_.size() < polyphony_) {
        voice = voicesFree_.front().value;
        TRACE_ALLOC("Allocate %u", voice->id);
        voicesFree_.pop_front();
        voicesUsed_.push_back(voice); // new voices at the back
    }
    else {
        TRACE_ALLOC("Exceed polyphony");

        // elect a voice that will be replaced
        pl_cell<Voice *> *elected = &voicesUsed_.front(); // old voices at the front

        // search for the voice which has been released for the longest time
        // TODO optimize this O(n)?
        for (pl_cell<Voice *> &cell : voicesUsed_) {
            TRACE_ALLOC(" * Candidate %u: release=%u", cell.value->id, cell.value->release);
            if (cell.value->release > elected->value->release)
                elected = &cell;
        }

        voice = elected->value;
        TRACE_ALLOC("Override %u note=%s", voice->id, MidiNoteName[voice->note]);

        // push it to the back
        voicesUsed_.erase(pl_iterator<pl_cell<Voice *>>{elected});
        voicesUsed_.push_back(voice);
    }

    return *voice;
}

FMSynth::Voice *FMSynth::findVoiceKeyedOn(unsigned channel, unsigned note)
{
    // TODO worth optimizing this O(n)? bounded by the maximum polyphony
    //      also I now support multiple voices per midi note (if I ever add MPE)

    for (pl_cell<Voice *> &cell : voicesUsed_) {
        Voice &voice = *cell.value;
        if (voice.channel == channel && voice.note == note) {
            if (!voice.hasReleased())
                return &voice;
        }
    }
    return nullptr;
}

void FMSynth::generateVoiceAdding(Voice &voice, float *outputL, float *outputR, unsigned count)
{
    float *temp = buffer_.get();
    voice.arr.run(temp, count);

    for (unsigned i = 0; i < count; ++i)
        outputL[i] += temp[i];

    if (voice.hasReleased())
        voice.release += count;

    if (voice.hasFinished())
        voice.clear();

    std::memcpy(outputR, outputL, count * sizeof(float));
}

void FMSynth::loadSettings(const Settings &s)
{
    settings_ = s;
}

void FMSynth::loadSettingsFromPatch(const FMPatch &patch)
{
    Settings &s = settings_;

    int trpsSemitones = (int)patch.trps - 24;
    double trpsRatio = std::exp2(trpsSemitones * (1.0 / 12.0));

    for (unsigned i = 0; i < 4; ++i) {
        const FMPatchOperator &patchOp = patch.op[i];

        TX81Z::Envelope::Settings &eg = s.arr.op[i].eg;
        eg.attackRate = patchOp.ar;
        eg.decay1Rate = patchOp.d1r;
        eg.decay2Rate = patchOp.d2r;
        eg.releaseRate = patchOp.rr;
        eg.decay1Level = patchOp.d1l;
        eg.keyVelocitySensitivity = patchOp.kvs();
        eg.keyRateScaling = patchOp.rs();

        TX81Z::Oscillator::Settings &osc = s.arr.op[i].osc;
        osc.waveshape = patchOp.opw();
        osc.level = patchOp.out;
        if (!patchOp.fix()) {
            osc.fixedPitch = 0.0f;
            osc.multiplier = trpsRatio * TX81Z::computeRatio(patchOp.f, patchOp.fine());
        }
        else {
            osc.fixedPitch = TX81Z::computeFrequency(patchOp.fixrg(), patchOp.f, patchOp.fine());
            osc.multiplier = 0.0f;
        }
    }

    s.arr.alg = patch.alg();
    s.arr.op[3].osc.feedbackFactor = patch.fbl();

    // TODO...
}

FMSynth::Settings FMSynth::createInitialSettings()
{
    Settings s;
    // TODO: detune at center, etc
    // NOTE: see Init patch in service manual
    return s;
}
