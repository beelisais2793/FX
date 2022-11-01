%module gamma
%{
#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"
using namespace gam;
using namespace gam::phsInc;

%}
%include "stdint.i"
%include "std_vector.i"
%include "lua_fnptr.i"

%include "Gamma/Types.h"
%include "Gamma/Access.h"
%include "Gamma/Containers.h"
%include "Gamma/Conversion.h"
%include "Gamma/Node.h"
%include "Gamma/UnitMaps.h"
%include "Gamma/arr.h"
%include "Gamma/gen.h"

%include "Gamma/Analysis.h"

%ignore gam::AudioIOData::in;
%include "Gamma/AudioIO.h"
%rename(get_in) gam::AudioIOData::in;

%include "Gamma/DFT.h"
%include "Gamma/Delay.h"
%include "Gamma/Domain.h"
%include "Gamma/Effects.h"
%include "Gamma/Envelope.h"
%include "Gamma/FFT.h"
%include "Gamma/Filter.h"
%include "Gamma/FormantData.h"
//%include "Gamma/HRFilter.h"

%include "Gamma/Noise.h"
%include "Gamma/Oscillator.h"
%include "Gamma/Print.h"
%include "Gamma/Recorder.h"
%include "Gamma/Sample.h"
%include "Gamma/SamplePlayer.h"
%ignore gam::Func;
%ignore gam::EchoCSine;
%include "Gamma/Scheduler.h"
%include "Gamma/SoundFile.h"
%include "Gamma/Spatial.h"
%ignore gam::phsInc::Switchable;
%include "Gamma/Strategy.h"
%include "Gamma/Timer.h"
%include "Gamma/TransferFunc.h"
%ignore gam::IndexPool;
%include "Gamma/Voices.h"


%inline %{
    /// Sound file formats
enum Format{
    FORMAT_WAV = 1,	/**< Microsoft WAV format (little endian default). */
    FORMAT_AIFF,		/**< Apple/SGI AIFF format (big endian). */
    FORMAT_AU,			/**< Sun/NeXT AU format (big endian). */
    FORMAT_RAW,		/**< RAW PCM data. */
    FORMAT_NO_FORMAT
};

/// Sound file sample encoding types
enum EncodingType{
    ENC_PCM_S8 = 1,	/**< Signed 8 bit data */
    ENC_PCM_16,		/**< Signed 16 bit data */
    ENC_PCM_24,		/**< Signed 24 bit data */
    ENC_PCM_32,		/**< Signed 32 bit data */
    ENC_PCM_U8,		/**< Unsigned 8 bit data (WAV and RAW only) */

    ENC_FLOAT,		/**< 32 bit SampleType data */
    ENC_DOUBLE,		/**< 64 bit SampleType data */

    ENC_ULAW,		/**< U-Law encoded. */
    ENC_ALAW,		/**< A-Law encoded. */
};

typedef double SampleType;
%}

%extend gam::IndexMap<T> {
    T __getitem(size_t i) { return (*$self)(i); }
    void __setitem(size_t i, T val) { (*$self)(i) = val; }
}

%extend gam::Delay<T> {
    T __getitem(size_t i) { return (*$self)(i); }
    void __setitem(size_t i, T val) { (*$self)(i) = val; }
}

%extend gam::Multitap<T> {
    T __getitem(size_t i) { return (*$self)(i); }
    void __setitem(size_t i, T val) { (*$self)(i) = val; }
}

%template(Gamma_Polar) gam::Polar<SampleType>;
%template(Gamma_Complex) gam::Complex<SampleType>;
//%template(Gamma_Vec) gam::Vec<SampleType>;

%template(Gamma_IndexMap) gam::IndexMap<SampleType>;
%template(Gamma_Slice) gam::Slice<SampleType>;

%template(Gamma_EnvFollow) gam::EnvFollow<SampleType,SampleType>;
%template(Gamma_Threshold) gam::Threshold<SampleType>;
%template(Gamma_ZeroCross) gam::ZeroCross<SampleType>;
%template(Gamma_MaxAbs) gam::MaxAbs<SampleType>;
%template(Gamma_ZeroCrossRate) gam::ZeroCrossRate<SampleType>;
%template(Gamma_Array) gam::Array<SampleType>;
%template(Gamma_ArrayPow2) gam::ArrayPow2<SampleType>;
%template(Gamma_RingBuffer) gam::Ring<SampleType>;
%template(Gamma_DoubleRing) gam::DoubleRing<SampleType>;
%template(Gamma_DelayN) gam::DelayN<SampleType>;

%template(Gamma_SlidingWindow) gam::SlidingWindow<SampleType>;
%template(Gamma_SlidingDFT) gam::SlidingDFT<SampleType>;

%template(Gamma_Delay) gam::Delay<SampleType>;
%template(Gamma_Multitap) gam::Multitap<SampleType>;
%template(Gamma_Delay1) gam::Delay1<SampleType>;
%template(Gamma_Delay2) gam::Delay2<SampleType>;
%template(Gamma_Comb) gam::Comb<SampleType>;

%template(Gamma_AM) gam::AM<SampleType>;
%template(Gamma_Chirp) gam::Chirp<SampleType>;
%template(Gamma_Chorus) gam::Chorus<SampleType>;
%template(Gamma_FreqShift) gam::FreqShift<SampleType>;
%template(Gamma_Pan) gam::Pan<SampleType>;
%template(Gamma_Quantizer) gam::Quantizer<SampleType>;

%template(Gamma_Curve) gam::Curve<SampleType,SampleType>;
%template(Gamma_AD) gam::AD<SampleType,SampleType>;
%template(Gamma_ADSR) gam::ADSR<SampleType,SampleType>;
%template(Gamma_Decay) gam::Decay<SampleType>;
%template(Gamma_Gate) gam::Gate<SampleType>;
%template(Gamma_Seg) gam::Seg<SampleType>;
%template(Gamma_SegExp) gam::SegExp<SampleType>;

%template(Gamma_CFFT) gam::CFFT<SampleType>;
%template(Gamma_RFFT) gam::RFFT<SampleType>;

%template(Gamm_poleRadius) gam::poleRadius<SampleType>;
%template(Gamma_freqToRad) gam::freqToRad<SampleType>;

%template(Gamma_AllPass1) gam::AllPass1<SampleType,SampleType>;
%template(Gamma_Biquad) gam::Biquad<SampleType,SampleType>;
%template(Gamma_BlockDC) gam::BlockDC<SampleType,SampleType>;
%template(Gamma_BlockNyq) gam::BlockNyq<SampleType,SampleType>;
%template(Gamma_Filter2) gam::Filter2<SampleType,SampleType>;
%template(Gamma_AllPass2) gam::AllPass2<SampleType,SampleType>;
%template(Gamma_Notch) gam::Notch<SampleType,SampleType>;
%template(Gamma_Reson) gam::Reson<SampleType,SampleType>;
%template(Gamma_Hilbert) gam::Hilbert<SampleType,SampleType>;
%template(Gamma_Inegrator) gam::Integrator<SampleType,SampleType>;
%template(Gamma_Differencer) gam::Differencer<SampleType>;
%template(Gamma_MovingAvg) gam::MovingAvg<SampleType>;
%template(Gamma_OnePole) gam::OnePole<SampleType,SampleType>;

/*
%template(headShadowf) gam::headShadow<SampleType>;
%template(Gamma_HRScene2) gam::HRScene<2>;
%template(Gamma_HRScene3) gam::HRScene<3>;
%template(Gamma_HRScene4) gam::HRScene<4>;
%template(Gamma_HRScene5) gam::HRScene<5>;
%template(Gamma_HRScene6) gam::HRScene<6>;
%template(Gamma_HRScene7) gam::HRScene<7>;
%template(Gamma_HRScene8) gam::HRScene<8>;
%template(Gamma_HRScene9) gam::HRScene<9>;
*/

%template(Gamma_Accumulator) gam::Accum<>;
%template(Gamma_Sweep) gam::Sweep<>;
%template(Gamma_AccumPhase) gam::AccumPhase<SampleType>;
%template(Gamma_Osc) gam::Osc<SampleType>;
%template(Gamma_CSine) gam::CSine<SampleType>;
%template(Gamma_Sine) gam::Sine<SampleType>;
%template(Gamma_SineR) gam::SineR<SampleType>;
%template(Gamma_SineRs) gam::SineRs<SampleType>;
%template(Gamma_SineD) gam::SineD<SampleType>;
%template(Gamma_SineDs) gam::SineDs<SampleType>;
%template(Gamma_Chirplet) gam::Chirplet<SampleType>;
%template(Gamma_LFO) gam::LFO<>;
%template(Gamma_DWO) gam::DWO<>;
%template(Gamma_Buzz) gam::Buzz<SampleType>;
%template(Gamma_Impulse) gam::Impulse<SampleType>;
%template(Gamma_Saw) gam::Saw<SampleType>;
%template(Gamma_Square) gam::Square<SampleType>;
%template(Gamma_DSF) gam::DSF<SampleType,gam::GAM_DEFAULT_DOMAIN>;
//%template(Gamma_Upsample) gam::Upsample<>;

%template(Gamma_Sample) gam::Sample<SampleType>;
%template(Gamma_SamplePlayer) gam::SamplePlayer<SampleType>;

%template(Gamma_readAll) gam::SoundFile::readAll<SampleType>;
%template(Gamma_read) gam::SoundFile::read<SampleType>;
%template(Gamma_readAllD) gam::SoundFile::readAllD<SampleType>;
%template(Gamma_write) gam::SoundFile::write<SampleType>;

%template(Gamma_LoopGain) gam::LoopGain<SampleType>;
%template(Gamma_Loop1P) gam::Loop1P<SampleType>;
%template(Gamma_Loop1P1Z) gam::Loop1P1Z<SampleType>;
%template(Gamma_Echo) gam::Echo<SampleType>;
%template(Gamma_EchoCSine) gam::EchoCSine<SampleType>;
%template(Gamma_ReverbMS) gam::ReverbMS<SampleType>;
//%template(Gamma_Dist) gam::Dist<SampleType>;

//%template(Gamma_decayToFbk) ::decayToFbk<SampleType>;
//%template(Gamma_fbkToDecay) ::fbkToDecay<SampleType>;

%template(Gamma_ProcessAudioData) gam::Process<gam::AudioIOData>;

%inline %{

    struct LuaAudioApp : public gam::AudioCallback
    {
        SWIGLUA_REF callback;    
        AudioIO mAudioIO;

        LuaAudioApp(SWIGLUA_REF cb) {
            mAudioIO.append(*this);
            initAudio(44100);
            callback = cb;
        }
        
        void onAudio(AudioIOData & io) {
            swiglua_ref_set(&callback,callback.L,-1);
            if( lua_pcall(callback.L,0,0,0) != 0)
                printf("callback error: %s\n",lua_tostring(callback.L,-1));
        }            

        void initAudio(
            double framesPerSec, unsigned framesPerBuffer=128,
            unsigned outChans=2, unsigned inChans=0
        ){
            mAudioIO.framesPerSecond(framesPerSec);
            mAudioIO.framesPerBuffer(framesPerBuffer);
            mAudioIO.channelsOut(outChans);
            mAudioIO.channelsIn(inChans);
            sampleRate(framesPerSec);
        }

        AudioIO& audioIO(){ return mAudioIO; }

        void start(bool block=true){
            mAudioIO.start();
            if(block){		
                printf("Press 'enter' to quit...\n");
                fflush(stdout);
                getchar();
            }
        }
    };

    struct LuaProcess : public gam::Process<gam::AudioIOData>
    {
        SWIGLUA_REF callback;    

        void onProcess(AudioIOData & io) {

        }
    };
        
    struct LuaVoice : public gam::Voice<SampleType>
    {
        SWIGLUA_REF callback;    
        
        void onAttack() {
        
        }
        void onAttack(SampleType a, SampleType b) {

        }

        SampleType operator()() {
            return 0;
        }
    };
    
%}

%template(Gamma_Scheduler_Add) gam::Scheduler::add<LuaProcess>;

%template(Gamma_iplTrunc) gam::ipl::Trunc<SampleType>;
%template(Gamma_iplRound) gam::ipl::Round<SampleType>;
%template(Gamma_iplMean2) gam::ipl::Mean2<SampleType>;
%template(Gamma_iplLinear) gam::ipl::Linear<SampleType>;
%template(Gamma_iplCubic) gam::ipl::Cubic<SampleType>;
%template(Gamma_iplAllPass) gam::ipl::AllPass<SampleType>;
//%template(Gamma_iplSwitchable) gam::ipl::Switchable<SampleType>;
%template(Gamma_iplSeqNone) gam::iplSeq::None<SampleType>;
%template(Gamma_iplSeqTrunc) gam::iplSeq::Trunc<SampleType>;
%template(Gamma_iplSeqRound) gam::iplSeq::Round<SampleType>;
%template(Gamma_iplSeqLinear) gam::iplSeq::Linear<SampleType>;
%template(Gamma_iplSeqCubic) gam::iplSeq::Cubic<SampleType>;
%template(Gamma_iplSeqCosine) gam::iplSeq::Cosine<SampleType>;
%template(Gamma_phsIncClip) gam::phsInc::incClip<SampleType>;

%template(Gamma_LookupTable) gam::LookupTable<SampleType>;
//%template(Gamma_TablePow2) gam::TablePow2<SampleType>;
//%template(Gamma_CSinTable) gam::CSinTable<SampleType>;
%template(Gamma_UnitMapper) gam::UnitMapper<SampleType>;

%template(Gamma_LuaVoices) gam::Voices<LuaVoice>;

%template(Gamma_dot) gam::arr::dot<SampleType>;
%template(Gamma_sumSquares) gam::arr::sumSquares<SampleType>;
%template(Gamma_add) gam::arr::add<SampleType>;
%template(Gamma_addToRing) gam::arr::addToRing<SampleType>;
%template(Gamma_dot4) gam::arr::dot4<SampleType>;
//%template(Gamma_extrema) gam::arr::extrema<SampleType>;
//%template(Gamma_histogram) gam::arr::histogram<SampleType>;
%template(Gamma_indexOfMax) gam::arr::indexOfMax<SampleType>;
%template(Gamma_indexOfMaxNorm) gam::arr::indexOfMaxNorm<SampleType>;
%template(Gamma_indexOfMin) gam::arr::indexOfMin<SampleType>;
//%template(Gamma_lineFit) gam::arr::lineFit<SampleType>;
//%template(Gamma_maxima) gam::arr:maxima<SampleType>;
%template(Gamma_meanAbsDiff) gam::arr::meanAbsDiff<SampleType>;
%template(Gamma_meanWeighted) gam::arr::meanWeighted<SampleType>;
%template(Gamma_meanWeightedIndex) gam::arr::meanWeightedIndex<SampleType>;
%template(Gamma_minMaxMean) gam::arr::minMaxMean<SampleType>;
//%template(Gamma_minimaRemove) gam::arr::minimaRemove<SampleType>;
%template(Gamma_mirror_dp) gam::arr::mirror_dp<SampleType>;
%template(Gamma_mirror_dq) gam::arr::mirror_dq<SampleType>;
%template(Gamma_mul) gam::arr::mul<SampleType>;
%template(Gamma_mulBartlett) gam::arr::mulBartlett<SampleType>;
%template(Gamma_mulHalfWindow) gam::arr::mulHalfWindow<SampleType>;
%template(Gamma_normalize) gam::arr::normalize<SampleType>;
%template(Gamma_norm) gam::arr::norm<SampleType>;
%template(Gamma_normTaxi) gam::arr::normTaxi<SampleType>;
%template(Gamma_nyquist) gam::arr::nyquist<SampleType>;
%template(Gamma_rms) gam::arr::rms<SampleType>;
//%template(Gamma_slopeAbsMax) gam::arr::slopeAbsMax<SampleType>;
%template(Gamma_slopeMax) gam::arr::slopeMax<SampleType>;
%template(Gamma_sortInsertion) gam::arr::sortInsertion<SampleType>;
//%template(Gamma_sortQuick) gam::arr::sortQuick<SampleType>;
%template(Gamma_sum) gam::arr::sum<SampleType>;
%template(Gamma_variance) gam::arr::variance<SampleType>;
%template(Gamma_within) gam::arr::within<SampleType>;
%template(Gamma_zeroCount) gam::arr::zeroCount<SampleType>;
//%template(Gamma_zeroCross) gam::arr::zeroCross<SampleType>;
//%template(Gamma_zeroCrossFirst) gam::arr::zeroCrossFirst<SampleType>;
%template(Gamma_zeroCrossMax) gam::arr::zeroCrossMax<SampleType>;
//%template(Gamma_zeroCrossN) gam::arr::zeroCrossN<SampleType>;

