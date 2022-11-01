#pragma once
#include "SoundAlchemy.hpp"

namespace SoundAlchemy::WaveShaper
{
    template<class T>
    class WaveShaper
    {
        public:
            WaveShaper( T amount, T level );

            T getAmount();
            void setAmount( T value ); // range between -1 and +1
            T getLevel();
            void setLevel( T value );
            void process( T* inBuffer, int bufferSize );

        private:
            T _amount;
            T _multiplier;
            T _level;
    };

    template<class T>
    WaveShaper<T>::WaveShaper( T amount, T level )
    {
        setAmount( amount );
        setLevel ( level );
    }

    template<class T>
    void WaveShaper<T>::process( T* inBuffer, int bufferSize )
    {
        for ( int j = 0; j < bufferSize; ++j )
        {
            T input = inBuffer[ j ];
            inBuffer[ j ] =  (( 1.0 + _multiplier ) * input / ( 1.0 + _multiplier * std::abs( input ))) * _level;
        }
    }

    template<class T>
    T WaveShaper<T>::getAmount()
    {
        return _amount;
    }

    template<class T>
    void WaveShaper<T>::setAmount( T value )
    {
        _amount     = value;
        _multiplier = 2.0f * _amount / ( 1.0f - fmin(0.99999f, _amount));
    }

    template<class T>
    T WaveShaper<T>::getLevel()
    {
        return _level;
    }

    template<class T>
    void WaveShaper<T>::setLevel( T value )
    {
        _level = value;
    }
}