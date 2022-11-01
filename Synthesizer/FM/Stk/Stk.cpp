#include "Stk.hpp"
#include "StkInetWvIn.hpp"

namespace stk 
{

std::vector<Stk*> Stk::alertList_;
static double srate_ = 44100.0f;
static std::string rawwavepath_;
static bool showWarnings_;
static bool printErrors_;
std::ostringstream oStream_;


double sampleRate( void ) { return srate_; }

void SetSampleRate(double r) {
  srate_ = r;
}
  
  
//! Static method that returns the current rawwave path.
std::string rawwavePath(void) { return rawwavepath_; }

void setRawwavePath( std::string path )
{
    if ( !path.empty() )
        rawwavepath_ = path;

    // Make sure the path includes a "/"
    if ( rawwavepath_[rawwavepath_.length()-1] != '/' )
        rawwavepath_ += "/";
}


  //! Static method that byte-swaps a 16-bit data type.
void swap16( unsigned char *ptr )
{
    unsigned char val;

    // Swap 1st and 2nd bytes
    val = *(ptr);
    *(ptr) = *(ptr+1);
    *(ptr+1) = val;
}



//! Static method that byte-swaps a 32-bit data type.
void swap32( unsigned char *ptr )
{
unsigned char val;

// Swap 1st and 4th bytes
val = *(ptr);
*(ptr) = *(ptr+3);
*(ptr+3) = val;

//Swap 2nd and 3rd bytes
ptr += 1;
val = *(ptr);
*(ptr) = *(ptr+1);
*(ptr+1) = val;
}



//! Static method that byte-swaps a 64-bit data type.
void swap64( unsigned char *ptr ) 
{
unsigned char val;

// Swap 1st and 8th bytes
val = *(ptr);
*(ptr) = *(ptr+7);
*(ptr+7) = val;

// Swap 2nd and 7th bytes
ptr += 1;
val = *(ptr);
*(ptr) = *(ptr+5);
*(ptr+5) = val;

// Swap 3rd and 6th bytes
ptr += 1;
val = *(ptr);
*(ptr) = *(ptr+3);
*(ptr+3) = val;

// Swap 4th and 5th bytes
ptr += 1;
val = *(ptr);
*(ptr) = *(ptr+1);
*(ptr+1) = val;
}

//! Static cross-platform method to sleep for a number of milliseconds.
void sleep( unsigned long milliseconds )
{
  #if defined(__OS_WINDOWS__)
      Sleep((DWORD) milliseconds);
  #elif (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))
      usleep( (unsigned long) (milliseconds * 1000.0) );
  #endif
}





//! Static function for error reporting and handling using c-strings.
void handleError( const char *message, StkError::Type type )
{
    std::string msg( message );
    handleError( msg, type );    
}

//! Internal function for error reporting that assumes message in \c oStream_ variable.
void handleError( StkError::Type type )
{
    handleError( oStream_.str(), type );
    oStream_.str( std::string() ); // reset the ostringstream buffer
}



//! Static function for error reporting and handling using c++ strings.
void handleError( std::string message, StkError::Type type )
{
if ( type == StkError::WARNING || type == StkError::STATUS ) {
    if ( !showWarnings_ ) return;
    std::cerr << '\n' << message << '\n' << std::endl;
}
else if (type == StkError::DEBUG_PRINT) {
#if defined(_STK_DEBUG_)
    std::cerr << '\n' << message << '\n' << std::endl;
#endif
}
else {
    if ( printErrors_ ) {
    // Print error message before throwing.
    std::cerr << '\n' << message << '\n' << std::endl;
    }
    throw StkError(message, type);
}
}

//! Toggle display of WARNING and STATUS messages.
void showWarnings( bool status ) { showWarnings_ = status; }

//! Toggle display of error messages before throwing exceptions.
void printErrors( bool status ) { printErrors_ = status; }



}