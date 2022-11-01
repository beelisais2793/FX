#include "simple_synth.h"

static constexpr unsigned kBufferSize = 32;

struct AudioContext {
    FMSynth *synth;
    jack_port_t *portMidiIn;
    jack_port_t *portAudioOutL;
    jack_port_t *portAudioOutR;
    std::mutex mutex;
};

static int process(jack_nframes_t numFrames, void *userData)
{
    AudioContext &ctx = *(AudioContext *)userData;
    FMSynth &synth = *ctx.synth;

    void *midiIn = jack_port_get_buffer(ctx.portMidiIn, numFrames);
    float *audioOutL = (float *)jack_port_get_buffer(ctx.portAudioOutL, numFrames);
    float *audioOutR = (float *)jack_port_get_buffer(ctx.portAudioOutR, numFrames);

    std::unique_lock<std::mutex> lock(ctx.mutex, std::try_to_lock);
    if (!lock.owns_lock()) {
        std::memset(audioOutL, 0, numFrames * sizeof(float));
        std::memset(audioOutR, 0, numFrames * sizeof(float));
        return 0;
    }

    unsigned indexFrame = 0;
    unsigned eventIndex = 0;

    while (indexFrame < numFrames) {
        unsigned blockSize = std::min<unsigned>(32, numFrames - indexFrame);

        jack_midi_event_t event;
        while (jack_midi_event_get(&event, midiIn, eventIndex) == 0 &&
               event.time < indexFrame + blockSize)
        {
            synth.handleMidi(event.buffer, event.size);
            ++eventIndex;
        }

        float *outputs[2] = {
            audioOutL + indexFrame,
            audioOutR + indexFrame,
        };
        synth.generate(outputs, blockSize);

        indexFrame += blockSize;
    }

    jack_midi_event_t event;
    while (jack_midi_event_get(&event, midiIn, eventIndex) == 0) {
        synth.handleMidi(event.buffer, event.size);
        ++eventIndex;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Please indicate a bank file in SYX format.\n");
        return 1;
    }

    const char *bankPath = argv[1];

    FMBank bank;
    {
        std::unique_ptr<uint8_t[]> data(new uint8_t[65536]);
        FILE_u handle(fopen(bankPath, "rb"));
        if (!handle) {
            fprintf(stderr, "Cannot open the bank file for reading.\n");
            return 1;
        }
        size_t length = fread(data.get(), 1, 65536, handle.get());
        if (!load_VMEM_bank(data.get(), length, &bank)) {
            fprintf(stderr, "Cannot load the bank file.\n");
            return 1;
        }
    }

    FMSynth synth;
    if (bank.count > 0)
        synth.loadSettingsFromPatch(bank.patch[0]);

    jack_client_u client(jack_client_open("Simple P81Z", JackNoStartServer, nullptr));
    if (!client) {
        fprintf(stderr, "Cannot start a new JACK client.\n");
        return 1;
    }

    float sampleRate = jack_get_sample_rate(client.get());

    synth.setup(sampleRate);
    synth.adjustBuffer(kBufferSize);

    AudioContext ctx;
    ctx.synth = &synth;

    if (!(ctx.portMidiIn = jack_port_register(client.get(), "MIDI in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0)) ||
        !(ctx.portAudioOutL = jack_port_register(client.get(), "Audio out L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0)) ||
        !(ctx.portAudioOutR = jack_port_register(client.get(), "Audio out R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0)))
    {
        fprintf(stderr, "Cannot register JACK ports.\n");
        return 1;
    }

    jack_set_process_callback(client.get(), &process, &ctx);

    if (jack_activate(client.get()) != 0) {
        fprintf(stderr, "Cannot activate JACK client.\n");
        return 1;
    }

    ///
    printf("** Loaded bank with %u patches\n", bank.count);

    bool quit = false;
    char line[1024] = {};
    while (!quit && (printf("P81Z> "), fflush(stdout),
                     fgets(line, sizeof(line) - 1, stdin)))
    {
        unsigned lineLen = strlen(line);
        if (lineLen > 0 && line[lineLen - 1] == '\n')
            line[--lineLen] = '\0';

        unsigned scanLen;
        int patchNum;

        if (!strcmp(line, "quit"))
            quit = true;
        else if (sscanf(line, "%d%n", &patchNum, &scanLen) == 1 && scanLen == lineLen) {
            if (patchNum < 1 || patchNum > bank.count)
                fprintf(stderr, "Patch number out of range\n");
            else {
                const FMPatch &patch = bank.patch[patchNum - 1];
                printf("** Load patch %d: %s\n", patchNum, patch.name);
                {
                    std::lock_guard<std::mutex> lock(ctx.mutex);
                    synth.loadSettingsFromPatch(patch);
                }
            }
        }
    }

    return 0;
}
