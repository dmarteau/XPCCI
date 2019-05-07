
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

#ifndef XPCCI_GLUE
#error You must compile with XPCCI_GLUE !
#endif

#include "cciCore.h"
#include "cciStubs.h"
#include "cciString.h"
#include "cciPrivateHook.h"

extern "C" {

#undef  __daim_export
#define __daim_export

__daim_export void*  __daim_call( CCI_MemAlloc   )(size_t size);
__daim_export void   __daim_call( CCI_MemFree    )(void* memblock);
__daim_export void*  __daim_call( CCI_MemRealloc )(void *memblock, size_t );
__daim_export void*  __daim_call( CCI_MemClone   )(const void *src, size_t);

__daim_export void   __daim_call( CCI_DebugBreak )(dm_uint32,const char*,const char*,
                                                   const char*,dm_uint32);

__daim_export void*       __daim_call( CCI_NewObject    )(size_t n, CCI_STATIC_DESTRUCTOR );
__daim_export void        __daim_call( CCI_RevokeObject )(void* p ,bool );
__daim_export cci_result  __daim_call( CCI_VerifyObject )(void* p);

__daim_export cci_result  __daim_call( CCI_GetComponentManager  )(cciIComponentManager**);
__daim_export cci_result  __daim_call( CCI_GetServiceManager    )(cciIServiceManager**);
__daim_export cci_result  __daim_call( CCI_GetComponentRegistrar)(cciIComponentRegistrar**);
__daim_export cci_result  __daim_call( CCI_NewGenericModule     )(cciModuleInfo*,cciIModule**);
__daim_export cci_result  __daim_call( CCI_NewGenericFactory    )(cciIGenericFactory* *result, const cciModuleComponentInfo*);

__daim_export cci_result  __daim_call( CCI_CStringContainerInit   )(cciCStringContainer &aContainer);
__daim_export void        __daim_call( CCI_CStringContainerFinish )(cciCStringContainer &aContainer);
__daim_export cci_result  __daim_call( CCI_CStringSetData   )(dmACString &aStr, const char  *aData, dm_uint32 aDataLength);
__daim_export dm_uint32   __daim_call( CCI_CStringGetData   )(const dmACString &aStr, const char **aData );
__daim_export cci_result  __daim_call( CCI_CStringAppendData)(dmACString &aStr, const char  *aData, dm_uint32 aDataLength );
__daim_export cci_result  __daim_call( CCI_CStringCopy      )(dmACString &aDestStr, const dmACString& aSrcStr );

__daim_export cci_result  __daim_call( CCI_StringContainerInit   )(cciStringContainer &aContainer);
__daim_export void        __daim_call( CCI_StringContainerFinish )(cciStringContainer &aContainer);
__daim_export cci_result  __daim_call( CCI_StringSetData   )(dmAString &aStr, const dm_unichar  *aData, dm_uint32 aDataLength);
__daim_export dm_uint32   __daim_call( CCI_StringGetData   )(const dmAString &aStr, const dm_unichar **aData );
__daim_export cci_result  __daim_call( CCI_StringAppendData)(dmAString &aStr, const dm_unichar *aData, dm_uint32 aDataLength );
__daim_export cci_result  __daim_call( CCI_StringCopy      )(dmAString &aDestStr, const dmAString& aSrcStr );


} // extern "C"

