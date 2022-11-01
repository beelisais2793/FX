//
//  root.h
//  AudioGraph_FM
//
//  Created by Owen Campbell on 2/19/15.
//
//

#ifndef __AudioGraph_FM__root__
#define __AudioGraph_FM__root__

#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <assert.h>

#include "Node.h"

using namespace std;

template <class TFloat>
class Root : public Node<TFloat>{
public:
    Root(const int numIns, const int numOuts) :
    Node<TFloat>("root", numIns, numOuts){
        Node<TFloat>::_isHead = true;
        for(int i = 0; i < numOuts; i++){
            Node<TFloat>::_outputs[i] = make_shared<TFloat>(0);
        }
    };

    //TODO: Make sure tree will get deep copied...
//    Root(Root&& rhs) : _head(move(rhs._head)){};
//    
//    const Root& operator= (Root&& rhs){
//        swap(*this, rhs);
//        return *this;
//    };
    
    void tick(){
        if(Node<TFloat>::_numIns == Node<TFloat>::_numOuts){
            for(int i = 0; i < Node<TFloat>::_numOuts; i++){
                *Node<TFloat>::_outputs[i] = *Node<TFloat>::_inputs[i];
            }
        }
        else{
            TFloat sum = 0;
            for(int i = 0; i < Node<TFloat>::_numIns; i++){
                sum += *Node<TFloat>::_inputs[i];
            }
            sum /= (TFloat)Node<TFloat>::_numIns;
            //assert(sum <= 1.0 && sum >= -1.0);
            for(int i = 0; i < Node<TFloat>::_numOuts; i++){
                *Node<TFloat>::_outputs[i] = sum;
            }
        }
    };
    
    shared_ptr<Node<TFloat>> getNode(const string& name){
        return _nodes[name];
    }
    
    shared_ptr<Node<TFloat>> addNode(shared_ptr<Node<TFloat>> n){
        if(_nodes.count(n->name)){
            cout << "Error: a node named " + n->name + " already exists!" << endl;
            abort();
        }
        else{
            _nodes[n->name] = n;
            return n;
        }
    };
    
    TFloat output(const int channel){
        return *Node<TFloat>::_outputs[channel];
    };
    
    
    
private:
    map<string, shared_ptr<Node<TFloat>>> _nodes;
};


template<class TFloat> using root_ptr = shared_ptr<Root<TFloat>>;
using root_f = Root<float>;
using root_d = Root<double>;
using root_ptr_f = shared_ptr<Root<float>>;
using root_ptr_d = shared_ptr<Root<double>>;

#endif /* defined(__AudioGraph_FM__root__) */
