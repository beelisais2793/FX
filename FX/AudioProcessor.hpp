#pragma once

enum AudioProcessorType
{
    MONO_AUDIO,
    STEREO_AUDIO,
};

struct AudioProcessor
{
    virtual ~AudioProcessor =default;

    virtual int getType() const = 0;

    virtual void setParameter(int parameter, float value) = 0;
};

using AudioProcessorBlock = std::list<AudioProcessor*>;
using AudioProcessorMap   = std::map<int,AudioProcessor*>;

struct MonoAudioProcessor : public AudioProcessor
{
    virtual ~MonoAudioProcessor =default;
    MonoAudioProcessor() : AudioProcessor()
    {
        
    }
    int getType() const { return MONO_AUDIO; }
    
    virtual void ProcessBlock(size_t n, float * in, float * out) = 0;
};

struct StereoAudioProcessor : public AudioProcessor
{
    virtual ~StereoAudioProcessor =default;
    StereoAudioProcessor() : AudioProcessor()
    {

    }
    int getType() const { return STEREO_AUDIO; }
    
    virtual void ProcessBlock(size_t n, float ** in, float ** out) = 0;
};


struct MonoLuaAudioProcessor : public MonoAudioProcessor
{
    LuaJIT * lua;

    MonoLuaAudioProcessor() : MonoAudioProcessor()
    {

    }    
};
 
struct StereoLuaAudioProcessor : public StereoAudioProcessor
{
    LuaJIT * lua;

    StereoLuaAudioProcessor() : StereoAudioProcessor()
    {

    }    
};


// MonoOctopusAudioProcessor
// StereoOctopusAudioProcessor
// MonoJellyfishAudioProcessor
// StereoJellyfishAudioProcessor
// MonoStingrayAudioProcessor
// StereoStingrayAudioProcessor
// MonoDLLAudioProcessor
// StereoDLLAudioProcessor

struct MonoAudioBlockProcessor
{
    std::vector<std::shared_ptr<MonoAudioProcessor>> processors;

    StereoAudioBlockProcessor()
    {

    }

    void addProcessor(MonoAudioBlockProcessor * p) {
        processors.push_back( std::shared_ptr<MonoAudioProcessor>(p, [](MonoAudioProcessor * ptr){ delete ptr; }));
    }

    void ProcessBlock(size_t n, float * inputs, float * outputs) {
        auto i = processors.begin();
        i->ProcessBlock(n,inputs,outputs);
        for(; i != processors.end(); i++)
        {
            i->ProcessBlock(n,outputs,outputs);
        }
    }
};

struct StereoAudioBlockProcessor
{
    std::vector<std::shared_ptr<StereoAudioProcessor>> processors;

    StereoAudioBlockProcessor()
    {

    }

    void addProcessor(StereoAudioBlockProcessor * p) {
        processors.push_back( std::shared_ptr<StereoAudioProcessor>(p, [](StereoAudioProcessor * ptr){ delete ptr; }));
    }

    void ProcessBlock(size_t n, float ** inputs, float ** outputs) {
        auto i = processors.begin();
        i->ProcessBlock(n,inputs,outputs);
        for(; i != processors.end(); i++)
        {
            i->ProcessBlock(n,outputs,outputs);
        }
    }
};