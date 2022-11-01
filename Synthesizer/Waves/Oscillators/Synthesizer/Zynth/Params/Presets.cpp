/*
  ZynAddSubFX - a software synthesizer

  Presets.cpp - Presets and Clipboard management
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

#include "Presets.h"
#include <string.h>


Presets::Presets()
{
    type[0] = 0;
}

Presets::~Presets()
{}

void Presets::setpresettype(const std::string& type)
{
    this->type = type;
}

void Presets::copy(const char *name)
{
    XMLwrapper *xml = new XMLwrapper();

    //used only for the clipboard
    if(name == NULL)
        xml->minimal = false;

    std::string type;
    type = this->type;

    if(name == NULL)
        if(type.find("Plfo") == std::string::npos)
            type = type + std::string("Plfo");

    xml->beginbranch(type.c_str());
    add2XML(xml);
    xml->endbranch();

    if(name == NULL)
        presetsstore.copyclipboard(xml, type.c_str());
    else
        presetsstore.copypreset(xml, type.c_str(), name);

    delete (xml);
}

void Presets::paste(int npreset)
{
    std::string type = this->type;

    if(npreset == 0)
        if(type.find("Plfo") == std::string::npos)
            type = type + std::string("Plfo");

    XMLwrapper *xml = new XMLwrapper();
    if(npreset == 0) {
        if(!checkclipboardtype()) {
            delete (xml);
            return;
        }
        if(!presetsstore.pasteclipboard(xml)) {
            delete (xml);
            return;
        }
    }
    else
    if(!presetsstore.pastepreset(xml, npreset)) {
        delete (xml);
        return;
    }

    if(xml->enterbranch(type) == 0)
        return;

    defaults();
    getfromXML(xml);

    xml->exitbranch();

    delete (xml);
}

bool Presets::checkclipboardtype()
{
    return presetsstore.checkclipboardtype(this->type.c_str());
}

void Presets::rescanforpresets()
{
    presetsstore.rescanforpresets(this->type.c_str());
}


void Presets::deletepreset(int npreset)
{
    presetsstore.deletepreset(npreset);
}
