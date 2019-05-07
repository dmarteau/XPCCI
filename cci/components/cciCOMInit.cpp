
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
#include "cciComponentManager.h"
#include "cciCategoryManager.h"
#include "cciAllocatorImpl.h"

bool gCCIShuttingDown = true;

extern "C" {

__daim_export cci_result
CCI_Init(cciIServiceManager* *result,
         cciStaticModuleInfo const *aStaticModules,
         dm_uint32 aStaticModuleCount,
         bool registerStaticModules)
{
  // Init allocators
  cci_result rv = cciAllocatorBase::InitAllocatorState();
  if(CCI_FAILED(rv))
     return rv;

  cciComponentManagerImpl* cmpMngr = new cciComponentManagerImpl();
  if(!cmpMngr)
     return CCI_ERROR_OUT_OF_MEMORY;

  CCI_ADDREF(cmpMngr);

  cciComponentManagerImpl::gComponentManager = cmpMngr;

  // Officially start cci
  gCCIShuttingDown = false;

  // Register category manager
  cci_Ptr<cciIFactory> categoryManagerFactory;
  rv = CCI_CategoryManagerGetFactory(getter_AddRefs(categoryManagerFactory));
  if(CCI_FAILED(rv))
     return rv;

  CCI_DEFINE_CID(kCategoryManagerCID,CCI_CATEGORYMANAGER_CID);

  rv = cmpMngr->Init(aStaticModules,aStaticModuleCount,registerStaticModules);
  if(CCI_FAILED(rv))
     return rv;

  rv = cmpMngr->RegisterFactory(kCategoryManagerCID,
                                CCI_CATEGORYMANAGER_CLASSNAME,
                                CCI_CATEGORYMANAGER_CONTRACTID,
                                categoryManagerFactory);
  if(result)
  {
     cciIServiceManager *serviceManager =
            static_cast<cciIServiceManager*>(cmpMngr);

     CCI_ADDREF(*result = serviceManager);
  }

  return rv;
}

__daim_export cci_result
CCI_Shutdown(cciIServiceManager* servMgr)
{
  // Officially stop CCI
  gCCIShuttingDown = true;

  CCI_IF_RELEASE(servMgr);

  if(cciComponentManagerImpl::gComponentManager)
     cciComponentManagerImpl::gComponentManager->FreeServices();

  // Release the global component manager
  if(cciComponentManagerImpl::gComponentManager) {
    cci_refcnt cnt;
    CCI_RELEASE2(cciComponentManagerImpl::gComponentManager, cnt);
    CCI_ASSERTION(cnt == 0, "Component Manager being held past CCI shutdown.");
  }

  cciComponentManagerImpl::gComponentManager = dm_null;

  cciAllocatorBase::ReleaseAllocatorState();

  return CCI_OK;
}

__daim_export void
CCI_DebugBreak(dm_uint32 aSeverity,
                 const char *aStr,
                 const char *aExpr,
                 const char *aFile, dm_uint32 aLine)
{
  dmDebugBreak(aSeverity,aStr,aExpr,aFile,aLine);
}


} // extern "C"
