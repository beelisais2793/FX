#pragma once

#include <vector>
#include "gain.h"

#define PAL_AUDIO_PLAYER_FILE_PATH_SIZE (512)

namespace pal
{
/// Plays back audio files with a nice UI.
class AudioPlayer
{
    public:
    /// Initialize a new audio player.
    AudioPlayer();

    /// Draw the AudioPlayer UI.
    void draw();

    /// Get the next sample of the loaded audio.
    float getNext();

    /// Load an audio file to be played.
    void loadFile(std::string path);

    /// Start playing the loaded audio.
    void play();

    /// Pause playing the loaded audio. Audio may be resumed from the current
    /// using `play`.
    void pause();

    /// Enable or disable looping.
    void setLoopEnabled(bool value) { loop = value; };

    /// Set the playback speed where e.g. 1 is normal speed, 2 is double speed,
    /// and 0.5 is half speed.
    void setSpeed(float value) { speed = value; };

    /// Set the output gain in dB.
    void setGainDb(float value) { outputGain = value; };

    /// Stop audio playback. Audio will be played from the beginning when `play`
    /// is called.
    void stop();

    private:
    std::vector<float> samples;
    bool isPlaying = false;
    bool loop = false;
    float readIndex = 0;
    float progress = 0;
    float speed = 1;
    float samplesUntilNextSample = 0;
    bool loadError = false;
    bool loadSuccess = false;
    Gain outputGain;
    char filePath[PAL_AUDIO_PLAYER_FILE_PATH_SIZE];
};
}