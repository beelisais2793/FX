// https://github.com/igorski/fogpad/blob/master/src/calc.h
#pragma once

#include <cstdint>
#include <cmath>
#include <cstring>
#include <climits>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#define undenormalise(sample) ((((*(uint32_t *)&(sample))&0x7f800000)==0)&&((sample)!=0.f))

namespace VST {

    static const int   ID       = 9715186;
    static const char* NAME     = "FogPad";
    static const char* VENDOR   = "igorski.nl";


    extern float SAMPLE_RATE; // set upon initialization, see vst.cpp

    static const float PI     = 3.141592653589793f;
    static const float TWO_PI = PI * 2.f;

    // maximum and minimum filter frequency ranges
    // also see plugin.uidesc to update the controls to match

    static const float FILTER_MIN_FREQ      = 30.f;
    static const float FILTER_MAX_FREQ      = 20000.f;
    static const float FILTER_MIN_RESONANCE = 0.1f;
    static const float FILTER_MAX_RESONANCE = 0.7071067811865476f; //sqrt( 2.f ) / 2.f;

    // maximum and minimum rate of oscillation in Hz
    // also see plugin.uidesc to update the controls to match

    static const float MAX_LFO_RATE() { return 10.f; }
    static const float MIN_LFO_RATE() { return .1f; }

    // sine waveform used for the oscillator
    static const float TABLE[ 128 ] = { 0, 0.0490677, 0.0980171, 0.14673, 0.19509, 0.24298, 0.290285, 0.33689, 0.382683, 0.427555, 0.471397, 0.514103, 0.55557, 0.595699, 0.634393, 0.671559, 0.707107, 0.740951, 0.77301, 0.803208, 0.83147, 0.857729, 0.881921, 0.903989, 0.92388, 0.941544, 0.95694, 0.970031, 0.980785, 0.989177, 0.995185, 0.998795, 1, 0.998795, 0.995185, 0.989177, 0.980785, 0.970031, 0.95694, 0.941544, 0.92388, 0.903989, 0.881921, 0.857729, 0.83147, 0.803208, 0.77301, 0.740951, 0.707107, 0.671559, 0.634393, 0.595699, 0.55557, 0.514103, 0.471397, 0.427555, 0.382683, 0.33689, 0.290285, 0.24298, 0.19509, 0.14673, 0.0980171, 0.0490677, 1.22465e-16, -0.0490677, -0.0980171, -0.14673, -0.19509, -0.24298, -0.290285, -0.33689, -0.382683, -0.427555, -0.471397, -0.514103, -0.55557, -0.595699, -0.634393, -0.671559, -0.707107, -0.740951, -0.77301, -0.803208, -0.83147, -0.857729, -0.881921, -0.903989, -0.92388, -0.941544, -0.95694, -0.970031, -0.980785, -0.989177, -0.995185, -0.998795, -1, -0.998795, -0.995185, -0.989177, -0.980785, -0.970031, -0.95694, -0.941544, -0.92388, -0.903989, -0.881921, -0.857729, -0.83147, -0.803208, -0.77301, -0.740951, -0.707107, -0.671559, -0.634393, -0.595699, -0.55557, -0.514103, -0.471397, -0.427555, -0.382683, -0.33689, -0.290285, -0.24298, -0.19509, -0.14673, -0.0980171, -0.0490677 };

    // These values are tuned to 44.1 kHz sample rate and will be
    // recalculated to match the host sample recalculated

    static const int NUM_COMBS     = 8;
    static const int NUM_ALLPASSES = 4;

    static const int COMB_TUNINGS[ NUM_COMBS ] = { 1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617 };
    static const int ALLPASS_TUNINGS[ NUM_ALLPASSES ] = { 556, 441, 341, 225 };
}

namespace Calc {

    /**
     * convert given value in seconds to the appropriate
     * value in samples (for the current sampling rate)
     */
    inline int secondsToBuffer( float seconds )
    {
        return ( int )( seconds * VST::SAMPLE_RATE );
    }

    /**
     * convert given value in milliseconds to the appropriate
     * value in samples (for the current sampling rate)
     */
    inline int millisecondsToBuffer( float milliseconds )
    {
        return secondsToBuffer( milliseconds / 1000.f );
    }

    // convenience method to ensure given value is within the 0.f - +1.f range

    inline float cap( float value )
    {
        return std::min( 1.f, std::max( 0.f, value ));
    }

    // convenience method to ensure a sample is in the valid -1.f - +1.f range

    inline float capSample( float value )
    {
        return std::min( 1.f, std::max( -1.f, value ));
    }

    // convenience method to round given number value to the nearest
    // multiple of valueToRoundTo
    // e.g. roundTo( 236.32, 10 ) == 240 and roundTo( 236.32, 5 ) == 235

    inline float roundTo( float value, float valueToRoundTo )
    {
        float resto = fmod( value, valueToRoundTo );

        if ( resto <= ( valueToRoundTo / 2 ))
            return value - resto;

        return value + valueToRoundTo - resto;
    }

    // convenience method to scale given value and its expected maxValue against
    // an arbitrary range (defined by maxCompareValue in relation to maxValue)

    inline float scale( float value, float maxValue, float maxCompareValue )
    {
        float ratio = maxCompareValue / maxValue;
        return ( float ) ( std::min( maxValue, value ) * ratio );
    }

    // cast a floating point value to a boolean true/false

    inline bool toBool( float value )
    {
        return value >= .5;
    }
}


namespace Util {

    /**
     * Convenience method to log a message to a file
     * multiple messages can be written to the same file (are
     * separated by a new line)
     *
     * This should be used for debugging purposes only
     */
    void log( const char* message, const char* filename )
    {
        std::ofstream out;

        char buff[20];
        struct tm *sTm;

        time_t now = time( 0 );
        sTm        = gmtime( &now );

        strftime( buff, sizeof( buff ), "%Y-%m-%d %H:%M:%S", sTm );

        out.open( filename, std::ios_base::app );
        out << buff << " " << message << "\n";

        out.close();
    }

    void log( std::string message, const char* filename )
    {
        log( message.c_str(), filename );
    }

    void log( float value, const char* filename )
    {
        log( std::to_string( value ), filename );
    }

    void log( int value, const char* filename )
    {
        log( std::to_string( value ), filename );
    }

}


enum
{
    // ids for all visual controls

    kReverbSizeId = 1,        // reverb size
    kReverbWidthId,           // reverb width

    kFilterCutoffId,          // filter cutoff
    kFilterResonanceId,       // filter resonance
    kLFOFilterId,             // filter LFO rate
    kLFOFilterDepthId,        // depth for filter LFO

    kReverbPlaybackRateId,    // reverb playback rate
    kDecimatorId,             // decimator

    kBitResolutionId,         // bit resolution
    kLFOBitResolutionId,      // bit resolution LFO rate
    kLFOBitResolutionDepthId, // depth for bit resolution LFO
    kBitResolutionChainId,    // bit resolution pre/post delay mix

    kReverbFreezeId,          // reverb freeze
    kReverbDryMixId,          // reverb wet mix
    kReverbWetMixId,          // reverb dry mix

    kVuPPMId                  // for the Vu value return to host
};

/**
 * An AudioBuffer represents multiple channels of audio
 * each of equal buffer length.
 * AudioBuffer has convenience methods for cloning, silencing and mixing
 */
class AudioBuffer
{
    public:
        AudioBuffer( int aAmountOfChannels, int aBufferSize );
        ~AudioBuffer();

        int amountOfChannels;
        int bufferSize;
        bool loopeable;

        float* getBufferForChannel( int aChannelNum );
        int mergeBuffers( AudioBuffer* aBuffer, int aReadOffset, int aWriteOffset, float aMixVolume );
        void silenceBuffers();
        void adjustBufferVolumes( float volume );
        bool isSilent();
        AudioBuffer* clone();

    protected:
        std::vector<float*>* _buffers;
};

class AllPass
{
    public:
        AllPass();
        void setBuffer( float *buf, int size );
        inline float process( float input )
        {
            float output;
            float bufout = _buffer[ _bufIndex ];
            undenormalise( bufout );

            output = -input + bufout;
            _buffer[ _bufIndex ] = input + ( bufout * _feedback );

            if ( ++_bufIndex >= _bufSize ) {
                _bufIndex = 0;
            }
            return output;
        }
        void mute();
        float getFeedback();
        void setFeedback( float val );

    private:
        float  _feedback;
        float* _buffer;
        int _bufSize;
        int _bufIndex;
};


class Comb
{
    public:
        Comb();
        void setBuffer( float *buf, int size );
        inline float process( float input )
        {
            float output = _buffer[ _bufIndex ];
            undenormalise( output );

            _filterStore = ( output * _damp2 ) + ( _filterStore * _damp1 );
            undenormalise( _filterStore );

            _buffer[_bufIndex] = input + ( _filterStore * _feedback );
            if ( ++_bufIndex >= _bufSize ) {
                _bufIndex = 0;
            }
            return output;
        }
        void mute();
        float getDamp();
        void setDamp( float val );
        float getFeedback();
        void setFeedback( float val );

    private:
        float  _feedback;
        float  _filterStore;
        float  _damp1;
        float  _damp2;
        float* _buffer;
        int _bufSize;
        int _bufIndex;
};

class Decimator {

    public:

        Decimator( int bits, float rate );
        ~Decimator();

        // the output resolution, value between 1 - 32
        int getBits();
        void setBits( int value );

        // decimator has an internal oscillator
        // as the effect is applied at the peak of the cycle
        // the range is 0 - 1 where 1 implies the original sample rate
        float getRate();
        void setRate( float value );

        void process( float* sampleBuffer, int bufferSize );

        // store/restore the processor properties
        // this ensures that multi channel processing for a
        // single buffer uses all properties across all channels

        void store();
        void restore();

    private:
        int _bits;
        long _m;
        float _rate;
        float _accumulator;
        float _accumulatorStored;
};


class LFO {

    public:
        LFO();
        ~LFO();

        float getRate();
        void setRate( float value );

        // accumulators are used to retrieve a sample from the wave table
        // in other words: track the progress of the oscillator against its range

        float getAccumulator();
        void setAccumulator( float offset );

        /**
         * retrieve a value from the wave table for the current
         * accumulator position, this method also increments
         * the accumulator and keeps it within bounds
         */
        inline float peek()
        {
            // the wave table offset to read from
            float SR_OVER_LENGTH = VST::SAMPLE_RATE / ( float ) TABLE_SIZE;
            int readOffset = ( _accumulator == 0.f ) ? 0 : ( int ) ( _accumulator / SR_OVER_LENGTH );

            // increment the accumulators read offset
            _accumulator += _rate;

            // keep the accumulator within the bounds of the sample frequency
            if ( _accumulator > VST::SAMPLE_RATE )
                _accumulator -= VST::SAMPLE_RATE;

            // return the sample present at the calculated offset within the table
            return VST::TABLE[ readOffset ];
        }

    private:

        // see VST::LFO_TABLE;
        static const int TABLE_SIZE = 128;

        // used internally

        float _rate;
        float _accumulator;   // is read offset in wave table buffer
};


class BitCrusher {

    public:
        BitCrusher( float amount, float inputMix, float outputMix );
        ~BitCrusher();

        void setLFO( float LFORatePercentage, float LFODepth );
        void process( float* inBuffer, int bufferSize );

        void setAmount( float value ); // range between -1 to +1
        void setInputMix( float value );
        void setOutputMix( float value );

        LFO* lfo;
        bool hasLFO;

    private:
        int _bits; // we scale the amount to integers in the 1-16 range
        float _amount;
        float _inputMix;
        float _outputMix;

        void cacheLFO();
        void calcBits();
        float _tempAmount;
        float _lfoDepth;
        float _lfoRange;
        float _lfoMax;
        float _lfoMin;
};


class Filter {

    public:
        Filter();
        ~Filter();

        void  setCutoff( float frequency );
        float getCutoff();
        void  setResonance( float resonance );
        float getResonance();
        void setDepth( float depth );
        float getDepth();
        void setLFO( bool enabled );

        void calculateParameters();

        // update Filter properties, the values here are in normalized 0 - 1 range
        void updateProperties( float cutoffPercentage, float resonancePercentage, float LFORatePercentage, float fLFODepth );

        // apply filter to incoming sampleBuffer contents
        void process( float* sampleBuffer, int bufferSize, int c );

        LFO* lfo;

        // store/restore the processor properties
        // this ensures that multi channel processing for a
        // single buffer uses all properties across all channels

        void store();
        void restore();

    private:
        float _cutoff;
        float _tempCutoff;
        float _resonance;
        float _depth;
        float _lfoMin;
        float _lfoMax;
        float _lfoRange;
        bool  _hasLFO;

        // used internally

        float _accumulatorStored;
        float _tempCutoffStored;

        float _a1;
        float _a2;
        float _a3;
        float _b1;
        float _b2;
        float _c;

        float* _in1;
        float* _in2;
        float* _out1;
        float* _out2;

        void cacheLFOProperties();
};


class Limiter
{
    public:
        Limiter();
        Limiter( float attackMs, float releaseMs, float thresholdDb );
        ~Limiter();

        template <typename SampleType>
        void process( SampleType** outputBuffer, int bufferSize, int numOutChannels );

        void setAttack( float attackMs );
        void setRelease( float releaseMs );
        void setThreshold( float thresholdDb );

        float getLinearGR();

    protected:
        void init( float attackMs, float releaseMs, float thresholdDb );
        void recalculate();

        float pTresh;   // in dB, -20 - 20
        float pTrim;
        float pAttack;  // in microseconds
        float pRelease; // in ms
        float pKnee;

        float thresh, gain, att, rel, trim;
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





class ReverbProcess {

    struct combFilters {
        std::vector<Comb*> filters;
        std::vector<float*> buffers;

        ~combFilters() {
            while ( !filters.empty() ) {
                delete filters.at( 0 );
                filters.erase( filters.begin() );
            }
            while ( !buffers.empty() ) {
                delete buffers.at( 0 );
                buffers.erase( buffers.begin() );
            }
        }
    };

    struct allpassFilters {
        std::vector<AllPass*> filters;
        std::vector<float*> buffers;

        ~allpassFilters() {
            while ( !filters.empty() ) {
                delete filters.at( 0 );
                filters.erase( filters.begin() );
            }
            while ( !buffers.empty() ) {
                delete buffers.at( 0 );
                buffers.erase( buffers.begin() );
            }
        }
    };

    static constexpr float MAX_RECORD_TIME_MS = 5000.f;
    static constexpr float MUTED              = 0;
    static constexpr float FIXED_GAIN         = 0.015f;
    static constexpr float SCALE_WET          = 1.f;
    static constexpr float SCALE_DRY          = 1.f;
    static constexpr float SCALE_DAMP         = 0.4f;
    static constexpr float SCALE_ROOM         = 0.28f;
    static constexpr float OFFSET_ROOM        = 0.7f;
    static constexpr float INITIAL_ROOM       = 0.5f;
    static constexpr float INITIAL_DAMP       = 0.5f;
    static constexpr float INITIAL_WET        = 1 / SCALE_WET;
    static constexpr float INITIAL_DRY        = 0.5;
    static constexpr float INITIAL_WIDTH      = 1;
    static constexpr float INITIAL_MODE       = 0;
    static constexpr float FREEZE_MODE        = 0.5f;
    static constexpr int STEREO_SPREAD        = 23;

    // we allow only a slowdown and speed up of 100 pct

    static constexpr float MIN_PLAYBACK_RATE = 0.5f;
    static constexpr float MAX_PLAYBACK_RATE = 1.5f;

    public:
        ReverbProcess( int amountOfChannels );
        ~ReverbProcess();

        // apply effect to incoming sampleBuffer contents

        template <typename SampleType>
        void process( SampleType** inBuffer, SampleType** outBuffer, int numInChannels, int numOutChannels,
            int bufferSize, uint32_t sampleFramesSize
        );

        void mute();
        void setRoomSize( float value );
        float getRoomSize();
        void setDamp( float value );
        float getDamp();
        void setWet( float value );
        float getWet();
        void setDry( float value );
        float getDry();
        float getWidth();
        void setWidth( float value );
        float getMode();
        void setMode( float value );
        float getPlaybackRate();
        void setPlaybackRate( float value );

        BitCrusher* bitCrusher;
        Decimator* decimator;
        Filter* filter;
        Limiter* limiter;

        // whether effects are applied onto the input delay signal or onto
        // the delayed signal itself (false = on input, true = on delay)

        bool bitCrusherPostMix;

    private:
        AudioBuffer* _recordBuffer;  // contains the sample memory for drift mode
        AudioBuffer* _preMixBuffer;  // buffer used for the pre-delay effect mixing
        AudioBuffer* _postMixBuffer; // buffer used for the post-delay effect mixing
        int  _amountOfChannels;
        int  _maxRecordIndex;
        int* _recordIndices;

        void setupFilters();         // generates comb and allpass filter buffers
        void clearFilters();         // frees memory allocated to comb and allpass filter buffers
        void update();

        float _playbackRate;
        float _playbackReadIndex;

        float _gain;
        float _roomSize, _roomSize1;
        float _damp, _damp1;
        float _wet, _wet1, _wet2;
        float _dry;
        float _width;
        float _mode;

        std::vector<combFilters*>    _combFilters;
        std::vector<allpassFilters*> _allpassFilters;

        // ensures the pre- and post mix buffers match the appropriate amount of channels
        // and buffer size. this also clones the contents of given in buffer into the pre-mix buffer
        // the buffers are pooled so this can be called upon each process cycle without allocation overhead

        template <typename SampleType>
        void prepareMixBuffers( SampleType** inBuffer, int numInChannels, int bufferSize );

};

template <typename SampleType>
void ReverbProcess::process( SampleType** inBuffer, SampleType** outBuffer, int numInChannels, int numOutChannels,
                             int bufferSize, uint32_t sampleFramesSize ) {

    // input and output buffers can be float or double as defined
    // by the templates SampleType value. Internally we process
    // audio as floats

    SampleType inSample;
    float frac, s1, s2;
    int i, t, t2, readIndex, channelDelayBufferChannel;
    bool hasDrift = ( _playbackRate != 1.0f );
    float orgPlaybackReadIndex = _playbackReadIndex;

    // prepare the mix buffers and clone the incoming buffer contents into the pre-mix buffer

    prepareMixBuffers( inBuffer, numInChannels, bufferSize );

    for ( int32_t c = 0; c < numInChannels; ++c )
    {
        SampleType* channelInBuffer  = inBuffer[ c ];
        SampleType* channelOutBuffer = outBuffer[ c ];
        float* channelRecordBuffer   = _recordBuffer->getBufferForChannel( c );
        float* channelPreMixBuffer   = _preMixBuffer->getBufferForChannel( c );
        float* channelPostMixBuffer  = _postMixBuffer->getBufferForChannel( c );

        orgPlaybackReadIndex = _playbackReadIndex;

        // when processing the first channel, store the current effects properties
        // so each subsequent channel is processed using the same processor variables

        if ( c == 0 ) {
            decimator->store();
            filter->store();
        }

        // PRE MIX processing

        if ( !bitCrusherPostMix )
            bitCrusher->process( channelPreMixBuffer, bufferSize );

        decimator->process( channelPreMixBuffer, bufferSize );

        // record the incoming premixed, processed signal into the record buffer (for use with drift mode)

        int recordIndex = _recordIndices[ c ];
        for ( i = 0; i < bufferSize; ++i ) {
            channelRecordBuffer[ recordIndex ] = ( float ) channelPreMixBuffer[ i ];
            if ( ++recordIndex >= _maxRecordIndex ) {
                recordIndex = 0;
            }
        }
        // update last recording index for this channel
        _recordIndices[ c ] = recordIndex;

        // REVERB processing applied onto the temp buffer

        SampleType inputSample, processedSample;
        combFilters* combs        = _combFilters.at( c );
        allpassFilters* allpasses = _allpassFilters.at( c );

        for ( i = 0; i < bufferSize; ++i )
        {
            // in case the process is running in drift mode, read sample
            // from the pre-recorded buffer so we can vary playback speeds
            if ( hasDrift ) {
                t    = ( int ) _playbackReadIndex;
                t2   = t + 1;
                frac = _playbackReadIndex - t;

                s1 = channelRecordBuffer[ t ];
                s2 = channelRecordBuffer[ t2 < _maxRecordIndex ? t2 : t ];

                inputSample = s1 + ( s2 - s1 ) * frac;

                if (( _playbackReadIndex += _playbackRate ) >= _maxRecordIndex ) {
                    _playbackReadIndex = 0.f;
                }
            }
            else {
                // no drift enabled, take sample directly from the input buffer
                inputSample = channelPreMixBuffer[ i ];
            }

            // ---- REVERB process

            processedSample = 0;
            inputSample *= _gain;

            // Accumulate comb filters in parallel
            for ( int i = 0; i < VST::NUM_COMBS; i++ ) {
                processedSample += combs->filters.at( i )->process( inputSample );
            }

            // Feed through allPasses in series
            for ( int i = 0; i < VST::NUM_ALLPASSES; i++ ) {
                processedSample = allpasses->filters.at( i )->process( processedSample );
            }

            // write the reverberated sample into the post mix buffer
            channelPostMixBuffer[ i ] = processedSample/* * _wet1 + ( input * _dry ) */;
        }

        // POST MIX processing
        // apply the post mix effect processing

        filter->process( channelPostMixBuffer, bufferSize, c );

        if ( bitCrusherPostMix )
            bitCrusher->process( channelPostMixBuffer, bufferSize );

        // mix the input and processed post mix buffers into the output buffer

        for ( i = 0; i < bufferSize; ++i ) {

            // before writing to the out buffer we take a snapshot of the current in sample
            // value as VST2 in Ableton Live supplies the same buffer for in and out!
            inSample = channelInBuffer[ i ];

            // wet mix (e.g. the effected signal)
            channelOutBuffer[ i ] = ( SampleType ) channelPostMixBuffer[ i ] * _wet1;

            // dry mix (e.g. mix in the input signal)
            channelOutBuffer[ i ] += ( inSample * _dry );
        }

        // prepare effects for the next channel

        if ( c < ( numInChannels - 1 )) {
            decimator->restore();
            filter->restore();
        }
    }

    // limit the output signal as it can get quite hot
    limiter->process<SampleType>( outBuffer, bufferSize, numOutChannels );
}

template <typename SampleType>
void ReverbProcess::prepareMixBuffers( SampleType** inBuffer, int numInChannels, int bufferSize )
{
    // if the pre mix buffer wasn't created yet or the buffer size has changed
    // delete existing buffer and create new one to match properties

    if ( _preMixBuffer == nullptr || _preMixBuffer->bufferSize != bufferSize ) {
        delete _preMixBuffer;
        _preMixBuffer = new AudioBuffer( numInChannels, bufferSize );
    }

    // clone the in buffer contents
    // note the clone is always cast to float as it is
    // used for internal processing (see ReverbProcess::process)

    for ( int c = 0; c < numInChannels; ++c ) {

        SampleType* inChannelBuffer = ( SampleType* ) inBuffer[ c ];
        float* channelPremixBuffer  = ( float* ) _preMixBuffer->getBufferForChannel( c );

        for ( int i = 0; i < bufferSize; ++i ) {
            // clone into the pre mix buffer for pre-processing
            channelPremixBuffer[ i ] = ( float ) inChannelBuffer[ i ];
        }
    }

    // if the post mix buffer wasn't created yet or the buffer size has changed
    // delete existing buffer and create new one to match properties

    if ( _postMixBuffer == nullptr || _postMixBuffer->bufferSize != bufferSize ) {
        delete _postMixBuffer;
        _postMixBuffer = new AudioBuffer( numInChannels, bufferSize );
    }
}

 AllPass::AllPass()
 {
     _bufIndex = 0;
     setFeedback( 0.5f );
 }

 void AllPass::setBuffer( float *buf, int size )
 {
     _buffer  = buf;
     _bufSize = size;
 }

 void AllPass::mute()
 {
     for ( int i = 0; i < _bufSize; i++ ) {
         _buffer[ i ] = 0;
     }
 }

 float AllPass::getFeedback()
 {
     return _feedback;
 }

 void AllPass::setFeedback( float val )
 {
     _feedback = val;
 }

 AudioBuffer::AudioBuffer( int aAmountOfChannels, int aBufferSize )
{
    loopeable        = false;
    amountOfChannels = aAmountOfChannels;
    bufferSize       = aBufferSize;

    // create silent buffers for each channel

    _buffers = new std::vector<float*>( amountOfChannels );

    // fill buffers with silence

    for ( int i = 0; i < amountOfChannels; ++i ) {
        _buffers->at( i ) = new float[ aBufferSize ];
        memset( _buffers->at( i ), 0, aBufferSize * sizeof( float )); // zero bits should equal 0.f
    }
}

AudioBuffer::~AudioBuffer()
{
    while ( !_buffers->empty()) {
        delete[] _buffers->back(), _buffers->pop_back();
    }
    delete _buffers;
}

/* public methods */

float* AudioBuffer::getBufferForChannel( int aChannelNum )
{
    return _buffers->at( aChannelNum );
}

int AudioBuffer::mergeBuffers( AudioBuffer* aBuffer, int aReadOffset, int aWriteOffset, float aMixVolume )
{
    if ( aBuffer == 0 || aWriteOffset >= bufferSize )
        return 0;

    int sourceLength     = aBuffer->bufferSize;
    int maxSourceChannel = aBuffer->amountOfChannels - 1;
    int writeLength      = bufferSize;
    int writtenSamples   = 0;

    // keep writes within the bounds of this buffer

    if (( aWriteOffset + writeLength ) >= bufferSize )
        writeLength = bufferSize - aWriteOffset;

    int maxWriteOffset = aWriteOffset + writeLength;
    int c;

    for ( c = 0; c < amountOfChannels; ++c )
    {
        if ( c > maxSourceChannel )
            break;

        float* srcBuffer    = aBuffer->getBufferForChannel( c );
        float* targetBuffer = getBufferForChannel( c );

        for ( int i = aWriteOffset, r = aReadOffset; i < maxWriteOffset; ++i, ++r )
        {
            if ( r >= sourceLength )
            {
                if ( aBuffer->loopeable )
                    r = 0;
                else
                    break;
            }
            targetBuffer[ i ] += ( srcBuffer[ r ] * aMixVolume );
            ++writtenSamples;
        }
    }
    // return the amount of samples written (per buffer)
    return ( c == 0 ) ? writtenSamples : writtenSamples / c;
}

/**
 * fills the buffers with silence
 * clearing their previous contents
 */
void AudioBuffer::silenceBuffers()
{
    // use mem set to quickly erase existing buffer contents, zero bits should equal 0.f
    for ( int i = 0; i < amountOfChannels; ++i )
        memset( getBufferForChannel( i ), 0, bufferSize * sizeof( float ));
}

void AudioBuffer::adjustBufferVolumes( float amp )
{
    for ( int i = 0; i < amountOfChannels; ++i )
    {
        float* buffer = getBufferForChannel( i );

        for ( int j = 0; j < bufferSize; ++j )
            buffer[ j ] *= amp;
    }
}

bool AudioBuffer::isSilent()
{
    for ( int i = 0; i < amountOfChannels; ++i )
    {
        float* buffer = getBufferForChannel( i );
        for ( int j = 0; j < bufferSize; ++j )
        {
            if ( buffer[ j ] != 0.f )
                return false;
        }
    }
    return true;
}

AudioBuffer* AudioBuffer::clone()
{
    AudioBuffer* output = new AudioBuffer( amountOfChannels, bufferSize );

    for ( int i = 0; i < amountOfChannels; ++i )
    {
        float* sourceBuffer = getBufferForChannel( i );
        float* targetBuffer = output->getBufferForChannel( i );

        memcpy( targetBuffer, sourceBuffer, bufferSize * sizeof( float ));
    }
    return output;
}


/* constructor */

BitCrusher::BitCrusher( float amount, float inputMix, float outputMix )
{
    setAmount   ( amount );
    setInputMix ( inputMix );
    setOutputMix( outputMix );

    _tempAmount = _amount;

    lfo = new LFO();
    hasLFO = false;
}

BitCrusher::~BitCrusher()
{
    delete lfo;
}

/* public methods */

void BitCrusher::setLFO( float LFORatePercentage, float LFODepth )
{
    bool wasEnabled = hasLFO;
    bool enabled    = LFORatePercentage > 0.f;

    hasLFO = enabled;

    bool hadChange = ( wasEnabled != enabled ) || _lfoDepth != LFODepth;

    if ( enabled )
        lfo->setRate(
            VST::MIN_LFO_RATE() + (
                LFORatePercentage * ( VST::MAX_LFO_RATE() - VST::MIN_LFO_RATE() )
            )
        );

    // turning LFO off
    if ( !hasLFO && wasEnabled ) {
        _tempAmount = _amount;
        calcBits();
    }

    if ( hadChange ) {
        _lfoDepth = LFODepth;
        cacheLFO();
    }
}

void BitCrusher::process( float* inBuffer, int bufferSize )
{
    // sound should not be crushed ? do nothing
    if ( _bits == 16 && !hasLFO )
        return;

    int bitsPlusOne = _bits + 1;

    for ( int i = 0; i < bufferSize; ++i )
    {
        short input = ( short ) (( inBuffer[ i ] * _inputMix ) * SHRT_MAX );
        short prevent_offset = ( short )( -1 >> bitsPlusOne );
        input &= ( -1 << ( 16 - _bits ));
        inBuffer[ i ] = (( input + prevent_offset ) * _outputMix ) / SHRT_MAX;

        if ( hasLFO ) {
            // multiply by .5 and add .5 to make the LFO's bipolar waveform unipolar
            float lfoValue = lfo->peek() * .5f  + .5f;
            _tempAmount = std::min( _lfoMax, _lfoMin + _lfoRange * lfoValue );

            // recalculate the current resolution
            calcBits();
            bitsPlusOne = _bits + 1;
        }
    }
}

/* setters */

void BitCrusher::setAmount( float value )
{
    // invert the range 0 == max bits (no distortion), 1 == min bits (severely distorted)
    float scaledValue = abs(value - 1.f);

    float tempRatio = _tempAmount / std::max( 0.000000001f, _amount );

    _amount = scaledValue;

    // in case BitCrusher is attached to oscillator, keep relative offset
    // of currently moving bit resolution in place
    _tempAmount = ( hasLFO ) ? _amount * tempRatio : _amount;

    cacheLFO();
    calcBits();
}

void BitCrusher::setInputMix( float value )
{
    _inputMix = Calc::cap( value );
}

void BitCrusher::setOutputMix( float value )
{
    _outputMix = Calc::cap( value );
}

/* private methods */

void BitCrusher::cacheLFO()
{
    _lfoRange = ( float ) _amount * _lfoDepth;
    _lfoMax   = std::min( 1.f, ( float ) _amount + _lfoRange / 2.f );
    _lfoMin   = std::max( 0.f, ( float ) _amount - _lfoRange / 2.f );
}

void BitCrusher::calcBits()
{
    // scale float to 1 - 16 bit range
    _bits = ( int ) floor( Calc::scale( _tempAmount, 1, 15 )) + 1;
}


Comb::Comb()
 {
     _filterStore = 0;
     _bufIndex    = 0;
 }

 void Comb::setBuffer( float *buf, int size )
 {
     _buffer  = buf;
     _bufSize = size;
 }

 void Comb::mute()
 {
     for ( int i = 0; i < _bufSize; i++ ) {
         _buffer[ i ] = 0;
     }
 }

 float Comb::getDamp()
 {
     return _damp1;
 }

 void Comb::setDamp( float val )
 {
     _damp1 = val;
     _damp2 = 1 - val;
 }

 float Comb::getFeedback()
 {
     return _feedback;
 }

 void Comb::setFeedback( float val )
 {
     _feedback = val;
 }

 /* constructor / destructor */

Decimator::Decimator( int bits, float rate )
{
    setBits( bits );
    setRate( rate );

    _accumulator = 0.0;
}

Decimator::~Decimator()
{
    // nowt...
}

/* getters / setters */

int Decimator::getBits()
{
    return _bits;
}

void Decimator::setBits( int value )
{
    // cap in 1 - 32 range
    _bits = std::min( 32, std::max( 1, value ));
    _m    = 1 << ( _bits - 1 );
}

float Decimator::getRate()
{
    return _rate;
}

void Decimator::setRate( float value )
{
    _rate = Calc::cap( value );
}

void Decimator::store()
{
    _accumulatorStored = _accumulator;
}

void Decimator::restore()
{
    _accumulator = _accumulatorStored;
}

/* public methods */

void Decimator::process( float* sampleBuffer, int bufferSize )
{
    float sample;
    bool doProcess = ( _bits < 32 );

    for ( int i = 0; i < bufferSize; ++i )
    {
        sample = sampleBuffer[ i ];
        _accumulator += _rate;

        if ( _accumulator >= 1.f )
        {
            _accumulator -= 1.f;

            if ( doProcess )
                sample = ( float ) _m * floor( sample / ( float ) _m + 0.5f );
        }
        sampleBuffer[ i ] = sample;
    }
}

Filter::Filter() {

    _cutoff     = VST::FILTER_MIN_FREQ;
    _resonance  = VST::FILTER_MIN_RESONANCE;
    _depth      = 1.f;
    _lfoMin     = VST::FILTER_MIN_FREQ;
    _lfoMax     = VST::FILTER_MAX_FREQ;
    _lfoRange   = _cutoff * _depth;
    _tempCutoff = _cutoff; // used when applying LFO

    _a1 = 0.f;
    _a2 = 0.f;
    _a3 = 0.f;
    _b1 = 0.f;
    _b2 = 0.f;
    _c  = 0.f;

    lfo = new LFO();

    _hasLFO = false;

    // stereo (2) probably enough...
    int numChannels = 8;

    _in1  = new float[ numChannels ];
    _in2  = new float[ numChannels ];
    _out1 = new float[ numChannels ];
    _out2 = new float[ numChannels ];

    for ( int i = 0; i < numChannels; ++i )
    {
        _in1 [ i ] = 0.f;
        _in2 [ i ] = 0.f;
        _out1[ i ] = 0.f;
        _out2[ i ] = 0.f;
    }
    setCutoff( VST::FILTER_MAX_FREQ / 2 );
}

Filter::~Filter() {
    delete lfo;
    delete[] _in1;
    delete[] _in2;
    delete[] _out1;
    delete[] _out2;
}

/* public methods */

void Filter::updateProperties( float cutoffPercentage, float resonancePercentage, float LFORatePercentage, float LFODepth )
{
    float co  = VST::FILTER_MIN_FREQ + ( cutoffPercentage * ( VST::FILTER_MAX_FREQ - VST::FILTER_MIN_FREQ ));
    float res = VST::FILTER_MIN_RESONANCE + ( resonancePercentage * ( VST::FILTER_MAX_RESONANCE - VST::FILTER_MIN_RESONANCE ));

    if ( _cutoff != co || _resonance != res ) {
        setCutoff( co );
        setResonance( res );
    }
    _depth = LFODepth;

    bool doLFO = LFORatePercentage != 0.f;

    if ( !doLFO && _hasLFO ) {
        setLFO( false );
    }
    else if ( doLFO ) {
        setLFO( true );
        cacheLFOProperties();
        lfo->setRate(
            VST::MIN_LFO_RATE() + (
                LFORatePercentage * ( VST::MAX_LFO_RATE() - VST::MIN_LFO_RATE() )
            )
        );
    }
}

void Filter::process( float* sampleBuffer, int bufferSize, int c )
{
    for ( int32_t i = 0; i < bufferSize; ++i )
    {
        float input  = sampleBuffer[ i ];
        float output = _a1 * input + _a2 * _in1[ c ] + _a3 * _in2[ c ] - _b1 * _out1[ c ] - _b2 * _out2[ c ];

        _in2 [ c ] = _in1[ c ];
        _in1 [ c ] = input;
        _out2[ c ] = _out1[ c ];
        _out1[ c ] = output;

        // oscillator attached to Filter ? travel the cutoff values
        // between the minimum and maximum frequencies

        if ( _hasLFO )
        {
            // multiply by .5 and add .5 to make bipolar waveform unipolar
            float lfoValue = lfo->peek() * .5f  + .5f;
            _tempCutoff = std::min( _lfoMax, _lfoMin + _lfoRange * lfoValue );

            calculateParameters();
        }

        // commit the effect
        sampleBuffer[ i ] = output;
    }
}

void Filter::setCutoff( float frequency )
{
    // in case LFO is moving, set the current temp cutoff (last LFO value)
    // to the relative value for the new cutoff frequency)

    float tempRatio = _tempCutoff / _cutoff;

    _cutoff     = std::max( VST::FILTER_MIN_FREQ, std::min( frequency, VST::FILTER_MAX_FREQ ));
    _tempCutoff = _cutoff * tempRatio;

    calculateParameters();
}

float Filter::getCutoff()
{
    return _cutoff;
}

void Filter::setResonance( float resonance )
{
    _resonance = std::max( VST::FILTER_MIN_RESONANCE, std::min( resonance, VST::FILTER_MAX_RESONANCE ));
    calculateParameters();
}

float Filter::getResonance()
{
    return _resonance;
}

void Filter::setLFO( bool enabled )
{
    _hasLFO = enabled;

    // no LFO ? make sure the filter returns to its default parameters

    if ( !enabled )
    {
        _tempCutoff = _cutoff;
        cacheLFOProperties();
        calculateParameters();
    }
}

void Filter::store()
{
    _accumulatorStored = lfo->getAccumulator();
    _tempCutoffStored  = _tempCutoff;
}

void Filter::restore()
{
    lfo->setAccumulator( _accumulatorStored );
    _tempCutoff = _tempCutoffStored;
    calculateParameters();
}

void Filter::calculateParameters()
{
    _c  = 1.f / tan( VST::PI * _tempCutoff / VST::SAMPLE_RATE );
    _a1 = 1.f / ( 1.f + _resonance * _c + _c * _c );
    _a2 = 2.f * _a1;
    _a3 = _a1;
    _b1 = 2.f * ( 1.f - _c * _c ) * _a1;
    _b2 = ( 1.f - _resonance * _c + _c * _c ) * _a1;
}

void Filter::cacheLFOProperties()
{
    _lfoRange = _cutoff * _depth;
    _lfoMax   = std::min( VST::FILTER_MAX_FREQ, _cutoff + _lfoRange / 2.f );
    _lfoMin   = std::max( VST::FILTER_MIN_FREQ, _cutoff - _lfoRange / 2.f );
}

LFO::LFO() {
    _rate        = VST::MIN_LFO_RATE();
    _accumulator = 0.f;
}

LFO::~LFO() {

}

/* public methods */

float LFO::getRate()
{
    return _rate;
}

void LFO::setRate( float value )
{
    _rate = value;
}

void LFO::setAccumulator( float value )
{
    _accumulator = value;
}

float LFO::getAccumulator()
{
    return _accumulator;
}

Limiter::Limiter()
{
    init( 0.15, 0.50, 0.60 );
}

Limiter::Limiter( float attackMs, float releaseMs, float thresholdDb )
{
    init( attackMs, releaseMs, thresholdDb );
}

Limiter::~Limiter()
{
    // nowt...
}

/* public methods */

void Limiter::setAttack( float attackMs )
{
    pAttack = ( float ) attackMs;
    recalculate();
}

void Limiter::setRelease( float releaseMs )
{
    pRelease = ( float ) releaseMs;
    recalculate();
}

void Limiter::setThreshold( float thresholdDb )
{
    pTresh = ( float ) thresholdDb;
    recalculate();
}

float Limiter::getLinearGR()
{
    return gain > 1.f ? 1.f / gain : 1.f;
}

/* protected methods */

void Limiter::init( float attackMs, float releaseMs, float thresholdDb )
{
    pAttack  = ( float ) attackMs;
    pRelease = ( float ) releaseMs;
    pTresh   = ( float ) thresholdDb;
    pTrim    = ( float ) 0.60;
    pKnee    = ( float ) 0.40;

    gain = 1.f;

    recalculate();
}

void Limiter::recalculate()
{
    if ( pKnee > 0.5 ) {
        // soft knee
        thresh = ( float ) pow( 10.0, 1.f - ( 2.0 * pTresh ));
    }
    else {
        // hard knee
        thresh = ( float ) pow( 10.0, ( 2.0 * pTresh ) - 2.0 );
    }
    trim = ( float )( pow( 10.0, ( 2.0 * pTrim) - 1.f ));
    att  = ( float )  pow( 10.0, -2.0 * pAttack );
    rel  = ( float )  pow( 10.0, -2.0 - ( 3.0 * pRelease ));
}

ReverbProcess::ReverbProcess( int amountOfChannels ) {
    _amountOfChannels = amountOfChannels;

    _maxRecordIndex = Calc::millisecondsToBuffer( MAX_RECORD_TIME_MS );
    _recordBuffer   = new AudioBuffer( amountOfChannels, _maxRecordIndex );
    _recordIndices  = new int[ amountOfChannels ];
    for ( int i = 0; i < amountOfChannels; ++i ) {
        _recordIndices[ i ] = 0;
    }
    _playbackReadIndex = 0.f;

    bitCrusher = new BitCrusher( 8, .5f, .5f );
    decimator  = new Decimator( 32, 0.f );
    filter     = new Filter();
    limiter    = new Limiter( 10.f, 500.f, .6f );

    setupFilters();

    setWet     ( INITIAL_WET );
    setRoomSize( INITIAL_ROOM );
    setDry     ( INITIAL_DRY );
    setDamp    ( INITIAL_DAMP );
    setWidth   ( INITIAL_WIDTH );
    setMode    ( INITIAL_MODE );

    // this will initialize the buffers with silence
    mute();

    // will be lazily created in the process function
    _preMixBuffer  = nullptr;
    _postMixBuffer = nullptr;
    _playbackRate  = 1.f;
}

ReverbProcess::~ReverbProcess() {
    delete _recordIndices;
    delete _recordBuffer;
    delete _postMixBuffer;
    delete _preMixBuffer;
    delete bitCrusher;
    delete decimator;
    delete filter;
    delete limiter;
    clearFilters();
}

void ReverbProcess::mute()
{
    if ( getMode() >= FREEZE_MODE )
        return;

    for ( int c = 0; c < _amountOfChannels; ++c ) {
        auto combData = _combFilters.at( c );
        for ( int i = 0; i < VST::NUM_COMBS; i++ ) {
            combData->filters.at( i )->mute();
        }

        auto allPassData = _allpassFilters.at( c );
        for ( int i = 0; i < VST::NUM_ALLPASSES; i++ ) {
            allPassData->filters.at( i )->mute();
        }
    }
}

float ReverbProcess::getRoomSize()
{
    return ( _roomSize - OFFSET_ROOM ) / SCALE_ROOM;
}

void ReverbProcess::setRoomSize( float value )
{
    _roomSize = ( value * SCALE_ROOM ) + OFFSET_ROOM;
    update();
}

float ReverbProcess::getDamp()
{
    return _damp / SCALE_DAMP;
}

void ReverbProcess::setDamp( float value )
{
    _damp = value * SCALE_DAMP;
    update();
}

float ReverbProcess::getWet()
{
    return _wet / SCALE_WET;
}

void ReverbProcess::setWet( float value )
{
    _wet = value * SCALE_WET;
    update();
}

float ReverbProcess::getDry()
{
    return _dry / SCALE_DRY;
}

void ReverbProcess::setDry( float value )
{
    _dry = value * SCALE_DRY;
}

float ReverbProcess::getWidth()
{
    return _width;
}

void ReverbProcess::setWidth( float value )
{
    _width = value;
    update();
}

float ReverbProcess::getPlaybackRate()
{
    return Calc::scale( _playbackRate - MIN_PLAYBACK_RATE, 1.0f, 1.0f );
}

void ReverbProcess::setPlaybackRate( float value )
{
    // "snap" to neutral setting when roughly halfway
    if ( value >= .48f && value <= .52f ) {
        _playbackRate = 1.0f;
    }
     else {
        _playbackRate = MIN_PLAYBACK_RATE + Calc::scale(value, 1.0f, 1.0f );
    }
}

float ReverbProcess::getMode()
{
    return ( _mode >= FREEZE_MODE ) ? 1 : 0;
}

void ReverbProcess::setMode( float value )
{
    _mode = value;
    update();
}

void ReverbProcess::setupFilters()
{
    clearFilters();

    // create filters and buffers per output channel

    for ( int c = 0; c < _amountOfChannels; ++c ) {
        combFilters* combData = new combFilters();
        _combFilters.push_back( combData );

        // comb filters

        for ( int i = 0; i < VST::NUM_COMBS; ++i ) {
            // tune the comb to the host environments sample rate
            int tuning = ( int ) ((( float ) VST::COMB_TUNINGS[ i ] / 44100.f ) * VST::SAMPLE_RATE );
            int size = tuning + ( c * STEREO_SPREAD );
            float* buffer = new float[ size ];

            Comb* comb = new Comb();
            comb->setBuffer( buffer, size );
            combData->filters.push_back( comb );
            combData->buffers.push_back( buffer );
        }

        // all pass filters

        allpassFilters* allpassData = new allpassFilters();
        _allpassFilters.push_back( allpassData );

        for ( int i = 0; i < VST::NUM_ALLPASSES; ++i ) {
            // tune the comb to the host environments sample rate
            int tuning = ( int ) ((( float ) VST::ALLPASS_TUNINGS[ i ] / 44100.f ) * VST::SAMPLE_RATE );
            int size = tuning + ( c * STEREO_SPREAD );
            float* buffer = new float[ size ];

            AllPass* allPass = new AllPass();
            allPass->setBuffer( buffer, size );
            allpassData->filters.push_back( allPass );
            allpassData->buffers.push_back( buffer );
        }
    }
}

void ReverbProcess::clearFilters()
{
    while ( !_combFilters.empty() ) {
        delete _combFilters.at( 0 );
        _combFilters.erase( _combFilters.begin() );
    }
    while ( !_allpassFilters.empty() ) {
        delete _allpassFilters.at( 0 );
        _allpassFilters.erase( _allpassFilters.begin() );
    }
}

void ReverbProcess::update()
{
    // Recalculate internal values after parameter change

    _wet1 = _wet * ( _width / 2 + 0.5f );
    _wet2 = _wet * (( 1 - _width ) / 2 );

    if ( _mode >= FREEZE_MODE ){
        _roomSize1 = 1;
        _damp1     = 0;
        _gain      = MUTED;
    }
    else {
        _roomSize1 = _roomSize;
        _damp1     = _damp;
        _gain      = FIXED_GAIN;
    }

    for ( int c = 0; c < _amountOfChannels; ++c ) {
        auto combData = _combFilters.at( c );
        for ( int i = 0; i < VST::NUM_COMBS; i++ ) {
            combData->filters.at( i )->setFeedback( _roomSize1 );
            combData->filters.at( i )->setDamp( _damp1 );
        }
    }
}
