%{
#include "Gamma/Recorder.h"
%}

namespace gam
{
    /// Sound recorder
    class Recorder {
    public:

        Recorder();
        Recorder(int channels, int frames=8192);

        int channels() const;                
        int frames() const;
        int size() const;
        
        void overwrite(float v, int chan);
        void advance();
        void write(float v, int chan=0);
        void write(float v1, float v2, int chan=0);
        int write(const float * src, int numFrames);
        int read(float *& buf);
        void resize(int chans, int frames);
   };
}