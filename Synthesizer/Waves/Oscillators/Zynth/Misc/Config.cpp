/*
  ZynAddSubFX - a software synthesizer

  Config.cpp - Configuration file functions
  Copyright (C) 2003-2005 Nasca Octavian Paul
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
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <direct.h>
#endif

#include "Config.h"
#include "XMLwrapper.h"

using namespace std;

Config::Config()
{}
void Config::Init()
{
    //defaults
    cfg.SampleRate      = 44100;
    cfg.SoundBufferSize = 256;
    cfg.OscilSize  = 1024;
    cfg.SwapStereo = 0;

    cfg.Interpolation = 0;
    cfg.CheckPADsynth = 1;

//get the midi input devices name
    cfg.currentBankDir = "./testbnk";

    this->ReadConfig(this->GetConfigFileName());

    if(cfg.bankRootDirList.empty()) {
        //banks
        cfg.bankRootDirList.push_back("~/banks");
        cfg.bankRootDirList.push_back("./");
        cfg.bankRootDirList.push_back("/usr/share/zynaddsubfx/banks");
        cfg.bankRootDirList.push_back("/usr/local/share/zynaddsubfx/banks");
        cfg.bankRootDirList.push_back("../banks");
        cfg.bankRootDirList.push_back("banks");
    }

    if(cfg.presetsDirList.empty()) {
        //presets
        cfg.presetsDirList.push_back("./");
        cfg.presetsDirList.push_back("../presets");
        cfg.presetsDirList.push_back("presets");
        cfg.presetsDirList.push_back("/usr/share/zynaddsubfx/presets");
        cfg.presetsDirList.push_back("/usr/local/share/zynaddsubfx/presets");
    }
}

Config::~Config()
{ }


void Config::Save()
{
    this->SaveConfig(this->GetConfigFileName());
}

void Config::ClearBankRootDirList()
{
    cfg.bankRootDirList.clear();
}

void Config::ClearPresetsDirList()
{
    cfg.presetsDirList.clear();
}

void Config::ReadConfig(const std::string& filename)
{
    XMLwrapper xmlcfg;
    if(xmlcfg.loadXMLfile(filename) < 0)
        return;
    if(xmlcfg.enterbranch("CONFIGURATION"))
    {
        cfg.SampleRate = xmlcfg.getpar("sample_rate",
                                       cfg.SampleRate,
                                       4000,
                                       1024000);
        cfg.SoundBufferSize = xmlcfg.getpar("sound_buffer_size",
                                            cfg.SoundBufferSize,
                                            16,
                                            8192);
        cfg.OscilSize = xmlcfg.getpar("oscil_size",
                                      cfg.OscilSize,
                                      MAX_AD_HARMONICS * 2,
                                      131072);
        cfg.SwapStereo = xmlcfg.getpar("swap_stereo",
                                       cfg.SwapStereo,
                                       0,
                                       1);

        cfg.currentBankDir = xmlcfg.getparstr("bank_current", "");
        cfg.Interpolation  = xmlcfg.getpar("interpolation",
                                           cfg.Interpolation,
                                           0,
                                           1);

        cfg.CheckPADsynth = xmlcfg.getpar("check_pad_synth",
                                          cfg.CheckPADsynth,
                                          0,
                                          1);

        //get bankroot dirs
        int i = 0;
        while (xmlcfg.enterbranch("BANKROOT", i++)) {
                cfg.bankRootDirList.push_back(xmlcfg.getparstr("bank_root", ""));
                xmlcfg.exitbranch();
            }

        //get preset root dirs
        i = 0;
        while (xmlcfg.enterbranch("PRESETSROOT", i++)) {
                cfg.presetsDirList.push_back(xmlcfg.getparstr("presets_root", ""));
                xmlcfg.exitbranch();
            }

        xmlcfg.exitbranch();
    }

    cfg.OscilSize = (int) powf(2, ceil(logf(cfg.OscilSize - 1.0f) / logf(2.0f)));
}

void Config::SaveConfig(const std::string& filename)
{
    XMLwrapper *xmlcfg = new XMLwrapper();

    xmlcfg->beginbranch("CONFIGURATION");

    xmlcfg->addpar("sample_rate", cfg.SampleRate);
    xmlcfg->addpar("sound_buffer_size", cfg.SoundBufferSize);
    xmlcfg->addpar("oscil_size", cfg.OscilSize);
    xmlcfg->addpar("swap_stereo", cfg.SwapStereo);

    xmlcfg->addpar("check_pad_synth", cfg.CheckPADsynth);

    xmlcfg->addparstr("bank_current", cfg.currentBankDir);


    for(unsigned int i = 0; i < cfg.bankRootDirList.size(); ++i) {
        xmlcfg->beginbranch("BANKROOT", i);
        xmlcfg->addparstr("bank_root", cfg.bankRootDirList[i]);
        xmlcfg->endbranch();
    }

    for(unsigned int i = 0; i < cfg.presetsDirList.size(); ++i) {
        xmlcfg->beginbranch("PRESETSROOT", i);
        xmlcfg->addparstr("presets_root", cfg.presetsDirList[i]);
        xmlcfg->endbranch();
    }

    xmlcfg->addpar("interpolation", cfg.Interpolation);

    xmlcfg->endbranch();

    xmlcfg->saveXMLfile(filename);

    delete (xmlcfg);
}

std::string Config::GetConfigFileName()
{
#ifdef WIN32
    char win[256] = { 0 };
    _getcwd(win, 256);
    return std::string(win) + std::string("/.libzynth.cfg");
#else
    return std::string(getenv("HOME")) + std::string("/.libzynth.cfg");
#endif
}
