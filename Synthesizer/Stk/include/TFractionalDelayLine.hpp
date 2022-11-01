/*
  ==============================================================================
    FractionalDelayBuffer.h
  ==============================================================================
*/

#pragma once

#include <iostream>
#include "SoundAlchemy.hpp"

namespace SoundAlchemy::Delays
{

template<typename T>
class FractionalDelayBuffer
{
public:
    FractionalDelayBuffer();
    ~FractionalDelayBuffer();
    
    void clear();
    void setBufferSize(int size);
    void addSample(T sample);
    T getSample(T sampleIndex);
    int getValidIndex(int index);
    T linearInterpolate(T* buffer, int bufferSize, int bufferPosition);
    
private:
    int index;
    int bufferSize;
    T* buffer;
};


template<typename T>
FractionalDelayBuffer<T>::FractionalDelayBuffer()
{
    index = 0;
    bufferSize = 0;
    buffer = NULL;
}


template<typename T>
FractionalDelayBuffer<T>::~FractionalDelayBuffer()
{}


template<typename T>
void FractionalDelayBuffer<T>::clear()
{
    memset(buffer, 0, bufferSize*sizeof(T));
}


template<typename T>
void FractionalDelayBuffer<T>::setBufferSize(int size)
{
    if (buffer) {
        delete [] buffer;
    }
    
    bufferSize = size;
    buffer = new T[bufferSize];
    clear();
}


template<typename T>
void FractionalDelayBuffer<T>::addSample(T sample)
{
    index = getValidIndex(index);
    buffer[index] = sample;
    index++;
}


template<typename T>
int FractionalDelayBuffer<T>::getValidIndex(int index) {
    return index % (bufferSize);
}




template<typename T>
T FractionalDelayBuffer<T>::getSample(T sampleIndex)
{
    T localIndex = (T)index - sampleIndex;
    
    if   (localIndex >= bufferSize) {
        localIndex -= bufferSize;
    }
    
    if  (localIndex < 0) {
        localIndex += bufferSize;
    }
    
    return linearInterpolate(buffer, bufferSize, localIndex);
}


template<typename T>
T FractionalDelayBuffer<T>::linearInterpolate(T *buffer, int bufferSize, int bufferPosition)
{
    int lower = (int) bufferPosition;
    int upper = lower + 1;
    if (upper == bufferSize)
        upper = 0;
    T difference = bufferPosition - lower;
    
    return (buffer[upper] * difference) + (buffer[lower] * (T (1) - difference));
}
}