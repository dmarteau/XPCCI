/* :::BEGIN LICENSE BLOCK:::
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
 *  :::END LICENSE BLOCK::: */

//--------------------------------------------------------
// File         : xpcciglue.cpp
// Date         : 7 may. 2019
// Author       : David Marteau
//--------------------------------------------------------

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

#include "xpcciglue.h"
#include "dmLoader.h"

#include "../../utilities/include/dmLoader.h"

#include "dmCrt.h"

#ifdef DM_CONFIG_TARGET_UNIX 
#include <unistd.h>
#endif

static dm_hmodule gXPCCIModule  = dm_null;
static bool    gInitialized = false;

extern "C" {

#undef  __daim_export
#define __daim_export

__daim_export cci_result __daim_call( DM_Initialize )(const char** argv, int argc, dmLOG_FUNCTION pfnLog) = dm_null;
__daim_export void       __daim_call( DM_Finalize   )(bool force) = dm_null;

}

template<class _F>
static inline bool __loadsym1( _F* _ppfn, const char* procname )
{
  *_ppfn = reinterpret_cast<_F>(__dm_dlsym(gXPCCIModule,procname));
  return *_ppfn!=NULL;
}

#define __loadsym( _ppfn )          \
  if(!__loadsym1(&_ppfn,#_ppfn)) {  \
    return CCI_ERROR_NOT_AVAILABLE; \
  }

#define DM_DLL DM_DLL_PREFIX "xpcci" DM_DLL_SUFFIX

#ifdef DM_CONFIG_TARGET_WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

cci_result DM_InitXPCCIGlue( const char* location, const char** argv, int argc, dmLOG_FUNCTION pfnLog )
{
  if(gInitialized)
     return CCI_ERROR_ALREADY_INITIALIZED;

  char libpath[MAX_PATH] = "\0";
  // Bootstrap the library
  snprintf(libpath,MAX_PATH-1,"%s" PATH_SEP "%s",location,DM_DLL);

  gXPCCIModule = __dm_dlopen( libpath );

  if(!gXPCCIModule)
  {
    __dm_log_dlerror();

    if(pfnLog)
       pfnLog("Failed to load daim library !",DM_LOG_ERROR);

    return CCI_ERROR_FAILURE;
  }

  __loadsym(DM_Initialize);
  __loadsym(DM_Finalize);

  cciPrivateHook** ppHook = (cciPrivateHook**)__dm_dlsym(gXPCCIModule,"_CCI_Hook");
  if(!ppHook)
    return CCI_ERROR_NOT_AVAILABLE;

  cci_result rv = DM_Initialize(argv,argc,pfnLog);
  if(CCI_SUCCEEDED(rv) || rv == CCI_ERROR_ALREADY_INITIALIZED) {
     rv = CCI_InitCOMGlue2(*ppHook);
  }

  gInitialized = true;
  return rv;
}

void DM_ReleaseXPCCIGlue( bool force )
{
  if(gInitialized)
    CCI_ReleaseCOMGlue2();

  if(DM_Finalize)
     DM_Finalize(force);

  if(gXPCCIModule)
     __dm_dlclose(gXPCCIModule);

  gXPCCIModule  = dm_null;
  gInitialized = false;
}
