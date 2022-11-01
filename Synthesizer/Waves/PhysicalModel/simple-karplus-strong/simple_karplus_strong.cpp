#include <alsa/asoundlib.h> // midi
#include <vector>
#include <iostream>
#include <jack/jack.h> // audio
#include <math.h> // sine
#include <stdlib.h> // Random
#include <limits.h> // INT_MAX

#define NOTE_ON 0x90
#define NOTHING_HAPPENED_STATUS -11
#define NOTHING_HAPPENED_MIDI 0x00 // Probably actually reserved, but doesn't matter
#define MIDI_EVENT 0x80 // handle events > this number

#define MAX_DELAY_LINE_LENGTH 4096

#define PI 3.14159265

jack_port_t* output_port[2];
jack_client_t* client;

class KarplusStrong {
    public:
        KarplusStrong(int sampleRate) {
            note = 0;
            fs = sampleRate;
            freq = 440.0;
            maxLength = MAX_DELAY_LINE_LENGTH;
            dlPosition = 0;
            noteOn = false;
            sustain = 0.99;
            damping = 0.1;
            delayLine.reserve(maxLength);
            for(int i = 0; i < maxLength; i++) {
                delayLine[i] = 0.0;
            }
        }

        ~KarplusStrong() {
        }

        void strikeString(unsigned char note_, double velocity) {
            note = note_;
            noteOn = true;
            double a = 440.0;
            freq = (a / 32.0) * (pow(2.0, (((double)note - 9.0) / 12.0)));
            dlLength = fmin(fs/freq, maxLength);
            double previous = 0.0;
            for(int i = 0; i < dlLength; i++) {
                delayLine[i] = (rand()/(RAND_MAX + 1.0) - 0.5)*velocity;
                delayLine[i] = delayLine[i]*velocity + previous*(1.0-velocity);
                previous = delayLine[i];
            }
        }

        double getNextSample() {
            int prev = dlPosition;
            dlPosition = (dlPosition+1)%dlLength;
            double currentSample = delayLine[dlPosition];
            delayLine[dlPosition] = sustain*(currentSample*(1.0-damping) + delayLine[prev]*(damping));
            return currentSample;
        }

        void setNoteOff(unsigned char note_) {
            if(note_ == note)
                noteOn = false;
        }

        bool getNoteOn() {
            return noteOn;
        }
    private:
        std::vector<double> delayLine;
        int dlLength;
        int maxLength;
        int fs;
        unsigned char note;
        int dlPosition;
        bool noteOn;
        double freq;
        double damping; // between 0 and 1
        double sustain; // between 0 and 1
};

class PolyphonyManagerKPS {
    public:
        PolyphonyManagerKPS(int maxVoices_, int sampleRate) {
            maxVoices = maxVoices_;
            for(int i = 0; i < maxVoices; i++) {
                voices.push_back(new KarplusStrong(sampleRate));
                lastPressed.push_back(0);
            }
            amountOn = 0;
            numPressed = 0;
        }

        ~PolyphonyManagerKPS() {
            voices.clear();
        }

        void setNoteOn(unsigned char note, double velocity) {
            // If all voices are in use, reuse the voice that has
            // the most amount of time since being pressed
            if(amountOn == maxVoices) {
                unsigned int smallest = INT_MAX;
                int smallest_i = 0;
                for(int i = 0; i < maxVoices; i++) {
                    if(lastPressed[i] < smallest) {
                        smallest = lastPressed[i];
                        smallest_i = i;
                    }
                }
                voices[smallest_i]->strikeString(note, velocity);
                lastPressed[smallest_i] = numPressed + 1;
                numPressed = lastPressed[smallest_i];
            } else {
                for(int i = 0; i < maxVoices; i++) {
                    if(!voices[i]->getNoteOn()) {
                        voices[i]->strikeString(note, velocity);
                        amountOn++;
                        lastPressed[i] = numPressed + 1;
                        numPressed = lastPressed[i];
                        break;
                    }
                }
            }
        }

        void setNoteOff(unsigned char note) {
            int am = 0;
            for(int i = 0; i < maxVoices; i++) {
                voices[i]->setNoteOff(note);
                if(voices[i]->getNoteOn())
                    am++;
            }
            amountOn = am;
        }

        double getNextSample() {
            double value = 0.0;
            for(int i = 0; i < maxVoices; i++) {
                if(voices[i]->getNoteOn())
                    value += voices[i]->getNextSample();
            }
            return value;
        }
    private:
        std::vector<KarplusStrong*> voices;
        std::vector<unsigned int> lastPressed;
        int maxVoices;
        int amountOn;
        unsigned int numPressed;
};

class MidiDevice {
    public:
        MidiDevice(std::string pn) {
            midiIn = NULL;
            status = 0;
            int mode = SND_RAWMIDI_NONBLOCK;
            if ((status = snd_rawmidi_open(&midiIn, NULL, pn.c_str(), mode)) < 0) {
                std::cout << "error discovered opening MIDI device" << std::endl;
                exit(1);
            }
        }
        
        ~MidiDevice() {
            snd_rawmidi_close(midiIn);
            midiIn = NULL;
        }
        
        unsigned char* readBuffer() {
            status = 0;
            while (status != -EAGAIN) {
                status = snd_rawmidi_read(midiIn, buffer, 3);
                if(status == NOTHING_HAPPENED_STATUS) {
                    buffer[0] = NOTHING_HAPPENED_MIDI;
                } else {
                    return buffer;
                }
            }
            return buffer;
        }
    private:
        snd_rawmidi_t* midiIn;
        unsigned char buffer[3];
        int status;
};

MidiDevice* md = NULL;
PolyphonyManagerKPS* pm = NULL;

int process(jack_nframes_t nframes, void *arg) {
    unsigned char *midiBuffer;
    double midiNote = 60.0;
    double a = 440.0;

    jack_default_audio_sample_t *out_right = (jack_default_audio_sample_t*)
                                jack_port_get_buffer(output_port[0], nframes);
    jack_default_audio_sample_t *out_left = (jack_default_audio_sample_t*)
                                jack_port_get_buffer(output_port[1], nframes);
                                
    if(md != NULL && pm != NULL) {
        midiBuffer = md->readBuffer();
        if(midiBuffer[0] == 0x90) {
            std::cout << (int)midiBuffer[2] << std::endl;
            if(midiBuffer[2] > 0) {
                midiNote = midiBuffer[1];
                double velocity = (double)midiBuffer[2]/127.0;
                pm->setNoteOn(midiNote, velocity);
                std::cout << "note freq: " << (a / 32.0) * (pow(2.0, (((double)midiNote - 9.0) / 12.0))) << std::endl;
            } else {
                midiNote = midiBuffer[1];
                pm->setNoteOff(midiNote);
            }
        }
    }

    for(unsigned int i = 0; i < nframes; i++) {
        double nextsmp = 0.0;
        if(pm != NULL) {
            nextsmp = pm->getNextSample();
        }
        out_right[i] = nextsmp;
        out_left[i] = out_right[i];
    }
    
    return 0;
}

void jack_shutdown(void* arg) {
    if(md != NULL && pm != NULL) {
        delete md;
        delete pm;
    }
    exit(1);
}

int set_jack() {
    if((client = jack_client_new("karplus stronger")) == 0) {
        std::cout << "Can't create jack client!" << std::endl;
        return 1;
    }
    
    output_port[0] = jack_port_register(client, "left-out",
            JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    output_port[1] = jack_port_register(client, "right-out",
            JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
            
    jack_set_process_callback(client, process, 0);
    jack_on_shutdown(client, jack_shutdown, 0);
    
    if(jack_activate(client)) {
        std::cout << "can't activate client" << std::endl;
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    std::string midiDeviceString = "hw:1,0,0";
    if(argc > 1) {
        midiDeviceString = argv[1];
    }
    if( set_jack() ) {
        std::cout << "Something wrong with jack" << std::endl;
        return 0;
    }
    md = new MidiDevice(midiDeviceString);
    pm = new PolyphonyManagerKPS(24, 44100);
    
    std::string exitstring;
    std::cin >> exitstring;
    jack_client_close(client);
    
    delete md;
    delete pm;
    
}
