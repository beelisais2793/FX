//
//  Parameter.h
//  AudioGraph_FM
//
//  Created by Owen Campbell on 2/17/15.
//
//

#ifndef __AudioGraph_FM__Parameter__
#define __AudioGraph_FM__Parameter__

#include <stdio.h>
#include <string>

using namespace std;

template <class TFloat>
class Parameter {
public:
    Parameter(){
        _name = "null";
        _defaultVal = 0.5;
        _currentVal = 0.5;
        _minVal = 0.0;
        _maxVal = 1.0;
    };
    Parameter(const Parameter<TFloat>& p){
        _name = p._name;
        _defaultVal = p._defaultVal;
        _currentVal = p._currentVal;
        _minVal = p._minVal;
        _maxVal = p._maxVal;
    };
    Parameter(const string name, const TFloat defaultVal, const TFloat minVal, const TFloat maxVal) : _name(name), _defaultVal(defaultVal), _currentVal(defaultVal), _minVal(minVal), _maxVal(maxVal){
        
    };
    const Parameter& operator= (const Parameter& rhs){
        _name = rhs->_name;
        _defaultVal = rhs->_defaultVal;
        _currentVal = rhs->_currentVal;
        _minVal = rhs->_minVal;
        _maxVal = rhs->_maxVal;
    };
    ~Parameter(){
    
    };
    
    
    enum class PARAMETER_VALUE {
        DEFAULT = 0,
        CURRENT,
        MIN,
        MAX
    };
    
    const string& getName(){
        return &_name;
    };
    
    void setValue(const TFloat value){
        if(value <= _minVal){
            _currentVal = _minVal;
        }
        else if(value >= _maxVal){
            _currentVal = _maxVal;
        }
        else{
            _currentVal = value;
        }
    };
    TFloat getValue(int which = (int)PARAMETER_VALUE::CURRENT){
        switch(which){
            case (int)PARAMETER_VALUE::DEFAULT:
                return _defaultVal;
            case (int)PARAMETER_VALUE::CURRENT:
                return _currentVal;
            case (int)PARAMETER_VALUE::MIN:
                return _minVal;
            case (int)PARAMETER_VALUE::MAX:
                return _maxVal;
        }
        return 0.0;
    };
private:
    string _name;
    TFloat _currentVal, _defaultVal, _minVal, _maxVal;
};

#endif /* defined(__AudioGraph_FM__Parameter__) */
