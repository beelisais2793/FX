int main() {}
/*
#include <pthread.h>

pid_t child_pid;
struct sched_param old_param;
struct pthread_mutex_t mutex;

void raise_child_priority() {
  struct sched_param current;

  // Get the priority of the current thread
  sched_getparam(0, &current);

  // Get the priority of the child thread
  sched_getparam(child_pid, &old_param);

  // If the current thread's priority is greater than the
  // child's priority, raise the child's priority
  if(current.sched_priority > old_param.sched_priority) {
    sched_setparam(child_pid, &current);
  }
}

void lower_child_priority() {
  // Set the child thread's priority back to its old priority
  sched_setparam(child_pid, &old_param);
}

int access_shared_resource() {
  // First, try to get a lock on the resource
  if(pthread_mutex_trylock(&mutex) < 0) {
    if(errno == EBUSY) {
      // The other thread has the resource, 
      // raise its priority then perform a blocking lock
      raise_child_priority();
      if(pthread_mutex_lock(&mutex) < 0) return -1;
    }
    else {
      return -1;
    }
  }

  // Lower the child's priority back to normal
  lower_child_priority();

  //Do stuff with the locked resource

  // Release the lock
  if(pthread_mutex_unlock(&lock) < 0) return -1;
  return 0;
}

int main() {
}


//////////////////////////

#include <pthread.h>

int start_thread_with_priority(pthread_t* thread, (*start_routine)(void*), void* arg) {
  struct pthread_attr_t attr;
  struct sched_param param;
  
  pthread_attr_init(&attr);
  pthread_attr_setschedpolicy(&attr, SCHED_RR);
  param.sched_priority = 50;
  pthread_attr_setschedparam(&attr, &param);

  return pthread_create(thread, &attr, start_routine, arg)
}


////////////////////////////////////////

#include <fcntl.h>
#include <linux/mc146818rtc.h>
#include <sys/ioctl.h>

// The file descriptor for the rtc device
int rtc_fd;

// This will hold the status response from 
// /dev/rtc when an interrupt returns
unsigned long rtc_status;

int thread_main(void*)
{
  // Open the /dev/rtc device file
  rtc_fd = open("/dev/rtc", O_RDONLY);
  if(rtc_ft < 0) return -1;

  // Enable periodic interrupts, and set the interval to 32 Hz
  if(ioctl(rtc_fd, RTC_PIE_ON, 0) < 0) return -1;
  if(ioctl(rtc_fd, RTC_IRQP_SET, 32) < 0) return -1;
  
  while(!video_finished()) {
    show_next_frame();

    // Read from the rtc device. 
    // This will return when the next timer interrupt is raised
    if(read(rtc_fd, &data, sizeof(unsigned long)) < 0) break;

    // Check for missed interrupts, and skip frames if necessary.
    // We need to do the shift because the lower byte of the 
    //  returned unsigned long contains the type of the interrupt
    //  (i.e. whether it's an alarm or a periodic interrupt)
    data >>= 8;
    while(data-- > 1) skip_frame();
  }

  // Turn the periodic interrupt off
  if(ioctl(rtc_fd, RTC_PIE_OFF, 0) < 0) return -1;

  return 0;
}
*/
