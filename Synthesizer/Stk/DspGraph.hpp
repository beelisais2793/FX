#pragma once

#include <iostream>
#include <functional>
#include <vector>
#include <memory>
#include <string>
#include <cassert>
#include <cmath>
#include <map>
#include <list>
#include "samples.hpp"
#include "TWaveTable.hpp"

using std::function;
using std::vector;
using std::shared_ptr;
using std::string;
using std::map;
using std::list;

typedef float SampleType;

using   SampleVector = std::vector<SampleType>;
using   WavTable     = SoundWave::TWaveTable<SampleType>;

struct Node;
struct Connection;


struct Port
{
    SampleVector port;    
    string       name;
    size_t       n;
    vector<Connection*> connections;

    Port() = default;
    Port(const string & name, size_t nSamples) {
        this->name = name;
        n = nSamples;
        port.resize(n);        
    }
    Port(const Port & p) {
        port = p.port;
    }
    Port& operator = (const Port & p) {
        port = p.port;
        return *this;
    }

    size_t Size() const { return n; }
    const string& GetName() const { return name; }

    SampleType& operator[](size_t i) { return port[i]; }
    SampleType  __getitem(size_t i) { return port[i]; }
    void __setitem(size_t i, SampleType v) { port[i] = v; }
    
    void Connect(Connection * c) {
        connections.push_back(c);
    }
    void Disconnect(Connection * c) {
        typename vector<Connection*>::iterator i;
        for(i = connections.begin();
            i != connections.end();
            i++)
            if( *i == c) break;
        if(i != connections.end())
            connections.erase(i);
    }
};



struct Dsp
{   
    size_t nChannels; 
    Connection * connection;
    
    Dsp(Connection * c) : connection(c) {} 

    virtual void Run(size_t n, const SampleVector & in, SampleVector & out) = 0;

    Port* GetPort(const string & name);
    Port* operator[](const string & name);
    Port* __getitem(const string& name);

    void Set(const string & name, SampleVector & v);
    SampleVector Get(const string & name);
    Port* Add(const string & name, size_t n);
    void SetPortValues(const string & name, SampleType v);
                
};

struct WaveTable : public Dsp
{
    WavTable  * w;    
    SampleType  sampleRate;
    size_t      nSamples;
    size_t      nChannels;

    WaveTable(Connection * c, SampleType sr, size_t n, size_t channels = 1) : Dsp(c) {
        w = new WavTable;              
        sampleRate = sr;        
        Add("freq",n);
        Add("phaseOffset",n);
        Add("output",n);
        SetPortValues("freq",440.0/sr);
        SetPortValues("phaseOffset", 0);
        SetPortValues("output",0);
        nChannels = 1;
        nSamples = n;
    }
    ~WaveTable() {
        if(w) delete w;     
    }
    void AddWaveTable(const SampleVector & v, SampleType upper)  {
        std::vector<float> r(v.size());
        memcpy(r.data(),v.vector.data(),v.size()*sizeof(SampleType));
        w->addWaveTable(v.size(),r,upper);
    }
    void SetFrequency(SampleType f) {
        w->setFrequency(f/sampleRate);
    }
    void Run(size_t n, const SampleVector & in, SampleVector & out) {
        SampleVector tmp(n);
        tmp.zero();
        for(size_t i = 0; i < n; i++) {        
            tmp[i] = w->getOutput();        
            w->updatePhase();
        }
        if(nChannels == 2) {
            size_t x = 0;
            for(size_t i = 0; i < n; i++) 
            {
                out[x++] = tmp[i];
                out[x++] = tmp[i];
            }
        }
        else out = tmp;
    }
};

struct Node 
{
    shared_ptr<Dsp> dsp;      
 
    Node() = default;
    Node(Dsp * d) {
        dsp = shared_ptr<Dsp>(d, [](Dsp * p) { delete p; });
    }
    Node(const Node & n) {
        dsp = n.dsp;
    }
    ~Node() = default;
    
    Node& operator = (const Node & n) {
        dsp.reset();
        dsp = n.dsp;
        return *this;
    }
    void Run(size_t n, SampleVector & in, SampleVector & out) {        
        dsp->Run(n,in,out);                 
    }

    void SetDSP(Dsp * p) {
        dsp = shared_ptr<Dsp>( p, [](Dsp * p){ delete p; });
    }
};


struct Connection
{
    shared_ptr<Node>            src;    
    shared_ptr<Port>            srcp;
    shared_ptr<Port>            dstp;
    vector<shared_ptr<Port>>    dst;    
    map<string,Port*>           ports;
    bool                        hasRun;

    Connection(size_t n, Node * src)
    {
        this->src  = shared_ptr<Node>( src, [](Node * n) { delete n; } );                
        this->srcp = shared_ptr<Port>(new Port("srcp",n), [](Port* p) { delete p; });
        this->dstp = shared_ptr<Port>(new Port("dstp",n), [](Port* p) { delete p; });
        hasRun = false;
    }
    ~Connection() = default;

    Port * Add(size_t n, const string & name) {
        Port * p = new Port(name,n);        
        p->connections.push_back(this);
        dst.push_back ( shared_ptr<Port>( p, [] (Port * p) { delete p; } ) );
        ports[name] = p;
        return p;
    }
    
    Port* operator[] (const string& name) { return ports[name]; }
    Port* __getitem(const string & name)  { return ports[name]; }

    void Connect(Port * p) {
        dst.push_back( shared_ptr<Port>(p, [](Port *p) { delete p; } ) );
    }
        
    void Run()
    {
        if(hasRun == true) return;        
        dstp->port.zero();        
        for(size_t i = 0; i < dst.size(); i++)
            for(size_t j = 0; j < dst[i]->connections.size(); j++)        
                if(dst[i]->connections[j] != this) dst[i]->connections[j]->Run();
        src->Run(srcp->n, srcp->port, dstp->port);
        for(size_t i = 0; i < dst.size(); i++)
            dst[i]->port = dstp->port;
        hasRun = true;
    }
    
};


struct Graph
{    
    vector<shared_ptr<Connection>> connections;       
    list<Port*> inputs;
    list<Port*> outputs;
    
    Graph() {

    }
    ~Graph() {

    }

    Connection* Connect(size_t n, Node * src) {
        Connection *c = new Connection(n,src);
        connections.push_back( shared_ptr<Connection>(c, [](Connection *p){ delete p; }));
        return c;
    }    
    void ConnectInput(Port *p) {
        inputs.push_back(p);
    }
    void ConnectOutput(Port *p) {
        outputs.push_back(p);
    }
    void Run(size_t n, const SampleVector &in, SampleVector &out) 
    {
        for(size_t p = 0; p < connections.size(); p++) connections[p]->hasRun = false;        
        for(typename list<Port*>::iterator i = inputs.begin(); i != inputs.end(); i++) (*i)->port = in;
        for(size_t p = 0; p < connections.size(); p++) connections[p]->Run();
                
        memset(out.data(),0,n*sizeof(SampleType));        
        for(typename list<Port*>::iterator outs = outputs.begin(); outs != outputs.end(); outs++) 
        {            
            out += (*outs)->port;
        }
        
        SampleType max = out.max();
        if(max != 0.0f) out /= max;
    }
};

    
inline void Dsp::Set(const string & name, SampleVector & v) 
{
    (*connection)[name]->port = v;
}
inline SampleVector Dsp::Get(const string & name) 
{
    return (*connection)[name]->port;
}
inline Port* Dsp::Add(const string & name, size_t n) {
    return connection->Add(n,name);
}    
inline void Dsp::SetPortValues(const string & name, SampleType v) {
    (*connection)[name]->port.fill(v);
}    
inline Port* Dsp::GetPort(const string & name)
{
    return (*connection)[name];
}
inline Port* Dsp::operator[](const string & name)
{
    return (*connection)[name];
}
inline Port* Dsp::__getitem(const string& name)
{
    return (*connection)[name];
}
    