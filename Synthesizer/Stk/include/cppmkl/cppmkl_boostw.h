#ifndef __CPPMKL_BOOSTW_H__
#define __CPPMKL_BOOSTW_H__

namespace cppmkl
{
  template<typename T>
  inline const T* ptr_to_first(const boost::numeric::ublas::matrix<T>& m)
  {
    const T* ptr = &(m.data()[0]);
    return ptr;
  }
  template<typename T>
  inline T* ptr_to_first(boost::numeric::ublas::matrix<T>& m)
  {
    T* ptr = &(m.data()[0]);
    return ptr;
  }
}

#endif
