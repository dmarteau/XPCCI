
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
#include "cciId.h"
#include "cciStubs.h"
#include "cciMemory.h"
#include "cciSupportsUtils.h"
#include "cciIGenericFactory.h"
#include "cciIComponentManager.h"
#include "cciIAllocator.h"
#include "cciInterfaceRequestorUtils.h"
#include "cciString.h"

#include "cciPrivateHook.h"

extern "C" {

static cciIAllocator* gAllocator = dm_null;

static void* _CCI_NewObject(size_t n, CCI_STATIC_DESTRUCTOR _dtor )
{
  if(gAllocator)
     return gAllocator->NewObject(n,_dtor);

  return dm_null;
}

static void _CCI_RevokeObject(void* p ,bool _revoke )
{
  if(gAllocator)
     gAllocator->RevokeObject(p,_revoke);
}

static cci_result _CCI_VerifyObject(void* p)
{
  if(gAllocator)
     return gAllocator->IsObjectValid(p)?CCI_OK:CCI_ERROR_INVALID_POINTER;

  return CCI_ERROR_FAILURE;
}


static bool cci_glue_initialized = false;

cci_result
CCI_InitCOMGlue2( cciPrivateHook * pHook )
{
  if(cci_glue_initialized)
     return CCI_OK;

  if(!pHook)
   return CCI_ERROR_UNEXPECTED;

  CCI_MemAlloc   = pHook->memAllocProcPtr;
  CCI_MemFree    = pHook->memFreeProcPtr;
  CCI_MemRealloc = pHook->memReallocProcPtr;
  CCI_MemClone   = pHook->memCloneProcPtr;

  CCI_DebugBreak = pHook->debugBreakProcPtr;

  // Get an allocator
  cci_result rv = pHook->newAllocatorProcPtr(&gAllocator);
  if(CCI_FAILED(rv))
     return rv;

  CCI_NewObject              = _CCI_NewObject;
  CCI_RevokeObject           = _CCI_RevokeObject;
  CCI_VerifyObject           = _CCI_VerifyObject;

  CCI_GetComponentManager    = pHook->getComponentManagerProcPtr;
  CCI_GetServiceManager      = pHook->getServiceManagerProcPtr;
  CCI_GetComponentRegistrar  = pHook->getComponentRegistrarProcPtr;
  CCI_NewGenericModule       = pHook->newGenericModuleProcPtr;
  CCI_NewGenericFactory      = pHook->newGenericFactoryProcPtr;

  CCI_CStringContainerInit   = pHook->cstringContainerInitProcPtr;
  CCI_CStringContainerFinish = pHook->cstringContainerFinishProcPtr;
  CCI_CStringSetData         = pHook->cstringSetDataProcPtr;
  CCI_CStringGetData         = pHook->cstringGetDataProcPtr;
  CCI_CStringAppendData      = pHook->cstringAppendDataProcPtr;
  CCI_CStringCopy            = pHook->cstringCopyProcPtr;

  CCI_StringContainerInit    = pHook->stringContainerInitProcPtr;
  CCI_StringContainerFinish  = pHook->stringContainerFinishProcPtr;
  CCI_StringSetData          = pHook->stringSetDataProcPtr;
  CCI_StringGetData          = pHook->stringGetDataProcPtr;
  CCI_StringAppendData       = pHook->stringAppendDataProcPtr;
  CCI_StringCopy             = pHook->stringCopyProcPtr;

  cci_glue_initialized = true;

  return CCI_OK;
}

void CCI_ReleaseCOMGlue2()
{
  CCI_IF_RELEASE(gAllocator);

  cci_glue_initialized = false;
}

} // Extern "C"
