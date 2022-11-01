%{
#include "Gamma/SamplePlayer.h"
%}

namespace gam
{
    template<
        class T = float,
        template<class> class Si = ipl::Trunc,
        class Sp = phsInc::OneShot,
        class Td = GAM_DEFAULT_DOMAIN
    >
    class SamplePlayer: public Array<T>{
    public:
        using Array<T>::size;
        using Array<T>::elems;

        SamplePlayer();
        explicit SamplePlayer(SamplePlayer<T>& src, double rate=1);
        SamplePlayer(Array<T>& src, double smpRate, double rate=1);
        explicit SamplePlayer(const char * pathToSoundFile, double rate=1);

        bool load(const char * pathToSoundFile);
        T operator()(int channel=0);

        %extend {
            T Tick(int channel=0) { return (*$self)(channel); }
        }

        T read(int channel) const;
        float readFloat(int channel) const;
        void advance();
        void advance(double frames);
        void buffer(Array<T>& src, double frmRate, int chans, bool interleaved=false);
        void buffer(T * src, int numFrms, double frmRate, int chans, bool interleaved=false);
        void buffer(SamplePlayer& src);
        void free();							///< Free sample buffer (if owner)
        void freq(double v){ rate(v); }			///< Set frequency if sample buffer is a wavetable
        void max(double v);						///< Set playback interval maximum frame (open)
        void min(double v);						///< Set playback interval minimum frame (closed)
        void pos(double v);						///< Set playback position, in frames
        void phase(double v);					///< Set playback position, in [0, 1)
        void rate(double v);					///< Set playback rate scalar
        void range(double phs, double period);	///< Set playback interval start phase and period
        void reset();							///< Reset playback head
        void finish();							///< Set playback head to end
        bool loop();
        void fade(int fadeOutFrames=4, int fadeInFrames=2);
        bool done() const;

        int channels() const;
        int frames() const;
        double frameRate() const;
        double freq() const;
        double max() const;
        double min() const;
        double length() const;					///< Get total length (in seconds) of frame data
        double pos() const;
        double posInInterval(double frac) const;///< Get position from fraction within interval
        double phase() const;					///< Get playback position, in [0, 1)
        double rate() const;

        void onDomainChange(double r);
    };
}
%template(SamplePlayer) gam::SamplePlayer<SampleType>;