#include "AudioPlayer.h"
#include "imgui/imgui.h"
#include "wavfile.h"
#include "gain.h"

namespace pal
{
AudioPlayer::AudioPlayer()
{
    strcpy(filePath, "/path/to/some/file.wav");
}

void AudioPlayer::draw()
{
    ImGui::InputText("File path", filePath, PAL_AUDIO_PLAYER_FILE_PATH_SIZE);

    if (ImGui::Button("Load file"))
    {
        loadFile(filePath);
    }

    ImGui::SameLine();
    if (isPlaying)
    {
        if (ImGui::Button("Pause"))
        {
            pause();
        }
    }
    else
    {
        if (ImGui::Button("Play"))
        {
            play();
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Stop"))
    {
        stop();
    }

    ImGui::SameLine();
    ImGui::Checkbox("Loop", &loop);

    outputGain.draw();

    ImGui::SliderFloat("Speed", &speed, 0.0, 4);

    ImGui::ProgressBar(progress);

    if (loadError)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f,1.0f), "Failed to load audio file.");
    }

    if (loadSuccess)
    {
        ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f,1.0f), "Audio file loaded.");
    }
}

void AudioPlayer::loadFile(std::string path)
{
    strncpy(filePath, path.c_str(), PAL_AUDIO_PLAYER_FILE_PATH_SIZE);
    loadSuccess = readWavFile(path, samples, 0);
    loadError = !loadSuccess;
}

void AudioPlayer::play()
{
    if (!isPlaying)
    {
        isPlaying = true;
        samplesUntilNextSample = 0;
    }
}

void AudioPlayer::pause()
{
    isPlaying = false;
}

float AudioPlayer::getNext()
{
    if (!isPlaying)
    {
        return 0;
    }

    float y = 0;

    if (readIndex < samples.size())
    {
        y = samples[round(readIndex)];
    }

    readIndex += speed;
    
    if (round(readIndex) >= samples.size())
    {
        readIndex = 0;

        if (!loop)
        {
            isPlaying = false;
        }
    }

    progress = readIndex / (float)samples.size();

    return outputGain * y;
}

void AudioPlayer::stop()
{
    isPlaying = false;
    readIndex = 0;
    progress = 0;
}
}