%{
#include "Mutex.h"

%}

namespace stk
{
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

    class Thread : public Stk
    {
    public:

        Thread();
        ~Thread();

        bool start( THREAD_FUNCTION routine, void * ptr = NULL );
        bool cancel(void);
        bool wait(void);
        void testCancel(void);
    };
}