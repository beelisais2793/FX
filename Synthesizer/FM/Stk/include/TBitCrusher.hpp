#pragma once 

#include "SoundAlchemy.hpp"

namespace SoundAlchemy::BitCrusher
{
    template<typename T>
    class BitCrusher : public Object<T>
    {

        public:
            BitCrusher( T amount, T inputMix, T outputMix );
            ~BitCrusher();

            void process( T* inBuffer, int bufferSize );

            void setAmount( T value ); // range between -1 to +1
            void setInputMix( T value );
            void setOutputMix( T value );

            T Tick(T I, T A = 1, T X = 1, T Y = 1)
            {
                return A*process(&I,1);
            }

        private:
            int _bits; // we scale the amount to integers in the 1-16 range
            T _amount;
            T _inputMix;
            T _outputMix;

            void calcBits();
    };

    template<typename T>
    BitCrusher<T>::BitCrusher( T amount, T inputMix, T outputMix )
    {
        setAmount   ( amount );
        setInputMix ( inputMix );
        setOutputMix( outputMix );
    }

    template<typename T>
    BitCrusher<T>::~BitCrusher()
    {

    }

    /* public methods */

    template<typename T>
    void BitCrusher<T>::process( T* inBuffer, int bufferSize )
    {
        // sound should not be crushed ? do nothing
        if ( _bits == 16 ) {
            return;
        }

        int bitsPlusOne = _bits + 1;

        for ( int i = 0; i < bufferSize; ++i )
        {
            short input = ( short ) (( inBuffer[ i ] * _inputMix ) * SHRT_MAX );
            short prevent_offset = ( short )( -1 >> bitsPlusOne );
            input &= ( -1 << ( 16 - _bits ));
            inBuffer[ i ] = (( input + prevent_offset ) * _outputMix ) / SHRT_MAX;
        }
    }

    /* setters */
    template<typename T>
    void BitCrusher<T>::setAmount( T value )
    {
        // invert the range 0 == max bits (no distortion), 1 == min bits (severely distorted)
        _amount = abs(value - 1.f);

        calcBits();
    }

    template<typename T>
    void BitCrusher<T>::setInputMix( T value )
    {
        _inputMix = ( value );
    }

    template<typename T>
    void BitCrusher<T>::setOutputMix( T value )
    {
        _outputMix = ( value );
    }

    /* private methods */
    template<typename T>
    void BitCrusher<T>::calcBits()
    {
        // scale T to 1 - 16 bit range
        _bits = ( int ) floor( scale( _amount, 1, 15 )) + 1;
    }

}