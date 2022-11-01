#pragma once

#include <portaudio.h>
#include <functional>

class RealTimeAudio
{
    public:

    RealTimeAudio();
    virtual ~RealTimeAudio();

    /// Draws a UI allowing you to select which audio devices to use and start
    /// and stop audio.
    void draw();
    
    void start();
    void stop();

    /// The callback function used to render audio.
    std::function<void(int, int, float *, float *)> callback;

    private:
    int selectedInputDevice;
    int selectedOutputDevice;
    bool audioIsRunning = false;
    PaStream *stream;
};