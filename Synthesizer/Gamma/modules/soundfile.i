%{
#include "Gamma/SoundFile.h"
%}

namespace gam {
    
    class SoundFile{
    public:

        /// Sound file formats
        enum Format{
            WAV = 1,	/**< Microsoft WAV format (little endian default). */
            AIFF,		/**< Apple/SGI AIFF format (big endian). */
            AU,			/**< Sun/NeXT AU format (big endian). */
            RAW,		/**< RAW PCM data. */
            NO_FORMAT
        };

        /// Sound file sample encoding types
        enum EncodingType{
            PCM_S8 = 1,	/**< Signed 8 bit data */
            PCM_16,		/**< Signed 16 bit data */
            PCM_24,		/**< Signed 24 bit data */
            PCM_32,		/**< Signed 32 bit data */
            PCM_U8,		/**< Unsigned 8 bit data (WAV and RAW only) */

            FLOAT,		/**< 32 bit float data */
            DOUBLE,		/**< 64 bit float data */

            ULAW,		/**< U-Law encoded. */
            ALAW,		/**< A-Law encoded. */
        };

        static const char * toString(Format v);
        static const char * toString(EncodingType v);

        SoundFile(const std::string& path="");
        SoundFile(const std::string& path, const SoundFile& src);
        ~SoundFile();

        bool openRead();
        bool openRead(const std::string& path);
        bool openWrite();
        bool openWrite(const std::string& path);
        bool close();

        
        int read(SampleType * dst, int numFrames);
        int readAll(SampleType * dst);        
        int readAll(std::vector<SampleType>& dst);
        int readAllD(SampleType * dst);

        int write(const SampleType * src, int numFrames);
        int write(const std::vector<SampleType>& src);

        
        bool opened() const;						///< Returns whether the sound file is open
        EncodingType encoding() const;				///< Get encoding type
        Format format() const;						///< Get format
        double frameRate() const;					///< Get frames/second
        int frames() const;							///< Get number of frames
        int channels() const;						///< Get number of channels
        int samples() const;						///< Get number of samples ( = frames x channels)
        const char * extension();					///< Get file extension
        const std::string& path() const;			///< Get path of sound file

        SoundFile& encoding(EncodingType v);		///< Set encoding type
        SoundFile& format(Format v);				///< Set format
        SoundFile& channels(int num);				///< Set number of channels
        SoundFile& frameRate(double hz);			///< Set frames/second
        SoundFile& info(const SoundFile& src);		///< Copy file information from an other file
        SoundFile& path(const std::string& path);	///< Set path of sound file

        void seek(int pos, int seekMode);
        void print();			///< Print information about file to stdout.
    };
}



