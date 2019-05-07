
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

#include "dm_utilities.h"
#include "dmTraceMem.h"

#include <stdio.h>

#ifdef DM_ENABLE_TRACE_MEM

#define DEBUG_MEMTAG dm_make_id('M','D','B','G')

static size_t __dm_TRACE_MEMORY = 0;
static size_t __dm_ALLOC_MEMORY = 0;
static size_t __dm_FREED_MEMORY = 0;
static size_t __dm_MOVED_MEMORY = 0;
static size_t __dm_USAGE_MEMORY = 0;

#define EXTRA_MEM  2*sizeof(size_t)
#define OFFSET_MEM 2

static void* (*gMallocProc)  ( size_t size    );
static void  (*gFreeProc)    ( void *memblock );
static void* (*gReallocProc) ( void *memblock, size_t size );

#if defined(_DEBUG) && defined(DM_DEBUG_TRACE_MEMALLOC)
  #define DUMP_TRACEMEM() \
    fprintf(stderr, " ##MEMORY: %ld\n",__dm_ALLOC_MEMORY - __dm_FREED_MEMORY);
#else
  #define DUMP_TRACEMEM() //
#endif

//------------------------------------------
static void* __dm_Debug_Malloc( size_t _size )
{
  size_t* _dbg_ptr = static_cast<size_t*>( gMallocProc(_size + EXTRA_MEM ) );
  if(_dbg_ptr) {
    __dm_ALLOC_MEMORY += _size;

    _dbg_ptr[0] = DEBUG_MEMTAG;
    _dbg_ptr[1] = _size;
  }
  DUMP_TRACEMEM()

  size_t current = __dm_ALLOC_MEMORY - __dm_FREED_MEMORY;
  if(current > __dm_USAGE_MEMORY)
    __dm_USAGE_MEMORY = current;

  return (_dbg_ptr + OFFSET_MEM);
}
//------------------------------------------
static void __dm_Debug_Free( void* _ptr )
{
  if(_ptr)
  {
    size_t* _dbg_ptr = static_cast<size_t*>(_ptr) - OFFSET_MEM;
    DM_ABORT_IF_FALSE( _dbg_ptr[0] == DEBUG_MEMTAG, "Memory corruption");
    __dm_FREED_MEMORY += _dbg_ptr[1];
    DUMP_TRACEMEM()
    gFreeProc(_dbg_ptr);
  }
}
//------------------------------------------
static void* __dm_Debug_Realloc( void* _ptr , size_t _size )
{
  if(_ptr != NULL )
  {
    if(_size > 0)
    {
      size_t* _dbg_ptr = static_cast<size_t*>(_ptr) - OFFSET_MEM;
      DM_ABORT_IF_FALSE( _dbg_ptr[0] == DEBUG_MEMTAG, "Memory corruption");
      __dm_MOVED_MEMORY += _dbg_ptr[1];
      __dm_FREED_MEMORY += _dbg_ptr[1];
      __dm_ALLOC_MEMORY += (_dbg_ptr[1] = _size);
      _dbg_ptr =  static_cast<size_t*>( gReallocProc(_dbg_ptr,_size + EXTRA_MEM ) );
      DUMP_TRACEMEM()
      _ptr = (_dbg_ptr + OFFSET_MEM);
    }
    else
    {
      size_t* _dbg_ptr = static_cast<size_t*>(_ptr) - OFFSET_MEM;
      DM_ABORT_IF_FALSE( _dbg_ptr[0] == DEBUG_MEMTAG, "Memory corruption");
      __dm_FREED_MEMORY += _dbg_ptr[1];
      DUMP_TRACEMEM()
      _ptr = gReallocProc(_dbg_ptr,0);
    }
  }
  else // null ptr: alloc memory
  {
    if(_size > 0)
    {
      size_t* _dbg_ptr = static_cast<size_t*>(gReallocProc(_ptr,_size + EXTRA_MEM ));
      if(_dbg_ptr) {
        __dm_ALLOC_MEMORY += _size;
        _dbg_ptr[0] = DEBUG_MEMTAG;
        _dbg_ptr[1] = _size;
        DUMP_TRACEMEM()
        _ptr = (_dbg_ptr + OFFSET_MEM);
      }
    }
    else
      _ptr = NULL;
  }

  size_t current = __dm_ALLOC_MEMORY - __dm_FREED_MEMORY;
  if(current > __dm_USAGE_MEMORY)
    __dm_USAGE_MEMORY = current;

  return _ptr;
}
//------------------------------------------
bool dmTraceMem::IsTracing()
{
  return DM_LIKELY(__dm_TRACE_MEMORY == 1);
}
//------------------------------------------
void dmTraceMem::Init( dm_int32 status )
{
  if(!__dm_TRACE_MEMORY)
  {
     gMallocProc  = dmMemory::Malloc;
     gFreeProc    = dmMemory::Free;
     gReallocProc = dmMemory::Realloc;

     dmMemory::Malloc   = __dm_Debug_Malloc;
     dmMemory::Free     = __dm_Debug_Free;
     dmMemory::Realloc  = __dm_Debug_Realloc;

     __dm_TRACE_MEMORY = 1;
  }
}
//------------------------------------------
void dmTraceMem::Dump( dm_int32 status )
{
  if(!__dm_TRACE_MEMORY) {
    fprintf(stdout,"\nMemory usage is not traced...\n");
  } else
  if(status==1)
  {
    fprintf(stderr, "\nMemory report :\n");
    fprintf(stderr, "** ALLOC MEMORY (CUMULATED) : %lu\n",__dm_ALLOC_MEMORY);
    fprintf(stderr, "** FREED MEMORY (CUMULATED) : %lu\n",__dm_FREED_MEMORY);
    fprintf(stderr, "** MOVED MEMORY (CUMULATED) : %lu\n",__dm_MOVED_MEMORY);
    fprintf(stderr, "** MAX MEMORY USAGE         : %lu\n",__dm_USAGE_MEMORY);
    if(__dm_ALLOC_MEMORY > __dm_FREED_MEMORY) {
      size_t _bytes_leaked =  __dm_ALLOC_MEMORY - __dm_FREED_MEMORY;
      fprintf(stderr,"=> LEAKED : %lu\n",_bytes_leaked);
    }
  } else {
    fprintf(stderr,"** MEMORY USAGE: %lu\n",__dm_ALLOC_MEMORY - __dm_FREED_MEMORY);
  }

  fflush(stderr);
}
//------------------------------------------

#else // DM_ENABLE_TRACE_MEM

void dmTraceMem::Init( dm_int32 status )
{
  fprintf(stderr,"Sorry, memory tracing is disabled in this version !");
  fflush(stderr);
}
//------------------------------------------
void dmTraceMem::Dump( dm_int32 status )
{
  fprintf(stderr,"\nMemory usage is not traced...\n");
  fflush(stderr);
}
//------------------------------------------
bool dmTraceMem::IsTracing()
{
  return false;
}
//------------------------------------------

#endif // DM_ENABLE_TRACE_MEM
