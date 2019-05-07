#ifndef cciMemory_h
#define cciMemory_h

#ifndef cciCore_h
#include "cciCore.h"
#endif

extern "C" {

__daim_export void*  __daim_call( CCI_MemAlloc   )(size_t size);
__daim_export void   __daim_call( CCI_MemFree    )(void* memblock);
__daim_export void*  __daim_call( CCI_MemRealloc )(void *memblock, size_t size );
__daim_export void*  __daim_call( CCI_MemClone   )(const void *src, size_t size );

}

class cciMemory {
  public:
    static void* Alloc   ( size_t size    )              { return CCI_MemAlloc(size); }
    static void  Free    ( void *memblock )              { return CCI_MemFree(memblock); }
    static void* Realloc ( void *memblock, size_t size ) { return CCI_MemRealloc(memblock,size); }
    static void* Clone   ( const void *src, size_t size ){ return CCI_MemClone(src,size); }
};

#endif // cciStubs_h
