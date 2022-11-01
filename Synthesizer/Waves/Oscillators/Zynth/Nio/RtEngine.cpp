/*
  ZynAddSubFX - a software synthesizer

  RtEngine.h - Midi input through RtMidi for Windows
  Copyright (C) 2014 Wouter Saaltink
  Author: Wouter Saaltink

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
#include "RtEngine.h"
#include "InMgr.h"

RtEngine::RtEngine()
    : midiin(0)
{
    name = "RT";
}

RtEngine::~RtEngine()
{
    this->Stop();
}

bool RtEngine::Start()
{
    this->midiin = new RtMidiIn();
    if (this->midiin->getPortCount() > 0)
    {
        for (int i  = 0; i < this->midiin->getPortCount(); i++)
            this->ports.push_back(this->midiin->getPortName(i));

        this->midiin->setCallback(RtEngine::callback, this);
        this->midiin->openPort(0);
        return true;
    }

    delete this->midiin;
    this->midiin = 0;

    return false;
}

void RtEngine::Stop()
{
    if (this->midiin != 0)
        delete this->midiin;
    this->midiin = 0;
}

void RtEngine::setMidiEn(bool nval)
{
    if (nval)
        this->Start();
    else
        this->Stop();
}

bool RtEngine::getMidiEn() const
{
    return (this->midiin != 0);
}

const std::vector<std::string> RtEngine::GetPorts() const
{
    return this->ports;
}

void RtEngine::SetPort(int port)
{
    if (this->midiin != 0 && port >= 0 && port < this->ports.size())
    {
        this->midiin->closePort();
        this->midiin->openPort(port);
    }
}

void RtEngine::callback(double timeStamp, std::vector<unsigned char> *message, void *userData)
{
    MidiIn::midiProcess(message->at(0), message->at(1), message->at(2));
}
