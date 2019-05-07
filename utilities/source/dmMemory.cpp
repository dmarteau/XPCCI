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

 /*
  * Revision 1.1  2006/08/10 16:39  daim
  * Implemented 'dmMemory::Clone'
  */

#if   defined(DM_CONFIG_TARGET_UNIX)
  #include "plateform/linux/_dmMemory.cpp"
#elif defined(DM_CONFIG_TARGET_WIN32)
  #include "plateform/win/_dmMemory.cpp"
#endif // DM_CONFIG_TARGET

static void* _MemClone( const void* src, size_t size )
{
  void* dst = dmMemory::Malloc(size);
  if(dst)
     dmMemory::Memcpy(dst,src,size);

  return dst;
}

void* (*dmMemory::Clone)   ( const void* src, size_t size )  = _MemClone;


#ifndef DM_ENABLE_TRACE_MEM



#endif
