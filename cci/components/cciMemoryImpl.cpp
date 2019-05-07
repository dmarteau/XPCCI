
/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2008 David Marteau
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

#include "cciPrivate.h"
#include "cciMemory.h"

extern "C" {

__daim_export void*
CCI_MemAlloc(size_t size)
{
  return dmMemory::Malloc(size);
}
__daim_export void
CCI_MemFree(void* memblock)
{
  dmMemory::Free(memblock);
}

__daim_export void*
CCI_MemRealloc(void *memblock, size_t size )
{
  return dmMemory::Realloc(memblock,size);
}

__daim_export void*
CCI_MemClone(const void *src, size_t size )
{
  return dmMemory::Clone(src,size);
}


} // extern "C"

