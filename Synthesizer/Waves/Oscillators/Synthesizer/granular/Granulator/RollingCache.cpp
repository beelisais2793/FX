/*
  ==============================================================================

    RollingCache.cpp
    Created: 29 Mar 2021 11:40:54pm
    Author:  ciaran

  ==============================================================================
*/

#include "RollingCache.h"
#include <JuceHeader.h>

RollingCache::RollingCache() : RollingCache(0) {};

RollingCache::RollingCache(uint max_capacity) : capacity{max_capacity} {
  buffer.resize(max_capacity);
};

void RollingCache::write(float value) {
  if (capacity == 0)
    return;
  buffer[head] = value;
  inc_head();
}

void RollingCache::read_chunk(size_t chunk_size, int offset, std::vector<float> &dest) {
  jassert(dest.size() >= chunk_size);
  if (!is_full())
    DBG("CAN'T READ FROM UNFILLED BUFFER");
  int start = wrap_index((int) head - (int) chunk_size - offset);

  int targetIndex = wrap_index(head - offset);
  for (int i = 0; i < chunk_size; ++i) {
    dest[i] = buffer[wrap_index(start + i)];
  }
}

void RollingCache::set_capacity(uint new_size) {
  jassert(new_size <= buffer.size());

  for (int samp = 0; samp < new_size; ++samp) {
    buffer[samp] = 0.f;
  }
  capacity = new_size;
  head = 0;
  filled = false;
}

size_t RollingCache::get_capacity() { return capacity; }
bool RollingCache::is_full() { return filled; }

int RollingCache::wrap_index(int i) {
  while (i < 0)
    i += capacity;
  return i % capacity;
}

void RollingCache::inc_head() {
  if (!filled && head == capacity - 1) filled = true;
  head = wrap_index(head + 1);
}