#pragma once

#include <pthread.h>
#include <unistd.h>

    struct Mutex {
        pthread_mutex_t mutex;

        Mutex() {
            pthread_mutex_init(&mutex,NULL);
        }
        ~Mutex() {
            pthread_mutex_destroy(&mutex);
        }

        void lock() {
            pthread_mutex_lock(&mutex);
        }
        void unlock() {
            pthread_mutex_unlock(&mutex);
        }
    };

    extern Mutex mutex;
    extern volatile int run;
    extern volatile int end_thread;
    
    void *run_thread(void * ptr);

    struct Thread
    {
        SWIGLUA_REF ref;
        pthread_t thread;

        Thread(SWIGLUA_REF cb) { 
            pthread_create(&thread,NULL,*run_thread,(void*)this);
            ref = cb; 
        }
        ~Thread() {
            end_thread = 1;
            pthread_join(thread,NULL);            
        }
        void Start() {
            run = 1;
        }
        void Stop() {
            run = 0;
        }
    };
