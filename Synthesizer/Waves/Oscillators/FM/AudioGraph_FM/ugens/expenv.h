//
//  expenv.h
//  AudioGraph_FM
//
//  Created by Owen Campbell on 2/19/15.
//
//

#ifndef __AudioGraph_FM__expenv__
#define __AudioGraph_FM__expenv__

#include "Node.h"
#include <math.h>

using namespace std;

const int NUM_STAGES = 4;

template <class TFloat>
class Expenv : public Node<TFloat>{
public:
    Expenv(const string& name, const int numIns, const int numOuts, const int sr, const TFloat amp, const TFloat att, const TFloat dec, const TFloat sus, const TFloat rel, const TFloat suslvl, const TFloat curve) :
        Node<TFloat>(name, 0, numOuts),
    _sr(sr), _amp(amp), _suslvl(suslvl), _curve(curve), _counter(0), _stage(0){
        //initialize parameters
        Node<TFloat>::params.insert(make_pair("amp", Parameter<TFloat>("amp", amp, 0.0, 1.0)));
        Node<TFloat>::params.insert(make_pair("suslvl", Parameter<TFloat>("suslvl", suslvl, 0.01, 1.0)));
        Node<TFloat>::params.insert(make_pair("curve", Parameter<TFloat>("curve", curve, 0.01, 10.0)));
        Node<TFloat>::params.insert(make_pair("trigger", Parameter<TFloat>("trigger", 0.0, 0.0, 1.0)));
        
        Node<TFloat>::params.insert(make_pair("att", Parameter<TFloat>("att", att, 0.01, 1000.0)));
        Node<TFloat>::params.insert(make_pair("dec", Parameter<TFloat>("dec", dec, 0.01, 1000.0)));
        Node<TFloat>::params.insert(make_pair("sus", Parameter<TFloat>("sus", sus, 0.01, 1000.0)));
        Node<TFloat>::params.insert(make_pair("rel", Parameter<TFloat>("rel", rel, 0.01, 1000.0)));

        _samples.resize(NUM_STAGES);
        
        //        Node<TFloat>::params["att"].setValue(att);
        

        _samples[0] = getNumSamples(Node<TFloat>::params["att"].getValue());
        _samples[1] = getNumSamples(Node<TFloat>::params["dec"].getValue());
        _samples[2] = getNumSamples(Node<TFloat>::params["sus"].getValue());
        _samples[3] = getNumSamples(Node<TFloat>::params["rel"].getValue());
        _counter = _samples[0];
    };
    
    void setParameter(const string paramName, const TFloat newValue){
        try{
            Node<TFloat>::params[paramName].setValue(newValue);
            if(paramName == "att"){
                _samples[0] = getNumSamples(newValue);
            }
            else if(paramName == "dec"){
                _samples[1] = getNumSamples(newValue);
            }
            else if(paramName == "sus"){
                _samples[2] = getNumSamples(newValue);
            }
            else if(paramName == "rel"){
                _samples[3] = getNumSamples(newValue);
            }

        }
        catch (exception){
            cout << "Attempting to setParameter with missing paramName" << endl;
        }
    };
    
    void tick(){
        TFloat out;
        switch (_stage) {
            case 0:
                out = _amp * pow((_samples[_stage] - _counter)/(TFloat)_samples[_stage], _curve);
                break;
            case 1:
                out = _amp
                - ((_amp - _suslvl) *
                   pow((_samples[_stage] - _counter)/(TFloat)_samples[_stage], 1.0/_curve));
                break;
            case 2:
                out = _suslvl;
                break;
            case 3:
                out = _suslvl * (1 - pow((_samples[_stage] - _counter)/(TFloat)_samples[_stage], _curve));
                break;
            default:
                out = 0.0;
                break;
                
        }
        
        _counter--;
        
        if(_counter == 0 && _stage < 4) {
            _counter = _samples[++_stage];
        }
        
        for(int i = 0; i < Node<TFloat>::_numOuts; i++){
            *Node<TFloat>::_outputs[i] = out;
        }
    };
private:
    inline int getNumSamples(TFloat dur){
        return (int) _sr * dur;
    };
    
    vector<int> _samples;
    TFloat _amp, _curve, _suslvl;
    int _counter;
    int _stage;
    int _sr;
};


template<class TFloat> using expenv_ptr = shared_ptr<Expenv<TFloat>>;
using expenv_f = Expenv<float>;
using expenv_d = Expenv<double>;
using expenv_ptr_f = shared_ptr<Expenv<float>>;
using expenv_ptr_d = shared_ptr<Expenv<double>>;

#endif /* defined(__AudioGraph_FM__expenv__) */