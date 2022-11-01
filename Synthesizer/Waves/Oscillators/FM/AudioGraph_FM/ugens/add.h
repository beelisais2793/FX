//
//  add.h
//  AudioGraph_FM
//
//  Created by Owen Campbell on 2/19/15.
//
//

#ifndef __AudioGraph_FM__add__
#define __AudioGraph_FM__add__

#include "Node.h"

using namespace std;

//Takes as input single floating point numbers, sums them, and distributes the result to each registered output
template <class TFloat>
class Add : public Node<TFloat>{
public:
    Add(const string& name, const int numIns, const int numOuts) :
    Node<TFloat>(name, numIns, numOuts){
    
    };
    
    void tick(){
        TFloat sum = 0;
        for(int i = 0; i < Node<TFloat>::_numIns; i++){
            sum += *Node<TFloat>::_inputs[i];
        }
        for(int i = 0; i < Node<TFloat>::_numOuts; i++){
            *Node<TFloat>::_outputs[i] = sum;
        }
    };
};

template<class TFloat> using add_ptr = shared_ptr<Add<TFloat>>;
using add_f = Add<float>;
using add_d = Add<double>;
using add_ptr_f = shared_ptr<Add<float>>;
using add_ptr_d = shared_ptr<Add<double>>;

#endif /* defined(__AudioGraph_FM__add__) */