#ifndef dmFastMemory_h
#define dmFastMemory_h

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
// File         : dmFastMemory.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

class __dmUtilities dmMemoryManager
{
private:
  struct __MemChunk  { __MemChunk*  _next; };
  struct __Storage   { __Storage*   _next; void* _pool; };

  void**             chunks;
  size_t             blocks;
  size_t             chunkSize;
  size_t             count;
  __MemChunk*        freeStore;

  dmMutex            lock;

  bool            release_when_unusued;

public:
  dmMemoryManager( size_t _chunkSize, const char* id = "",
                   bool _release_when_unusued = false );

  virtual ~dmMemoryManager();

  void Free();

  void* Allocate(size_t _blockCount );
  void  Free(void* p);

  size_t Count() { return count; }
};
//---------------------------------------------------------------------------
template<class T>
class dmFastMemory : public dmMemoryManager
{
 public:
  ~dmFastMemory() {}
   dmFastMemory(const char* id = "", bool _release_when_unusued = false)
   : dmMemoryManager(sizeof(T),id,_release_when_unusued)
   {}
};

#define DM_DECL_ALLOCATOR_NEW(_Instance,blks)  \
private:                                    \
  static dmFastMemory<_Instance> _MemPool;  \
public:                                     \
  void* operator new(size_t )      { return _MemPool.Allocate(blks); } \
  void  operator delete( void* p ) { _MemPool.Free(p);  }

#define DM_IMPL_ALLOCATOR_NEW(_Instance,free_when_unusued)  \
dmFastMemory<_Instance> _Instance::_MemPool(#_Instance,free_when_unusued);

//---------------------------------------------------------------------------
// STL Allocator type
//---------------------------------------------------------------------------
template<class T>
struct _sharedMem : public dmMemoryManager, public dmShared
{
  ~_sharedMem() {}
   _sharedMem(const char* id = "") : dmMemoryManager(sizeof(T),id) {}
};
//---------------------------------------------------------------------------

#define __dmALLOCATOR_DEFAULT_SZ 200

template<class T>
class dmFastAllocator
{
  typedef _sharedMem<T> _MemPool_type;
  private:
    int            _BlockCount;
    _MemPool_type* _MemPool;

    dmFastAllocator<T>& operator=(const dmFastAllocator<T>& _toCopy) {}

  public:
    typedef size_t    size_type;
    typedef int       difference_type;
    typedef T        *pointer;
    typedef const T  *const_pointer;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef T         value_type;

    typedef _sharedMem<T> mem_pool_type;

    pointer       address(reference x)       const { return (&x); }
    const_pointer address(const_reference x) const { return (&x); }

    virtual ~dmFastAllocator() { _MemPool->Release(); }

    dmFastAllocator(const dmFastAllocator& _A) {
      _BlockCount = _A._BlockCount;
      _MemPool    = _A._MemPool;
      _MemPool->Obtain();
    }

    dmFastAllocator( size_t _nBlocks = __dmALLOCATOR_DEFAULT_SZ,_sharedMem<T>* _Mem = NULL )
    {
      _BlockCount = _nBlocks;
      _MemPool    = _Mem;
      if(_MemPool==NULL) _MemPool = new _MemPool_type("dmFastAllocator");
      else               _MemPool->Obtain();
    }

    pointer allocate(size_type n, const void * =NULL) {
      return n==1
         ? static_cast<pointer>(_MemPool->Allocate(_BlockCount))
         : static_cast<pointer>(dmMemory::Malloc(n*sizeof(T)));
    }

    void deallocate(pointer p, size_type n) {
      if(n==1) _MemPool->Free(p);
      else     dmMemory::Free(p);
    }

    void destroy(pointer p) { p->~T(); }

    template<class U>
    void construct(pointer p, const U& val) {
       new ((void*)p) T(val);
    }

    template<class U>
    pointer new_object( const U& val )
    {
      pointer p = (pointer)_MemPool->Allocate(_BlockCount);
      construct(p,val);
      return p;
    }

    void delete_object( pointer p ) { destroy(p); _MemPool->Free(p); }

    void   set_block_size( size_t N ) { _BlockCount = N;    }
    size_t get_block_size()           { return _BlockCount; }

    mem_pool_type* get_memory_pool() const { return _MemPool; }
};

//---------------------------------------------------------------------------
#endif
