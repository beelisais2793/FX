#include "Thread.h"

Mutex mutex;
volatile int run = 0;
volatile int end_thread = 0;
    
void *run_thread(void * ptr) {
    Thread * t = (Thread*)ptr;
    while(!end_thread) {
        if(run == 1) {
            mutex.lock();
            swiglua_ref_get(&t->ref);
            if(lua_pcall(t->ref.L,0,0,0) != 0) 
                printf("%s\n",lua_tostring(t->ref.L,-1));
            mutex.unlock();
        }
        usleep(10);
    }
    return ptr;
}    