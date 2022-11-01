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


namespace Stk::Generators {
        using Generator= stk::Generator;        
        using Blit = stk::Blit;
        using BlitSaw = stk::BlitSaw;
        using BlitSquare= stk::BlitSquare;
        using Modulator = stk::Modulate;
        using Noise = stk::Noise;
}
