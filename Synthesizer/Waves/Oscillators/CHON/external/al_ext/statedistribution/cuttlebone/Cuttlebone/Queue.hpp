#ifndef __QUEUE__
#define __QUEUE__

#include <atomic>
#include <cstring>      // memcpy
#include <type_traits>  // for is_pod
#include <stdlib.h>     // for size_t

namespace cuttlebone {

template <typename DATA, size_t SIZE = 10>
class Queue {
  //  static_assert(std::is_pod<DATA>::value, "DATA must be a POD type.");

  enum {
    CAPACITY = SIZE + 1
  };

 private:
  inline size_t increment(size_t i) const { return (i + 1) % CAPACITY; }
  std::atomic<size_t> _tail;  // tail(input) index
  std::atomic<size_t> _head;  // head(output) index
  DATA _array[CAPACITY];

 public:
  Queue() : _tail(0), _head(0) {}
  virtual ~Queue() {}

  bool push(const DATA& item) {  // pushByMOve?
    const auto current_tail = _tail.load(std::memory_order_relaxed);
    const auto next_tail = increment(current_tail);
    if (next_tail != _head.load(std::memory_order_acquire)) {
      // slow! _array[current_tail] = item;
      memcpy(&_array[current_tail], &item, sizeof(DATA));
      _tail.store(next_tail, std::memory_order_release);
      return true;
    }
    return false;  // full queue
  }

  // Pop by Consumer can only update the head (load with relaxed, store with
  // release) the tail must be accessed with at least aquire
  bool pop(DATA& item) {
    const auto current_head = _head.load(std::memory_order_relaxed);
    if (current_head == _tail.load(std::memory_order_acquire))
      return false;  // empty queue
    // slow! item = _array[current_head];
    memcpy(&item, &_array[current_head], sizeof(DATA));
    _head.store(increment(current_head), std::memory_order_release);
    return true;
  }
};

}  // cuttlebone

#endif
