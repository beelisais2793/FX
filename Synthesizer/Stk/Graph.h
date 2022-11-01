#pragma once


#include <string>
#include <vector>
#include <map>
#include <memory>
#include <list>
#include <functional>

//#include "KFR/KFR.h"
#include "Sample/SampleVector.h"
#include "IO/sndfile.h"
#include "SoundWave/Resampler.h"

typedef SampleType SampleType;

using std::string;
using std::vector;
using std::map;
using std::shared_ptr;
using SoundWave::SndFileReader;
using SoundWave::SndFileWriter;
using SoundWave::Resampler;
using std::list;
using SampleVector = Sample::SampleVector<SampleType>;
using std::function;

enum WavType {
    WAV_WAVE,
    WAV_MP3,
    WAV_FLAC,
    WAV_OGG,
    WAV_RAW,
    WAV_AU,
    WAV_AIFF
};

enum Quality
{
	QUALITY_BEST = SRC_SINC_BEST_QUALITY,
	QUALITY_MEDIUM = SRC_SINC_MEDIUM_QUALITY,
	QUALITY_FASTEST = SRC_SINC_FASTEST,
	QUALITY_ZERO_ORDER_HOLD = SRC_ZERO_ORDER_HOLD,
	QUALITY_LINEAR = SRC_LINEAR,
} ;

enum {
    LEFT_SAMPLE=0,
    RIGHT_SAMPLE=1,
};


struct StereoSample {
    SampleType samples[2];

    StereoSample() = default;
    StereoSample(SampleType leftSample, SampleType rightSample) { samples[0] = leftSample; samples[1] = rightSample; }

    StereoSample& operator = (const StereoSample & s) {
        samples[0] = s.samples[0];
        samples[1] = s.samples[1];
        return *this;
    }
    SampleType& left() { return samples[0]; }
    SampleType& right() { return samples[1];}

    SampleType left() const { return samples[0]; }
    SampleType right() const { return samples[1];}

    StereoSample pan(SampleType pan) {
        SampleType m = ((pan +1)/2.0) * (M_PI/2.0);
        StereoSample r(left()*sin(m), right()*cos(m));
        return r;
    }

};



enum DspType
{
    DSP_INPUT,      // input only    
    DSP_PROCESSOR,  // input output
    DSP_GENERATOR,  // output only
};

struct DspNode
{    
    virtual DspType type() const = 0;

    virtual void Process(size_t n, const SampleVector<SampleType> & input, SampleVector<SampleType> & output) = 0;
};

struct WaveSample : public DspNode
{    
    SampleVector<SampleType> samples;
    size_t pos;

    WaveSample(string filename) {
        Load(filename.c_str());
        pos = 0;
    }    

    SampleType operator[](size_t i) {
        return samples[i];
    }

    size_t size() const { return samples.size(); }
    
    void Load(const char * filename) {
        SndFileReader file(filename);
        std::vector<SampleType> temp(1024);                            
        int readcount;        
        size_t size = file.size();        
        int channels = file.channels();
        samples.resize(size);
        int spos = 0;        
        while(readcount = file.read(1024,temp)) {                
            memcpy(samples.data()+spos,temp.data(),readcount*sizeof(SampleType));
            spos += readcount;            
        }                
    }  
    
    
    void Process(size_t n, const SampleVector<SampleType> & input, SampleVector<SampleType> & output) {
        output.resize(n,input.num_channels());
        output.zero();
        if(pos >= samples.size()) return;        
        if((pos + n) > samples.size()) n = samples.size()-pos;
        if(input.num_channels()==2 && samples.num_channels()==1)
        {
            SampleVector<SampleType> chan = samples.get_channel_count(0,pos,n);
            output.set_channel(0,chan);
            output.set_channel(1,chan);
        }
        else if(input.num_channels()==1 && samples.num_channels()==2) {
            SampleVector<SampleType> chan = samples.get_channel_count(0,pos,n);
            output.set_channel(0,chan,pos,n);
        }
        else {
            output.copy(samples,pos,n);
        }
        pos += n;        
    }    
};


struct WaveChunk : public Dsp
{    
    SndFileReader file;

    SampleVector<SampleType> samples;
    size_t pos;
    size_t chunk;

    WaveChunk(string filename, size_t chunk_size) : file(filename.c_str()) {        
        pos = 0;
        chunk = chunk_size;
    }    

    SampleType operator[](size_t i) {
        return samples[i];
    }

    void Load(string filename) {        
        std::vector<SampleType> temp(chunk);                            
        int readcount;                
        samples.resize(chunk);
        int n = chunk;
        if((pos + chunk) > file.size()) n = (file.size() - pos);
        readcount = file.read(n,temp);
        memcpy(samples.data()+pos,temp.data(),readcount*sizeof(SampleType));                              
    }  

    void Process(size_t n, SampleVector<SampleType> & output) {
        if(pos >= samples.size()) return;
        if((pos + n) > samples.size()) n = samples.size()-pos;
        memcpy(output.data(),samples.data()+pos,n*sizeof(SampleType));
        pos += n;
    }
};


struct Node;
typedef std::function<void (Dsp * dsp, SampleType)> control_func;

struct Control
{
    SampleType       value;
    int              cc;
    string           name;
    control_func     func;

    Control(string& _name, int _cc, control_func & f) {
        cc = _cc;
        name = _name;
        func = f;
    }
    void Fire(Dsp * dsp) {
        func(dsp,value);
    }
};

struct Port
{
    SampleVector<SampleType> from_wire,to_wire;
    string name;
    Node * fromNode;
    Node * toNode;


    Port( string &_name, size_t channels, size_t n) {
        from_wire.resize(n,channels);
        to_wire.resize(n,channels);
        name = _name;
        fromNode = nullptr;
        toNode = nullptr;        
    }
    void ConnectFrom(Node * from) {
        fromNode = from;
    }
    void ConnectTo(Node * to) {
        toNode = to;
    }    
    void Fire(Dsp * dsp,size_t n) {
        dsp->Process(n,from_wire,to_wire);
    }
};

inline Port* make_port(string name, size_t channels, size_t n, Node * from, Node * to) {
    Port * p = new Port(name,channels,n);
    p->ConnectFrom(from);
    p->ConnectTo(to);
    return p;
}

struct Node 
{
    Dsp * dsp;

    vector<shared_ptr<Control>> control_vector;
    vector<shared_ptr<Port>> port_vector;
    map<string,Control*> controls;
    map<string,Port*> ports;        
    bool isProcessed;

    Node(Dsp * dsp, size_t samples_per_block) {
        Port* in = new Port("input",2,samples_per_block);
        port_vector.push_back(shared_ptr<Port>( in, [](Port *p){delete p;}));
        Port* out = new Port("output",2,samples_per_block);
        port_vector.push_back(shared_ptr<Port>( out, [](Port *p){delete p;}));
        ports["input"] = in;
        ports["output"] = out;
        isProcessed = false;
        this->dsp = dsp;
    }

    void AddControl(const string & name, Control * c) {        
        control_vector.push_back(shared_ptr<Control>(c,[](Control * p){ delete p; }));
        controls[name] = c;
    }
    void AddPort(const string & name, Node * from, Port * port) {
        port->ConnectFrom(from);
        port->ConnectTo(this);
        port_vector.push_back(shared_ptr<Port>(port,[](Port * p) { delete p; }));
        ports[name] = p;
    }

    Control * GetControl(const string & name) {
        return controls[name];
    }
    Port * GetPort(const string & name) {
        return ports[name];
    }

    void SetInput(const SampleVector<SampleType> & input) {
        ports["input"]->CopyFrom(input);
    }
    SampleVector<SampleType> GetOutput() {
        SampleVector<SampleType> r;
        port["output"]->CopyTo(r);
        return r;
    }
    void Process(size_t n,const SampleVector & input, SampleVector & output) {
        dsp->Process(n,input,output);
    }
    void Process(size_t n) {
        for(size_t i = 0; i < port_vector.size(); i++)
            port_vector[i]->Fire(dsp,n)
    }
    void Clear() { isProcessed = false; }

    void UpdateControls() {
        for(size_t i = 0; i < control_vector.size(); i++)
            control_vector[i]->Fire(dsp);
    }
};

// node must be inserted in order of execution it is not solved recursively
struct NodeList
{    
    list<shared_ptr<Node>> nodes;
    map<string,Node*> lookup;

    NodeList() = default;

    void AddNode(const std::string & name, Node * ptr) {
        lookup[name] = ptr;
        nodes.push_back(shared_ptr<Node>(ptr,[](Node * p){ delete p; }));
    }

    void Clear() {
        typename list<shared_ptr<Node>>::iterator i;
        for(i = nodes.begin(); i != nodes.end(); i++)
            (*i)->Clear();
    }
    void Process(size_t n, const SampleVector & input, SampleVector & output) {
        typename list<shared_ptr<Node>>::iterator i;
        SampleVector t(input.size());        
        for(i = nodes.begin(); i != nodes.end(); i++)
        {            
            (*i)->Process(n,in,output);
            in = output;
            (*i)->isProcessed = true;
        }
    }
    
};

// graph will solve it recursively the order of insertion doesn't matter.
struct Graph
{
    std::vector<shared_ptr<Node>> nodes;
    shared_ptr<Node> input;
    shared_ptr<Node> output;

    void Clear() {
        for(size_t i = 0; i < nodes.size(); i++)
            nodes[i]->Clear();
    }
    void Process(Node * n) {        
        if(n->isProcessed == true) return;        
        for(size_t i = 0; i < n->input_nodes.size(); i++)
            Process(n->input_nodes[i]);
        n->UpdateControls();
        n->UpdateConnections();
        n->Process();        
        n->isProcessed = true;
    }
    void Visit(size_t len, const SampleVector<SampleType> & input, SampleVector<SampleType> & output) {
        Clear();
        input.copy(input);        
        for(size_t i = 0; i < nodes.size(); i++) {
            Process(nodes[i]);        
        }
        output = output.copy();
    }
};