//
//  Node.h
//  AudioGraph_FM
//
//  Created by Owen Campbell on 2/16/15.
//
//

#ifndef __AudioGraph_FM__Node__
#define __AudioGraph_FM__Node__

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <memory>
#include <map>
#include <vector>
#include <chrono>

#include "Parameter.h"

using namespace std;
using namespace chrono;

template <class TFloat>
class Node{
public:
    Node(const string name, const int numIns, const int numOuts) :
    name(name), _numIns(numIns), _numOuts(numOuts), _isHead(false), _id(Node<TFloat>::_idCounter++), _lastTick(high_resolution_clock::now()){
        _inputs.resize(_numIns);
        _sources.resize(_numIns);
        _outputs.resize(_numOuts);
        for(int i = 0; i < _numOuts; i++){
            _outputs[i] = make_shared<TFloat>(0.0);
        }
        
        params.clear();
    };
    virtual ~Node(){
        
    };
    
    void traverse(const high_resolution_clock::time_point& currentTick = std::chrono::high_resolution_clock::now()){
        if(currentTick > _lastTick){
            for(int i = 0; i < _numIns; i++){
                if(_inputs[i] == nullptr)
                {
                    cout << "Input " << i << " of " + name + " has no source!" << endl;
                    abort();
                }
                else if(_sources[i] != nullptr)
                {
                    _sources[i]->traverse(currentTick);
                }
            }
            tick();
            _lastTick = currentTick;
        }
        else{
            return;
        }
    }

    virtual void tick() = 0;
    
    void setInput(const int inputID, const int outputID, shared_ptr<Node<TFloat>> source){
        _inputs[inputID] = source->_outputs[outputID];
        _sources[inputID] = source;
    };
    
    void setInput(const int inputID, TFloat value){
        _inputs[inputID] = make_shared<TFloat>(value);
        _sources[inputID] = nullptr;
    }
    
    virtual void setParameter(const string paramName, const TFloat newValue){
        try{
            params[paramName].setValue(newValue);
        }
        catch(exception){
            cout << "Attempting to setParameter with missing paramName" << endl;
        }
    };
    
    virtual TFloat getParameter(const string paramName){
        try{
            return params[paramName].getValue();
        }
        catch(exception){
            cout << "Attempting to getParameter with missing paramName" << endl;
        }
        return 0.0;
    };
    
    map< string, Parameter<TFloat> > params;
    const string name;
    
protected:
    static unsigned int _idCounter;
    vector<shared_ptr<TFloat>> _inputs;
    vector<shared_ptr<TFloat>> _outputs;
    vector<shared_ptr<Node<TFloat>>> _sources;
    const int _numIns, _numOuts;
    bool _isHead;
    const unsigned int _id;
    high_resolution_clock::time_point _lastTick;
};

template <typename TFloat>
unsigned int Node<TFloat>::_idCounter = 0;

template<class TFloat> using node = shared_ptr<Node<TFloat>>;
using node_ptr_f = shared_ptr<Node<float>>;
using node_ptr_d = shared_ptr<Node<double>>;




#endif /* defined(__AudioGraph_FM__Node__) */
