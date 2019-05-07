#ifndef dmMemory_h
#define dmMemory_h

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
// File         : dmMemory.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

 /*
  * Revision 1.1  2006/08/10 16:39  daim
  * added 'Clone' method
  */

class __dmUtilities dmMemory {
  public:
    static void* (*Malloc)  ( size_t size    );
    static void  (*Free)    ( void *memblock );
    static void* (*Realloc) ( void *memblock, size_t size );
    static void* (*Memmove) ( void *dest, const void *src, size_t count );
    static void* (*Memcpy)  ( void *dest, const void *src, size_t count );
    static void* (*Memset)  ( void *dest, int c, size_t count );
    static void* (*Calloc)  ( size_t nmemb, size_t size );
    static void* (*Clone)   ( const void *src, size_t size );

    // Return the status of the available memory. The size
    // passed willk be added to the computation of allocated memory.
    // This enable to estimate the resource that could be required.
    // @param blks     the number of blocks to be checked 
    // @param blk_size size of blocks (in bytes) 

    static int   (*Memstat) ( size_t blks, size_t blk_size, int flags );

    enum { E_MEM_PAGE = 0, E_MEM_PHYSICAL = 1 };
    enum { E_MEM_OK   = 0, E_MEM_LOW = 1      };    
};

#endif // dmMemory_h
