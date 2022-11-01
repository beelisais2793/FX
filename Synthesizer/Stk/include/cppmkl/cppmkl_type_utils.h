#ifndef __CPPMKL_TYPE_UTILS_H__
#define __CPPMKL_TYPE_UTILS_H__

namespace cppmkl
{
  template<typename MV_T>
  inline const typename MV_T::value_type* ptr_to_first(const MV_T& m)
  {
    const typename MV_T::value_type* ptr = &*m.begin();
    return ptr;
  }
  template<typename MV_T>
  inline typename MV_T::value_type* ptr_to_first(MV_T& m)
  {
    typename MV_T::value_type* ptr = &*m.begin();
    return ptr;
  }
  
}

#endif
