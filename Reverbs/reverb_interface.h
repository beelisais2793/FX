#ifndef REVERB_INTERFACE_H
#define REVERB_INTERFACE_H

#include <juce_audio_processors/juce_audio_processors.h>

namespace gv
{
/**
 * @brief The interface used by the plugin layer to process reverb.
 *
 * @tparam EarlyReflections The policy for handling early reflections.
 * @tparam LateReflections The policy for handling the tail of the signal.
 *
 * @details This class is what the higher layers of the plug use to pass the
 * dry signal through in order to get reverb. It is declared using an ER and LR
 * strategy to allow for swapping out different algos during development.
 */
template <typename EarlyReflections, typename LateReflections> class Reverb
{
  public:
    void setReverbAmount( float inReverbAmount )
    {
        mEarlyReflections.setReverbAmount( inReverbAmount );
        mLateReflections.setReverbAmount( inReverbAmount );
    }

    void prepareToPlay( double inSampleRate, int inSamplesPerBlock )
    {
        mEarlyReflections.prepareToPlay( inSampleRate, inSamplesPerBlock );
        mLateReflections.prepareToPlay( inSampleRate, inSamplesPerBlock );
    }

    void processBlock( juce::AudioBuffer<float>& inOutBuffer )
    {
        if ( mEnableER )
            mEarlyReflections.processBlock( inOutBuffer );

        if ( mEnableLR )
            mLateReflections.processBlock( inOutBuffer );
    }

    bool enableEarlyReflections( bool inEnable )
    {
        mEnableER = inEnable;
    }

    bool enableLateReflections( bool inEnable )
    {
        mEnableLR = inEnable;
    }

    void reset()
    {
        mEarlyReflections.reset();
        mLateReflections.reset();
    }

    int getLatency()
    {
        return mEarlyReflections.getLatency() + mLateReflections.getLatency();
    }

  private:
    EarlyReflections mEarlyReflections;
    LateReflections mLateReflections;
    bool mEnableER = true;
    bool mEnableLR = true;
};
} // namespace gv

#endif
