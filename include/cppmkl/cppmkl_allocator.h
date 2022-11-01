#ifndef __CPPMKL_ALLOCATOR_H__
#define __CPPMKL_ALLOCATOR_H__

#include <new>
#include <cstddef>
#include <mkl.h>

namespace cppmkl
{
  template <typename T>
    class cppmkl_allocator
    {
      public:
        typedef T                 value_type;
        typedef value_type*       pointer;
        typedef const value_type* const_pointer;
        typedef value_type&       reference;
        typedef const value_type& const_reference;
        typedef std::size_t       size_type;
        typedef std::ptrdiff_t    difference_type;
        pointer address(reference x) const {return &x;}
        const_pointer address(const_reference x) const { return &x;}
        pointer allocate(size_type n, const_pointer=0)
        {
          void *p = MKL_malloc(n*sizeof(T), 128);
          if(!p)
          {
            throw std::bad_alloc();
          }
          return static_cast<pointer>(p);
        }
        void deallocate(pointer p, size_type)
        {
          MKL_free(p);
        }
        size_type max_size() const { 
          return static_cast<size_type>(-1) / sizeof(value_type);
        }

        void construct(pointer p, const value_type& x) { 
          new(p) value_type(x); 
        }
        void destroy(pointer p) { p->~value_type(); }

        template <class U> cppmkl_allocator(const cppmkl_allocator<U>&) {}

        template <class U> struct rebind { typedef cppmkl_allocator<U> other; };

        cppmkl_allocator() {}
        cppmkl_allocator(const cppmkl_allocator&) {}
        ~cppmkl_allocator() {}

      private:
        void operator=(const cppmkl_allocator&);
    };

  template<> 
    class cppmkl_allocator<void>
    {
      typedef void        value_type;
      typedef void*       pointer;
      typedef const void* const_pointer;

      template <class U> 
        struct rebind { typedef cppmkl_allocator<U> other; };
    };

  template<class T>
  inline bool operator==(const cppmkl_allocator<T>&, 
      const cppmkl_allocator<T>&) {
    return true;
  }

  template <class T>
    inline bool operator!=(const cppmkl_allocator<T>&, 
        const cppmkl_allocator<T>&) {
      return false;
    }

}

#endif
