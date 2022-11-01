#pragma once

namespace OutputLimiter
{
    class EnvelopeFollower
    {
    public:
        EnvelopeFollower();

        void Setup( DspFloatType attackMs, DspFloatType releaseMs, int sampleRate );

        template<class T, int skip>
        void Process( size_t count, const T *src );

        DspFloatType envelope;

    protected:
        DspFloatType a;
        DspFloatType r;
    };

    //----------

    inline EnvelopeFollower::EnvelopeFollower()
    {
        envelope=0;
    }

    inline void EnvelopeFollower::Setup( DspFloatType attackMs, DspFloatType releaseMs, int sampleRate )
    {
        a = pow( 0.01, 1.0 / ( attackMs * sampleRate * 0.001 ) );
        r = pow( 0.01, 1.0 / ( releaseMs * sampleRate * 0.001 ) );
    }

    template<class T, int skip>
    void EnvelopeFollower::Process( size_t count, const T *src )
    {
        while( count-- )
        {
                DspFloatType v=::fabs( *src );
                src+=skip;
                if( v>envelope )
                        envelope = a * ( envelope - v ) + v;
                else
                        envelope = r * ( envelope - v ) + v;
        }
    }

    //----------

    struct Limiter
    {
        void Setup( DspFloatType attackMs, DspFloatType releaseMs, int sampleRate );

        template<class T, int skip>
        void Process( size_t nSamples, T *dest );

    private:
        EnvelopeFollower e;
    };

    //----------

    inline void Limiter::Setup( DspFloatType attackMs, DspFloatType releaseMs, int sampleRate )
    {
        e.Setup( attackMs, releaseMs, sampleRate );
    }

    template<class T, int skip>
    void Limiter::Process( size_t count, T *dest )
    {
        while( count-- )
        {
                T v=*dest;
                // don't worry, this should get optimized
                e.Process<T, skip>( 1, &v );
                if( e.envelope>1 )
                        *dest=*dest/e.envelope;
                dest+=skip;
        }
    }
}