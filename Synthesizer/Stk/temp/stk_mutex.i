%{
#include "Mutex.h"
%}

namespace stk {

    /***************************************************/
    /*! \class Mutex
        \brief STK mutex class.
        This class provides a uniform interface for
        cross-platform mutex use.  On Linux and IRIX
        systems, the pthread library is used. Under
        Windows, critical sections are used.
        by Perry R. Cook and Gary P. Scavone, 1995--2021.
    */
    /***************************************************/

    class Mutex : public Stk
    {
    public:

        Mutex();
        ~Mutex();

        void lock(void);
        void unlock(void);
        void wait(void);
        void signal(void);

    };
}