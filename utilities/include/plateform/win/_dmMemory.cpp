//--------------------------------------------------------
// Memory handlers for Win32
//--------------------------------------------------------

static int _MemStatWin32( size_t blks, size_t blk_size, int flags )
{
  DWORD requiredSize;
  DWORD totalSize, availSize;

  requiredSize = blks * blk_size;

  MEMORYSTATUS stat;
  GlobalMemoryStatus(&stat);

  if( flags == dmMemory::E_MEM_PHYSICAL ) {
    availSize = stat.dwAvailPhys; 
    totalSize = stat.dwTotalPhys;
  } else {
    availSize = stat.dwAvailPageFile;
    totalSize = stat.dwTotalPageFile;
  }

  if( requiredSize >= availSize)
      return dmMemory::E_MEM_LOW;

  availSize -= requiredSize;

  return ( ((float)availSize/totalSize) < 0.1 ? 
         dmMemory::E_MEM_LOW :
         dmMemory::E_MEM_OK);
}


void* (*dmMemory::Malloc)  ( size_t )                     = ::malloc;
void  (*dmMemory::Free)    ( void* )                      = ::free;
void* (*dmMemory::Realloc) ( void*, size_t )              = ::realloc;
void* (*dmMemory::Memcpy)  ( void*, const void*, size_t ) = ::memcpy;
void* (*dmMemory::Memmove) ( void*, const void*, size_t ) = ::memmove;
void* (*dmMemory::Memset)  ( void*, int, size_t )         = ::memset;
void* (*dmMemory::Calloc)  ( size_t nmemb, size_t size )  = ::calloc;

int   (*dmMemory::Memstat) ( size_t blks, size_t blk_size, int flags ) = _MemStatWin32;
