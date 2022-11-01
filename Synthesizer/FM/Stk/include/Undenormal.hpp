#pragma once

#include <xmmintrin.h>

class Undenormal
{
private:
    
    const int maskFTZ = 0x8000;    // Mask to switch FLUSH TO ZERO mode
    const int maskDAZ = 0x0040;    // Mask to switch DENORMALS ARE ZERO mode
    
public:
    
    Undenormal ()
    {
        // Set FLUSH TO ZERO mode to ON state
    #if defined (_MM_SET_FLUSH_ZERO_MODE) && defined (_MM_FLUSH_ZERO_ON)
        _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    #else
        _mm_setcsr(_mm_getcsr() | maskFTZ);
    #endif
        
        // Set DENORMALS ARE ZERO mode to ON state
    #if defined (_MM_SET_DENORMALS_ZERO_MODE) && defined(_MM_DENORMALS_ZERO_ON)
        _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
    #else
        _mm_setcsr(_mm_getcsr() | maskDAZ);
    #endif
    }
            
    ~Undenormal ()
    {
        // Set FLUSH TO ZERO mode back to OFF state
    #if defined (_MM_SET_FLUSH_ZERO_MODE) && defined (_MM_FLUSH_ZERO_OFF)
        _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_OFF);
    #else
        _mm_setcsr(_mm_getcsr() & ~maskFTZ);
    #endif
        
        // Set DENORMALS ARE ZERO mode back to OFF state
    #if defined (_MM_SET_DENORMALS_ZERO_MODE) && defined (_MM_DENORMALS_ZERO_OFF)
        _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_OFF);
    #else
        _mm_setcsr(_mm_getcsr() & ~maskDAZ);
    #endif
    }
    
}; // end class RCH::Undenormal

