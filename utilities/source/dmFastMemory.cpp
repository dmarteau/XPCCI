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

#include "dm_utilities.h"
#include "dmFastMemory.h"
#include "dmCrt.h"
//---------------------------------------------------------------------------
dmMemoryManager::dmMemoryManager( size_t _sz, const char* static_id, bool _release_when_unusued )
:chunks(NULL)
,blocks(0)
,chunkSize(_sz)
,count(0)
,freeStore(0)
,release_when_unusued(_release_when_unusued)
{
#ifdef _DEBUG
  if(static_id)
     fprintf(stderr,"dmMemoryManager %p : initializing %s\n",this,static_id);
#endif
}
//---------------------------------------------------------------------------
dmMemoryManager::~dmMemoryManager()
{
  // We do a 'Free If Possible' here in order
  // to not release memory that could be holded
  // by some objects not released (for example in case of exception)
  if(count==0)
     Free();
#ifdef _DEBUG
  else
     fprintf(stderr,"dmMemoryManager %p : Cannot release memory (%d object not free'd)!\n",this,count);
#endif
}
//---------------------------------------------------------------------------
void dmMemoryManager::Free()
{
  dmCRITICAL_SECTION( lock )

  if(chunks!=NULL)
  {
    #ifdef _DEBUG
     fprintf(stderr,"dmMemoryManager %p : Freeing %ld chunk of blocks of size %ld\n",this,blocks,sizeof(__MemChunk)+chunkSize);
    #endif

    for(int i=blocks;--i>=0;)
      dmMemory::Free(chunks[i]);

    freeStore = NULL;
    dmMemory::Realloc(chunks,0);
    blocks = 0;
    chunks = NULL;
  }
}
//---------------------------------------------------------------------------
void* dmMemoryManager::Allocate( size_t _count )
{
  dmCRITICAL_SECTION( lock )

  __MemChunk *p, *last;
  if (!freeStore)
  {
    size_t _sz = sizeof(__MemChunk)+chunkSize;
    freeStore  = p = static_cast<__MemChunk*>( dmMemory::Malloc( _count*_sz ) );

    #ifdef _DEBUG
     fprintf(stderr,"dmMemoryManager %p : Allocating %ld blocks of size %ld\n",this,_count,_sz);
    #endif

    chunks = reinterpret_cast<void**>(dmMemory::Realloc(chunks,++blocks * sizeof(void*)));
    chunks[blocks-1] = freeStore;

    last = reinterpret_cast<__MemChunk*>( reinterpret_cast<dm_byte*>(freeStore) + (_count-1)*_sz );
    for(;p!=last;p->_next = reinterpret_cast<__MemChunk*>( reinterpret_cast<dm_byte*>(p)+_sz ), p=p->_next );
    p->_next = 0;
  }

  ++count;

  p = freeStore;
  freeStore = freeStore->_next;
  return (p+1);
}
//----------------------------------------------------------
void dmMemoryManager::Free( void* p )
{
  dmCRITICAL_SECTION( lock )

  --count;

  __MemChunk* chunk = ( static_cast<__MemChunk*>(p) - 1 );
  chunk->_next = freeStore;
  freeStore    = chunk;

  if(release_when_unusued && count == 0)
     Free();
}
//----------------------------------------------------------
