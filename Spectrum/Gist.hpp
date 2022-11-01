#pragma once

#define USE_FFTW

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cfloat>
#include <vector>
#include <numeric>

#include <fftw3.h>

//=======================================================================
/** A type indicator for different windows */
enum WindowType
{
    RectangularWindow,
    HanningWindow,
    HammingWindow,
    BlackmanWindow,
    TukeyWindow
};

//=======================================================================
/** A collection of different window functions */
template <class T>
class WindowFunctions
{
public:
    
    //=======================================================================
    /** @Returns a window with a specified type */
    static std::vector<T> createWindow (int numSamples, WindowType windowType);
    
    //=======================================================================
    /** @Returns a Hanning window */
    static std::vector<T> createHanningWindow (int numSamples);
    
    /** @Returns a Hamming window */
    static std::vector<T> createHammingWindow (int numSamples);
    
    /** @Returns a Blackman window */
    static std::vector<T> createBlackmanWindow (int numSamples);
    
    /** @Returns a Tukey window */
    static std::vector<T> createTukeyWindow (int numSamples, T alpha = 0.5);
    
    /** @Returns a Rectangular window */
    static std::vector<T> createRectangularWindow (int numSamples);
};


/** template class for calculating common time domain
 * audio features. Instantiations of the class should be
 * of either 'float' or 'double' types and no others */
template <class T>
class CoreTimeDomainFeatures
{
public:
    /** constructor */
    CoreTimeDomainFeatures();

    //===========================================================
    /** calculates the Root Mean Square (RMS) of an audio buffer
     * in vector format
     * @param buffer a time domain buffer containing audio samples
     * @returns the RMS value
     */
    T rootMeanSquare (const std::vector<T>& buffer);

    //===========================================================
    /** calculates the peak energy (max absolute value) in a time
     * domain audio signal buffer in vector format
     * @param buffer a time domain buffer containing audio samples
     * @returns the peak energy value
     */
    T peakEnergy (const std::vector<T>& buffer);

    //===========================================================
    /** calculates the zero crossing rate of a time domain audio signal buffer
     * @param buffer a time domain buffer containing audio samples
     * @returns the zero crossing rate
     */
    T zeroCrossingRate (const std::vector<T>& buffer);
};


/** template class for calculating common frequency domain
 * audio features. Instantiations of the class should be
 * of either 'float' or 'double' types and no others */
template <class T>
class CoreFrequencyDomainFeatures
{
public:
    /** constructor */
    CoreFrequencyDomainFeatures();

    //===========================================================
    /** calculates the spectral centroid given the first half of the magnitude spectrum
     of an audio signal. Do not pass the whole (i.e. mirrored) magnitude spectrum into
     this function or you will always get the middle index as the spectral centroid 
     @param magnitudeSpectrum the first half of the magnitude spectrum (i.e. not mirrored)
     @returns the spectral centroid as an index value
     */
    T spectralCentroid (const std::vector<T>& magnitudeSpectrum);

    //===========================================================
    /** calculates the spectral flatness given the first half of the magnitude spectrum
     of an audio signal.
     @param magnitudeSpectrum the first half of the magnitude spectrum (i.e. not mirrored)
     @returns the spectral flatness
     */
    T spectralFlatness (const std::vector<T>& magnitudeSpectrum);

    //===========================================================
    /** calculates the spectral crest given the first half of the magnitude spectrum
     of an audio signal.
     @param magnitudeSpectrum the first half of the magnitude spectrum (i.e. not mirrored)
     @returns the spectral crest
     */
    T spectralCrest (const std::vector<T>& magnitudeSpectrum);
    
    //===========================================================
    /** calculates the spectral rolloff given the first half of the magnitude spectrum
     of an audio signal.
     @param magnitudeSpectrum the first half of the magnitude spectrum (i.e. not mirrored)
     @param percentile the rolloff threshold
     @returns the spectral rolloff
     */
    T spectralRolloff (const std::vector<T>& magnitudeSpectrum, T percentile = 0.85);
    
    //===========================================================
    /** calculates the spectral kurtosis given the first half of the magnitude spectrum
     of an audio signal.
     @param magnitudeSpectrum the first half of the magnitude spectrum (i.e. not mirrored)
     @returns the spectral kurtosis
     */
    T spectralKurtosis (const std::vector<T>& magnitudeSpectrum);
    
    
};


//=======================================================================
/** Template class for calculating Mel Frequency Cepstral Coefficients
 * Instantiations of the class should be of either 'float' or
 * 'double' types and no others */
template <class T>
class MFCC
{
public:
    
    //=======================================================================
    /** Constructor */
    MFCC (int frameSize_, int samplingFrequency_);

    //=======================================================================
    /** Set the number of coefficients to calculate
     * @param numCoefficients_ the number of coefficients to calculate 
     */
    void setNumCoefficients (int numCoefficients_);

    /** Set the frame size - N.B. this will be twice the length of the magnitude spectrum passed to calculateMFCC()
     * @param frameSize_ the frame size
     */
    void setFrameSize (int frameSize_);

    /** Set the sampling frequency
     * @param samplingFrequency_ the sampling frequency in hz
     */
    void setSamplingFrequency (int samplingFrequency_);

    //=======================================================================
    /** Calculates the Mel Frequency Cepstral Coefficients from the magnitude spectrum of a signal. The result
     * is stored in the public vector MFCCs.
     * 
     * Note that the magnitude spectrum passed to the function is not the full mirrored magnitude spectrum, but 
     * only the first half. The frame size passed to the constructor should be twice the length of the magnitude spectrum.
     * @param magnitudeSpectrum the magnitude spectrum in vector format
     */
    void calculateMelFrequencyCepstralCoefficients (const std::vector<T>& magnitudeSpectrum);

    /** Calculates the magnitude spectrum on a Mel scale. The result is stored in
     * the public vector melSpectrum.
     */
    void calculateMelFrequencySpectrum (const std::vector<T>& magnitudeSpectrum);

    //=======================================================================
    /** a vector to hold the mel spectrum once it has been computed */
    std::vector<T> melSpectrum;
    
    /** a vector to hold the MFCCs once they have been computed */
    std::vector<T> MFCCs;
    
private:
    /** Initialises the parts of the algorithm dependent on frame size, sampling frequency
     * and the number of coefficients
     */
    void initialise();

    /** Calculates the discrete cosine transform (version 2) of an input signal, performing it in place
     * (i.e. the result is stored in the vector passed to the function)
     *
     * @param inputSignal a vector containing the input signal
     * @param numElements the number of elements in the input signal
     */
    void discreteCosineTransform (std::vector<T>& inputSignal, const std::size_t numElements);

    /** Calculates the triangular filters used in the algorithm. These will be different depending
     * upon the frame size, sampling frequency and number of coefficients and so should be re-calculated
     * should any of those parameters change.
     */
    void calculateMelFilterBank();

    /** Calculates mel from frequency
     * @param frequency the frequency in Hz
     * @returns the equivalent mel value
     */
    T frequencyToMel (T frequency);

    /** the sampling frequency in Hz */
    int samplingFrequency;

    /** the number of MFCCs to calculate */
    int numCoefficents;

    /** the audio frame size */
    int frameSize;

    /** the magnitude spectrum size (this will be half the frame size) */
    int magnitudeSpectrumSize;

    /** the minimum frequency to be used in the calculation of MFCCs */
    T minFrequency;

    /** the maximum frequency to be used in the calculation of MFCCs */
    T maxFrequency;

    /** a vector of vectors to hold the values of the triangular filters */
    std::vector<std::vector<T> > filterBank;
    std::vector<T> dctSignal;
};


/** template class for calculating onset detection functions
 * Instantiations of the class should be of either 'float' or 
 * 'double' types and no others */
template <class T>
class OnsetDetectionFunction
{
public:
    //===========================================================
    /** constructor */
    OnsetDetectionFunction (int frameSize);

    //===========================================================
    /** Sets the frame size of internal buffers. Assumes all magnitude
     * spectra are passed as the first half (i.e. not mirrored)
     * @param frameSize the frame size
     */
    void setFrameSize (int frameSize);

    //===========================================================
    /** calculates the energy difference onset detection function
     * @param buffer the time domain audio frame containing audio samples
     * @returns the energy difference onset detection function sample for the frame
     */
    T energyDifference (const std::vector<T>& buffer);

    //===========================================================
    /** calculates the spectral difference between the current magnitude
     * spectrum and the previous magnitude spectrum
     * @param magnitudeSpectrum a vector containing the magnitude spectrum
     * @returns the spectral difference onset detection function sample
     */
    T spectralDifference (const std::vector<T>& magnitudeSpectrum);

    //===========================================================
    /** calculates the half wave rectified spectral difference between the 
     * current magnitude spectrum and the previous magnitude spectrum
     * @param magnitudeSpectrum a vector containing the magnitude spectrum
     * @returns the HWR spectral difference onset detection function sample
     */
    T spectralDifferenceHWR (const std::vector<T>& magnitudeSpectrum);

    //===========================================================
    /** calculates the complex spectral difference from the real and imaginary parts 
     * of the FFT
     * @param fftReal a vector containing the real part of the FFT
     * @param fftImag a vector containing the imaginary part of the FFT
     * @returns the complex spectral difference onset detection function sample
     */
    T complexSpectralDifference (const std::vector<T>& fftReal, const std::vector<T>& fftImag);

    //===========================================================
    /** calculates the high frequency content onset detection function from
     * the magnitude spectrum
     * @param magnitudeSpectrum a vector containing the magnitude spectrum
     * @returns the high frequency content onset detection function sample
     */
    T highFrequencyContent (const std::vector<T>& magnitudeSpectrum);

private:
    /** maps phasein into the [-pi:pi] range */
    T princarg (T phaseVal);

    //===========================================================
    /** holds the previous energy sum for the energy difference onset detection function */
    T prevEnergySum;

    /** a vector containing the previous magnitude spectrum passed to the
     last spectral difference call */
    std::vector<T> prevMagnitudeSpectrum_spectralDifference;

    /** a vector containing the previous magnitude spectrum passed to the
     last spectral difference (half wave rectified) call */
    std::vector<T> prevMagnitudeSpectrum_spectralDifferenceHWR;

    /** a vector containing the previous phase spectrum passed to the
     last complex spectral difference call */
    std::vector<T> prevPhaseSpectrum_complexSpectralDifference;

    /** a vector containing the second previous phase spectrum passed to the
     last complex spectral difference call */
    std::vector<T> prevPhaseSpectrum2_complexSpectralDifference;

    /** a vector containing the previous magnitude spectrum passed to the
     last complex spectral difference call */
    std::vector<T> prevMagnitudeSpectrum_complexSpectralDifference;
};


//===========================================================
/** template class for the pitch detection algorithm Yin.
 * Instantiations of the class should be of either 'float' or
 * 'double' types and no others */
template <class T>
class Yin
{
    
public:
    
    //===========================================================
    /** constructor
     * @param samplingFrequency the sampling frequency
     */
    Yin (int samplingFrequency);
    
    //===========================================================
    /** sets the sampling frequency used to calculate pitch values
     * @param samplingFrequency the sampling frequency
     */
    void setSamplingFrequency (int samplingFrequency);
    
    /** sets the maximum frequency that the algorithm will return
     * @param maxFreq the maximum frequency
     */
    void setMaxFrequency (T maxFreq);
    
    //===========================================================
    /** @returns the maximum frequency that the algorithm will return */
    T getMaxFrequency()
    {
        return ((T) fs) / ((T) minPeriod);
    }
    
    //===========================================================
    /** calculates the pitch of the audio frame passed to it
     * @param frame an audio frame stored in a vector
     * @returns the estimated pitch in Hz
     */
    T pitchYin (const std::vector<T>& frame);
        
private:
    
    //===========================================================
    /** converts periods to pitch in Hz
     * @param period the period in audio samples
     * @returns the pitch in Hz
     */
    T periodToPitch (T period);

    /** this method searches the previous period estimate for a 
     * minimum and if it finds one, it is used, for the sake of consistency, 
     * even if it is not the optimal choice 
     * @param delta the cumulative mean normalised difference function
     * @returns the period found if a minimum is found, or -1 if not
     */
    long searchForOtherRecentMinima (const std::vector<T>& delta);
    
    /** interpolates a period estimate using parabolic interpolation
     * @param period the period estimate
     * @param y1 the value of the cumulative mean normalised difference function at (period-1)
     * @param y2 the value of the cumulative mean normalised difference function at (period)
     * @param y3 the value of the cumulative mean normalised difference function at (period+1)
     * @returns the interpolated period
     */
    T parabolicInterpolation (unsigned long period,T y1,T y2,T y3);
    
    /** calculates the period candidate from the cumulative mean normalised difference function 
     * @param delta the cumulative mean normalised difference function
     * @returns the period estimate
     */
    unsigned long getPeriodCandidate (const std::vector<T>& delta);
    
    /** this calculates steps 1, 2 and 3 of the Yin algorithm as set out in
     * the paper (de Cheveigné and Kawahara,2002).
     * @param frame a vector containing the audio frame to be procesed
     */
    void cumulativeMeanNormalisedDifferenceFunction (const std::vector<T>& frame);
    
	T round (T val)
	{
		return floor(val + 0.5);
	}

    /** the previous period estimate found by the algorithm last time it was called - initially set to 1.0 */
    T prevPeriodEstimate;
    
    /** the sampling frequency */
    int fs;
    
    /** the minimum period the algorithm will look for. this is set indirectly by setMaxFrequency() */
    int minPeriod;
    
    std::vector<T> delta;
};


//=======================================================================
/** Class for all performing all Gist audio analyses */
template <class T>
class Gist
{
public:
    
    //=======================================================================
    /** Constructor
     * @param audioFrameSize the input audio frame size
     * @param fs the input audio sample rate
     * @param windowType the type of window function to use
     */
    Gist (int audioFrameSize, int fs, WindowType windowType = HanningWindow);

    /** Destructor */
    ~Gist();

    //=======================================================================
    /** Set the audio frame size.
     * @param frameSize_ the frame size to use
     */
    void setAudioFrameSize (int audioFrameSize);

    /** Set the sampling frequency of input audio 
     * @param fs the sampling frequency 
     */
    void setSamplingFrequency (int fs);
    
    //=======================================================================
    /** @Returns the audio frame size currently being used */
    int getAudioFrameSize();
    
    /** @Returns the audio sampling frequency being used for analysis */
    int getSamplingFrequency();

    //=======================================================================
    /** Process an audio frame
     * @param audioFrame a vector containing audio samples
     */
    void processAudioFrame (const std::vector<T>& audioFrame);

    /** Process an audio frame
     * @param frame a pointer to an array containing the audio frame
     * @param numSamples the number of samples in the audio frame
     */
    void processAudioFrame (const T* frame, int numSamples);

    /** Gist automatically calculates the magnitude spectrum when processAudioFrame() is called, this function returns it.
     @returns the current magnitude spectrum */
    const std::vector<T>& getMagnitudeSpectrum();

    //================= CORE TIME DOMAIN FEATURES =================

    /** @Returns the root mean square (RMS) of the currently stored audio frame */
    T rootMeanSquare();

    /** @Returns the peak energy of the currently stored audio frame */
    T peakEnergy();

    /** @Returns the zero crossing rate of the currently stored audio frame */
    T zeroCrossingRate();

    //=============== CORE FREQUENCY DOMAIN FEATURES ==============

    /** @Returns the spectral centroid from the magnitude spectrum */
    T spectralCentroid();

    /** @Returns the spectral crest */
    T spectralCrest();

    /** @Returns the spectral flatness of the magnitude spectrum */
    T spectralFlatness();
    
    /** @Returns the spectral rolloff of the magnitude spectrum */
    T spectralRolloff();
    
    /** @Returns the spectral kurtosis of the magnitude spectrum */
    T spectralKurtosis();

    //================= ONSET DETECTION FUNCTIONS =================

    /** @Returns the energy difference onset detection function sample for the magnitude spectrum frame */
    T energyDifference();

    /** @Returns the spectral difference onset detection function sample for the magnitude spectrum frame */
    T spectralDifference();

    /** @Returns the half wave rectified complex spectral difference onset detection function sample for the magnitude spectrum frame */
    T spectralDifferenceHWR();

    /** @Returns the complex spectral difference onset detection function sample for the magnitude spectrum frame */
    T complexSpectralDifference();

    /** @Returns the high frequency content onset detection function sample for the magnitude spectrum frame */
    T highFrequencyContent();

    //=========================== PITCH ============================

    /** @Returns a monophonic pitch estimate according to the Yin algorithm */
    T pitch();

    //=========================== MFCCs =============================
    
    /** Calculates the Mel Frequency Spectrum */
    const std::vector<T>& getMelFrequencySpectrum();

    /** Calculates the Mel-frequency Cepstral Coefficients */
    const std::vector<T>& getMelFrequencyCepstralCoefficients();
    
private:
    //=======================================================================

    /** Configure the FFT implementation given the audio frame size) */
    void configureFFT();

    /** Free all FFT-related data */
    void freeFFT();

    /** perform the FFT on the current audio frame */
    void performFFT();

    //=======================================================================

#ifdef USE_FFTW
    fftw_plan p;          /**< fftw plan */
    fftw_complex* fftIn;  /**< to hold complex fft values for input */
    fftw_complex* fftOut; /**< to hold complex fft values for output */
#endif

#ifdef USE_KISS_FFT
    kiss_fft_cfg cfg;     /**< Kiss FFT configuration */
    kiss_fft_cpx* fftIn;  /**< FFT input samples, in complex form */
    kiss_fft_cpx* fftOut; /**< FFT output samples, in complex form */
#endif
    
#ifdef USE_ACCELERATE_FFT
    AccelerateFFT<T> accelerateFFT;
#endif

    int frameSize;                    /**< The audio frame size */
    int samplingFrequency;            /**< The sampling frequency used for analysis */
    WindowType windowType;            /**< The window type used in FFT analysis */

    std::vector<T> audioFrame;        /**< The current audio frame */
    std::vector<T> windowFunction;    /**< The window function used in FFT processing */
    std::vector<T> fftReal;           /**< The real part of the FFT for the current audio frame */
    std::vector<T> fftImag;           /**< The imaginary part of the FFT for the current audio frame */
    std::vector<T> magnitudeSpectrum; /**< The magnitude spectrum of the current audio frame */

    bool fftConfigured;

    /** object to compute core time domain features */
    CoreTimeDomainFeatures<T> coreTimeDomainFeatures;

    /** object to compute core frequency domain features */
    CoreFrequencyDomainFeatures<T> coreFrequencyDomainFeatures;

    /** object to compute onset detection functions */
    OnsetDetectionFunction<T> onsetDetectionFunction;

    /** object to compute pitch estimates via the Yin algorithm */
    Yin<T> yin;

    /** object to compute MFCCs and mel-frequency specta */
    MFCC<T> mfcc;
};


//===========================================================
template class CoreFrequencyDomainFeatures<float>;
template class CoreFrequencyDomainFeatures<double>;


//===========================================================
template class CoreTimeDomainFeatures<float>;
template class CoreTimeDomainFeatures<double>;

//===========================================================
template class Gist<float>;
template class Gist<double>;

//===========================================================
template class MFCC<float>;
template class MFCC<double>;

//===========================================================
template class OnsetDetectionFunction<float>;
template class OnsetDetectionFunction<double>;

//===========================================================
template class WindowFunctions<float>;
template class WindowFunctions<double>;

//===========================================================
template class Yin<float>;
template class Yin<double>;

//===========================================================
template <class T>
CoreFrequencyDomainFeatures<T>::CoreFrequencyDomainFeatures()
{
}

//===========================================================
template <class T>
T CoreFrequencyDomainFeatures<T>::spectralCentroid (const std::vector<T>& magnitudeSpectrum)
{
    // to hold sum of amplitudes
    T sumAmplitudes = 0.0;

    // to hold sum of weighted amplitudes
    T sumWeightedAmplitudes = 0.0;

    // for each bin in the first half of the magnitude spectrum
    for (size_t i = 0; i < magnitudeSpectrum.size(); i++)
    {
        // sum amplitudes
        sumAmplitudes += magnitudeSpectrum[i];

        // sum amplitudes weighted by the bin number
        sumWeightedAmplitudes += magnitudeSpectrum[i] * i;
    }

    // if the sum of amplitudes is larger than zero (it should be if the buffer wasn't
    // all zeros)
    if (sumAmplitudes > 0)
    {
        // the spectral centroid is the sum of weighted amplitudes divided by the sum of amplitdues
        return sumWeightedAmplitudes / sumAmplitudes;
    }
    else // to be safe just return zero
    {
        return 0.0;
    }
}

//===========================================================
template <class T>
T CoreFrequencyDomainFeatures<T>::spectralFlatness (const std::vector<T>& magnitudeSpectrum)
{
    double sumVal = 0.0;
    double logSumVal = 0.0;
    double N = (double)magnitudeSpectrum.size();

    T flatness;

    for (size_t i = 0; i < magnitudeSpectrum.size(); i++)
    {
        // add one to stop zero values making it always zero
        double v = (double)(1 + magnitudeSpectrum[i]);

        sumVal += v;
        logSumVal += log (v);
    }

    sumVal = sumVal / N;
    logSumVal = logSumVal / N;

    if (sumVal > 0)
        flatness = (T)(exp (logSumVal) / sumVal);
    else
        flatness = 0.0;

    return flatness;
}

//===========================================================
template <class T>
T CoreFrequencyDomainFeatures<T>::spectralCrest (const std::vector<T>& magnitudeSpectrum)
{
    T sumVal = 0.0;
    T maxVal = 0.0;
    T N = (T)magnitudeSpectrum.size();

    for (size_t i = 0; i < magnitudeSpectrum.size(); i++)
    {
        T v = magnitudeSpectrum[i] * magnitudeSpectrum[i];
        sumVal += v;

        if (v > maxVal)
            maxVal = v;
    }

    T spectralCrest;

    if (sumVal > 0)
    {
        T meanVal = sumVal / N;
        spectralCrest = maxVal / meanVal;
    }
    else
    {
        // this is a ratio so we return 1.0 if the buffer is just zeros
        spectralCrest = 1.0;
    }

    return spectralCrest;
}

//===========================================================
template <class T>
T CoreFrequencyDomainFeatures<T>::spectralRolloff (const std::vector<T>& magnitudeSpectrum, T percentile)
{
    T sumOfMagnitudeSpectrum = std::accumulate (magnitudeSpectrum.begin(), magnitudeSpectrum.end(), 0);
    T threshold = sumOfMagnitudeSpectrum * percentile;
    
    T cumulativeSum = 0;
    int index = 0;
    
    for (size_t i = 0; i < magnitudeSpectrum.size(); i++)
    {
        cumulativeSum += magnitudeSpectrum[i];
        
        if (cumulativeSum > threshold)
        {
            index = static_cast<int> (i);
            break;
        }
    }
    
    T spectralRolloff = ((T)index) / ((T)magnitudeSpectrum.size());
    
    return spectralRolloff;
}

//===========================================================
template <class T>
T CoreFrequencyDomainFeatures<T>::spectralKurtosis (const std::vector<T>& magnitudeSpectrum)
{
    // https://en.wikipedia.org/wiki/Kurtosis#Sample_kurtosis
    
    T sumOfMagnitudeSpectrum = std::accumulate (magnitudeSpectrum.begin(), magnitudeSpectrum.end(), 0);
    
    T mean = sumOfMagnitudeSpectrum / (T)magnitudeSpectrum.size();
    
    T moment2 = 0;
    T moment4 = 0;
    
    for (size_t i = 0; i < magnitudeSpectrum.size(); i++)
    {
        T difference = magnitudeSpectrum[i] - mean;
        T squaredDifference = difference*difference;
        
        moment2 += squaredDifference;
        moment4 += squaredDifference*squaredDifference;
    }
    
    moment2 = moment2 / (T)magnitudeSpectrum.size();
    moment4 = moment4 / (T)magnitudeSpectrum.size();
        
    if (moment2 == 0)
    {
        return -3.;
    }
    else
    {
        return (moment4 / (moment2*moment2)) - 3.;
    }
}

//===========================================================
template <class T>
CoreTimeDomainFeatures<T>::CoreTimeDomainFeatures()
{
}

//===========================================================
template <class T>
T CoreTimeDomainFeatures<T>::rootMeanSquare (const std::vector<T>& buffer)
{
    // create variable to hold the sum
    T sum = 0;

    // sum the squared samples
    for (size_t i = 0; i < buffer.size(); i++)
    {
        sum += pow (buffer[i], 2);
    }

    // return the square root of the mean of squared samples
    return sqrt (sum / ((T)buffer.size()));
}

//===========================================================
template <class T>
T CoreTimeDomainFeatures<T>::peakEnergy (const std::vector<T>& buffer)
{
    // create variable with very small value to hold the peak value
    T peak = -10000.0;

    // for each audio sample
    for (size_t i = 0; i < buffer.size(); i++)
    {
        // store the absolute value of the sample
        T absSample = fabs (buffer[i]);

        // if the absolute value is larger than the peak
        if (absSample > peak)
        {
            // the peak takes on the sample value
            peak = absSample;
        }
    }

    // return the peak value
    return peak;
}

//===========================================================
template <class T>
T CoreTimeDomainFeatures<T>::zeroCrossingRate (const std::vector<T>& buffer)
{
    // create a variable to hold the zero crossing rate
    T zcr = 0;

    // for each audio sample, starting from the second one
    for (size_t i = 1; i < buffer.size(); i++)
    {
        // initialise two booleans indicating whether or not
        // the current and previous sample are positive
        bool current = (buffer[i] > 0);
        bool previous = (buffer[i - 1] > 0);

        // if the sign is different
        if (current != previous)
        {
            // add one to the zero crossing rate
            zcr = zcr + 1.0;
        }
    }

    // return the zero crossing rate
    return zcr;
}


//=======================================================================
template <class T>
Gist<T>::Gist (int audioFrameSize, int fs, WindowType windowType_)
 :  windowType (windowType_),
    fftConfigured (false),
    onsetDetectionFunction (audioFrameSize),
    yin (fs),
    mfcc (audioFrameSize, fs)
{
    samplingFrequency = fs;
    setAudioFrameSize (audioFrameSize);
}

//=======================================================================
template <class T>
Gist<T>::~Gist()
{
    if (fftConfigured)
    {
        freeFFT();
    }
}

//=======================================================================
template <class T>
void Gist<T>::setAudioFrameSize (int audioFrameSize)
{
    frameSize = audioFrameSize;
    
    audioFrame.resize (frameSize);
    
    windowFunction = WindowFunctions<T>::createWindow (audioFrameSize, windowType);
        
    fftReal.resize (frameSize);
    fftImag.resize (frameSize);
    magnitudeSpectrum.resize (frameSize / 2);
    
    configureFFT();
    
    onsetDetectionFunction.setFrameSize (frameSize);
    mfcc.setFrameSize (frameSize);
}

//=======================================================================
template <class T>
void Gist<T>::setSamplingFrequency (int fs)
{
    samplingFrequency = fs;
    yin.setSamplingFrequency (samplingFrequency);
    mfcc.setSamplingFrequency (samplingFrequency);
}

//=======================================================================
template <class T>
int Gist<T>::getAudioFrameSize()
{
    return frameSize;
}

//=======================================================================
template <class T>
int Gist<T>::getSamplingFrequency()
{
    return samplingFrequency;
}

//=======================================================================
template <class T>
void Gist<T>::processAudioFrame (const std::vector<T>& a)
{
    // you are passing an audio frame of a different size to the
    // audio frame size setup in Gist
    assert (a.size() == audioFrame.size());
    
    std::copy (a.begin(), a.end(), audioFrame.begin());
    performFFT();
}

//=======================================================================
template <class T>
void Gist<T>::processAudioFrame (const T* frame, int numSamples)
{
    // you are passing an audio frame of a different size to the
    // audio frame size setup in Gist
    assert (static_cast<size_t> (numSamples) == audioFrame.size());
    
    for (size_t i = 0; i < audioFrame.size(); i++)
        audioFrame[i] = frame[i];
    
    performFFT();
}

//=======================================================================
template <class T>
const std::vector<T>& Gist<T>::getMagnitudeSpectrum()
{
    return magnitudeSpectrum;
}

//=======================================================================
template <class T>
T Gist<T>::rootMeanSquare()
{
    return coreTimeDomainFeatures.rootMeanSquare (audioFrame);
}

//=======================================================================
template <class T>
T Gist<T>::peakEnergy()
{
    return coreTimeDomainFeatures.peakEnergy (audioFrame);
}

//=======================================================================
template <class T>
T Gist<T>::zeroCrossingRate()
{
    return coreTimeDomainFeatures.zeroCrossingRate (audioFrame);
}

//=======================================================================
template <class T>
T Gist<T>::spectralCentroid()
{
    return coreFrequencyDomainFeatures.spectralCentroid (magnitudeSpectrum);
}

//=======================================================================
template <class T>
T Gist<T>::spectralCrest()
{
    return coreFrequencyDomainFeatures.spectralCrest (magnitudeSpectrum);
}

//=======================================================================
template <class T>
T Gist<T>::spectralFlatness()
{
    return coreFrequencyDomainFeatures.spectralFlatness (magnitudeSpectrum);
}

//=======================================================================
template <class T>
T Gist<T>::spectralRolloff()
{
    return coreFrequencyDomainFeatures.spectralRolloff (magnitudeSpectrum);
}

//=======================================================================
template <class T>
T Gist<T>::spectralKurtosis()
{
    return coreFrequencyDomainFeatures.spectralKurtosis (magnitudeSpectrum);
}

//=======================================================================
template <class T>
T Gist<T>::energyDifference()
{
    return onsetDetectionFunction.energyDifference (audioFrame);
}

//=======================================================================
template <class T>
T Gist<T>::spectralDifference()
{
    return onsetDetectionFunction.spectralDifference (magnitudeSpectrum);
}

//=======================================================================
template <class T>
T Gist<T>::spectralDifferenceHWR()
{
    return onsetDetectionFunction.spectralDifferenceHWR (magnitudeSpectrum);
}

//=======================================================================
template <class T>
T Gist<T>::complexSpectralDifference()
{
    return onsetDetectionFunction.complexSpectralDifference (fftReal, fftImag);
}

//=======================================================================
template <class T>
T Gist<T>::highFrequencyContent()
{
    return onsetDetectionFunction.highFrequencyContent (magnitudeSpectrum);
}

//=======================================================================
template <class T>
T Gist<T>::pitch()
{
    return yin.pitchYin (audioFrame);
}

//=======================================================================
template <class T>
const std::vector<T>& Gist<T>::getMelFrequencySpectrum()
{
    mfcc.calculateMelFrequencySpectrum (magnitudeSpectrum);
    return mfcc.melSpectrum;
}

//=======================================================================
template <class T>
const std::vector<T>& Gist<T>::getMelFrequencyCepstralCoefficients()
{
    mfcc.calculateMelFrequencyCepstralCoefficients (magnitudeSpectrum);
    return mfcc.MFCCs;
}

//=======================================================================
template <class T>
void Gist<T>::configureFFT()
{
    if (fftConfigured)
    {
        freeFFT();
    }
    
#ifdef USE_FFTW
    // ------------------------------------------------------
    // initialise the fft time and frequency domain audio frame arrays
    fftIn = (fftw_complex*)fftw_malloc (sizeof (fftw_complex) * frameSize);  // complex array to hold fft data
    fftOut = (fftw_complex*)fftw_malloc (sizeof (fftw_complex) * frameSize); // complex array to hold fft data
    
    // FFT plan initialisation
    p = fftw_plan_dft_1d (frameSize, fftIn, fftOut, FFTW_FORWARD, FFTW_ESTIMATE);
#endif /* END USE_FFTW */
    
#ifdef USE_KISS_FFT
    // ------------------------------------------------------
    // initialise the fft time and frequency domain audio frame arrays
    fftIn = new kiss_fft_cpx[frameSize];
    fftOut = new kiss_fft_cpx[frameSize];
    cfg = kiss_fft_alloc (frameSize, 0, 0, 0);
#endif /* END USE_KISS_FFT */
    
#ifdef USE_ACCELERATE_FFT
    accelerateFFT.setAudioFrameSize (frameSize);
#endif
    
    fftConfigured = true;
}

//=======================================================================
template <class T>
void Gist<T>::freeFFT()
{
#ifdef USE_FFTW
    // destroy fft plan
    fftw_destroy_plan (p);
    
    fftw_free (fftIn);
    fftw_free (fftOut);
#endif
    
#ifdef USE_KISS_FFT
    // free the Kiss FFT configuration
    free (cfg);
    
    delete[] fftIn;
    delete[] fftOut;
#endif
}

//=======================================================================
template <class T>
void Gist<T>::performFFT()
{
#ifdef USE_FFTW
    // copy samples from audio frame
    for (int i = 0; i < frameSize; i++)
    {
        fftIn[i][0] = (double)(audioFrame[i] * windowFunction[i]);
        fftIn[i][1] = (double)0.0;
    }
    
    // perform the FFT
    fftw_execute (p);
    
    // store real and imaginary parts of FFT
    for (int i = 0; i < frameSize; i++)
    {
        fftReal[i] = (T)fftOut[i][0];
        fftImag[i] = (T)fftOut[i][1];
    }
#endif
    
#ifdef USE_KISS_FFT
    for (int i = 0; i < frameSize; i++)
    {
        fftIn[i].r = (double)(audioFrame[i] * windowFunction[i]);
        fftIn[i].i = 0.0;
    }
    
    // execute kiss fft
    kiss_fft (cfg, fftIn, fftOut);
    
    // store real and imaginary parts of FFT
    for (int i = 0; i < frameSize; i++)
    {
        fftReal[i] = (T)fftOut[i].r;
        fftImag[i] = (T)fftOut[i].i;
    }
#endif
    
#ifdef USE_ACCELERATE_FFT
    
    T inputFrame[frameSize];
    T outputReal[frameSize];
    T outputImag[frameSize];
    
    for (int i = 0; i < frameSize; i++)
    {
        inputFrame[i] = audioFrame[i] * windowFunction[i];
    }
    
    accelerateFFT.performFFT (inputFrame, outputReal, outputImag);
    
    for (int i = 0; i < frameSize; i++)
    {
        fftReal[i] = outputReal[i];
        fftImag[i] = outputImag[i];
    }
    
#endif
    
    // calculate the magnitude spectrum
    for (int i = 0; i < frameSize / 2; i++)
    {
        magnitudeSpectrum[i] = sqrt ((fftReal[i] * fftReal[i]) + (fftImag[i] * fftImag[i]));
    }
}

//==================================================================
template <class T>
MFCC<T>::MFCC (int frameSize_, int samplingFrequency_)
{
    numCoefficents = 13;
    frameSize = frameSize_;
    samplingFrequency = samplingFrequency_;

    initialise();
}

//==================================================================
template <class T>
void MFCC<T>::setNumCoefficients (int numCoefficients_)
{
    numCoefficents = numCoefficients_;
    initialise();
}

//==================================================================
template <class T>
void MFCC<T>::setFrameSize (int frameSize_)
{
    frameSize = frameSize_;
    initialise();
}

//==================================================================
template <class T>
void MFCC<T>::setSamplingFrequency (int samplingFrequency_)
{
    samplingFrequency = samplingFrequency_;
    initialise();
}

//==================================================================
template <class T>
void MFCC<T>::calculateMelFrequencyCepstralCoefficients (const std::vector<T>& magnitudeSpectrum)
{
    calculateMelFrequencySpectrum (magnitudeSpectrum);
    
    for (size_t i = 0; i < melSpectrum.size(); i++)
        MFCCs[i] = log (melSpectrum[i] + (T)FLT_MIN);

    discreteCosineTransform (MFCCs, MFCCs.size());
}

//==================================================================
template <class T>
void MFCC<T>::calculateMelFrequencySpectrum (const std::vector<T>& magnitudeSpectrum)
{
    for (int i = 0; i < numCoefficents; i++)
    {
        double coeff = 0;
        
        for (size_t j = 0; j < magnitudeSpectrum.size(); j++)
            coeff += (T)((magnitudeSpectrum[j] * magnitudeSpectrum[j]) * filterBank[i][j]);
        
        melSpectrum[i] = coeff;
    }
}

//==================================================================
template <class T>
void MFCC<T>::initialise()
{
    magnitudeSpectrumSize = frameSize / 2;
    minFrequency = 0;
    maxFrequency = samplingFrequency / 2;

    melSpectrum.resize (numCoefficents);
    MFCCs.resize (numCoefficents);
    dctSignal.resize (numCoefficents);
    
    calculateMelFilterBank();
}

//==================================================================
template <class T>
void MFCC<T>::discreteCosineTransform (std::vector<T>& inputSignal, const std::size_t numElements)
{
    // the input signal must have the number of elements specified in the numElements variable
    assert (inputSignal.size() == numElements);
    
    // this should already be the case - sanity check
    assert (dctSignal.size() == numElements);
        
    for (size_t i = 0; i < numElements; i++)
        dctSignal[i] = inputSignal[i];
    
    T N = (T)numElements;
    T piOverN = M_PI / N;

    for (size_t k = 0; k < numElements; k++)
    {
        T sum = 0;
        T kVal = (T)k;

        for (size_t n = 0; n < numElements; n++)
        {
            T tmp = piOverN * (((T)n) + 0.5) * kVal;
            sum += dctSignal[n] * cos (tmp);
        }

        inputSignal[k] = (T)(2 * sum);
    }
}

//==================================================================
template <class T>
void MFCC<T>::calculateMelFilterBank()
{
    int maxMel = floor (frequencyToMel (maxFrequency));
    int minMel = floor (frequencyToMel (minFrequency));

    filterBank.resize (numCoefficents);

    for (int i = 0; i < numCoefficents; i++)
    {
        filterBank[i].resize (magnitudeSpectrumSize);

        for (int j = 0; j < magnitudeSpectrumSize; j++)
            filterBank[i][j] = 0.0;
    }

    std::vector<int> centreIndices;

    for (int i = 0; i < numCoefficents + 2; i++)
    {
        double f = i * (maxMel - minMel) / (numCoefficents + 1) + minMel;

        double tmp = log (1 + 1000.0 / 700.0) / 1000.0;
        tmp = (exp (f * tmp) - 1) / (samplingFrequency / 2);
        tmp = 0.5 + 700 * ((double)magnitudeSpectrumSize) * tmp;
        tmp = floor (tmp);

        int centreIndex = (int)tmp;
        centreIndices.push_back (centreIndex);
    }

    for (int i = 0; i < numCoefficents; i++)
    {
        int filterBeginIndex = centreIndices[i];
        int filterCenterIndex = centreIndices[i + 1];
        int filterEndIndex = centreIndices[i + 2];

        T triangleRangeUp = (T)(filterCenterIndex - filterBeginIndex);
        T triangleRangeDown = (T)(filterEndIndex - filterCenterIndex);

        // upward slope
        for (int k = filterBeginIndex; k < filterCenterIndex; k++)
            filterBank[i][k] = ((T)(k - filterBeginIndex)) / triangleRangeUp;

        // downwards slope
        for (int k = filterCenterIndex; k < filterEndIndex; k++)
            filterBank[i][k] = ((T)(filterEndIndex - k)) / triangleRangeDown;
    }
}

//==================================================================
template <class T>
T MFCC<T>::frequencyToMel (T frequency)
{
    return int(1127) * log (1 + (frequency / 700.0));
}


//===========================================================
template <class T>
OnsetDetectionFunction<T>::OnsetDetectionFunction (int frameSize)
{
    // initialise buffers with the frame size
    setFrameSize (frameSize);
}

//===========================================================
template <class T>
void OnsetDetectionFunction<T>::setFrameSize (int frameSize)
{
    // resize the prev magnitude spectrum vector
    prevMagnitudeSpectrum_spectralDifference.resize (frameSize);
    prevMagnitudeSpectrum_spectralDifferenceHWR.resize (frameSize);
    prevPhaseSpectrum_complexSpectralDifference.resize (frameSize);
    prevPhaseSpectrum2_complexSpectralDifference.resize (frameSize);
    prevMagnitudeSpectrum_complexSpectralDifference.resize (frameSize);

    // fill it with zeros
    for (size_t i = 0; i < prevMagnitudeSpectrum_spectralDifference.size(); i++)
    {
        prevMagnitudeSpectrum_spectralDifference[i] = 0.0;
        prevMagnitudeSpectrum_spectralDifferenceHWR[i] = 0.0;
        prevPhaseSpectrum_complexSpectralDifference[i] = 0.0;
        prevPhaseSpectrum2_complexSpectralDifference[i] = 0.0;
        prevMagnitudeSpectrum_complexSpectralDifference[i] = 0.0;
    }

    prevEnergySum = 0;
}

//-----------------------------------------------------------
//-----------------------------------------------------------

//===========================================================
template <class T>
T OnsetDetectionFunction<T>::energyDifference (const std::vector<T>& buffer)
{
    T sum;
    T difference;

    sum = 0; // initialise sum

    // sum the squares of the samples
    for (size_t i = 0; i < buffer.size(); i++)
        sum = sum + (buffer[i] * buffer[i]);

    difference = sum - prevEnergySum; // sample is first order difference in energy

    prevEnergySum = sum; // store energy value for next calculation

    if (difference > 0)
        return difference;
    else
        return 0.0;
}

//===========================================================
template <class T>
T OnsetDetectionFunction<T>::spectralDifference (const std::vector<T>& magnitudeSpectrum)
{
    T sum = 0; // initialise sum to zero

    for (size_t i = 0; i < magnitudeSpectrum.size(); i++)
    {
        // calculate difference
        T diff = magnitudeSpectrum[i] - prevMagnitudeSpectrum_spectralDifference[i];

        // ensure all difference values are positive
        if (diff < 0)
        {
            diff = diff * -1;
        }

        // add difference to sum
        sum = sum + diff;

        // store the sample for next time
        prevMagnitudeSpectrum_spectralDifference[i] = magnitudeSpectrum[i];
    }

    return sum;
}

//===========================================================
template <class T>
T OnsetDetectionFunction<T>::spectralDifferenceHWR (const std::vector<T>& magnitudeSpectrum)
{
    T sum = 0; // initialise sum to zero

    for (size_t i = 0; i < magnitudeSpectrum.size(); i++)
    {
        // calculate difference
        T diff = magnitudeSpectrum[i] - prevMagnitudeSpectrum_spectralDifferenceHWR[i];

        // only for positive changes
        if (diff > 0)
        {
            // add difference to sum
            sum = sum + diff;
        }

        // store the sample for next time
        prevMagnitudeSpectrum_spectralDifferenceHWR[i] = magnitudeSpectrum[i];
    }

    return sum;
}

//===========================================================
template <class T>
T OnsetDetectionFunction<T>::complexSpectralDifference (const std::vector<T>& fftReal, const std::vector<T>& fftImag)
{
    T dev, pdev;
    T sum;
    T magDiff, phaseDiff;
    T value;
    T phaseVal;
    T magVal;

    sum = 0; // initialise sum to zero

    // compute phase values from fft output and sum deviations
    for (size_t i = 0; i < fftReal.size(); i++)
    {
        // calculate phase value
        phaseVal = atan2 (fftImag[i], fftReal[i]);

        // calculate magnitude value
        magVal = sqrt ((fftReal[i] * fftReal[i]) + (fftImag[i] * fftImag[i]));

        // phase deviation
        dev = phaseVal - (2 * prevPhaseSpectrum_complexSpectralDifference[i]) + prevPhaseSpectrum2_complexSpectralDifference[i];

        // wrap into [-pi,pi] range
        pdev = princarg (dev);

        // calculate magnitude difference (real part of Euclidean distance between complex frames)
        magDiff = magVal - prevMagnitudeSpectrum_complexSpectralDifference[i];

        // calculate phase difference (imaginary part of Euclidean distance between complex frames)
        phaseDiff = -magVal * sin (pdev);

        // square real and imaginary parts, sum and take square root
        value = sqrt ((magDiff * magDiff) + (phaseDiff * phaseDiff));

        // add to sum
        sum = sum + value;

        // store values for next calculation
        prevPhaseSpectrum2_complexSpectralDifference[i] = prevPhaseSpectrum_complexSpectralDifference[i];
        prevPhaseSpectrum_complexSpectralDifference[i] = phaseVal;
        prevMagnitudeSpectrum_complexSpectralDifference[i] = magVal;
    }

    return sum;
}

//===========================================================
template <class T>
T OnsetDetectionFunction<T>::highFrequencyContent (const std::vector<T>& magnitudeSpectrum)
{
    T sum = 0;

    for (size_t i = 0; i < magnitudeSpectrum.size(); i++)
        sum += (magnitudeSpectrum[i] * ((T)(i + 1)));

    return sum;
}

//===========================================================
template <class T>
T OnsetDetectionFunction<T>::princarg (T phaseVal)
{
    // if phase value is less than or equal to -pi then add 2*pi
    while (phaseVal <= (-M_PI))
        phaseVal = phaseVal + (2 * M_PI);

    // if phase value is larger than pi, then subtract 2*pi
    while (phaseVal > M_PI)
        phaseVal = phaseVal - (2 * M_PI);

    return phaseVal;
}


//===========================================================
template <class T>
std::vector<T> WindowFunctions<T>::createWindow (int numSamples, WindowType windowType)
{
    if (windowType == HanningWindow)
    {
        std::vector<T> window = createHanningWindow (numSamples);
        return window;
    }
    else if (windowType == HammingWindow)
    {
        std::vector<T> window = createHammingWindow (numSamples);
        return window;
    }
    else if (windowType == BlackmanWindow)
    {
        std::vector<T> window = createBlackmanWindow (numSamples);
        return window;
    }
    else if (windowType == TukeyWindow)
    {
        std::vector<T> window = createTukeyWindow (numSamples);
        return window;
    }
    else // NOTE THIS COVERS THE RECTANGULAR WINDOW CASE AND ANY OTHER UNKNOWN TYPE
    {
        std::vector<T> window = createRectangularWindow (numSamples);
        return window;
    }
}

//===========================================================
template <class T>
std::vector<T> WindowFunctions<T>::createHanningWindow (int numSamples)
{
    std::vector<T> window (numSamples);
    
    T numSamplesMinus1 = (T) (numSamples - 1);		// the number of samples minus 1
    
    for (int i = 0; i < numSamples; i++)
    {
        window[i] = 0.5 * (1 - cos (2. * M_PI * (i / numSamplesMinus1)));
    }
    
    return window;
}

//===========================================================
template <class T>
std::vector<T> WindowFunctions<T>::createHammingWindow (int numSamples)
{
    std::vector<T> window (numSamples);
    
    T numSamplesMinus1 = (T) (numSamples - 1);		// the number of samples minus 1
    
    for (int i = 0; i < numSamples; i++)
        window[i] = 0.54 - (0.46 * cos (2 * M_PI * ((T)i / numSamplesMinus1)));
    
    return window;
}

//===========================================================
template <class T>
std::vector<T> WindowFunctions<T>::createBlackmanWindow (int numSamples)
{
    std::vector<T> window (numSamples);
    
    T numSamplesMinus1 = (T) (numSamples - 1);		// the number of samples minus 1
        
    for (int i = 0; i < numSamples; i++)
        window[i] = 0.42 - (0.5 * cos (2. * M_PI * ((T)i / numSamplesMinus1))) + (0.08 * cos (4. * M_PI * ((T)i / numSamplesMinus1)));
    
    return window;
}

//===========================================================
template <class T>
std::vector<T> WindowFunctions<T>::createTukeyWindow (int numSamples, T alpha)
{
    std::vector<T> window (numSamples);
    
    T numSamplesMinus1 = (T) (numSamples - 1);		// the number of samples minus 1
        
    T value = (double) (-1* ((numSamples / 2))) + 1;
    
    for (int i = 0; i < numSamples; i++)	// left taper
    {
        if ((value >= 0) && (value <= (alpha * (numSamplesMinus1 / 2))))
        {
            window[i] = 1.0;
        }
        else if ((value <= 0) && (value >= (-1 * alpha * (numSamplesMinus1 / 2))))
        {
            window[i] = 1.0;
        }
        else
        {
            window[i] = 0.5 * (1 + cos (M_PI * ((( 2 * value) / (alpha * numSamplesMinus1)) - 1)));
        }
        
        value += 1;
    }
    
    return window;
}

//===========================================================
template <class T>
std::vector<T> WindowFunctions<T>::createRectangularWindow (int numSamples)
{
    std::vector<T> window (numSamples);
    
    for (int i = 0; i < numSamples; i++)
        window[i] = 1.;
    
    return window;
}

//===========================================================
template <class T>
Yin<T>::Yin (int samplingFrequency)
{
    fs = samplingFrequency;
    setMaxFrequency (1500);
    prevPeriodEstimate = 1.0;
}

//===========================================================
template <class T>
void Yin<T>::setSamplingFrequency (int samplingFrequency)
{
    int oldFs = fs;
    fs = samplingFrequency;
    minPeriod = ((float) fs) / ((float) oldFs) * minPeriod;
}

//===========================================================
template <class T>
void Yin<T>::setMaxFrequency (T maxFreq)
{
    T minPeriodFloating;
    
    // if maxFrequency is zero or less than 200Hz, assume a bug
    // and set it to an arbitrary value fo 2000Hz
    if (maxFreq <= 200)
        maxFreq = 2000.;

    minPeriodFloating = ((T) fs) / maxFreq;
    minPeriod = (int) ceil (minPeriodFloating);
}

//===========================================================
template <class T>
T Yin<T>::pitchYin (const std::vector<T>& frame)
{
    unsigned long period;
    T fPeriod;
    
    // steps 1, 2 and 3 of the Yin algorithm
    // get the difference function ("delta")
    cumulativeMeanNormalisedDifferenceFunction (frame);
    
    // first, see if the previous period estimate has a minima
    long continuityPeriod = searchForOtherRecentMinima (delta);
    
    // if there is no minima at the previous period estimate
    if (continuityPeriod == -1)
    {
        // then estimate the period from the function
        period = getPeriodCandidate(delta);
    }
    else // if there was a minima at the previous period estimate
    {
        // go with that
        period = (unsigned long)continuityPeriod;
    }
    
    // check that we can interpolate (i.e. that period isn't first or last element)
    if ((period > 0) && (period < (delta.size() - 1)))
    {
        // parabolic interpolation
        fPeriod = parabolicInterpolation (period,delta[period-1],delta[period],delta[period+1]);
    }
    else // if no interpolation is possible
    {
        // just use the period "as is"
        fPeriod = (T) period;
    }
    
    // store the previous period estimate for later
    prevPeriodEstimate = fPeriod;
    
    return periodToPitch (fPeriod);
}

//===========================================================
template <class T>
void Yin<T>::cumulativeMeanNormalisedDifferenceFunction (const std::vector<T>& frame)
{
    T cumulativeSum = 0.0;
    unsigned long L = (unsigned long) frame.size() / 2;
    
    delta.resize(L);
    
    T *deltaPointer = &delta[0];

    // for each time lag tau
    for (unsigned long tau = 0; tau < L; tau++)
    {
        *deltaPointer = 0.0;
        
        // sum all squared differences for all samples up to half way through
        // the frame between the sample and the sample 'tau' samples away
        for (unsigned long j = 0; j < L; j++)
        {
            T diff = frame[j] - frame[j + tau];
            *deltaPointer += (diff * diff);
        }
        
        // calculate the cumulative sum of tau values to date
        cumulativeSum = cumulativeSum + delta[tau];
        
        if (cumulativeSum > 0)
            *deltaPointer = *deltaPointer * tau / cumulativeSum;
        
        deltaPointer++;
    }
    
    // set the first element to zero
    delta[0] = 1.;
}

//===========================================================
template <class T>
unsigned long Yin<T>::getPeriodCandidate (const std::vector<T>& delta)
{
    unsigned long minPeriod = 30;
    unsigned long period;
    
    T thresh = 0.1;
    
    bool periodCandidateFound = false;
    
    T minVal = 100000;
    unsigned long minInd = 0;
    
    for (unsigned long i = minPeriod; i < (delta.size() - 1); i++)
    {
        if (delta[i] < minVal)
        {
            minVal = delta[i];
            minInd = i;
        }
        
        if (delta[i] < thresh)
        {
            if ((delta[i] < delta[i-1]) && (delta[i] < delta[i+1]))
            {
                // we have found a minimum below the threshold, and because we
                // look for them in order, this is the first one, so we accept it
                // as the candidate period (i.e. the minimum period), and break the loop
                period = i;
                periodCandidateFound = true;
                break;
            }
        }
    }
    
    if (! periodCandidateFound)
    {
        period = minInd;
    }
    
    return period;
}

//===========================================================
template <class T>
T Yin<T>::parabolicInterpolation (unsigned long period, T y1, T y2, T y3)
{
    // if all elements are the same, our interpolation algorithm
    // will end up with a divide-by-zero, so just return the original
    // period without interpolation
    if ((y3 == y2) && (y2 == y1))
    {
        return (T) period;
    }
    else
    {
        T newPeriod = ((T)period) + (y3 - y1) / (2. * (2 * y2 - y3 - y1));
        return newPeriod;
    }
}

//===========================================================
template <class T>
long Yin<T>::searchForOtherRecentMinima (const std::vector<T>& delta)
{
    long newMinima = -1;
    
    long prevEst;
    
    prevEst = (long) round(prevPeriodEstimate);
    
    for (long i = prevEst - 1; i <= prevEst + 1; i++)
    {
        if ((i > 0) && (i < static_cast<long> (delta.size() - 1)))
        {
            if ((delta[i] < delta[i - 1]) && (delta[i] < delta[i + 1]))
            {
                newMinima = i;
            }
        }
    }
    
    return newMinima;
    
}

//===========================================================
template <class T>
T Yin<T>::periodToPitch (T period)
{
    return ((T) fs) / period;
}
