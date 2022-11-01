#pragma once

#include "SoundAlchemy.hpp"

namespace SoundAlchemy::Envelopes
{
    template<typename T>
    class TEnvelopeFollower
    {
    public:
        TEnvelopeFollower();

        void Setup( T attackMs, T releaseMs, int sampleRate );
        
        void Process( size_t count, const T *src, int skip=1 );

        T envelope;

    protected:
        T a;
        T r;
    };

    //----------
    template<typename T>
    inline TEnvelopeFollower<T>::TEnvelopeFollower()
    {
        envelope=0;
    }

    template<typename T>
    inline void TEnvelopeFollower<T>::Setup( T attackMs, T releaseMs, int sampleRate )
    {
        a = pow( 0.01, 1.0 / ( attackMs * sampleRate * 0.001 ) );
        r = pow( 0.01, 1.0 / ( releaseMs * sampleRate * 0.001 ) );
    }

    template<class T>
    void TEnvelopeFollower<T>::Process( size_t count, const T *src, int skip )
    {
        while( count-- )
        {
                T v=::fabs( *src );
                src+=skip;
                if( v>envelope )
                        envelope = a * ( envelope - v ) + v;
                else
                        envelope = r * ( envelope - v ) + v;
        }
    }

    //----------

    template<typename T>
    struct TEnvelopeLimiter
    {
        void Setup( T attackMs, T releaseMs, int sampleRate );    
        void Process( size_t nSamples, T *dest, int skip=1 );

    private:
        TEnvelopeFollower e;
    };

    //----------
    template<typename T>
    inline void EnvelopeLimiter<T>::Setup( T attackMs, T releaseMs, int sampleRate )
    {
        e.Setup( attackMs, releaseMs, sampleRate );
    }

    template<typename T>
    inline void TEnvelopeLimiter<T>::Process( size_t count, T *dest, int skip )
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
};