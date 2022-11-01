// https://github.com/igorski/fogpad/blob/master/src/allpass.h
class AllPass
{
    public:
        AllPass();
        void setBuffer( float *buf, int size );
        inline float process( float input )
        {
            float output;
            float bufout = _buffer[ _bufIndex ];
            undenormalise( bufout );

            output = -input + bufout;
            _buffer[ _bufIndex ] = input + ( bufout * _feedback );

            if ( ++_bufIndex >= _bufSize ) {
                _bufIndex = 0;
            }
            return output;
        }
        void mute();
        float getFeedback();
        void setFeedback( float val );

    private:
        float  _feedback;
        float* _buffer;
        int _bufSize;
        int _bufIndex;
};
}

AllPass::AllPass()
 {
     _bufIndex = 0;
     setFeedback( 0.5f );
 }

 void AllPass::setBuffer( float *buf, int size )
 {
     _buffer  = buf;
     _bufSize = size;
 }

 void AllPass::mute()
 {
     for ( int i = 0; i < _bufSize; i++ ) {
         _buffer[ i ] = 0;
     }
 }

 float AllPass::getFeedback()
 {
     return _feedback;
 }

 void AllPass::setFeedback( float val )
 {
     _feedback = val;
 }