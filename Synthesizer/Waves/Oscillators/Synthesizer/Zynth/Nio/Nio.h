#ifndef NIO_H
#define NIO_H

#include <string>
#include <set>

#include "IMixer.h"
#include "globals.h"

class WavFile;

/**Interface to Nio Subsystem
 *
 * Should be only externally included header */
namespace Nio
{
    bool Start(Settings* settings);
    void Stop();

    void AddMixer(IMixer* mixer);
    void RemoveMixer(IMixer* mixer);

    void SetDefaultSource(std::string name);
    void SetDefaultSink(std::string name);

    bool SetSource(std::string name);
    bool SetSink(std::string name);

    void SetPostfix(std::string post);
    std::string GetPostfix();

    std::set<std::string> GetSources();
    std::set<std::string> GetSinks();

    std::string GetSource();
    std::string GetSink();

    //Get the prefered sample rate from jack (if running)
    void PreferedSampleRate(unsigned &rate);


    //Wave writing
    void WaveNew(class WavFile *wave);
    void WaveStart();
    void WaveStop();
    void WaveEnd();

    extern bool autoConnect;
    extern std::string defaultSource;
    extern std::string defaultSink;

}

#endif
