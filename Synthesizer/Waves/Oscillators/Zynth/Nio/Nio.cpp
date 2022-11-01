#include "Nio.h"
#include "OutMgr.h"
#include "InMgr.h"
#include "EngineMgr.h"
#include "MidiIn.h"
#include "AudioOut.h"
#include "WavEngine.h"
#include <iostream>
#include <algorithm>
using std::string;
using std::set;
using std::cerr;
using std::endl;

InMgr     *in  = NULL;
OutMgr    *out = NULL;
EngineMgr *eng = NULL;
string     postfix;

bool   Nio::autoConnect   = false;
string Nio::defaultSource = IN_DEFAULT;
string Nio::defaultSink   = OUT_DEFAULT;

Settings *synth;

bool Nio::Start(Settings *s)
{
    synth = s;
    in  = &InMgr::getInstance(); //Enable input wrapper
    out = &OutMgr::getInstance(); //Initialize the Output Systems
    eng = &EngineMgr::getInstance(); //Initialize The Engines

    return eng->start();
}

void Nio::Stop()
{
    eng->stop();
}

void Nio::AddMixer(IMixer* mixer)
{
    in->AddMixer(mixer);
    out->AddMixer(mixer);
}

void Nio::RemoveMixer(IMixer* mixer)
{
    in->RemoveMixer(mixer);
    out->RemoveMixer(mixer);
}

void Nio::SetDefaultSource(string name)
{
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);
    defaultSource = name;
}

void Nio::SetDefaultSink(string name)
{
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);
    defaultSink = name;
}

bool Nio::SetSource(string name)
{
    return in->SetSource(name);
}

bool Nio::SetSink(string name)
{
    return out->setSink(name);
}

void Nio::SetPostfix(std::string post)
{
    postfix = post;
}

std::string Nio::GetPostfix()
{
    return postfix;
}

set<string> Nio::GetSources()
{
    set<string> sources;
    for(std::list<Engine *>::iterator itr = eng->engines.begin();
        itr != eng->engines.end(); ++itr)
        if(dynamic_cast<MidiIn *>(*itr))
            sources.insert((*itr)->name);
    return sources;
}

set<string> Nio::GetSinks()
{
    set<string> sinks;
    for(std::list<Engine *>::iterator itr = eng->engines.begin();
        itr != eng->engines.end(); ++itr)
        if(dynamic_cast<AudioOut *>(*itr))
            sinks.insert((*itr)->name);
    return sinks;
}

string Nio::GetSource()
{
    return in->GetSource();
}

string Nio::GetSink()
{
    return out->getSink();
}

#if JACK
#include <jack/jack.h>
void Nio::PreferedSampleRate(unsigned &rate)
{
    jack_client_t *client = jack_client_open("temp-client",
                                             JackNoStartServer, 0);
    if(client) {
        rate = jack_get_sample_rate(client);
        jack_client_close(client);
    }
}
#else
void Nio::PreferedSampleRate(unsigned &)
{}
#endif

void Nio::WaveNew(class WavFile *wave)
{
    out->wave->newFile(wave);
}

void Nio::WaveStart()
{
    out->wave->Start();
}

void Nio::WaveStop()
{
    out->wave->Stop();
}

void Nio::WaveEnd()
{
    out->wave->destroyFile();
}
