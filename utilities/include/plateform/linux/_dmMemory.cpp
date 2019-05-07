//--------------------------------------------------------
// Memory handlers for Linux
//--------------------------------------------------------
#include <stdlib.h>
#include <string.h>

static int _MemStatLinux( size_t blks, size_t blk_size, int flags )
{
/*
  size_t requiredSize;
  size_t totalSize, availSize;

  requiredSize = blks * blk_size;

  size_t pageSize  = sysconf(_SC_PAGESIZE);
  size_t pageCount = sysconf(_SC_PHYS_PAGES);

  totalSize = pageSize * pageCount;
  availSize = XXX How to get available size ?

  availSize -= requiredSize;

  return ( ((float)availSize/totalSize) < 0.1 ?
         dmMemory::E_MEM_LOW :
         dmMemory::E_MEM_OK);

*/
  return dmMemory::E_MEM_OK;
}


void* (*dmMemory::Malloc)  ( size_t )                     = ::malloc;
void  (*dmMemory::Free)    ( void* )                      = ::free;
void* (*dmMemory::Realloc) ( void*, size_t )              = ::realloc;
void* (*dmMemory::Memcpy)  ( void*, const void*, size_t ) = ::memcpy;
void* (*dmMemory::Memmove) ( void*, const void*, size_t ) = ::memmove;
void* (*dmMemory::Memset)  ( void*, int, size_t )         = ::memset;
void* (*dmMemory::Calloc)  ( size_t nmemb, size_t size )  = ::calloc;

int   (*dmMemory::Memstat) ( size_t blks, size_t blk_size, int flags ) = _MemStatLinux;
