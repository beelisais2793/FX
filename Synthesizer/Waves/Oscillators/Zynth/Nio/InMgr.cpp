#include "InMgr.h"
#include "MidiIn.h"
#include "EngineMgr.h"
#include <iostream>

using namespace std;

ostream& operator << (ostream& out, const MidiEvent& ev)
{
    switch(ev.type) {
        case M_NOTE:
            out << "MidiNote: note(" << ev.num << ")\n"
                << "          channel(" << ev.channel << ")\n"
                << "          velocity(" << ev.value << ")";
            break;

        case M_CONTROLLER:
            out << "MidiCtl: controller(" << ev.num << ")\n"
                << "         channel(" << ev.channel << ")\n"
                << "         value(" << ev.value << ")";
            break;

        case M_PGMCHANGE:
            out << "PgmChange: program(" << ev.num << ")\n"
                << "           channel(" << ev.channel << ")";
            break;
    }

    return out;
}

MidiEvent::MidiEvent()
    :channel(0), type(0), num(0), value(0)
{}

InMgr &InMgr::getInstance()
{
    static InMgr instance;
    return instance;
}

InMgr::InMgr()
    :queue(100)
{
    current = NULL;
    sem_init(&work, PTHREAD_PROCESS_PRIVATE, 0);
}

InMgr::~InMgr()
{
    //lets stop the consumer thread
    sem_destroy(&work);
}

void InMgr::AddMixer(IMixer* mixer)
{
    this->mixers.insert(mixer);
}

void InMgr::RemoveMixer(IMixer* mixer)
{
    this->mixers.erase(mixer);
}

void InMgr::putEvent(MidiEvent ev)
{
    if(queue.push(ev)) //check for error
        cerr << "ERROR: Midi Ringbuffer is FULL" << endl;
    else
        sem_post(&work);
}

void InMgr::flush()
{
    MidiEvent ev;
    while(!sem_trywait(&work))
    {
        queue.pop(ev);
        //cout << ev << endl;

        for (std::set<IMixer*>::iterator i = this->mixers.begin(); i != this->mixers.end(); ++i)
        {
            ((IMixer*)*i)->Lock();
            switch(ev.type) {
                case M_NOTE:
                    if(ev.value)
                        ((IMixer*)*i)->NoteOn(ev.channel, ev.num, ev.value);
                    else
                        ((IMixer*)*i)->NoteOff(ev.channel, ev.num);
                    break;

                case M_CONTROLLER:
                    ((IMixer*)*i)->SetController(ev.channel, ev.num, ev.value);
                    break;

                case M_PGMCHANGE:
                    ((IMixer*)*i)->SetProgram(ev.channel, ev.num);
                    break;
                case M_PRESSURE:
                    ((IMixer*)*i)->PolyphonicAftertouch(ev.channel, ev.num, ev.value);
                    break;
            }
            ((IMixer*)*i)->UnLock();
        }
    }
}

bool InMgr::SetSource(string name)
{
    MidiIn *src = GetIn(name);

    if(!src)
        return false;

    if(current)
        current->setMidiEn(false);
    current = src;
    current->setMidiEn(true);

    bool success = current->getMidiEn();

    //Keep system in a valid state (aka with a running driver)
    if(!success)
        (current = GetIn("NULL"))->setMidiEn(true);

    return success;
}

string InMgr::GetSource() const
{
    if(current)
        return current->name;
    else
        return "ERROR";
}

MidiIn *InMgr::GetIn(string name)
{
    EngineMgr &eng = EngineMgr::getInstance();
    return dynamic_cast<MidiIn *>(eng.getEng(name));
}
