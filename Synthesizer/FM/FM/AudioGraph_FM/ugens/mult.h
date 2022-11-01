//
//  mult.h
//  AudioGraph_FM
//
//  Created by Owen Campbell on 2/19/15.
//
//

#ifndef __AudioGraph_FM__mult__
#define __AudioGraph_FM__mult__

#include "Node.h"

//Takes as input single floating point numbers, sums them, and distributes the result to each registered output
using namespace std;

template <class TFloat>
class Mult : public Node<TFloat>{
public:
    Mult(const string& name, const int numIns, const int numOuts) :
    Node<TFloat>(name, numIns, numOuts){
        
    };
    
    void tick(){
        TFloat product = 1.0;
        for(int i = 0; i < Node<TFloat>::_numIns; i++){
            product *= *Node<TFloat>::_inputs[i];
        }
        for(int i = 0; i < Node<TFloat>::_numOuts; i++){
            *Node<TFloat>::_outputs[i] = product;
        }
    };
};


template<class TFloat> using mult_ptr = shared_ptr<Mult<TFloat>>;
using mult_f = Mult<float>;
using mult_d = Mult<double>;
using mult_ptr_f = shared_ptr<Mult<float>>;
using mult_ptr_d = shared_ptr<Mult<double>>;

#endif /* defined(__AudioGraph_FM__mult__) */


