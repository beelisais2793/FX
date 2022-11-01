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


typedef void* (*ThreadFunction)(void*);
struct Thread
{        
    pthread_t thread;

    Thread(ThreadFunction cb,void * data) { 
        pthread_create(&thread,NULL,cb,data);            
    }
    ~Thread() {            
        pthread_join(thread,NULL);            
    }
    void Start() {
        
    }
    void Stop() {
        
    }
};

