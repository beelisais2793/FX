/*
  ZynAddSubFX - a software synthesizer

  Master.cpp - It sends Midi Messages to Parts, receives samples from parts,
             process them with system/insertion effects and mix them
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License (version 2 or later) for more details.

  You should have received a copy of the GNU General Public License (version 2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/

#include "Master.h"
#include "Instrument.h"

#include "../Params/LFOParams.h"
#include "../Effects/EffectMgr.h"
#include "../DSP/FFTwrapper.h"

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <algorithm>
#include <cmath>

#ifndef WIN32
#include <unistd.h>
#endif

using namespace std;

vuData::vuData(void)
    :outpeakl(0.0f), outpeakr(0.0f), maxoutpeakl(0.0f), maxoutpeakr(0.0f),
      rmspeakl(0.0f), rmspeakr(0.0f), clipped(0)
{}

static Master* masterInstance = NULL;

Master::Master()
    : engineManager(EngineMgr::getInstance())
{
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&vumutex, NULL);
    fft = new FFTwrapper(synth->oscilsize);

    shutup = 0;

    //System Effects init
    for(int nefx = 0; nefx < NUM_SYS_EFX; ++nefx)
        sysefx[nefx] = new EffectMgr(0, &mutex);

    defaults();
}

Master::~Master()
{
    while (this->instruments.empty() == false)
    {
        Instrument* part = this->instruments.back();
        this->instruments.pop_back();
        delete part;
    }

    delete fft;

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&vumutex);
}

void Master::defaults()
{
    volume = 1.0f;
    setPvolume(80);
    setPkeyshift(64);

    this->addInstrument();

    //System Effects init
    for(int nefx = 0; nefx < NUM_SYS_EFX; ++nefx) {
        sysefx[nefx]->defaults();
    }

    microtonal.defaults();
    ShutUp();
}

/*
 * Note On Messages (velocity=0 for NoteOff)
 */
void Master::NoteOn(char chan, char note, char velocity)
{
    if(velocity) {
        for(std::vector<Instrument*>::iterator i = this->instruments.begin(); i != this->instruments.end(); ++i)
            if(chan == (*i)->Prcvchn) {
                if((*i)->Penabled)
                    (*i)->NoteOn(note, velocity, keyshift);
            }
    }
    else
        this->NoteOff(chan, note);
}

/*
 * Note Off Messages
 */
void Master::NoteOff(char chan, char note)
{
    for(std::vector<Instrument*>::iterator i = this->instruments.begin(); i != this->instruments.end(); ++i)
        if((chan == (*i)->Prcvchn) && (*i)->Penabled)
            (*i)->NoteOff(note);
}

/*
 * Pressure Messages (velocity=0 for NoteOff)
 */
void Master::PolyphonicAftertouch(char chan, char note, char velocity)
{
    if(velocity) {
        for(std::vector<Instrument*>::iterator i = this->instruments.begin(); i != this->instruments.end(); ++i)
            if(chan == (*i)->Prcvchn)
                if((*i)->Penabled)
                    (*i)->PolyphonicAftertouch(note, velocity, keyshift);
    }
    else
        this->NoteOff(chan, note);
}

/*
 * Controllers
 */
void Master::SetController(char chan, int type, int par)
{
    if((type == C_dataentryhi) || (type == C_dataentrylo)
       || (type == C_nrpnhi) || (type == C_nrpnlo)) { //Process RPN and NRPN by the Master (ignore the chan)
        ctl.setparameternumber(type, par);
    }
    else
    if(type == C_bankselectmsb) {      // Change current bank
        if(((unsigned int)par < bank.banks.size())
           && (bank.banks[par].dir != bank.bankfiletitle))
            bank.loadbank(bank.banks[par].dir);
    }
    else {  //other controllers
        for(std::vector<Instrument*>::iterator i = this->instruments.begin(); i != this->instruments.end(); ++i)
            if((chan == (*i)->Prcvchn) && ((*i)->Penabled != 0))
                (*i)->SetController(type, par);
        ;
    }
}

void Master::SetProgram(char chan, unsigned int pgm)
{
    for(std::vector<Instrument*>::iterator i = this->instruments.begin(); i != this->instruments.end(); ++i)
        if(chan == (*i)->Prcvchn) {
            bank.loadfromslot(pgm, (*i));

            //Hack to get pad note parameters to update
            //this is not real time safe and makes assumptions about the calling
            //convention of this function...
            pthread_mutex_unlock(&mutex);
            (*i)->applyparameters();
            pthread_mutex_lock(&mutex);
        }
}

void Master::vuUpdate(const float *outl, const float *outr)
{
    //Peak computation (for vumeters)
    vu.outpeakl = 1e-12f;
    vu.outpeakr = 1e-12f;
    for(int i = 0; i < synth->buffersize; ++i) {
        if(fabs(outl[i]) > vu.outpeakl)
            vu.outpeakl = fabs(outl[i]);
        if(fabs(outr[i]) > vu.outpeakr)
            vu.outpeakr = fabs(outr[i]);
    }
    if((vu.outpeakl > 1.0f) || (vu.outpeakr > 1.0f))
        vu.clipped = 1;
    if(vu.maxoutpeakl < vu.outpeakl)
        vu.maxoutpeakl = vu.outpeakl;
    if(vu.maxoutpeakr < vu.outpeakr)
        vu.maxoutpeakr = vu.outpeakr;

    //RMS Peak computation (for vumeters)
    vu.rmspeakl = 1e-12f;
    vu.rmspeakr = 1e-12f;
    for(int i = 0; i < synth->buffersize; ++i) {
        vu.rmspeakl += outl[i] * outl[i];
        vu.rmspeakr += outr[i] * outr[i];
    }
    vu.rmspeakl = sqrt(vu.rmspeakl / synth->buffersize_f);
    vu.rmspeakr = sqrt(vu.rmspeakr / synth->buffersize_f);
}

Instrument* Master::addInstrument()
{
    pthread_mutex_lock(&(mutex));
    Instrument* instrument = new Instrument(&microtonal, fft, &mutex);

    instrument->defaults();
    instrument->Penabled = 1;
    instrument->Prcvchn = 0;
    this->instruments.push_back(instrument);
    pthread_mutex_unlock(&(mutex));

    return instrument;
}

void Master::removeInstrument(Instrument* instrument)
{
    pthread_mutex_lock(&mutex);

    for (std::vector<Instrument*>::iterator i = this->instruments.begin(); i != this->instruments.end(); ++i)
    {
        if (*i == instrument)
        {
            this->instruments.erase(i);
            delete instrument;
            break;
        }
    }

    pthread_mutex_unlock(&mutex);
}

int Master::instrumentIndex(class Instrument* instrument)
{
    for (int i = 0; i < int(this->instruments.size()); i++)
        if (instrument == this->instruments[i])
            return i;
    return -1;
}

/*
 * Master audio out (the final sound)
 */
void Master::AudioOut(float *outl, float *outr)
{
    //clean up the output samples (should not be needed?)
    memset(outl, 0, synth->bufferbytes);
    memset(outr, 0, synth->bufferbytes);

    //Compute part samples and store them part[npart]->partoutl,partoutr
    for(std::vector<Instrument*>::iterator i = this->instruments.begin(); i != this->instruments.end(); ++i) {
        Instrument* instrument = *i;
        if(instrument->Penabled != 0 && !pthread_mutex_trylock(&instrument->load_mutex)) {
            instrument->ComputePartSmps();
            pthread_mutex_unlock(&instrument->load_mutex);
        }
    }

    //Apply the part volumes and pannings (after insertion effects)
    for(std::vector<Instrument*>::iterator i = this->instruments.begin(); i != this->instruments.end(); ++i) {
        if((*i)->Penabled == 0)
            continue;

        Stereo<float> newvol((*i)->volume),
        oldvol((*i)->oldvolumel,
               (*i)->oldvolumer);

        float pan = (*i)->panning;
        if(pan < 0.5f)
            newvol.l *= pan * 2.0f;
        else
            newvol.r *= (1.0f - pan) * 2.0f;

        //the volume or the panning has changed and needs interpolation
        if(ABOVE_AMPLITUDE_THRESHOLD(oldvol.l, newvol.l)
           || ABOVE_AMPLITUDE_THRESHOLD(oldvol.r, newvol.r)) {
            for(int j = 0; j < synth->buffersize; ++j) {
                Stereo<float> vol(INTERPOLATE_AMPLITUDE(oldvol.l, newvol.l,
                                                        j, synth->buffersize),
                                  INTERPOLATE_AMPLITUDE(oldvol.r, newvol.r,
                                                        j, synth->buffersize));
                (*i)->partoutl[j] *= vol.l;
                (*i)->partoutr[j] *= vol.r;
            }
            (*i)->oldvolumel = newvol.l;
            (*i)->oldvolumer = newvol.r;
        }
        else
            for(int j = 0; j < synth->buffersize; ++j) { //the volume did not changed
                (*i)->partoutl[j] *= newvol.l;
                (*i)->partoutr[j] *= newvol.r;
            }
    }


    //System effects
    for(int nefx = 0; nefx < NUM_SYS_EFX; ++nefx) {
        if(sysefx[nefx]->geteffect() == 0)
            continue;  //the effect is disabled

        float *tmpmixl = getTmpBuffer();
        float *tmpmixr = getTmpBuffer();
        //Clean up the samples used by the system effects
        memset(tmpmixl, 0, synth->bufferbytes);
        memset(tmpmixr, 0, synth->bufferbytes);

        //Mix the channels according to the part settings about System Effect
        for(std::vector<Instrument*>::iterator i = this->instruments.begin(); i != this->instruments.end(); ++i)
        {
            Instrument* instrument = (*i);

            //skip if the part is disabled
            if(instrument->Penabled == 0)
                continue;

            //the output volume of each part to system effect
            const float vol = 1.0f;//sysefxvol[nefx][npart];
            for(int i = 0; i < synth->buffersize; ++i) {
                tmpmixl[i] += instrument->partoutl[i] * vol;
                tmpmixr[i] += instrument->partoutr[i] * vol;
            }
        }

        sysefx[nefx]->out(tmpmixl, tmpmixr);

        //Add the System Effect to sound output
        const float outvol = sysefx[nefx]->sysefxgetvolume();
        for(int i = 0; i < synth->buffersize; ++i) {
            outl[i] += tmpmixl[i] * outvol;
            outr[i] += tmpmixr[i] * outvol;
        }

        returnTmpBuffer(tmpmixl);
        returnTmpBuffer(tmpmixr);
    }


    //Mix all parts
    for(std::vector<Instrument*>::iterator i = this->instruments.begin(); i != this->instruments.end(); ++i)
        if((*i)->Penabled)   //only mix active parts
            for(int j = 0; j < synth->buffersize; ++j) { //the volume did not changed
                outl[j] += (*i)->partoutl[j];
                outr[j] += (*i)->partoutr[j];
            }

    //Master Volume
    for(int i = 0; i < synth->buffersize; ++i) {
        outl[i] *= volume;
        outr[i] *= volume;
    }

    if(!pthread_mutex_trylock(&vumutex)) {
        vuUpdate(outl, outr);
        pthread_mutex_unlock(&vumutex);
    }

    //Shutup if it is asked (with fade-out)
    if(shutup) {
        for(int i = 0; i < synth->buffersize; ++i) {
            float tmp = (synth->buffersize_f - i) / synth->buffersize_f;
            outl[i] *= tmp;
            outr[i] *= tmp;
        }
        ShutUp();
    }

    //update the LFO's time
    LFOParams::time++;
}


/*
 * Parameter control
 */
void Master::setPvolume(char Pvolume_)
{
    Pvolume = Pvolume_;
    volume  = dB2rap((Pvolume - 96.0f) / 96.0f * 40.0f);
}

void Master::setPkeyshift(char Pkeyshift_)
{
    Pkeyshift = Pkeyshift_;
    keyshift  = (int)Pkeyshift - 64;
}

/*
 * Panic! (Clean up all parts and effects)
 */
void Master::ShutUp()
{
    for(std::vector<Instrument*>::iterator i = this->instruments.begin(); i != this->instruments.end(); ++i) {
        (*i)->cleanup();
    }
    vuresetpeaks();
    shutup = 0;
}


/*
 * Reset peaks and clear the "cliped" flag (for VU-meter)
 */
void Master::vuresetpeaks()
{
    pthread_mutex_lock(&vumutex);
    vu.outpeakl    = 1e-9f;
    vu.outpeakr    = 1e-9f;
    vu.maxoutpeakl = 1e-9f;
    vu.maxoutpeakr = 1e-9f;
    vu.clipped     = 0;
    pthread_mutex_unlock(&vumutex);
}

vuData Master::getVuData()
{
    vuData tmp;
    pthread_mutex_lock(&vumutex);
    tmp = vu;
    pthread_mutex_unlock(&vumutex);
    return tmp;
}

void Master::applyparameters(bool lockmutex)
{
    for(std::vector<Instrument*>::iterator i = this->instruments.begin(); i != this->instruments.end(); ++i)
        (*i)->applyparameters(lockmutex);
}

void Master::add2XML(XMLwrapper *xml)
{
    xml->addpar("volume", Pvolume);
    xml->addpar("key_shift", Pkeyshift);
    xml->addparbool("nrpn_receive", ctl.NRPN.receive);

    xml->beginbranch("MICROTONAL");
    microtonal.add2XML(xml);
    xml->endbranch();

    for(int npart = 0; npart < int(this->instruments.size()); ++npart) {
        xml->beginbranch("PART", npart);
        this->instruments[npart]->add2XML(xml);
        xml->endbranch();
    }
}

int Master::saveXML(const char *filename)
{
    XMLwrapper *xml = new XMLwrapper();

    xml->beginbranch("MASTER");
    add2XML(xml);
    xml->endbranch();

    int result = xml->saveXMLfile(filename);
    delete (xml);
    return result;
}

int Master::loadXML(const char *filename)
{
    XMLwrapper *xml = new XMLwrapper();
    if(xml->loadXMLfile(filename) < 0) {
        delete (xml);
        return -1;
    }

    if(xml->enterbranch("MASTER") == 0)
        return -10;
    getfromXML(xml);
    xml->exitbranch();

    delete (xml);
    return 0;
}

void Master::getfromXML(XMLwrapper *xml)
{
    setPvolume(xml->getpar127("volume", Pvolume));
    setPkeyshift(xml->getpar127("key_shift", Pkeyshift));
    ctl.NRPN.receive = xml->getparbool("nrpn_receive", ctl.NRPN.receive);


    instruments[0]->Penabled = 0;
    int npart = 0;
    for(std::vector<Instrument*>::iterator i = this->instruments.begin(); i != this->instruments.end(); ++i, ++npart) {
        if(xml->enterbranch("PART", npart) == 0)
            continue;
        (*i)->getfromXML(xml);
        xml->exitbranch();
    }

    if(xml->enterbranch("MICROTONAL")) {
        microtonal.getfromXML(xml);
        xml->exitbranch();
    }
}
