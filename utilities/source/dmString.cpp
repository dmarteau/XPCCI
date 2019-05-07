/* ::: BEGIN LICENSE BLOCK:::
 * Copyright (c) 2005 David Marteau
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

/* $ChangeLog  v2.5.0, 22 ao�t 2006 01:22:14 , David
 *
 * Removed (useless and misplaced!) URL data handling
 *
 * Added file name normalization
 */

#include "dmUtilitiesBase.h"
#include "dmMemory.h"
#include "dmNew.h"
#include "dmShared.h"
#include "dmMutex.h"
#include "dmFastMemory.h"

#include "dmCrt.h"
#include "dmString.h"
#include "dmDate.h"
#include "dmTime.h"
#include "dmPortability.h"
#include "dmVector.h"

#include <algorithm>


#ifndef DM_CONFIG_NOALLOCPROXY
//-------------------------------------------------
// Use a proxy allocator for small strings (<PROXY_STRING)
// PROXY_STRING is then the smallest allocatable string
// It's not big deal because strings are shared among string
// instances, but it will prevent memory fragmentation
// and can even be faster than getting memory from system
// each time.
//-------------------------------------------------
#define PROXY_STRING 64
#define PROXY_BLOCKS 200

struct __dm_proxy_alloc : public dmMemoryManager
{
  __dm_proxy_alloc(size_t _chunkSize,const char* static_id) : dmMemoryManager(_chunkSize,static_id,true) {}
 ~__dm_proxy_alloc() {}

  void* allocate() {
    return dmMemoryManager::Allocate(PROXY_BLOCKS);
  }

  void deallocate( void* p ) {
    dmMemoryManager::Free(p);
  }
};

#endif // DM_CONFIG_NOALLOCPROXY


//-------------------------------------------------
// Allocator specific
// Ensure that there is always room for __EOS at end of allocated
// block
//-------------------------------------------------
template<class char_type>
class __dm_string_allocator
{
  public:
    typedef size_t             size_type;
    typedef int                difference_type;
    typedef char_type        *pointer;
    typedef const char_type  *const_pointer;
    typedef char_type&        reference;
    typedef const char_type&  const_reference;
    typedef char_type         value_type;

    pointer address(reference x) const             { return (&x); }
    const_pointer address(const_reference x) const { return (&x); }

    __dm_string_allocator& operator=(const __dm_string_allocator<char_type>& _toCopy) {
      return *this;
    }

    __dm_string_allocator() {}
    __dm_string_allocator(const __dm_string_allocator<char_type>& _A) { operator=(_A); }

    void construct(pointer p, value_type val) { *p = val; }
    void destroy(pointer p) { *p = end_of_string;  }
    void erase  (pointer p) { destroy(p); }

  #ifndef DM_CONFIG_NOALLOCPROXY

    static __dm_proxy_alloc __proxy;

    pointer allocate(size_type n, const void *hint)
    {
       pointer p = (n<PROXY_STRING
                    ? static_cast<char_type*>(__proxy.allocate())
                    : new (dm_arena) char_type[n+1]);

       std::fill(p,p+(n+1),end_of_string);
       return p;
    }

    void deallocate(pointer p, size_type n) {
      if( n < PROXY_STRING ) __proxy.deallocate(p);
      else ::operator delete [] (p,dm_arena);
    }

  #else  // DM_CONFIG_NOALLOCPROXY

    pointer allocate(size_type n, const void *hint)
    {
       pointer p = new (dm_arena) char_type[n+1];
       std::fill(p,p+(n+1),end_of_string);
       return p;
    }

    void deallocate(pointer p, size_type n) {
      ::operator delete [] (p,dm_arena);
    }

  #endif // DM_CONFIG_NOALLOCPROXY

  static const char_type end_of_string;
  static const char_type* white_spaces;
  static const char_type* empty_string;

};

#ifndef DM_CONFIG_NOALLOCPROXY
template<class char_type>
__dm_proxy_alloc __dm_string_allocator<char_type>::__proxy(PROXY_STRING*sizeof(char_type),"(strings allocator)");
#endif // DM_CONFIG_NOALLOCPROXY


//-------------------------------------------------
// String implementation is implemented as a shared
// vector of char_type
//-------------------------------------------------

//typedef dmVector<_CHAR_TYPE,__dm_string_allocator<_CHAR_TYPE> > dmStringVector;

template<class char_type>
class __dm_string_implementation :
            public dmVector<char_type,__dm_string_allocator<char_type> >,
            public dmShared
{
#ifndef DM_CONFIG_NOALLOCPROXY
  static __dm_proxy_alloc _Proxy;
#endif

  typedef dmVector<char_type,__dm_string_allocator<char_type> > super;

public:
  typedef typename super::iterator       iterator;
  typedef typename super::const_iterator const_iterator;
  typedef typename super::size_type      size_type;
  typedef typename super::value_type     value_type;
  typedef typename super::pointer        pointer;

  typedef typename super::allocator_type  allocator_type;

public:
  ~__dm_string_implementation() {}

   __dm_string_implementation() {}

   __dm_string_implementation( const __dm_string_implementation<char_type>& aImpl )
   : super(aImpl) {}

   __dm_string_implementation( size_t n, char_type c )
   : super(n,c) {}

   __dm_string_implementation( const char_type* aString )
   {
     Assign(aString,_dm_strlen(aString));
   }

   //-------------------------------------------------
   // Implement faster insertion for POD type
   //-------------------------------------------------
   iterator Insert(iterator __position,const_iterator _First, const_iterator _Last )
   {
     size_type __n        = __position - super::Begin();
     size_type __fill     = (_Last - _First) ;
     size_type __new_size = __fill + super::Size();
     if (super::Capacity() > __new_size) {
       if (__position == super::End())
         super::_M_End = this->_M_Uninitialized_Copy(_First,_Last,__position);
       else {
         this->_M_Backward_Copy(__position,super::_M_End,super::_M_End+__fill);
         this->_M_Copy(_First,_Last,__position);
         super::_M_End+=__fill;
       }
     }
     else
       this->_M_Insert_Overflow(__position,_First,_Last);

     return super::Begin() + __n;
   }


   iterator Insert(iterator __position,value_type __x, size_type __fill )
   {
     size_type __n        = __position - super::Begin();
     size_type __new_size = __fill + super::Size();
     this->Reserve(__new_size);
     __position = super::Begin() + __n;

     if (__position == super::End())
       super::_M_End = this->_M_Fill(__position,__x,__fill);
     else {
       this->_M_Backward_Copy(__position,super::_M_End-1,super::_M_End+__fill-1);
       this->_M_Fill(__position,__x,__fill);
       super::_M_End+=__fill;
     }

     return __position;
   }

   void Assign( const char_type* s, size_t n ) {
      super::Reserve(n);
      pointer p = this->_M_Copy(s,s+n,super::Begin()); // this prevent clang errors
      if(p < super::_M_End)
        this->_M_Destroy(p,super::_M_End);
      super::_M_End = p;
   }

   void Assign( const char_type c, size_t n ) {
     super::Reserve(n);
     pointer p = this->_M_Fill(super::Begin(),c,n);
      if(p < super::_M_End)
        this->_M_Destroy(p,super::_M_End);
      super::_M_End = p;
   }

   void Append( const_iterator _First, const_iterator _Last ) {
     Insert(super::End(),_First,_Last);
   }

   void Append( value_type __x, size_type __fill  ) {
     Insert(super::End(),__x,__fill);
   }

   void Eos() {
     while( *super::_M_End != allocator_type::end_of_string) {
        ++super::_M_End;
     }
   }

 #ifndef DM_CONFIG_NOALLOCPROXY

   // Will allocate __dm_string_implementation using proxy allocator

   void  operator delete( void* p ) { _Proxy.deallocate(p);  }
   void* operator new(size_t)       {
    return static_cast<__dm_string_implementation*>(_Proxy.allocate());
   }

 #endif // DM_CONFIG_NOALLOCPROXY

   static int _Format( __dm_string_implementation<char_type>* _impl, size_t n,
                        const char_type* fmt, va_list ap );

};

#ifndef DM_CONFIG_NOALLOCPROXY
// Memory pool for string implementation
template<class char_type>
__dm_proxy_alloc __dm_string_implementation<char_type>::_Proxy(sizeof(__dm_string_implementation<char_type>),"(strings implementation)");
#endif // DM_CONFIG_NOALLOCPROXY


#define __NPOS DM_UINT32_MAX

//------------------------------------------
#ifndef DM_CONFIG_PRINTF_BUFFER_SIZE
#define DM_CONFIG_PRINTF_BUFFER_SIZE 1024
#endif
//------------------------------------------
template<class char_type>
int __dm_string_implementation<char_type>::_Format(
        __dm_string_implementation<char_type>* _impl, size_t n,
       const char_type* fmt, va_list ap )
{
  if(n==__NPOS) n = DM_CONFIG_PRINTF_BUFFER_SIZE;

  // Make sure that we are not copying from the same buffer
  // otherwise fmt would become just garbage.
  __dm_string_implementation* _local_cpy = NULL;
  if(fmt>=_impl->Begin() && fmt <= _impl->End()) {
    _local_cpy = new __dm_string_implementation(fmt);
    fmt  = _local_cpy->Begin();
  }

  _impl->Reserve(n);
  int count = _dm_vsnprintf(_impl->Begin(),n,fmt,ap);

  //XXXDavid 05/07/2005
  // Use the number of char written returned by _vsntprintf

   _impl->_M_Set(_impl->_M_Begin,
                 _impl->_M_Begin+count,
                 _impl->_M_Last);

  *_impl->_M_End = __dm_string_allocator<char_type>::end_of_string;

  if(_local_cpy) delete _local_cpy;
  return count;
}
//------------------------------------------
// Helpers
//------------------------------------------
template<class char_type>
struct ___IsCharWithin
{
  typedef __dm_string_allocator<char_type>  alloc_type ;
  const char_type* s;
  ___IsCharWithin( const char_type* _s ) : s(_s) {}
  bool operator()( char_type c ) {
    for(const char_type* p = s;*p!=alloc_type::end_of_string;++p) {
      if(*p==c) return true;
    }
    return false;
  }
};

template<class char_type>
struct ___IsCharNotWithin
{
  typedef __dm_string_allocator<char_type>  alloc_type ;
  const char_type* s;
  ___IsCharNotWithin( const char_type* _s ) : s(_s) {}
  bool operator()( char_type c ) {
    for(const char_type* p = s;*p!=alloc_type::end_of_string;++p) {
     if(*p==c) return false;
    }
    return true;
  }
};
//------------------------------------------

#include "dm-string-template-def-char.h"
#include "dmTString.cpp"
#include "dm-string-template-undef.h"

#include "dm-string-template-def-wchar.h"
#include "dmTString.cpp"
#include "dm-string-template-undef.h"

