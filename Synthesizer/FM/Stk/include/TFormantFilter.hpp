
// doesn't compile yet
#pragma once
#include <cmath>
#include <vector>

namespace SoundWave {
enum
{
    // ids for all visual controls

    kVowelLId = 1,         // formant vowel L
    kVowelRId,             // formant vowel R
    kVowelSyncId,          // formant vowel sync
    kLFOVowelLId,          // formant vowel L LFO rate
    kLFOVowelLDepthId,     // formant vowel L LFO depth
    kLFOVowelRId,          // formant vowel R LFO rate
    kLFOVowelRDepthId,     // formant vowel R LFO depth
    kDistortionTypeId,     // distortion type
    kDriveId,              // distortion drive amount
    kDistortionChainId,    // distortion pre/pos formant mix
    kVuPPMId               // for the Vu value return to host
};
enum {
	type_lowpass = 1,
	type_highpass,
	type_bandpass,
	type_notch,
	type_peak,
	type_lowshelf,
	type_highshelf
};

template<class T>
class TFormantFilter
{
public:
	TFormantFilter();
	~TFormantFilter();

	void prepareToPlay(T sampleRate, int samplesPerBlock);

	T simpleFIRHiPass(T currentSample, T sliderValue);
	T simpleIIRHiPass(T currentSample, T sliderValue);
	T simpleIIRLowPass(T currentSample, T sliderValue);

	void setBiquad(int type, T Freq, T Q, T peakGain);
	T processBiquad(T in);

	void setButterworth(int type, T Freq, T Q, T peakGain);
	T processButterworth(T in);
	//Static values, set up in prepare to play.
	int bufferLength;
	int sampleRate;
	T* sn;
	T* cs;

protected:
	void calcBiquad(void);
	void calcButterworth(void);

	//type of filter
	int type;
	//Coefficient values
	T a0, a1, a2, b1, b2 , c0, c1, c2, d1, d2;
	T frequency, Q, peakGain;
	T z1, z2;

	//Post-process delayed samples y[n]
	T delayedSampleY1;
	T delayedSampleY2;
	//Pre-processed delayed samples x[n]
	T delayedSampleX1;
	T delayedSampleX2;
};

template<class T>
inline T TFormantFilter<T>::processBiquad(T in)
{
	T out = in * a0 + z1;
	z1 = in * a1 + z2 - b1 * out;
	z2 = in * a2 - b2 * out;
	return out;
}

template<class T>
inline T TFormantFilter<T>::processButterworth(T in)
{
	T V = pow(10, fabs(peakGain) / 20.0);
	T out = in * a0 + delayedSampleX1 * a1 + delayedSampleX2 * a2 - delayedSampleY1 * b1 - delayedSampleY2 * b2;
	delayedSampleX2 = delayedSampleX1;
	delayedSampleY2 = delayedSampleY1;
	delayedSampleX1 = in;
	delayedSampleY1 = out;

	return out;
}

template<class T>
TFormantFilter<T>::TFormantFilter()
{
	sampleRate = 0;
	type = type_lowpass;
	a0 = 1.0;
	a1 = a2 = b1 = b2 = 0.0;
	frequency = 20;
	Q = 0.707;
	peakGain = 0.0;
	//Dummy variables to set sin/cos pointers.
	T a = 4;
	T b = 4;
	sn = &a;
	cs = &b;
}

template<class T>
TFormantFilter<T>::~TFormantFilter()
{
}

template<class T>
void TFormantFilter<T>::prepareToPlay(T sampleRate, int samplesPerBlock)
{
	this->sampleRate = sampleRate;
	this->bufferLength = samplesPerBlock;
	delayedSampleY1 = 0.0;
	delayedSampleX1 = 0.0;
	delayedSampleY2 = 0.0;
	delayedSampleX2 = 0.0;
}

template<class T>
T TFormantFilter<T>::simpleFIRHiPass(T currentSample, T sliderValue)
{
	//design equation a0 = a1 - 1.0
	T a0 = sliderValue - 1.0;
	T previousSample = delayedSampleX1;

	T processedSample = a0 * currentSample + sliderValue * previousSample;
	delayedSampleX1 = currentSample;
	return processedSample;
}

template<class T>
T TFormantFilter<T>::simpleIIRLowPass(T currentSample, T sliderValue)
{
	T previousSample = delayedSampleY1;

	//Should be same thing, Test and remove top if it is
	//T processedSample = (sliderValue * currentSample) + ((1-sliderValue) * previousSample);
	T processedSample = previousSample + sliderValue * (currentSample - previousSample);
	delayedSampleY1 = processedSample;
	return processedSample;
}

template<class T>
void TFormantFilter<T>::setBiquad(int type, T Fc, T Q, T peakGainDB) {
	this->type = type;
	this->Q = Q;
	this->frequency = Fc;
	this->peakGain = peakGainDB;
	calcBiquad();
}

template<class T>
void TFormantFilter<T>::setButterworth(int type, T Fc, T Q, T peakGainDB) {
	this->type = type;
	this->Q = Q;
	this->frequency = Fc;
	this->peakGain = peakGainDB;
	calcButterworth();
}

template<class T>
void TFormantFilter<T>::calcBiquad(void) {
	T norm;
	T V = pow(10, fabs(peakGain) / 20.0);
	T K = tan(M_PI * (frequency / sampleRate));
	switch (this->type) {
	case type_lowpass:
		norm = 1 / (1 + K / Q + K * K);
		a0 = K * K * norm;
		a1 = 2 * a0;
		a2 = a0;
		b1 = 2 * (K * K - 1) * norm;
		b2 = (1 - K / Q + K * K) * norm;
		break;

	case type_highpass:
		norm = 1 / (1 + K / Q + K * K);
		a0 = 1 * norm;
		a1 = -2 * a0;
		a2 = a0;
		b1 = 2 * (K * K - 1) * norm;
		b2 = (1 - K / Q + K * K) * norm;
		break;

	case type_bandpass:
		norm = 1 / (1 + K / Q + K * K);
		a0 = K / Q * norm;
		a1 = 0;
		a2 = -a0;
		b1 = 2 * (K * K - 1) * norm;
		b2 = (1 - K / Q + K * K) * norm;
		break;

	case type_notch:
		norm = 1 / (1 + K / Q + K * K);
		a0 = (1 + K * K) * norm;
		a1 = 2 * (K * K - 1) * norm;
		a2 = a0;
		b1 = a1;
		b2 = (1 - K / Q + K * K) * norm;
		break;

	case type_peak:
		if (peakGain >= 0) {    // boost
			norm = 1 / (1 + 1 / Q * K + K * K);
			a0 = (1 + V / Q * K + K * K) * norm;
			a1 = 2 * (K * K - 1) * norm;
			a2 = (1 - V / Q * K + K * K) * norm;
			b1 = a1;
			b2 = (1 - 1 / Q * K + K * K) * norm;
		}
		else {    // cut
			norm = 1 / (1 + V / Q * K + K * K);
			a0 = (1 + 1 / Q * K + K * K) * norm;
			a1 = 2 * (K * K - 1) * norm;
			a2 = (1 - 1 / Q * K + K * K) * norm;
			b1 = a1;
			b2 = (1 - V / Q * K + K * K) * norm;
		}
		break;
	case type_lowshelf:
		if (peakGain >= 0) {    // boost
			norm = 1 / (1 + sqrt(2) * K + K * K);
			a0 = (1 + sqrt(2 * V) * K + V * K * K) * norm;
			a1 = 2 * (V * K * K - 1) * norm;
			a2 = (1 - sqrt(2 * V) * K + V * K * K) * norm;
			b1 = 2 * (K * K - 1) * norm;
			b2 = (1 - sqrt(2) * K + K * K) * norm;
		}
		else {    // cut
			norm = 1 / (1 + sqrt(2 * V) * K + V * K * K);
			a0 = (1 + sqrt(2) * K + K * K) * norm;
			a1 = 2 * (K * K - 1) * norm;
			a2 = (1 - sqrt(2) * K + K * K) * norm;
			b1 = 2 * (V * K * K - 1) * norm;
			b2 = (1 - sqrt(2 * V) * K + V * K * K) * norm;
		}
		break;
	case type_highshelf:
		if (peakGain >= 0) {    // boost
			norm = 1 / (1 + sqrt(2) * K + K * K);
			a0 = (V + sqrt(2 * V) * K + K * K) * norm;
			a1 = 2 * (K * K - V) * norm;
			a2 = (V - sqrt(2 * V) * K + K * K) * norm;
			b1 = 2 * (K * K - 1) * norm;
			b2 = (1 - sqrt(2) * K + K * K) * norm;
		}
		else {    // cut
			norm = 1 / (V + sqrt(2 * V) * K + K * K);
			a0 = (1 + sqrt(2) * K + K * K) * norm;
			a1 = 2 * (K * K - 1) * norm;
			a2 = (1 - sqrt(2) * K + K * K) * norm;
			b1 = 2 * (K * K - V) * norm;
			b2 = (V - sqrt(2 * V) * K + K * K) * norm;
		}
		break;
	}

	return;
}

template<class T>
void TFormantFilter<T>::calcButterworth(void)
{
	T norm;
	T freqT = 2 * tan(frequency * M_PI / sampleRate);
	T V = pow(10, fabs(peakGain) / 20.0);
	switch (this->type)
	{
	case type_lowpass:
		a0 = (freqT * freqT) / (4 + (2 * sqrt(2) * freqT) + freqT * freqT);
		a1 = 2 * a0;
		a2 = a0;
		b1 = ((2 * freqT * freqT) - 8) / (4 + (2 * sqrt(2) * freqT) + freqT * freqT);
		b2 = (4 - (2 * sqrt(2) * freqT) + freqT * freqT) / (4 + (2 * sqrt(2) * freqT) + freqT * freqT);
		break;
	case type_highpass:
		norm = freqT * freqT / 4 + freqT / sqrt(2) + 1;
		a0 = 1 / norm;
		a1 = -2 * a0;
		a2 = a0;
		b1 = ((freqT * freqT) / 2 - 2) / norm;
		b2 = ((freqT * freqT) / 4 - (freqT / sqrt(2)) + 1) / norm;
		break;
	}

	
}


template<class T>
T TFormantFilter<T>::simpleIIRHiPass(T currentSample, T sliderValue)
{
	T a0 = sliderValue - 1.0;
	T previousSampleX = delayedSampleX1;
	T previousSampleY = delayedSampleY1;

	T processedSample = a0 * (previousSampleY + currentSample - previousSampleX);
	delayedSampleY1 = processedSample;
	delayedSampleX1 = currentSample;
	return processedSample;
}



/**
 * An AudioBuffer represents multiple channels of audio
 * each of equal buffer length.
 * AudioBuffer has convenience methods for cloning, silencing and mixing
 */
template<class T>
class AudioBuffer
{
    public:
        AudioBuffer( int aAmountOfChannels, int aBufferSize );
        ~AudioBuffer();

        int amountOfChannels;
        int bufferSize;
        bool loopeable;

        T* getBufferForChannel( int aChannelNum );
        int mergeBuffers( AudioBuffer* aBuffer, int aReadOffset, int aWriteOffset, T aMixVolume );
        void silenceBuffers();
        void adjustBufferVolumes( T volume );
        bool isSilent();
        AudioBuffer* clone();

    protected:
        std::vector<T*>* _buffers;
};


template<class T>
AudioBuffer<T>::AudioBuffer( int aAmountOfChannels, int aBufferSize )
{
    loopeable        = false;
    amountOfChannels = aAmountOfChannels;
    bufferSize       = aBufferSize;

    // create silent buffers for each channel

    _buffers = new std::vector<T*>( amountOfChannels );

    // fill buffers with silence

    for ( int i = 0; i < amountOfChannels; ++i ) {
        _buffers->at( i ) = new T[ aBufferSize ];
        memset( _buffers->at( i ), 0, aBufferSize * sizeof( T )); // zero bits should equal 0.f
    }
}

template<class T>
AudioBuffer<T>::~AudioBuffer()
{
    while ( !_buffers->empty()) {
        delete[] _buffers->back(), _buffers->pop_back();
    }
    delete _buffers;
}

/* public methods */
template<class T>
T* AudioBuffer<T>::getBufferForChannel( int aChannelNum )
{
    return _buffers->at( aChannelNum );
}

template<class T>
int AudioBuffer<T>::mergeBuffers( AudioBuffer* aBuffer, int aReadOffset, int aWriteOffset, T aMixVolume )
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

        auto srcBuffer    = aBuffer->getBufferForChannel( c );
        auto targetBuffer = getBufferForChannel( c );

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
template<class T>
void AudioBuffer<T>::silenceBuffers()
{
    // use mem set to quickly erase existing buffer contents, zero bits should equal 0.f
    for ( int i = 0; i < amountOfChannels; ++i )
        memset( getBufferForChannel( i ), 0, bufferSize * sizeof( T ));
}

template<class T>
void AudioBuffer<T>::adjustBufferVolumes( T amp )
{
    for ( int i = 0; i < amountOfChannels; ++i )
    {
        auto buffer = getBufferForChannel( i );

        for ( int j = 0; j < bufferSize; ++j )
            buffer[ j ] *= amp;
    }
}

template<class T>
bool AudioBuffer<T>::isSilent()
{
    for ( int i = 0; i < amountOfChannels; ++i )
    {
        auto buffer = getBufferForChannel( i );
        for ( int j = 0; j < bufferSize; ++j )
        {
            if ( buffer[ j ] != 0.f )
                return false;
        }
    }
    return true;
}

template<class T>
AudioBuffer<T>* AudioBuffer<T>::clone()
{
    AudioBuffer<T>* output = new AudioBuffer<T>( amountOfChannels, bufferSize );

    for ( int i = 0; i < amountOfChannels; ++i )
    {
        auto sourceBuffer = getBufferForChannel( i );
        auto targetBuffer = output->getBufferForChannel( i );

        memcpy( targetBuffer, sourceBuffer, bufferSize * sizeof( T ));
    }
    return output;
}

template<typename T>
class BitCrusher {

    public:
        BitCrusher( T amount, T inputMix, T outputMix );
        ~BitCrusher();

        void process( T* inBuffer, int bufferSize );

        void setAmount( T value ); // range between -1 to +1
        void setInputMix( T value );
        void setOutputMix( T value );

    private:
        int _bits; // we scale the amount to integers in the 1-16 range
        T _amount;
        T _inputMix;
        T _outputMix;

        void calcBits();
};

template<typename T>
BitCrusher<T>::BitCrusher( T amount, T inputMix, T outputMix )
{
    setAmount   ( amount );
    setInputMix ( inputMix );
    setOutputMix( outputMix );
}

template<typename T>
BitCrusher<T>::~BitCrusher()
{

}

/* public methods */

template<typename T>
void BitCrusher<T>::process( T* inBuffer, int bufferSize )
{
    // sound should not be crushed ? do nothing
    if ( _bits == 16 ) {
        return;
    }

    int bitsPlusOne = _bits + 1;

    for ( int i = 0; i < bufferSize; ++i )
    {
        short input = ( short ) (( inBuffer[ i ] * _inputMix ) * SHRT_MAX );
        short prevent_offset = ( short )( -1 >> bitsPlusOne );
        input &= ( -1 << ( 16 - _bits ));
        inBuffer[ i ] = (( input + prevent_offset ) * _outputMix ) / SHRT_MAX;
    }
}

/* setters */
template<typename T>
void BitCrusher<T>::setAmount( T value )
{
    // invert the range 0 == max bits (no distortion), 1 == min bits (severely distorted)
    _amount = abs(value - 1.f);

    calcBits();
}

template<typename T>
void BitCrusher<T>::setInputMix( T value )
{
    _inputMix = ( value );
}

template<typename T>
void BitCrusher<T>::setOutputMix( T value )
{
    _outputMix = ( value );
}

/* private methods */
template<typename T>
void BitCrusher<T>::calcBits()
{
    // scale T to 1 - 16 bit range
    _bits = ( int ) floor( scale( _amount, 1, 15 )) + 1;
}

template<class T>
class WaveShaper
{
    public:
        WaveShaper( T amount, T level );

        T getAmount();
        void setAmount( T value ); // range between -1 and +1
        T getLevel();
        void setLevel( T value );
        void process( T* inBuffer, int bufferSize );

    private:
        T _amount;
        T _multiplier;
        T _level;
};

template<class T>
WaveShaper<T>::WaveShaper( T amount, T level )
{
    setAmount( amount );
    setLevel ( level );
}

template<class T>
void WaveShaper<T>::process( T* inBuffer, int bufferSize )
{
    for ( int j = 0; j < bufferSize; ++j )
    {
        T input = inBuffer[ j ];
        inBuffer[ j ] =  (( 1.0 + _multiplier ) * input / ( 1.0 + _multiplier * std::abs( input ))) * _level;
    }
}

template<class T>
T WaveShaper<T>::getAmount()
{
    return _amount;
}

template<class T>
void WaveShaper<T>::setAmount( T value )
{
    _amount     = value;
    _multiplier = 2.0f * _amount / ( 1.0f - fmin(0.99999f, _amount));
}

template<class T>
T WaveShaper<T>::getLevel()
{
    return _level;
}

template<class T>
void WaveShaper<T>::setLevel( T value )
{
    _level = value;
}

template<class T>
class Limiter
{
    public:
        Limiter();
        Limiter( T attackMs, T releaseMs, T thresholdDb );
        ~Limiter();
        
        void process( std::vector<T*> outputBuffer, int bufferSize, int numOutChannels );

        void setAttack( T attackMs );
        void setRelease( T releaseMs );
        void setThreshold( T thresholdDb );

        T getLinearGR();

    protected:
        void init( T attackMs, T releaseMs, T thresholdDb );
        void recalculate();

        T pTresh;   // in dB, -20 - 20
        T pTrim;
        T pAttack;  // in microseconds
        T pRelease; // in ms
        T pKnee;

        T thresh, gain, att, rel, trim;
};

template <typename T>
void Limiter<T>::process( std::vector<T*> outputBuffer, int bufferSize, int numOutChannels )
{
//    if ( gain > 0.9999f && outputBuffer->isSilent )
//    {
//        // don't process if input is silent
//        return;
//    }

    T g, at, re, tr, th, lev, ol, or_;

    th = thresh;
    g = gain;
    at = att;
    re = rel;
    tr = trim;

    bool hasRight = ( numOutChannels > 1 );

    T* leftBuffer  = outputBuffer[ 0 ];
    T* rightBuffer = hasRight ? outputBuffer[ 1 ] : 0;

    if ( pKnee > 0.5 )
    {
        // soft knee

        for ( int i = 0; i < bufferSize; ++i ) {

            ol  = leftBuffer[ i ];
            or_ = hasRight ? rightBuffer[ i ] : 0;

            lev = ( T ) ( 1.f / ( 1.f + th * fabs( ol + or_ )));

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

            lev = ( T ) ( 0.5 * g * fabs( ol + or_ ));

            if ( lev > th ) {
                g = g - ( at * ( lev - th ));
            }
            else {
                // below threshold
                g = g + ( T )( re * ( 1.f - g ));
            }

            leftBuffer[ i ] = ( ol * tr * g );

            if ( hasRight )
                rightBuffer[ i ] = ( or_ * tr * g );
        }
    }
    gain = g;
}

template<class T>
class LFO {

    public:
        LFO( T sampleRate );
        ~LFO();

        T getRate();
        void setRate( T value );

        // accumulators are used to retrieve a sample from the wave table
        // in other words: track the progress of the oscillator against its range

        T getAccumulator();
        void setAccumulator( T offset );

        /**
         * retrieve a value from the wave table for the current
         * accumulator position, this method also increments
         * the accumulator and keeps it within bounds
         */
        inline T peek()
        {
            // the wave table offset to read from
            T SR_OVER_LENGTH = _sampleRate / ( T ) TABLE_SIZE;
            int readOffset = ( _accumulator == 0.f ) ? 0 : ( int ) ( _accumulator / SR_OVER_LENGTH );

            // increment the accumulators read offset
            _accumulator += _rate;

            // keep the accumulator within the bounds of the sample frequency
            if ( _accumulator > _sampleRate )
                _accumulator -= _sampleRate;

            // return the sample present at the calculated offset within the table
            return VST::TABLE[ readOffset ];
        }

    private:

        // see Igorski::VST::LFO_TABLE;
        static const int TABLE_SIZE = 128;

        // used internally

        T _rate;
        T _accumulator;   // is read offset in wave table buffer
        T _sampleRate;
};


template<class T>
LFO<T>::LFO( T sampleRate ) {
    _rate        = VST::MIN_LFO_RATE();
    _accumulator = 0.f;
    _sampleRate  = sampleRate;
}

template<class T>
LFO<T>::~LFO() {

}

template<class T>
T LFO<T>::getRate()
{
    return _rate;
}

template<class T>
void LFO<T>::setRate( T value )
{
    _rate = value;
}

template<class T>
void LFO<T>::setAccumulator( T value )
{
    _accumulator = value;
}

template<class T>
T LFO<T>::getAccumulator()
{
    return _accumulator;
}

template<typename T>
class FormantFilter
{
    static const int VOWEL_AMOUNT       = 4;
    static const int COEFF_AMOUNT       = 9;
    static const int FORMANT_TABLE_SIZE = (256+1); // The last entry of the table equals the first (to avoid a modulo)
    static const int MAX_FORMANT_WIDTH  = 64;
    static constexpr T ATTENUATOR  = 0.0005;

    // hard coded values for dynamics processing, in -1 to +1 range

    static constexpr T DYNAMICS_THRESHOLD                  = 0.10;
    static constexpr T DYNAMICS_RATIO                      = 0.50;
    static constexpr T DYNAMICS_LEVEL                      = 0.65;
    static constexpr T DYNAMICS_ATTACK                     = 0.18;
    static constexpr T DYNAMICS_RELEASE                    = 0.55;
    static constexpr T DYNAMICS_LIMITER_DYNAMICS_THRESHOLD = 0.99;
    static constexpr T DYNAMICS_GATE_DYNAMICS_THRESHOLD    = 0.02;
    static constexpr T DYNAMICS_GATE_DYNAMICS_ATTACK       = 0.10;
    static constexpr T DYNAMICS_GATE_DECAY                 = 0.50;
    static constexpr T DYNAMICS_MIX                        = 1.00;

    // whether to apply the formant synthesis to the signal
    // otherwise the input is applied to the carrier directly

    static const bool APPLY_SYNTHESIS_SIGNAL = false;

    public:
        FormantFilter( T aVowel, T sampleRate );
        ~FormantFilter();

        void setVowel( T aVowel );
        T getVowel();
        void setLFO( T LFORatePercentage, T LFODepth );
        void process( T* inBuffer, int bufferSize );

        LFO* lfo;
        bool hasLFO;

    private:

        T  _sampleRate;
        T  _halfSampleRateFrac;
        T _vowel;
        T _tempVowel;
        int    _coeffOffset;
        T  _lfoDepth;
        T _lfoRange;
        T _lfoMax;
        T _lfoMin;

        void cacheLFO();
        inline void cacheCoeffOffset()
        {
            _coeffOffset = ( int ) Calc::scale( _tempVowel, 1.f, ( T ) COEFF_AMOUNT - 1 );
        }

        // vowel definitions

        struct Formant {
            T value;
            T coeffs[ COEFF_AMOUNT ];
        };

        T FORMANT_WIDTH_SCALE[ VOWEL_AMOUNT ] = { 100, 120, 150, 300 };

        Formant A_COEFFICIENTS[ VOWEL_AMOUNT ] = {
            { 0.0, { 1.0, 0.5, 1.0, 1.0, 0.7, 1.0, 1.0, 0.3, 1.0 } },
            { 0.0, { 2.0, 0.5, 0.7, 0.7,0.35, 0.3, 0.5, 1.0, 0.7 } },
            { 0.0, { 0.3,0.15, 0.2, 0.4, 0.1, 0.3, 0.7, 0.2, 0.2 } },
            { 0.0, { 0.2, 0.1, 0.2, 0.3, 0.1, 0.1, 0.3, 0.2, 0.3 } }
        };

        Formant F_COEFFICIENTS[ VOWEL_AMOUNT ] = {
            { 100.0, {  730,  200,  400,  250,  190,  350,  550,  550,  450 } },
            { 100.0, { 1090, 2100,  900, 1700,  800, 1900, 1600,  850, 1100 } },
            { 100.0, { 2440, 3100, 2300, 2100, 2000, 2500, 2250, 1900, 1500 } },
            { 100.0, { 3400, 4700, 3000, 3300, 3400, 3700, 3200, 3000, 3000 } }
        };

        // the below are used for the formant synthesis

        T FORMANT_TABLE[ FORMANT_TABLE_SIZE * MAX_FORMANT_WIDTH ];
        T _phase = 0.0;

        T generateFormant( T phase, const T width );
        T getFormant( T phase, T width );
        T getCarrier( const T position, const T phase );

        // Fast approximation of cos( pi * x ) for x in -1 to +1 range

        inline T fast_cos( const T x )
        {
            T x2 = x * x;
            return 1 + x2 * ( -4 + 2 * x2 );
        }

        // dynamics processing (compression and limiting to keep vowel level constant)

        inline T compress( T sample )
        {
            T a, b, i, j, g, out;
            T e   = _dEnv,
                   e2  = _dEnv2,
                   ge  = _dGainEnv,
                   re  = ( 1.f - _dRelease ),
                   lth = _dLimThreshold;

            if ( _fullDynamicsProcessing ) {

                // apply compression, gating and limiting

                if ( lth == 0.f ) {
                    lth = 1000.f;
                }
                a = sample;
                i = ( a < 0.f ) ? -a : a;

                e  = ( i > e ) ? e + _dAttack * ( i - e ) : e * re;
                e2 = ( i > e ) ? i : e2 * re; // ir;

                g = ( e > _dThreshold ) ? _dTrim / ( 1.f + _dRatio * (( e / _dThreshold ) - 1.f )) : _dTrim;

                if ( g < 0.f ) {
                    g = 0.f;
                }
                if ( g * e2 > lth ) {
                    g = lth / e2; // limiting
                }
                ge  = ( e > _dExpThreshold ) ? ge + _dGateAttack - _dGateAttack * ge : ge * _dExpRatio; // gating
                out = a * ( g * ge + _dDry );
            }
            else {
                // compression only
                a = sample;
                i = ( a < 0.f ) ? -a : a;

                e = ( i > e )  ? e + _dAttack * ( i - e ) : e * re; // envelope
                g = ( e > _dThreshold ) ? _dTrim / ( 1.f + _dRatio * (( e / _dThreshold ) - 1.f )) : _dTrim; // gain

                out = a * ( g + _dDry ); // VCA
            }

            // catch denormals

            _dEnv     = ( e  < 1.0e-10 ) ? 0.0 : e;
            _dEnv2    = ( e2 < 1.0e-10 ) ? 0.0 : e2;
            _dGainEnv = ( ge < 1.0e-10 ) ? 0.0 : ge;

            return out;
        }

        void cacheDynamicsProcessing();

        T _dThreshold;
        T _dRatio;
        T _dAttack;
        T _dRelease;
        T _dTrim;
        T _dLimThreshold;
        T _dExpThreshold;
        T _dExpRatio;
        T _dDry;
        T _dEnv;
        T _dEnv2;
        T _dGainEnv;
        T _dGateAttack;
        bool _fullDynamicsProcessing;

    };

    template<typename T>
    FormantFilter<T>::FormantFilter( T aVowel, T sampleRate )
    {
        T coeff = 2.0 / ( FORMANT_TABLE_SIZE - 1 );

        for ( size_t i = 0; i < MAX_FORMANT_WIDTH; i++ )
        {
            for ( size_t j = 0; j < FORMANT_TABLE_SIZE; j++ ) {
                FORMANT_TABLE[ j + i * FORMANT_TABLE_SIZE ] = generateFormant( -1 + j * coeff, T( i ));
            }
        }

        _sampleRate         = sampleRate;
        _halfSampleRateFrac = 1.f / ( _sampleRate * 0.5f );

        setVowel( aVowel );
        cacheDynamicsProcessing();

        // note: LFO is always "on" as its used by the formant synthesis
        // when we want the audible oscillation of vowels to stop, the LFO
        // depth is merely at 0

        lfo = new LFO( _sampleRate );
        setLFO( 0.f, 0.f );
    }

    template<typename T>
    FormantFilter<T>::~FormantFilter()
    {
        delete lfo;
    }

    /* public methods */
    template<typename T>
    T FormantFilter<T>::getVowel()
    {
        return ( T ) _vowel;
    }

    template<typename T>
    void FormantFilter<T>::setVowel( T aVowel )
    {
        _vowel = ( T ) aVowel;

        T tempRatio = _tempVowel / std::max( 0.000000001, _vowel );

        // in case FormantFilter is attached to oscillator, keep relative offset
        // of currently moving vowel in place
        _tempVowel = ( hasLFO ) ? _vowel * tempRatio : _vowel;

        cacheCoeffOffset();
        cacheLFO();
    }

    template<typename T>
    void FormantFilter<T>::setLFO( T LFORatePercentage, T LFODepth )
    {
        bool isLFOenabled = LFORatePercentage > 0.f;
        bool wasChanged   = hasLFO != isLFOenabled || _lfoDepth != LFODepth;

        hasLFO = isLFOenabled;

        lfo->setRate(
            VST::MIN_LFO_RATE() + (
                LFORatePercentage * ( VST::MAX_LFO_RATE() - VST::MIN_LFO_RATE() )
            )
        );

        if ( wasChanged ) {
            _lfoDepth = LFODepth;
            cacheLFO();
        }
    }

    template<typename T>
    void FormantFilter<T>::process( T* inBuffer, int bufferSize )
    {
        T lfoValue;
        T in, out, fp, ufp, phaseAcc, formant, carrier;

        for ( size_t i = 0; i < bufferSize; ++i )
        {
            in  = inBuffer[ i ];
            out = 0.0;

            // sweep the LFO

            lfoValue   = lfo->peek() * .5f  + .5f; // make waveform unipolar
            _tempVowel = std::min( _lfoMax, _lfoMin + _lfoRange * lfoValue ); // relative to LFO depth

            cacheCoeffOffset(); // ensure the appropriate coeff is used for the new _tempVowel value

            // calculate the phase for the formant synthesis and carrier

            fp  = 12 * powf( 2.0, 4 - 4 * _tempVowel );   // sweep
            // fp *= ( 1.0 + 0.01 * sinf( tmp * 0.0015 )); // optional vibrato (sinf value determines speed)
            ufp = 1.0 / fp;

            phaseAcc = fp * _halfSampleRateFrac;
            _phase  += phaseAcc;
            _phase  -= 2 * ( _phase > 1 );

            // calculate the coefficients

            for ( size_t j = 0; j < VOWEL_AMOUNT; ++j )
            {
                auto a = &A_COEFFICIENTS[ j ];
                auto f = &F_COEFFICIENTS[ j ];

                a->value += ATTENUATOR * ( a->coeffs[ _coeffOffset ] - a->value );
                f->value += ATTENUATOR * ( f->coeffs[ _coeffOffset ] - f->value );

                // apply formant onto the input signal

                T formant = APPLY_SYNTHESIS_SIGNAL ? getFormant( _phase, FORMANT_WIDTH_SCALE[ j ] * ufp ) : 1.0;
                T carrier = getCarrier( f->value * ufp, _phase );

                // the fp/fn coefficients stand for a -3dB/oct spectral envelope
                out += a->value * ( fp / f->value ) * in * formant * carrier;
            }

            // catch denormals

            undenormaliseT( out );

            // compress signal and write to output

            inBuffer[ i ] = compress( out );
        }
    }

    /* private methods */
    template<typename T>
    void FormantFilter<T>::cacheLFO()
    {
        // when LFO is "off" we mock a depth of 0. In reality we keep
        // the LFO moving to feed the carrier signal. The LFO won't
        // change the active vowel coefficient in this mode.

        _lfoRange = _vowel * ( hasLFO ? _lfoDepth : 0 );
        _lfoMax   = std::min( 1., _vowel + _lfoRange / 2. );
        _lfoMin   = std::max( 0., _vowel - _lfoRange / 2. );
    }

    template<typename T>
    T FormantFilter<T>::generateFormant( T phase, const T width )
    {
        int hmax    = int( 10 * width ) > FORMANT_TABLE_SIZE / 2 ? FORMANT_TABLE_SIZE / 2 : int( 10 * width );
        T jupe = 0.15f;

        T a = 0.5f;
        T phi = 0.0f;
        T hann, gaussian, harmonic;

        for ( size_t h = 1; h < hmax; h++ ) {
            phi     += VST::PI * phase;
            hann     = 0.5f + 0.5f * fast_cos( h * ( 1.0 / hmax ));
            gaussian = 0.85f * exp( -h * h / ( width * width ));
            harmonic = cosf( phi );
            a += hann * ( gaussian + jupe ) * harmonic;
        }
        return a;
    }

    template<typename T>
    T FormantFilter<T>::getFormant( T phase, T width )
    {
        width = ( width < 0 ) ? 0 : width > MAX_FORMANT_WIDTH - 2 ? MAX_FORMANT_WIDTH - 2 : width;
        T P = ( FORMANT_TABLE_SIZE - 1 ) * ( phase + 1 ) * 0.5f; // normalize phase

        // calculate the integer and fractional parts of the phase and width

        int phaseI    = ( int ) P;
        T phaseF = P - phaseI;

        int widthI    = ( int ) width;
        T widthF = width - widthI;

        int i00 = phaseI + FORMANT_TABLE_SIZE * widthI;
        int i10 = i00 + FORMANT_TABLE_SIZE;

        // bilinear interpolation of formant values
        return ( 1 - widthF ) *
            ( FORMANT_TABLE[ i00 ] + phaseF * ( FORMANT_TABLE[ i00 + 1 ] - FORMANT_TABLE[ i00 ])) +
                widthF * ( FORMANT_TABLE[ i10 ] + phaseF * ( FORMANT_TABLE[ i10 + 1 ] - FORMANT_TABLE[ i10 ]));
    }

    template<typename T>
    T FormantFilter<T>::getCarrier( const T position, const T phase )
    {
        T harmI = floor( position ); // integer and
        T harmF = position - harmI;  // fractional part of harmonic number

        // keep within -1 to +1 range
        T phi1 = fmodf( phase *  harmI        + 1 + 1000, 2.0 ) - 1.0;
        T phi2 = fmodf( phase * ( harmI + 1 ) + 1 + 1000, 2.0 ) - 1.0;

        // calculate the two carriers
        T carrier1 = fast_cos( phi1 );
        T carrier2 = fast_cos( phi2 );

        // return interpolation between the two carriers
        return carrier1 + harmF * ( carrier2 - carrier1 );
    }

    template<typename T>
    void FormantFilter<T>::cacheDynamicsProcessing()
    {
        _fullDynamicsProcessing = false;

        _dThreshold = pow( 10.0, ( 2.0 * DYNAMICS_THRESHOLD - 2.0 ));
        _dRatio     = 2.5 * DYNAMICS_RATIO - 0.5;

        if ( _dRatio > 1.0 ) {
            _dRatio = 1.f + 16.f * ( _dRatio - 1.f ) * ( _dRatio - 1.f );
            _fullDynamicsProcessing = true;
        }
        if ( _dRatio < 0.0 ) {
            _dRatio = 0.6f * _dRatio;
            _fullDynamicsProcessing = true;
        }
        _dTrim    = pow( 10.0,( 2.0 * DYNAMICS_LEVEL ));
        _dAttack  = pow( 10.0,( -0.002 - 2.0 * DYNAMICS_ATTACK ));
        _dRelease = pow( 10.0,( -2.0 - 3.0 * DYNAMICS_RELEASE ));
        
        // limiter
        
        if ( DYNAMICS_LIMITER_DYNAMICS_THRESHOLD > 0.98 ) {
            _dLimThreshold = 0.f;
        }
        else {
            _dLimThreshold = 0.99 * pow( 10.0, int( 30.0 * DYNAMICS_LIMITER_DYNAMICS_THRESHOLD - 20.0 ) / 20.f );
            _fullDynamicsProcessing = true;
        }
        
        // expander
        
        if ( DYNAMICS_GATE_DYNAMICS_THRESHOLD < 0.02 ) {
            _dExpThreshold = 0.f;
        }
        else {
            _dExpThreshold = pow( 10.f, ( 3.0 * DYNAMICS_GATE_DYNAMICS_THRESHOLD - 3.0 ));
            _fullDynamicsProcessing = true;
        }
        _dExpRatio   = 1.0 - pow( 10.f, ( -2.0 - 3.3 * DYNAMICS_GATE_DECAY ));
        _dGateAttack = pow( 10.0, (-0.002 - 3.0 * DYNAMICS_GATE_DYNAMICS_ATTACK ));
        
        if ( _dRatio < 0.0f && _dThreshold < 0.1f ) {
            _dRatio *= _dThreshold * 15.f;
        }
        _dDry   = 1.0f - DYNAMICS_MIX;
        _dTrim *= DYNAMICS_MIX;
    }
}