// https://github.com/igorski/fogpad/blob/master/src/limiter.cpp
class Limiter
{
    public:
        Limiter();
        Limiter( DspFloatType attackMs, DspFloatType releaseMs, DspFloatType thresholdDb );
        ~Limiter();

        template <typename SampleType>
        void process( SampleType** outputBuffer, int bufferSize, int numOutChannels );

        void setAttack( DspFloatType attackMs );
        void setRelease( DspFloatType releaseMs );
        void setThreshold( DspFloatType thresholdDb );

        DspFloatType getLinearGR();

    protected:
        void init( DspFloatType attackMs, DspFloatType releaseMs, DspFloatType thresholdDb );
        void recalculate();

        DspFloatType pTresh;   // in dB, -20 - 20
        DspFloatType pTrim;
        DspFloatType pAttack;  // in microseconds
        DspFloatType pRelease; // in ms
        DspFloatType pKnee;

        DspFloatType thresh, gain, att, rel, trim;
};

template <typename SampleType>
void Limiter::process( SampleType** outputBuffer, int bufferSize, int numOutChannels )
{
//    if ( gain > 0.9999f && outputBuffer->isSilent )
//    {
//        // don't process if input is silent
//        return;
//    }

    SampleType g, at, re, tr, th, lev, ol, or_;

    th = thresh;
    g = gain;
    at = att;
    re = rel;
    tr = trim;

    bool hasRight = ( numOutChannels > 1 );

    SampleType* leftBuffer  = outputBuffer[ 0 ];
    SampleType* rightBuffer = hasRight ? outputBuffer[ 1 ] : 0;

    if ( pKnee > 0.5 )
    {
        // soft knee

        for ( int i = 0; i < bufferSize; ++i ) {

            ol  = leftBuffer[ i ];
            or_ = hasRight ? rightBuffer[ i ] : 0;

            lev = ( SampleType ) ( 1.f / ( 1.f + th * fabs( ol + or_ )));

            if ( g > lev ) {
                g = g - at * ( g - lev );
            }
            else {
                g = g + re * ( lev - g );
            }

            leftBuffer[ i ] = ( ol * tr * g );

            if ( hasRight )
                rightBuffer[ i ] = ( or_ * tr * g );
        }
    }
    else
    {
        for ( int i = 0; i < bufferSize; ++i ) {

            ol  = leftBuffer[ i ];
            or_ = hasRight ? rightBuffer[ i ] : 0;

            lev = ( SampleType ) ( 0.5 * g * fabs( ol + or_ ));

            if ( lev > th ) {
                g = g - ( at * ( lev - th ));
            }
            else {
                // below threshold
                g = g + ( SampleType )( re * ( 1.f - g ));
            }

            leftBuffer[ i ] = ( ol * tr * g );

            if ( hasRight )
                rightBuffer[ i ] = ( or_ * tr * g );
        }
    }
    gain = g;
}

// constructors / destructor

Limiter::Limiter()
{
    init( 0.15, 0.50, 0.60 );
}

Limiter::Limiter( DspFloatType attackMs, DspFloatType releaseMs, DspFloatType thresholdDb )
{
    init( attackMs, releaseMs, thresholdDb );
}

Limiter::~Limiter()
{
    // nowt...
}

/* public methods */

void Limiter::setAttack( DspFloatType attackMs )
{
    pAttack = ( DspFloatType ) attackMs;
    recalculate();
}

void Limiter::setRelease( DspFloatType releaseMs )
{
    pRelease = ( DspFloatType ) releaseMs;
    recalculate();
}

void Limiter::setThreshold( DspFloatType thresholdDb )
{
    pTresh = ( DspFloatType ) thresholdDb;
    recalculate();
}

DspFloatType Limiter::getLinearGR()
{
    return gain > 1.f ? 1.f / gain : 1.f;
}

/* protected methods */

void Limiter::init( DspFloatType attackMs, DspFloatType releaseMs, DspFloatType thresholdDb )
{
    pAttack  = ( DspFloatType ) attackMs;
    pRelease = ( DspFloatType ) releaseMs;
    pTresh   = ( DspFloatType ) thresholdDb;
    pTrim    = ( DspFloatType ) 0.60;
    pKnee    = ( DspFloatType ) 0.40;

    gain = 1.f;

    recalculate();
}

void Limiter::recalculate()
{
    if ( pKnee > 0.5 ) {
        // soft knee
        thresh = ( DspFloatType ) pow( 10.0, 1.f - ( 2.0 * pTresh ));
    }
    else {
        // hard knee
        thresh = ( DspFloatType ) pow( 10.0, ( 2.0 * pTresh ) - 2.0 );
    }
    trim = ( DspFloatType )( pow( 10.0, ( 2.0 * pTrim) - 1.f ));
    att  = ( DspFloatType )  pow( 10.0, -2.0 * pAttack );
    rel  = ( DspFloatType )  pow( 10.0, -2.0 - ( 3.0 * pRelease ));
}