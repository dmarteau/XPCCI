#ifndef dmNew_h
#define dmNew_h

/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2005 David Marteau
 *
 * This file is part of the DAIM Image Processing Library (DAIM library)
 *
 * The DAIM library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License (LGPL)
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * The DAIM library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the DAIM library; see the file LICENSE.  If
 * not, write to the Free Software Foundation, Inc., 59 Temple Place -
 * Suite 330, Boston, MA 02111-1307, USA.
 *
 *  ::: END LICENSE BLOCK::: */

//--------------------------------------------------------
// File         : dmNew.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#ifdef DM_UTILITIES

  #define dm_malloc_handler  dmMemory::Malloc
  #define dm_free_handler    dmMemory::Free
  #define dm_memcpy_handler  dmMemory::Memcpy
  #define dm_realloc_handler dmMemory::Realloc
  #define dm_memmove_handler dmMemory::Memmove
  #define dm_memset_handler  dmMemory::Memset

#else // DM_UTILITIES

  #define dm_malloc_handler  malloc
  #define dm_free_handler    free
  #define dm_memcpy_handler  memcpy
  #define dm_realloc_handler realloc
  #define dm_memmove_handler memmove
  #define dm_memset_handler  memset

#endif // DM_UTILITIES

#define DM_CPP_NEW_DEFINED

#include <new>

// GCC need this:

#ifdef DM_THROW_BAD_ALLOC_SIG
#define __DM_THROW_BAD_ALLOC() throw (std::bad_alloc)
#define __DM_THROW() throw()
#else
#define __DM_THROW_BAD_ALLOC()
#define __DM_THROW()
#endif

enum dmAllocType { dm_arena };

inline void* operator new    ( size_t n   , dmAllocType ) __DM_THROW_BAD_ALLOC() { return dm_malloc_handler(n); }
inline void  operator delete ( void*  p   , dmAllocType ) __DM_THROW()           { dm_free_handler(p);   }
inline void* operator new    [] ( size_t n, dmAllocType ) __DM_THROW_BAD_ALLOC() { return dm_malloc_handler(n); }
inline void  operator delete [] ( void*  p, dmAllocType ) __DM_THROW()           { dm_free_handler(p);    }

#define DM_DECL_ARENA_OPERATOR_NEW(_Instance) \
void* operator new(size_t n)     { return dm_malloc_handler(n); } \
void  operator delete( void* p ) { dm_free_handler(p);   }


// define C++ standartd allocator based on previous definition of
// memory functions

template <class T>
class dmAllocator {
  public:
    typedef T            value_type;
    typedef value_type*  pointer;
    typedef const T*     const_pointer;
    typedef T&           reference;
    typedef const T&     const_reference;
    typedef size_t       size_type;
    typedef int          difference_type;

    template <class U> struct rebind {
      typedef dmAllocator<U> other;
    };

   template <class U> dmAllocator(const dmAllocator<U>&) {}

    dmAllocator() {}
    dmAllocator(const dmAllocator<T>&) {}
   ~dmAllocator() {}

    pointer       address(reference x)       const { return &x; }
    const_pointer address(const_reference x) const { return &x; }

    T* allocate(size_type n, const void* = 0) {
       return n != 0
        ? reinterpret_cast<value_type*>(dm_malloc_handler(n * sizeof(value_type)))
        : NULL;
    }

    void deallocate(pointer p, size_type n) {
      //dmDEBUG_ASSERT( (__p == 0) == (__n == 0) )
      if(p != 0) dm_free_handler((void*)p);
    }

    void deallocate(pointer p) const {
      if(p != 0) dm_free_handler((void*)p);
    }

    void destroy(pointer p) { p->~T(); }

    template<class U>
    void construct(pointer p, const U& val) {
      new ((void*)p) T(val);
    }

    size_type max_size() const { return size_t(-1) / sizeof(value_type); }
 };

#endif // dmNew_h
