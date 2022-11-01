#ifndef __CPPMKL_MATRIX_H__
#define __CPPMKL_MATRIX_H__
#include <assert.h>
#include <stddef.h>
#include <vector>
#include <mkl.h>

#include "cppmkl/cppmkl_allocator.h"

namespace cppmkl
{
  /**
 * example of a matrix type that can be used with cppmkl wrapper functions
 * the std::vector used for underlying storage uses the cppmkl allocator
 * so storage should be properly aligned; using vector here alleviates us 
 * from any storage management complexity, which is nice.
 * This matrix can double up as a column or row vector, just set the other
 * dimension size to 1, i.e. you can pass it to wrapped functions expecting
 * a conformant vector type as it has data() and size().
 */
  template<typename T>  
  class matrix
  {
    std::vector<T, cppmkl::cppmkl_allocator<T> > d;
    size_t s1;
    size_t s2;
    public:
    typedef T value_type;
    matrix(): s1(0), s2(0)
    {
    }
    matrix(size_t r, size_t c):s1(r), s2(c)
    {
      d.resize(r*c);
    }
    matrix(const std::vector<T> _d, size_t r, size_t c):s1(r), s2(c)
    {
      d.resize(r*c);
      for(size_t i=0;i<d.size();++i) { d[i] = _d[i];}
    }
    // creates new matrix of specified size
    void resize(size_t r, size_t c)
    {
      d.resize(r*c);
      s1 = r;
      s2 = c;
    }
    T& operator()(size_t row, size_t col)
    {
      assert(row<s1);
      assert(col<s2);
      return d[row*s2+col];
    }
    const T& operator()(size_t row, size_t col) const
    {
      assert(row<s1);
      assert(col<s2);
      return d[row*s2+col];
    }
    T* data() { return &d[0]; }
    const T* data() const { return &d[0]; }
    size_t size1() const { return s1; }
    size_t size2() const { return s2; }
    size_t size() const { return s1*s2; } 
  };
  template<typename T>
  inline const T* ptr_to_first(const cppmkl::matrix<T>& m)
  {
    const T* ptr = m.data();
    return ptr;
  }
  template<typename T>
  inline T* ptr_to_first(cppmkl::matrix<T>& m)
  {
    T* ptr = m.data();
    return ptr;
  }
}

#endif
