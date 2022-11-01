#pragma once

typedef int     MidiDeviceID;
typedef long    MidiTimeStamp;

enum MidiError
{
    MIDI_NO_ERROR = pmNoError,
    MIDI_NO_DATA  = pmNoData,
    MIDI_GOT_DATA = pmGotData,
    MIDI_HOST_ERROR =  pmHostError,
    MIDI_INVALID_DEVICEID =  pmInvalidDeviceId, 
    MIDI_INSUFFICIENT_MEMORY =  pmInsufficientMemory,
    MIDI_BUFFER_TOO_SMALL =  pmBufferTooSmall,
    MIDI_BUFFER_OVERFLOW  =  pmBufferOverflow,
    MIDI_BAD_PTR =  pmBadPtr, 
    MIDI_BAD_DATA =  pmBadData, 
    MIDI_INTERNAL_ERROR =  pmInternalError,
    MIDI_BUFFER_MAX_SIZE = pmBufferMaxSize 
 };
 

void Initialize();
void Terminate();
const char* GetErrorText();
int GetDeviceCount();
MidiDeviceID GetDefaultInputDeviceID();
MidiDeviceID GetDefaultOutputDeviceID();

struct MidiDeviceInfo
{
    PmDeviceInfo *info;
    MidiDeviceID id;
    MIDIDeviceInfo(MidiDeviceID id) {
        info = Pm_GetDeviceInfo(id);
        this->id = id;
        assert(info != NULL);
    }

    int structVersion() const { return info->structVersion; }
    const char* interf() const { return info->interf; }
    const char* name() const { return info->name;}
    int input() const { return info->input; }
    int output() const { return info->output; }
    bool opened() const { return info->opened; }
};


struct MidiStream
{
    PortMidiStream * stream;
};

struct MidiInputStream : public MidiStream
{   
    MidiInputStream( MidiDeviceInfo& info, long bufferSize) {
        PmError err = Pm_OpenInput(&stream, info.device_id, info.info, bufferSize,NULL,NULL);
        assert(err == MIDI_NO_ERROR);    
    }
    ~MidiInputStream() {
        if(stream) Pm_Close(stream);
    }
};

struct MidiOutputStream
{
    MidiOutputStream( MidiDeviceInfo& info, long bufferSize, long latency) {
        PmError err = Pm_OpenOutput(&stream, info.device_id, info.info, bufferSize,NULL,NULL, latency);
        assert(err == MIDI_NO_ERROR);    
    }
    ~MidiOutputStream() {
        if(stream) Pm_Close(stream);
    }
}

inline void Initialize()
{
    Pm_Initialize();
}
inline void Terminate()
{
    Pm_Terminate();
}
inline const char* GetErrorText(int error)
{
    return Pm_GetErrorText(error);
}
inline int GetDeviceCount() {
    return Pm_CountDevices();
}
inline MidiDeviceID GetDefaultInputDeviceID()
{
    return Pm_GetDefaultInputDeviceID();
}
inline MidiDeviceID GetDefaultOutputDeviceID()
{
    return Pm_GetDefaultOutputDeviceID();
}

struct MidiSystem
{
    MidiSytem() {
        Pm_Initialize();
    }
    ~MidiSystem() {
        Pm_Terminate();
    }
};
extern MidiSystem midi_system;

struct Midi
{
    void (*process_midi)(PtTimestamp timestamp, void * userData) = NULL;

    void SetMidiProcessFunc( void (*f)(PtTimestamp timestamp, void * userData))
    {
        process_midi = f;
    }
    void StartMidi()
    {
        assert(process_midi != NULL);
        Pt_Start(10,&process_midi,0);        
    }
    void StopMidi()
    {
        Pt_Stop();
    }
};