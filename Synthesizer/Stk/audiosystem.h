#pragma once

#include <portaudio.h>
#include <stdbool.h>
#include <portmidi.h>
#include <porttime.h>
#include <pthread.h>

typedef struct midimsg
{
    int status,data1,data2, msg, channel;
    struct midimsg * next;
}
MidiMsg;

MidiMsg* NewMessage(int status, int data1, int data2, int channel, int msg);
void AddMessage(MidiMsg * head, MidiMsg * last);
void ExecQueue(MidiMsg * msgs);

void Init();
void InitMidiDevice(int channel, int input_id, int output_id);
size_t GetNumMidiDevices();
const char* GetMidiDeviceName(size_t i);
void LockMidi();
void UnlockMidi();
void StopMidi();

int InitAudioDevice(int output_device_number, int input_device_number, size_t num_channels, int sample_rate, int frames_per_second, void * userData = NULL);
int InitAudio(int sample_rate, int frames_per_second, void * userData = NULL);

int StopAudio();
void RunAudio();
int GetNumAudioDevices();
const char* GetAudioDeviceName(size_t i);
void Stop();

typedef int (*midi_func)(MidiMsg *msg);
typedef int  (*audio_func)( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                    const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData );

void set_note_on_callback( midi_func cb);
void set_note_off_callback( midi_func cb);

void set_audio_callback(audio_func callback);

