#pragma once
#include "Stk.hpp"

#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  #include <pthread.h>
  typedef pthread_mutex_t MUTEX;
  typedef pthread_cond_t CONDITION;

#elif defined(__OS_WINDOWS__)

  #include <windows.h>
  #include <process.h>
  typedef CRITICAL_SECTION MUTEX;
  typedef HANDLE CONDITION;

#endif

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
  //! Default constructor.
  Mutex();

  //! Class destructor.
  ~Mutex();

  //! Lock the mutex.
  void lock(void);

  //! Unlock the mutex.
  void unlock(void);

  //! Wait indefinitely on the mutex condition variable.
  /*!
    The mutex must be locked before calling this function, and then
    subsequently unlocked after this function returns.
   */
  void wait(void);

  //! Signal the condition variable.
  /*!
    The mutex must be locked before calling this function, and then
    subsequently unlocked after this function returns.
   */
  void signal(void);

 protected:

  MUTEX mutex_;
  CONDITION condition_;

};

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

inline
Mutex::Mutex()
{

#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  pthread_mutex_init(&mutex_, NULL);
  pthread_cond_init(&condition_, NULL);

#elif defined(__OS_WINDOWS__)

  InitializeCriticalSection(&mutex_);
  condition_ = CreateEvent(NULL,  // no security
                           true,  // manual-reset
                           false, // non-signaled initially
                           NULL); // unnamed

#endif 
}

inline
Mutex::~Mutex()
{
#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  pthread_mutex_destroy(&mutex_);
  pthread_cond_destroy(&condition_);

#elif defined(__OS_WINDOWS__)

  DeleteCriticalSection(&mutex_);
  CloseHandle( condition_ );

#endif 
}

inline
void Mutex::lock()
{
#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  pthread_mutex_lock(&mutex_);

#elif defined(__OS_WINDOWS__)

  EnterCriticalSection(&mutex_);

#endif 
}

inline
void Mutex::unlock()
{
#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  pthread_mutex_unlock(&mutex_);

#elif defined(__OS_WINDOWS__)

  LeaveCriticalSection(&mutex_);

#endif 
}

inline
void Mutex::wait()
{
#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  pthread_cond_wait(&condition_, &mutex_);

#elif defined(__OS_WINDOWS__)

  WaitForMultipleObjects(1, &condition_, false, INFINITE);

#endif 
}

inline
void Mutex::signal()
{
#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  pthread_cond_signal(&condition_);

#elif defined(__OS_WINDOWS__)

  SetEvent( condition_ );

#endif 
}

} // stk namespace
