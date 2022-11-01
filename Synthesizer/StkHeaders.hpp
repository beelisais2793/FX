// todo: ports, randomizers, modulators


#pragma once 

#define __OS_LINUX__
#define __STK_REALTIME__
#include "stk/Stk.h"
#include "stk/ADSR.h"
#include "stk/Asymp.h"
#include "stk/Envelope.h"
#include "stk/Blit.h"
#include "stk/BlitSaw.h"
#include "stk/BlitSquare.h"
#include "stk/BiQuad.h"
#include "stk/BandedWG.h"
#include "stk/BeeThree.h"
#include "stk/BlowBotl.h"
#include "stk/BlowHole.h"
#include "stk/BowTable.h"
#include "stk/Bowed.h"
#include "stk/Brass.h"
#include "stk/Chorus.h"
#include "stk/Clarinet.h"
#include "stk/Cubic.h"
#include "stk/Delay.h"
#include "stk/DelayA.h"
#include "stk/DelayL.h"
#include "stk/Drummer.h"
#include "stk/Echo.h"
#include "stk/Effect.h"
#include "stk/FM.h"
#include "stk/FMVoices.h"
#include "stk/FileLoop.h"
#include "stk/FileRead.h"
#include "stk/FileWrite.h"
#include "stk/FileWvIn.h"
#include "stk/FileWvOut.h"
#include "stk/Filter.h"
#include "stk/Fir.h"
#include "stk/Flute.h"
#include "stk/FormSwep.h"
#include "stk/FreeVerb.h"
#include "stk/Function.h"
#include "stk/Generator.h"
#include "stk/Granulate.h"
#include "stk/Guitar.h"
#include "stk/HevyMetl.h"
#include "stk/Iir.h"
#include "stk/InetWvIn.h"
#include "stk/InetWvOut.h"
#include "stk/Instrmnt.h"
#include "stk/JCRev.h"
#include "stk/JetTable.h"
#include "stk/LentPitShift.h"
#include "stk/Mandolin.h"
#include "stk/Mesh2D.h"
#include "stk/Messager.h"
#include "stk/MidiFileIn.h"
#include "stk/Modal.h"
#include "stk/ModalBar.h"
#include "stk/Modulate.h"
#include "stk/Moog.h"
#include "stk/Mutex.h"
#include "stk/NRev.h"
#include "stk/Noise.h"
#include "stk/OnePole.h"
#include "stk/OneZero.h"
#include "stk/TwoPole.h"
#include "stk/TwoZero.h"
#include "stk/PoleZero.h"
#include "stk/PRCRev.h"
#include "stk/PercFlut.h"
#include "stk/Phonemes.h"
#include "stk/Plucked.h"
#include "stk/Recorder.h"
#include "stk/ReedTable.h"
#include "stk/Resonate.h"
#include "stk/Rhodey.h"
#include "stk/SKINImsg.h"
#include "stk/SKINItbl.h"
#include "stk/Sampler.h"
#include "stk/Saxofony.h"
#include "stk/Shakers.h"
#include "stk/Simple.h"
#include "stk/SineWave.h"
#include "stk/SingWave.h"
#include "stk/Sitar.h"
#include "stk/Skini.h"
#include "stk/Socket.h"
#include "stk/Sphere.h"
#include "stk/StifKarp.h"
#include "stk/TapDelay.h"
#include "stk/TcpClient.h"
#include "stk/TcpServer.h"
#include "stk/Thread.h"
#include "stk/TubeBell.h"
#include "stk/Twang.h"
#include "stk/UdpSocket.h"
#include "stk/Vector3D.h"
#include "stk/VoicForm.h"
#include "stk/Voicer.h"
#include "stk/Whistle.h"
#include "stk/Wurley.h"
#include "stk/WvIn.h"
#include "stk/WvOut.h"

namespace Stk 
{
    namespace System
    {
        using FileLoop = stk::FileLoop;
        using FileRead = stk::FileRead;
        using FileWrite= stk::FileWrite;
        using FileWvIn = stk::FileWvIn;
        using FileWvOut = stk::FileWvOut;
        using InetWvIn = stk::InetWvIn;
        using InetWvOut = stk::InetWvOut;
        using Messager = stk::Messager;
        using MidiFileIn = stk::MidiFileIn;
        using Mutex = stk::Mutex;
        using Socket = stk::Socket;
        using TcpClient = stk::TcpClient;
        using TcpServer = stk::TcpServer;
        using UdpSocket = stk::UdpSocket;
        using Thread = stk::Thread;
        using WvIn = stk::WvIn;
        using WvOut = stk::WvOut;
    };

    namespace Generators {
        using Generator= stk::Generator;
        using ADSR = stk::ADSR;
        using Asymp= stk::Asymp;
        using Envelope= stk::Envelope;
        using Blit = stk::Blit;
        using BlitSaw = stk::BlitSaw;
        using BlitSquare= stk::BlitSquare;
        using Modulator = stk::Modulate;
        using Noise = stk::Noise;
    }

    namespace Filters {
        using Filter   = stk::Filter;
        using BiQuad   = stk::BiQuad;
        using Fir      = stk::Fir;
        using Iir      = stk::Iir;
        using OnePole  = stk::OnePole;
        using OneZero  = stk::OneZero;
        using PoleZero = stk::PoleZero;
        using TwoPole  = stk::TwoPole;
        using TwoZero  = stk::TwoZero;
    }

    namespace FX 
    {
        using Effect   = stk::Effect;
        using Chorus   = stk::Chorus;
        using Delay    = stk::Delay;
        using DelayL   = stk::DelayL;
        using DelayA   = stk::DelayA;
        using Echo     = stk::Echo;
        using FreeVerb = stk::FreeVerb;
        using JCRev    = stk::JCRev;
        using LentPitchShift = stk::LentPitShift;
        using NRev     = stk::NRev;
        using PRCRev   = stk::PRCRev;
        using TapDelay = stk::TapDelay;
    }

    namespace Functions
    {
        using Function = stk::Function;
        using Cubic    = stk::Cubic;
        using Phonemes = stk::Phonemes;
        using Sphere   = stk::Sphere;
        using Vector3d = stk::Vector3D;
    }


    namespace Instruments
    {
        using Instrument = stk::Instrmnt;
        using Sampler    = stk::Sampler;
        using Moog       = stk::Moog;

        using BlowBotl = stk::BlowBotl;
        using BlowHole = stk::BlowHole;
        using BowTable = stk::BowTable;
        using Bowed    = stk::Bowed;
        using JetTable = stk::JetTable;
        using Mesh2D   = stk::Mesh2D;
        using Modal    = stk::Modal;
        using ModalBar = stk::ModalBar;

        using BandedWG = stk::BandedWG;
        using Brass    = stk::Brass;
        using Clarinet = stk::Clarinet;
        using Flute    = stk::Flute;
        using Guitar   = stk::Guitar;
        using Mandolin = stk::Mandolin;
        using Plucked  = stk::Plucked;
        using Recorder = stk::Recorder;
        using ReedTable= stk::ReedTable;
        using Resonate = stk::Resonate;

        using Drummer  = stk::Drummer;
        using FormSwep = stk::FormSwep;
        using Granulate= stk::Granulate;

        using FM        = stk::FM;
        using FMVoice   = stk::FMVoices;
        using BeeThree  = stk::BeeThree;
        using HeavyMetal= stk::HevyMetl;
        using PercFlute = stk::PercFlut;
        using Rhodey    = stk::Rhodey;

        using Saxofony = stk::Saxofony;
        using Shakers  = stk::Shakers;
        using Simple   = stk::Simple;
        using SineWave = stk::SineWave;
        using SingWave = stk::SingWave;
        using Sitar    = stk::Sitar;
        using TubeBell = stk::TubeBell;
        using Twang    = stk::Twang;
        using VoiceForm = stk::VoicForm;
        using Voicer = stk::Voicer;
        using Whistle = stk::Whistle;
        using StifKarp = stk::StifKarp;
    }
    
    // todo: ports
    struct FileLoop : public GeneratorProcessorPlugin<stk::FileLoop>
    {
        FileLoop() : GeneratorProcessorPlugin<stk::FileLoop>()
        {

        }

        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };

    // todo: ports
    struct FileWvIn : public GeneratorProcessorPlugin<stk::FileWvIn>
    {
        FileWvIn() : GeneratorProcessorPlugin<stk::FileWvIn>()
        {

        }

        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };

    // todo: ports
    struct FileWvOut : public FunctionProcessorPlugin<stk::FileWvOut>
    {
        FileWvOut() : FunctionProcessorPlugin<stk::FileWvOut>()
        {

        }

        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            this->tick(I);
            return I;
        }
    };
    
    // todo: ports
    struct InetWvIn : public GeneratorProcessorPlugin<stk::InetWvIn>
    {
        InetWvIn() : GeneratorProcessorPlugin<stk::InetWvIn>()
        {

        }

        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };

    // todo: ports
    struct InetWvOut : public FunctionProcessorPlugin<stk::InetWvOut>
    {
        InetWvOut() : FunctionProcessorPlugin<stk::InetWvOut>()
        {

        }

        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            this->tick(I);
            return A*I;
        }
    };


    // todo: ports
    struct Sphere : public GeneratorProcessorPlugin<stk::Sphere>
    {
        Sphere() : GeneratorProcessorPlugin<stk::Sphere>()
        {

        }

        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            this->tick(I);
            return I*A;
        }
    };
}