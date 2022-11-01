//
//  sinosc.h
//  AudioGraph_FM
//
//  Created by Owen Campbell on 2/19/15.
//
//

#ifndef __AudioGraph_FM__sinosc__
#define __AudioGraph_FM__sinosc__

#include "inttypes.h"
#include <math.h>
#include "Node.h"
#include "Parameter.h"

using namespace std;

const uint64_t PHSMASK64 = 0x0FFFFFFFFFFFFFFFL;
const uint64_t MAXLEN64 = 0x1000000000000000L;

const int SINOSC_BUFFER_SIZE = 4096;

template <class TFloat>
class Sinosc : public Node<TFloat>{
public:
    enum class INPUTS{
        FREQ = 0,
        AMP
    };
    
    
    Sinosc(const string& name, const int numIns, const int numOuts, const int sr, const TFloat freq, const TFloat amp) :
    Node<TFloat>(name, 2, numOuts),
    _phs(0), _inc(0), _lobits(0), _sr(sr){
        //initialize parameters
        Node<TFloat>::params.insert(make_pair("freq", Parameter<TFloat>("freq", freq, 1.0, 22000.0)));
        Node<TFloat>::params.insert(make_pair("amp", Parameter<TFloat>("amp", amp, 0.0, 1.0)));
        Node<TFloat>::setInput((int)INPUTS::FREQ, freq);
        Node<TFloat>::setInput((int)INPUTS::AMP, amp);
        
        _phs = 0;
        _lobits = log2(MAXLEN64/SINOSC_BUFFER_SIZE);
        setParameter("freq", freq);
        setParameter("amp", amp);
        
        for (int i = 0; i < SINOSC_BUFFER_SIZE; i++) {
            _buffer[i] = sin(2.0 * M_PI * i / (TFloat) SINOSC_BUFFER_SIZE);
        }
    };
    
    void setParameter(const string paramName, const TFloat newValue){
        try{
            Node<TFloat>::params[paramName].setValue(newValue);
            if(paramName == "freq"){
                _inc = (u_int64_t) lrint(newValue * (TFloat) MAXLEN64/ _sr);
            }
        }
        catch(exception){
            cout << "Attempting to setParameter with missing paramName" << endl;
        }
    };
    
    void tick(){
        TFloat out, amp, final;
        setParameter("freq", *Node<TFloat>::_inputs[(int)INPUTS::FREQ]);
        out = _buffer[_phs >> _lobits];
        _phs = (_phs + _inc) & PHSMASK64;
        amp = *Node<TFloat>::_inputs[(int)INPUTS::AMP];
        for(int i = 0; i < Node<TFloat>::_numOuts; i++){
            final = out * amp;
            assert(fabsf(final) <= fabsf(amp));
            *Node<TFloat>::_outputs[i] = final;
        }
    };
private:
    uint64_t _phs;
    uint64_t _inc;
    uint64_t _lobits;

    TFloat _buffer[SINOSC_BUFFER_SIZE];
    int _sr;
};

template<class TFloat> using sinosc_ptr = shared_ptr<Sinosc<TFloat>>;
using sinosc_f = Sinosc<float>;
using sinosc_d = Sinosc<double>;
using sinosc_ptr_f = shared_ptr<Sinosc<float>>;
using sinosc_ptr_d = shared_ptr<Sinosc<double>>;

const int SINOSC_FREQ = (int)Sinosc<float>::INPUTS::FREQ;
const int SINOSC_AMP = (int)Sinosc<float>::INPUTS::AMP;

#endif /* defined(__AudioGraph_FM__sinosc__) */