/*
  ZynAddSubFX - a software synthesizer

  Master.h - It sends Midi Messages to Parts, receives samples from parts,
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

#ifndef MASTER_H
#define MASTER_H
#include <pthread.h>

#include "../globals.h"
#include "Microtonal.h"

#include "Bank.h"
#include "XMLwrapper.h"
#include "Instrument.h"

#include "../Params/Controller.h"
#include "../Nio/IMixer.h"
#include "../Nio/EngineMgr.h"

struct vuData {
    vuData(void);
    float outpeakl, outpeakr, maxoutpeakl, maxoutpeakr,
          rmspeakl, rmspeakr;
    int clipped;
};


/** It sends Midi Messages to Parts, receives samples from parts,
 *  process them with system/insertion effects and mix them */
class Master : public IMixer
{
public:
    Master();
    ~Master();

    /**Saves all settings to a XML file
     * @return 0 for ok or <0 if there is an error*/
    int saveXML(const char *filename);

    /**This adds the parameters to the XML data*/
    void add2XML(XMLwrapper *xml);

    void defaults();

    /**loads all settings from a XML file
     * @return 0 for ok or -1 if there is an error*/
    int loadXML(const char *filename);
    void applyparameters(bool lockmutex = true);

    void getfromXML(XMLwrapper *xml);

    /**Audio Output*/
    virtual void AudioOut(float *outl, float *outr);

    //Midi IN
    virtual void NoteOn(char chan, char note, char velocity);
    virtual void NoteOff(char chan, char note);
    virtual void PolyphonicAftertouch(char chan, char note, char velocity);
    virtual void SetController(char chan, int type, int par);
    virtual void SetProgram(char chan, unsigned int pgm);
    //void NRPN...


    void ShutUp();
    int shutup;

    void vuUpdate(const float *outl, const float *outr);

    std::vector<Instrument*>& Instruments() { return this->instruments; }
    Instrument* addInstrument();
    void removeInstrument(Instrument* channel);
    int instrumentIndex(Instrument* channel);

    unsigned char Pvolume;
    unsigned char Pkeyshift;

    //parameters control
    void setPvolume(char Pvolume_);
    void setPkeyshift(char Pkeyshift_);

    //effects
    class EffectMgr * sysefx[NUM_SYS_EFX]; //system
    class EffectMgr * insefx[NUM_INS_EFX]; //insertion

    //peaks for VU-meter
    void vuresetpeaks();
    //get VU-meter data
    vuData getVuData();

    Controller ctl;
    Microtonal microtonal;
    Bank       bank;

    class FFTwrapper * fft;

    EngineMgr& engineManager;
    virtual void Lock() { pthread_mutex_lock(&this->mutex); }
    virtual void UnLock() { pthread_mutex_unlock(&this->mutex); }
private:
    std::vector<Instrument *> instruments;
    vuData vu;
    float  volume;
    int    keyshift;

    pthread_mutex_t mutex;
    pthread_mutex_t vumutex;
};

#endif
