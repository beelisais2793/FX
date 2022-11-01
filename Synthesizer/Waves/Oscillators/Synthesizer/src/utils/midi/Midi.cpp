#include "Midi.hpp"
#include <iostream>

void Callback(double deltatime, std::vector<unsigned char>* message, void* userData)
{
    // Get amount of bytes in message
    unsigned int nBytes = message->size();

    // Distribute event
    if (nBytes >= 3)
    {
        Midi* data = (Midi*)userData;

        if (message->at(0) == 144 && data->MidiPress)
            data->MidiPress(message->at(1), message->at(2));
        else if (message->at(0) == 128 && data->MidiRelease)
            data->MidiRelease(message->at(1), message->at(2));


        std::cout << (int) message->at(0) << ", "
            << (int) message->at(1) << ", "
            << (int) message->at(2)
            << std::endl;
    }

}

Midi::Midi() 
{
    // new midiin
    midiin = new RtMidiIn();

    // Set thread to active
    active = true;
    connectThread = std::thread([this]() { this->Loop(); });
}

Midi::~Midi() 
{
    active = false;
    connectThread.join();
    delete midiin;
};

void Midi::Loop() 
{
    // Set callback
    bool connected = false;
    midiin->setCallback(&Callback, this);
    midiin->ignoreTypes(false, false, false);

    // This loop ensures that if theres a disconnect it can reconnect later.
    while (active)
    {
        // Get nmr of ports
        unsigned int nPorts = midiin->getPortCount();

        // Check if connected 
        if (nPorts > 1 && !connected)
        {
            midiin->openPort(1);
            connected = true;
        }
        else if (nPorts <= 1 && connected)
        {
            midiin->closePort();
            connected = false;
        }

        // Only check every 500ms
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int Midi::NoteToScale(int note, int* scale, int size)
{
    int mod = note % 12;

    int ldi = 12;
    for (int i = 0; i < size; i++)
    {
        int dif = mod - scale[i];
        if (dif < 0) break;
        if (dif < ldi) ldi = dif;
    }

    return note - ldi;
}

double Midi::NoteToFreq(int note)
{
    return std::pow(2, (note - 69) / 12.0) * 440;
}