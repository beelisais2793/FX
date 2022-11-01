%{
#include "FileWrite.h"
%}

namespace stk {
    class FileWrite : public Stk
    {
    public:

        typedef unsigned long FILE_TYPE;

        static const FILE_TYPE FILE_RAW; /*!< STK RAW file type. */
        static const FILE_TYPE FILE_WAV; /*!< WAV file type. */
        static const FILE_TYPE FILE_SND; /*!< SND (AU) file type. */
        static const FILE_TYPE FILE_AIF; /*!< AIFF file type. */
        static const FILE_TYPE FILE_MAT; /*!< Matlab MAT-file type. */


        FileWrite( void );
        FileWrite( std::string fileName, unsigned int nChannels = 1, FILE_TYPE type = FILE_WAV, Stk::StkFormat format = STK_SINT16 );
        virtual ~FileWrite();

        void open( std::string fileName, unsigned int nChannels = 1,
                    FileWrite::FILE_TYPE type = FILE_WAV, Stk::StkFormat format = STK_SINT16 );
        void close( void );
        bool isOpen( void );
        void write( StkFrames& buffer );
    };
}