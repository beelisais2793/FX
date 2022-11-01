%module audiosystem
%{
#include <portaudio.h>
#include <stdbool.h>
#include <portmidi.h>
#include <porttime.h>
#include <pthread.h>

%}

%include "stdint.i"
%include "lua_fnptr.i"

%include <portaudio.h>
%include <portmidi.h>
%include <porttime.h>

int InitAudio(int sample_rate, int frames_per_second);
int StopAudio();
void RunAudio();

float* float_new(size_t size);
void   float_free(float * p);
float  float_get(float * p, size_t index);
void   float_set(float *p, size_t index, float value);

%inline
%{

/////////////////////////////////////////////
// MIDI
/////////////////////////////////////////////

// most of these will probably never get used
SWIGLUA_REF note_on  = {0,0};
SWIGLUA_REF note_off = {0,0};
SWIGLUA_REF control_change  = {0,0};
SWIGLUA_REF program_change  = {0,0};
SWIGLUA_REF polyphonic_key_pressure={0,0};
SWIGLUA_REF channel_pressure  = {0,0};
SWIGLUA_REF pitch_bend  = {0,0};
SWIGLUA_REF realtime_clock = {0,0};
SWIGLUA_REF start_sequence = {0,0};
SWIGLUA_REF continue_sequence = {0,0};
SWIGLUA_REF stop_sequence = {0,0};
SWIGLUA_REF active_sensing = {0,0};
SWIGLUA_REF system_reset = {0,0};
SWIGLUA_REF system_exclusive = {0,0};
SWIGLUA_REF local_control = {0,0};
SWIGLUA_REF all_notes_off = {0,0};
SWIGLUA_REF omni_off = {0,0};
SWIGLUA_REF omni_on = {0,0};
SWIGLUA_REF mono_mode = {0,0};
SWIGLUA_REF poly_mode = {0,0};
SWIGLUA_REF midi_clock = {0,0};
SWIGLUA_REF midi_timing_code = {0,0};
SWIGLUA_REF reset_all_controllers = {0,0};
SWIGLUA_REF song_position = {0,0};
SWIGLUA_REF song_select = {0,0};
SWIGLUA_REF tuning_request = {0,0};

void set_note_on_func(SWIGLUA_REF f)
{
    note_on = f;         
    /*
    lua_State *L = lua_newthread(f.L);        
    luaL_ref(L,LUA_REGISTRYINDEX);
    note_on.L = L;        
    */
}
void set_note_off_func(SWIGLUA_REF f)
{
    note_off = f;            
    /*
    lua_State *L = lua_newthread(f.L);        
    luaL_ref(L,LUA_REGISTRYINDEX);
    note_off.L = L;        
    */
}
void set_control_change_func(SWIGLUA_REF f)
{
    control_change = f;    
    /*
    lua_State *L = lua_newthread(f.L);        
    luaL_ref(L,LUA_REGISTRYINDEX);
    control_change.L = L;        
    */
}
void set_program_change_func(SWIGLUA_REF f)
{
    program_change = f;    
    /*
    lua_State *L = lua_newthread(f.L);        
    luaL_ref(L,LUA_REGISTRYINDEX);
    program_change.L = L;        
    */
}
void set_polyphonic_key_pressure_func(SWIGLUA_REF f)
{
    polyphonic_key_pressure = f;    
    /*
    lua_State *L = lua_newthread(f.L);        
    luaL_ref(L,LUA_REGISTRYINDEX);
    polyphonic_key_pressure.L = L;        
    */
}

void set_channel_pressure_func(SWIGLUA_REF f)
{
    channel_pressure = f;   
    /*
    lua_State *L = lua_newthread(f.L);        
    luaL_ref(L,LUA_REGISTRYINDEX);
    channel_pressure.L = L;         
    */
}

void set_pitch_bend_func(SWIGLUA_REF f)
{
    pitch_bend = f; 
    /*
    lua_State *L = lua_newthread(f.L);        
    luaL_ref(L,LUA_REGISTRYINDEX);
    pitch_bend.L = L;           
    */
}

void set_realtime_clock_func(SWIGLUA_REF f)
{
    realtime_clock = f;    
    /*
    lua_State *L = lua_newthread(f.L);        
    luaL_ref(L,LUA_REGISTRYINDEX);
    realtime_clock.L = L;        
    */
}

void set_start_sequence_func(SWIGLUA_REF f)
{
    start_sequence = f;    
    /*
    lua_State *L = lua_newthread(f.L);        
    luaL_ref(L,LUA_REGISTRYINDEX);
    start_sequence.L = L;        
    */
}
void set_continue_sequence_func(SWIGLUA_REF f)
{
    continue_sequence = f;    
    /*
    lua_State *L = lua_newthread(f.L);        
    luaL_ref(L,LUA_REGISTRYINDEX);
    continue_sequence.L = L;        
    */
}
void set_stop_sequence_func(SWIGLUA_REF f)
{
    stop_sequence = f;    
    /*
    lua_State *L = lua_newthread(f.L);        
    luaL_ref(L,LUA_REGISTRYINDEX);
    stop_sequence.L = L;        
    */
}
void set_active_sensing_func(SWIGLUA_REF f)
{
    active_sensing = f;    
    /*
    lua_State *L = lua_newthread(f.L);        
    luaL_ref(L,LUA_REGISTRYINDEX);
    active_sensing.L = L;        
    */
}
void set_system_reset_func(SWIGLUA_REF f)
{
    system_reset = f;    
    /*
    lua_State *L = lua_newthread(f.L);        
    luaL_ref(L,LUA_REGISTRYINDEX);
    system_reset.L = L;        
    */
}
void set_system_exclusive_func(SWIGLUA_REF f)
{
    system_exclusive = f;    
    /*
    lua_State *L = lua_newthread(f.L);        
    luaL_ref(L,LUA_REGISTRYINDEX);
    system_exclusive.L = L;        
    */
}
void set_local_control_func(SWIGLUA_REF f)
{
    local_control = f;    
}
void set_all_notes_off_func(SWIGLUA_REF f)
{
    all_notes_off = f;    
}
void set_omni_off_func(SWIGLUA_REF f)
{
    omni_off = f;    
}
void set_omni_on_func(SWIGLUA_REF f)
{
    omni_on = f;    
}
void set_mono_mode_func(SWIGLUA_REF f)
{
    mono_mode = f;    
}
void set_poly_mode_func(SWIGLUA_REF f)
{
    poly_mode = f;    
}
void set_clock_func(SWIGLUA_REF f)
{
    midi_clock = f;    
}
void set_midi_timing_code_func(SWIGLUA_REF f)
{
    midi_timing_code = f;    
}
void set_reset_all_controllers_func(SWIGLUA_REF f)
{
    reset_all_controllers = f;    
}
void set_song_position_func(SWIGLUA_REF f)
{
    song_position = f;    
}
void set_select_func(SWIGLUA_REF f)
{
    song_select = f;    
}
void set_tuning_request_func(SWIGLUA_REF f)
{
    tuning_request = f;    
}

int midi_channel = 0;

PmStream * pm_midi_input = NULL;
PmStream * pm_midi_output = NULL;
pthread_mutex_t Lock;

size_t GetNumMidiDevices()
{
    return Pm_CountDevices();
}

const char* GetMidiDeviceName(size_t i)
{
    const PmDeviceInfo * pm = Pm_GetDeviceInfo(i);
    return pm->name;
    
}

void LockMidi()
{
    while( pthread_mutex_lock(&Lock) != 0);
}
void UnlockMidi()
{
    while(pthread_mutex_unlock(&Lock) != 0);
}

typedef struct midimsg
{
    int status,data1,data2,msg,channel;    
    struct midimsg * next;
} 
MidiMsg;

MidiMsg* NewMessage(int status, int data1, int data2, int msg, int channel) {
    MidiMsg * p = (MidiMsg*)calloc(1,sizeof(MidiMsg));
    p->status = status;
    p->data1  = data1;
    p->data2  = data2;
    p->msg    = msg;
    p->channel = channel;
    p->next   = NULL;
    return p;
}
void AddMessage(MidiMsg * head, MidiMsg * last) {
    MidiMsg * p = head;
    if(p == NULL) return;
    while(p->next != NULL) {
        p = p->next;
    }
    p->next = last;
    last->next = NULL;
}
MidiMsg * midi_queue = NULL;

void CallbackLua(SWIGLUA_REF ref, MidiMsg * msg)
{
    if(ref.L == NULL) return;
    swiglua_ref_get(&ref);	
    lua_pushnumber(ref.L,msg->channel);
    lua_pushnumber(ref.L,msg->data1);
    lua_pushnumber(ref.L,msg->data2);
    if(lua_pcall(ref.L,3,0,0) != 0)
    {
        printf("%s", lua_tostring(ref.L,-1));
    }
}
void ExecQueue(MidiMsg * msgs) 
{
    MidiMsg * p = msgs, *t;
    while(p != NULL) 
    {
        int status = p->status;
        int data1  = p->data1;
        int data2  = p->data2;
        int msg    = p->msg & 0xF0;
        int channel= p->msg & 0x0F;
        
        if( msg == 0x90 && note_on.L != 0)
        {                
            // note on
            CallbackLua(note_on,p);
        }
        else if( msg == 0x80 && note_off.L != 0)
        {
            // note off                
            CallbackLua(note_off,p);
        }
        else if(msg == 0xA0)
        {
            // polyphonic pressure
            CallbackLua(polyphonic_key_pressure,p);
        }
        else if(msg == 0xB0)
        {
            // control change
            CallbackLua(control_change,p);
        }
        else if(msg == 0xC0)
        {
            // program change        
            CallbackLua(program_change,p);
        }
        else if(msg == 0xD0)
        {
            // channel pressure
            CallbackLua(channel_pressure,p);
            
        }
        else if(msg == 0xE0)
        {
            // pitchbend
            CallbackLua(pitch_bend,p);
        }
        else if(status == 0x79)
        {
            // reset all conrollers
            CallbackLua(reset_all_controllers,p);
        }
        else if(status == 0x7A)
        {
            // local control
            CallbackLua(local_control,p);
        }
        else if(status == 0x7B)
        {
            // all notes off
            CallbackLua(all_notes_off,p);
        }
        else if(status == 0x7C)
        {
            // omni off
            CallbackLua(omni_off,p);
        }
        else if(status == 0x7D)
        {
            // omni on
            CallbackLua(omni_on,p);
        }
        else if(status == 0x7E)
        {
            // mono mode
            CallbackLua(mono_mode,p);
        }
        else if(status == 0x7F)
        {
            // poly mode
            CallbackLua(poly_mode,p);
        }
        else if(status == 0xF8)
        {
            // clock
            CallbackLua(midi_clock,p);
        }
        else if(status == 0xFA)
        {
            // start sequences
            CallbackLua(start_sequence,p);
        }
        else if(status == 0xFB)
        {
            // continue sequence
            CallbackLua(continue_sequence,p);
        }
        else if(status == 0xFC)
        {
            // stop sequence
            CallbackLua(stop_sequence,p);
        }
        else if(status == 0xFE)
        {
            // active sense
            CallbackLua(active_sensing,p);
        }
        else if(status == 0xFF)
        {
            // system reset
            CallbackLua(system_reset,p);
        }
        else if(status == 0xF1)
        {
            // midi timing code
            CallbackLua(midi_timing_code,p);
        }
        else if(status == 0xF2)
        {
            // song position
            CallbackLua(song_position,p);
        }
        else if(status == 0xF3)
        {
            // song select
            CallbackLua(song_select,p);
        }
        else if(status == 0xF6)
        {
            // tune request
            CallbackLua(tuning_request,p);
        }
        else if(status == 0xF0)
        {
            // system exclusive
            CallbackLua(system_exclusive,p);
        }
        t = p->next;
        free(p);
        p = t;
    }
} 


void RunQueue() {    
    MidiMsg * msgs = midi_queue;
    midi_queue = NULL;
    ExecQueue(msgs);        
}

static void process_midi(PtTimestamp timestamp, void * userData)
{
    PmError result;
    PmEvent buffer;
    int channel;
    lua_State *L;

    LockMidi();
    do
    {
        result = Pm_Poll(pm_midi_input);        
        if(result)
        {
            int status,data1,data2,msg;
            if(Pm_Read(pm_midi_input, &buffer, 1) == pmBufferOverflow)
                continue;
            status = Pm_MessageStatus(buffer.message);
            data1  = Pm_MessageData1(buffer.message);
            data2  = Pm_MessageData2(buffer.message);
            channel = status & 0x0F;
            msg = status & 0xF0;   
            MidiMsg * pMsg = NewMessage(status,data1,data2,msg,channel);
            if(midi_queue == NULL) midi_queue = pMsg;
            else AddMessage(midi_queue,pMsg);
        }
    }        
    while(result);   
    UnlockMidi();
    
}

void Init() {
    pthread_mutex_init(&Lock,NULL);
    LockMidi();
}
void InitMidiDevice(int channel, int input_id, int output_id)
{
    const PmDeviceInfo *in_info,*out_info;
    midi_channel = channel;
    int output_buffer_size = 100;
    int latency = 0;

    
    Pt_Start(10,&process_midi,0);
    Pm_Initialize();
    
    if(input_id > -1)
    {
        in_info  = Pm_GetDeviceInfo(input_id);
        Pm_OpenInput(&pm_midi_input, input_id, NULL, output_buffer_size, NULL, NULL);
    }
    if(output_id > -1)
    {
        out_info  = Pm_GetDeviceInfo(output_id);
        Pm_OpenOutput(&pm_midi_output, output_id, NULL, output_buffer_size, NULL, NULL, latency);
    }
}

void StopMidi()
{    
    Pt_Stop();
    if(pm_midi_input)  Pm_Close(pm_midi_input);
    if(pm_midi_output) Pm_Close(pm_midi_output);    
}


float* float_new(size_t size)
{
    return (float*)calloc(sizeof(float),size);
}
void float_free(float * p)
{
    free(p);
}
float float_get(float * p, size_t index)
{
    return p[index];
}
void float_set(float *p, size_t index, float value)
{
    p[index] = value;
}

/////////////////////////////////////////////
// Audio
/////////////////////////////////////////////
SWIGLUA_REF audio_func={0,0};
SWIGLUA_REF callback_func={0,0};
lua_State * L, *Lua;
PaStreamParameters outputParameters;
PaStreamParameters inputParameters;
PaStream *stream;
int isAudioRunning=false;

void set_audio_func(SWIGLUA_REF f)
{        
    audio_func = f;    
    Lua = f.L;
    L = lua_newthread(f.L);        
    luaL_ref(L,LUA_REGISTRYINDEX);
    audio_func.L = L;    
}

pthread_t repl_thread;
void set_callback_func(SWIGLUA_REF f)
{
    callback_func = f;      
}

void *REPL(void * args) {
    lua_State * L = lua_newthread(Lua);        
    luaL_ref(L,LUA_REGISTRYINDEX);    
    luaL_dofile(L,"scripts/lua.lua");
    return NULL;
}

void RunAudio()
{
    //int r = pthread_create(&repl_thread, NULL, REPL,NULL);
    UnlockMidi();
    while(1)    
    {               
        LockMidi();     
        RunQueue();
        UnlockMidi();            
        Pa_Sleep(10);
    }
}

static int luaCallback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{
    swig_type_info * info = SWIGTYPE_p_float;
    
    if(L == NULL) { return -1; }
    LockMidi();    
    isAudioRunning=true;
    swiglua_ref_get(&audio_func);	
    SWIG_NewPointerObj(L,inputBuffer,info,0);
    SWIG_NewPointerObj(L,outputBuffer,info,0);
    lua_pushinteger(L,framesPerBuffer);        
	if(lua_pcall(L,3,0,0) != 0) 
    {
        printf("%s", lua_tostring(L,-1));
    }
    UnlockMidi();
    isAudioRunning=false;
    return paContinue;
}   

static void StreamFinished( void* userData )
{

}

int GetNumAudioDevices()
{
    return Pa_GetDeviceCount();
}

const char* GetAudioDeviceName(size_t i)
{
    const PaDeviceInfo* di = Pa_GetDeviceInfo(i);
    return di->name;
}


int InitAudioDevice(int output_device_number, int input_device_number, size_t num_channels, int sample_rate, int frames_per_second)
{
    PaError err;
    err = Pa_Initialize();    
    
    if( err != paNoError ) goto error;

    if(output_device_number > -1)
    {        
        outputParameters.device = output_device_number;
        if (outputParameters.device == paNoDevice) {
            fprintf(stderr,"Error: No default output device.\n");
            goto error;
        }
        outputParameters.channelCount = num_channels;       /* stereo output */
        outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
        outputParameters.suggestedLatency = 0.001; //Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
        outputParameters.hostApiSpecificStreamInfo = NULL;        
    }
    if(input_device_number > -1)
    {        
        inputParameters.device = input_device_number;
        if (inputParameters.device == paNoDevice) {
            fprintf(stderr,"Error: No default output device.\n");
            goto error;
        }
        inputParameters.channelCount = num_channels;       /* stereo output */
        inputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
        inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowOutputLatency;
        inputParameters.hostApiSpecificStreamInfo = NULL;        
    }

    err = Pa_OpenStream(
              &stream,
              input_device_number > -1? &inputParameters : NULL, /* no input */
              output_device_number > -1? &outputParameters : NULL,
              sample_rate,
              frames_per_second,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              luaCallback,
              NULL );
              
    if( err != paNoError ) goto error;

    printf("Start\n") ;
    //err = Pa_SetStreamFinishedCallback( stream, &StreamFinished );
    //if( err != paNoError ) goto error;
    
    err = Pa_StartStream( stream );    
    if( err != paNoError ) goto error;
    
    
    return err;
error:
    Pa_Terminate();
    fprintf( stderr, "An error occurred while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    exit(-1);
    return err;
}

int InitAudio(int sample_rate, int frames_per_second)
{      
    PaError err;
    err = Pa_Initialize();    
    printf("Init\n");
    if( err != paNoError ) goto error;
   
    
    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default output device.\n");
        goto error;
    }
    outputParameters.channelCount = 2;       /* stereo output */
    outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    
    err = Pa_OpenStream(
              &stream,
              NULL, /* no input */
              &outputParameters,
              sample_rate,
              frames_per_second,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              luaCallback,
              NULL );
    if( err != paNoError ) goto error;

    printf("Start\n") ;
    //err = Pa_SetStreamFinishedCallback( stream, &StreamFinished );
    //if( err != paNoError ) goto error;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto error;

    
    return err;
error:
    Pa_Terminate();
    fprintf( stderr, "An error occurred while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    exit(-1);
    return err;
}


int StopAudio()
{
    PaError err;
    err = Pa_StopStream( stream );
    if( err != paNoError ) goto error;

    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;

    Pa_Terminate();
    printf("Test finished.\n");
    return 0;
error:
    Pa_Terminate();
    fprintf( stderr, "An error occurred while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    exit(-1);
    return err;

}

void Stop()
{
    LockMidi();
    StopMidi();
    StopAudio();    
    pthread_mutex_destroy(&Lock);
}
%}
