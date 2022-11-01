#include "RealTimeAudio.h"
#include "Gui.h"
#include <portaudio.h>

#if __APPLE__
#include <pa_mac_core.h>
#endif

RealTimeAudio::RealTimeAudio()
{
    Pa_Initialize();
    selectedInputDevice = Pa_GetDefaultInputDevice();
    selectedOutputDevice = Pa_GetDefaultOutputDevice();
}

RealTimeAudio::~RealTimeAudio()
{
    stop();
    Pa_Terminate();
}

int paCallback(
    const void *inputBuffer,
    void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData)
{
    RealTimeAudio *caller = (RealTimeAudio *)userData;
    auto *in = (float*)inputBuffer;
    auto *out = (float*)outputBuffer;

    caller->callback(framesPerBuffer, 2, in, out);
    return 0;
}

void RealTimeAudio::draw()
{
    int numDevices = Pa_GetDeviceCount();

    if (ImGui::BeginCombo("Input device", Pa_GetDeviceInfo(selectedInputDevice)->name, 0))
    {
        for (int i = 0; i < numDevices; i++)
        {
            bool isSelected = i == selectedInputDevice;
            const PaDeviceInfo *info = Pa_GetDeviceInfo(i);

            if (info->maxInputChannels == 0)
            {
                continue;
            }

            if (ImGui::Selectable(info->name, isSelected))
            {
                selectedInputDevice = i;
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    if (ImGui::BeginCombo("Output device", Pa_GetDeviceInfo(selectedOutputDevice)->name, 0))
    {
        for (int i = 0; i < numDevices; i++)
        {
            bool isSelected = i == selectedOutputDevice;
            const PaDeviceInfo *info = Pa_GetDeviceInfo(i);

            if (info->maxOutputChannels == 0)
            {
                continue;
            }

            if (ImGui::Selectable(info->name, isSelected))
            {
                selectedOutputDevice = i;
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    if (audioIsRunning)
    {
        if (ImGui::Button("Stop audio"))
        {
            stop();
        }

        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f,1.0f), "Audio running");
    }
    else
    {
        if (ImGui::Button("Start audio"))
        {
            start();
        }

        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f,1.0f), "Audio stopped");
    }
}

void RealTimeAudio::start()
{
    if (audioIsRunning)
    {
        return;
    }

    PaStreamParameters inputParameters;
    PaStreamParameters outputParameters;
    inputParameters.device = selectedInputDevice;
    inputParameters.channelCount = 2;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = 0.005;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    outputParameters.device = selectedOutputDevice;
    outputParameters.channelCount = 2;
    outputParameters.sampleFormat = paFloat32;   
    outputParameters.suggestedLatency = 0.005; 
    outputParameters.hostApiSpecificStreamInfo = NULL;

#if __APPLE__
    PaMacCoreStreamInfo macCoreStreamInfo;
    PaMacCore_SetupStreamInfo(&macCoreStreamInfo, paMacCoreChangeDeviceParameters);
    inputParameters.hostApiSpecificStreamInfo = &macCoreStreamInfo;
    outputParameters.hostApiSpecificStreamInfo = &macCoreStreamInfo;
#endif

    auto err = Pa_OpenStream(
        &this->stream,
        &inputParameters,
        &outputParameters,
        44100,
        512,
        paClipOff,
        paCallback,
        this);

    if (err != paNoError)
    {
        // throw std::runtime_error(Pa_GetErrorText(err));
    }

    err = Pa_StartStream(stream);

    if (err != paNoError)
    {
        // throw std::runtime_error(Pa_GetErrorText(err));
    }

    audioIsRunning = true;
}

void RealTimeAudio::stop()
{
    if (audioIsRunning)
    {
        audioIsRunning = false;
        Pa_StopStream(stream);
    }
}