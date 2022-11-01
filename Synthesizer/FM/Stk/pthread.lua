-- it won't work from inside lua
ffi = require('ffi')
ffi.cdef[[
typedef unsigned long int pthread_t;

/* these numbers may be different on other machines this is 64-bit linux/gcc
# define __SIZEOF_PTHREAD_ATTR_T        64
# define __SIZEOF_PTHREAD_MUTEX_T       48
# define __SIZEOF_PTHREAD_MUTEXATTR_T    8
# define __SIZEOF_PTHREAD_CONDATTR_T     8
# define __SIZEOF_PTHREAD_RWLOCK_T      56
# define __SIZEOF_PTHREAD_BARRIER_T     32
# define __SIZEOF_PTHREAD_BARRIERATTR_T  8
*/
    typedef void pthread_mutex_t;
    typedef void pthread_mutexattr_t;
    int pthread_mutex_destroy(pthread_mutex_t *mutex);
    int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
    int pthread_mutex_lock(pthread_mutex_t *mutex);
    int pthread_mutex_trylock(pthread_mutex_t *mutex);
    int pthread_mutex_unlock(pthread_mutex_t *mutex); 
]]
pthread = ffi.load('pthread')