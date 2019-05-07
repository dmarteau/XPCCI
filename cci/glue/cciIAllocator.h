#ifndef cciIAllocator_h
#define cciIAllocator_h

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


#ifndef cciISupportsBase_h
#include "cciISupportsBase.h"
#endif

/* starting interface:    cciIAllocator */
#define CCI_IALLOCATOR_IID_STR "64e50197-248a-49a0-b8a0-aee53d6b4452"

#define CCI_IALLOCATOR_IID \
  {0x64e50197, 0x248a, 0x49a0, \
	 { 0xb8, 0xa0, 0xae, 0xe5, 0x3d, 0x6b, 0x44, 0x52 }}


class CCI_NO_VTABLE cciIAllocator : public cciISupports
{
 public:

  CCI_DECLARE_STATIC_IID_ACCESSOR(CCI_IALLOCATOR_IID);

  CCI_IMETHOD_(void*)      NewObject( size_t n, CCI_STATIC_DESTRUCTOR _dtor ) = 0;
  CCI_IMETHOD_(cci_result) RevokeObject( void* p ,bool discard ) = 0;

  CCI_IMETHOD_(bool)    IsObjectValid(void* p) = 0;
  CCI_IMETHOD_(dm_uint32)  NumberOfInstances() = 0;

};

CCI_DEFINE_STATIC_IID_ACCESSOR(cciIAllocator, CCI_IALLOCATOR_IID)

//---------------------------------------------------------

#endif // cciIAllocator_h
