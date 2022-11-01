#include "Stk.hpp"
#include "StkADSR.hpp"
#include "StkAsymp.hpp"
#include "StkBandedWG.hpp"
#include "StkBeeThree.hpp"
#include "StkBlit.hpp"
#include "StkBlitSaw.hpp"
#include "StkBlitSquare.hpp"
#include "StkBlowBotl.hpp"
#include "StkBlowHole.hpp"
#include "StkBowed.hpp"
#include "StkBowTable.hpp"
#include "StkBrass.hpp"
#include "StkChorus.hpp"
#include "StkClarinet.hpp"
#include "StkCubic.hpp"
#include "StkDelay.hpp"
#include "StkDelayA.hpp"
#include "StkDelayL.hpp"
#include "StkDrummer.hpp"
#include "StkEcho.hpp"
#include "StkEnvelope.hpp"
#include "StkFilter.hpp"
#include "StkFir.hpp"
#include "StkFlute.hpp"
#include "StkFM.hpp"
#include "StkFMVoices.hpp"
#include "StkFormSwep.hpp"
#include "StkFreeVerb.hpp"
#include "StkGranulate.hpp"
#include "StkGuitar.hpp"
#include "StkHevyMetl.hpp"
#include "StkIir.hpp"
#include "StkJCRev.hpp"
#include "StkJetTable.hpp"
#include "StkLentPitShift.hpp"
#include "StkMandolin.hpp"
#include "StkMesh2D.hpp"
#include "StkModal.hpp"
#include "StkModalBar.hpp"
#include "StkModulate.hpp"
#include "StkMoog.hpp"
#include "StkNoise.hpp"
#include "StkNRev.hpp"
#include "StkOnePole.hpp"
#include "StkOneZero.hpp"
#include "StkPercFlut.hpp"
#include "StkPitShift.hpp"
#include "StkPlucked.hpp"
#include "StkPoleZero.hpp"
#include "StkPolyBLEP.hpp"
#include "StkRecorder.hpp"
#include "StkReedTable.hpp"
#include "StkResonate.hpp"
#include "StkRhodey.hpp"
#include "StkSampler.hpp"
#include "StkSaxofony.hpp"
#include "StkShakers.hpp"
#include "StkSimple.hpp"
#include "StkSineWave.hpp"
#include "StkSingWave.hpp"
#include "StkSitar.hpp"
#include "StkSkini.hpp"
#include "StkSocket.hpp"
#include "StkSphere.hpp"
#include "StkStifKarp.hpp"
#include "StkTapDelay.hpp"
#include "StkTcpClient.hpp"
#include "StkTcpServer.hpp"
#include "StkThread.hpp"
#include "StkTubeBell.hpp"
#include "StkTwang.hpp"
#include "StkTwoPole.hpp"
#include "StkTwoZero.hpp"
#include "StkUdpSocket.hpp"
#include "StkVoicer.hpp"
#include "StkVoicForm.hpp"
#include "StkWhistle.hpp"

// these classes don't need to be templates
// They are right now because Stk is the base class of everything
#include "StkInetWvIn.hpp"
#include "StkInetWvOut.hpp"
#include "StkMessager.hpp"
#include "StkMidiFileIn.hpp"
#include "StkFileLoop.hpp"
#include "StkFileRead.hpp"
#include "StkFileWrite.hpp"
#include "StkFileWvIn.hpp"
#include "StkFileWvOut.hpp"
#include "StkMutex.hpp"
#include "StkPRCRev.hpp"
#include "StkRtWvIn.hpp"
#include "StkRtWvOut.hpp"

using namespace stk;



int main()
{
    setRawwavePath("stk/rawwaves");
    ADSR<float> ADSR;
    Asymp<float> Asymp;
    BandedWG<float> bwg;
    BeeThree<float> b3;
    BlowBotl<float> blowbotl;
    BlowHole<float> blowhole(60.0f);
    Bowed<float> bowed;
    BowTable<float> bowtable;
    Brass<float> brass;
    Chorus<float> chorus;
    Clarinet<float> clarinet;
    Cubic<float> cubie;
    Delay<float> delay;
    DelayA<float> delaya;
    DelayL<float> delayl;
    Drummer<float> drums;
    Echo<float> echo;
    Envelope<float> env;
    Flute<float> flute(100.0f);
    FormSwep<float> form;
    FreeVerb<float> freeverb;
    Granulate<float> grains;
    Guitar<float> guitar;
    HevyMetl<float> heavy;
    Iir<float> iir;
    InetWvIn<float> inetin;
    InetWvOut<float> inetout;
    JCRev<float> jcrev;
    JetTable<float> jet;
    LentPitShift<float> pitch;
    Mandolin<float> mandolin(100.0f);
    Mesh2D<float> mesh2d(2,2);
    Messager msg;
    //MidiFileIn<float> midiin;    
    ModalBar<float> modalbar;
    Modulate<float> mod;
    Moog<float> moog;
    Mutex m;
    Noise<float> noise;
    PercFlut<float> perc;
    PitShift<float> pitshift;
    Plucked<float> plucky;
    StkPolyBLEP<float> blep(44100.0f);
    PRCRev<float> prc;
    Recorder<float> recorder;
    ReedTable<float> reed;
    Resonate<float> res;
    Rhodey<float> rhodey;
    Saxofony<float> sax(440);
    Shakers<float> shakers();
    Simple<float> simpy;
    Sitar<float> sitar;

    SetSampleRate(44100);
    StkFrames<float> f(1024,2);
    f.fill(0,1);
    f.fill(1,2);
    f.print();
    std::cout << f.frames() << std::endl;
    std::cout << f.channels() << std::endl;
    StkFrames<float> l(1024,1);
    f.getChannel(0,l,0);
    l.print();
    std::vector<float> right = f.GetVectorChannel(1);
    l = right;
    l.print();
    f.setChannel(0,l,0);
    f.print();
}
