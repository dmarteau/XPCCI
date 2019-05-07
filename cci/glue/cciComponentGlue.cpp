
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
#include "cciIModule.h"
#include "cciIAllocator.h"
#include "cciString.h"

#include "cciPrivateHook.h"

extern "C" {

// Must be defined in module implementation
cci_result CCIGetModule ( cciIComponentManager* compMgr, const char* location,
                          cciIModule** result);

// Define default entrypoint for module
static cci_result
CCI_InitModule( cciModuleHook* _CCI_Hook, const char* location, cciIModule** _result )
{
  cci_result rv = CCI_InitCOMGlue2(_CCI_Hook->cc_Hooks);
  if(CCI_FAILED(rv))
     return rv;

  cciIComponentManager* compMgr;

  rv = CCI_GetComponentManager(&compMgr);
  if(CCI_SUCCEEDED(rv)) {
    rv = CCIGetModule(compMgr,location,_result);
  }

  CCI_IF_RELEASE(compMgr);

  return rv;
}

static
void CCI_ReleaseModule( struct cciModuleHook* )
{
  CCI_ReleaseCOMGlue2();
}

// Exported Module tag
struct cciModuleTag
__DLL_EXPORT_TAG _CCI_Module = {
    dm_make_id('C','C','I','M'),
    CCI_HOOK_VERSION,
    CCI_InitModule,
    CCI_ReleaseModule
};


} // Extern "C"
