template<class T>
class LFO {

    public:
        LFO( T sampleRate );
        ~LFO();

        T getRate();
        void setRate( T value );

        // accumulators are used to retrieve a sample from the wave table
        // in other words: track the progress of the oscillator against its range

        T getAccumulator();
        void setAccumulator( T offset );

        /**
         * retrieve a value from the wave table for the current
         * accumulator position, this method also increments
         * the accumulator and keeps it within bounds
         */
        inline T peek()
        {
            // the wave table offset to read from
            T SR_OVER_LENGTH = _sampleRate / ( T ) TABLE_SIZE;
            int readOffset = ( _accumulator == 0.f ) ? 0 : ( int ) ( _accumulator / SR_OVER_LENGTH );

            // increment the accumulators read offset
            _accumulator += _rate;

            // keep the accumulator within the bounds of the sample frequency
            if ( _accumulator > _sampleRate )
                _accumulator -= _sampleRate;

            // return the sample present at the calculated offset within the table
            return VST::TABLE[ readOffset ];
        }

    private:

        // see Igorski::VST::LFO_TABLE;
        static const int TABLE_SIZE = 128;

        // used internally

        T _rate;
        T _accumulator;   // is read offset in wave table buffer
        T _sampleRate;
};


template<class T>
LFO<T>::LFO( T sampleRate ) {
    _rate        = VST::MIN_LFO_RATE();
    _accumulator = 0.f;
    _sampleRate  = sampleRate;
}

template<class T>
LFO<T>::~LFO() {

}

template<class T>
T LFO<T>::getRate()
{
    return _rate;
}

template<class T>
void LFO<T>::setRate( T value )
{
    _rate = value;
}

template<class T>
void LFO<T>::setAccumulator( T value )
{
    _accumulator = value;
}

template<class T>
T LFO<T>::getAccumulator()
{
    return _accumulator;
}
