/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org Code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1999
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either of the GNU General Public License Version 2 or later (the "GPL"),
 * or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK *****
 * This Original Code has been modified by IBM Corporation.
 * Modifications made by IBM described herein are
 * Copyright (c) International Business Machines
 * Corporation, 2000
 *
 * Modifications to Mozilla code or documentation
 * identified per MPL Section 3.3
 *
 * Date             Modified by     Description of modification
 * 04/20/2000       IBM Corp.      Added PR_CALLBACK for Optlink use in OS2
 *
 * Original code modified by David Marteau
 * for CCI COM implementation
 */

#include "cciCOM.h"
#include "cciPrivate.h"
#include "cciComponentManager.h"
#include "cciIGenericFactory.h"
#include "cciMemory.h"
#include "cciString.h"
#include "cciPrivateHook.h"
#include "cciIModule.h"

#include "dmLoader.h"

////////////////////////////////////////////////////////////////////////////////
// Module Private hook
// Define hooks used by glue client to populate CCI method stubs
////////////////////////////////////////////////////////////////////////////////

extern "C" {

static cciPrivateHook gPrivateHooks = {
    CCI_HOOK_VERSION,

    CCI_GetServiceManager,
    CCI_GetComponentManager,
    CCI_GetComponentRegistrar,

    CCI_NewGenericModule,
    CCI_NewGenericFactory,
    CCI_NewAllocator,

    CCI_MemAlloc,
    CCI_MemFree,
    CCI_MemRealloc,
    CCI_MemClone,

    CCI_DebugBreak,

    CCI_CStringContainerInit,
    CCI_CStringContainerFinish,
    CCI_CStringSetData,
    CCI_CStringGetData,
    CCI_CStringAppendData,
    CCI_CStringCopy,

    CCI_StringContainerInit,
    CCI_StringContainerFinish,
    CCI_StringSetData,
    CCI_StringGetData,
    CCI_StringAppendData,
    CCI_StringCopy
};

// Export hooks
struct cciPrivateHook __DLL_EXPORT_TAG *_CCI_Hook = &gPrivateHooks;

}

#define NATIVE_MODULE_ENTRY_BLOCKS 50

/**
 * NativeModule entry
 */
class cciNativeModuleEntry
{
public:
  cciNativeModuleEntry( const dmCString& loc  )
  : mLocation(loc)
  , mIdle(0)
  {}

  cciModuleHook          mHook;
  cci_Ptr<cciIModule>    mModule;
  dmCString              mLocation;
  long                   mIdle;

  DM_DECL_ALLOCATOR_NEW(cciNativeModuleEntry,NATIVE_MODULE_ENTRY_BLOCKS)
};

DM_IMPL_ALLOCATOR_NEW(cciNativeModuleEntry,true);

//======================================
// cciNativeModuleLoader implementation
//======================================

// This component is fully owned by the component manager

CCI_IMPL_QUERY_INTERFACE1(cciNativeModuleLoader,cciIModuleLoader);
CCI_IMPL_ADDREF_USING_AGGREGATOR(cciNativeModuleLoader,cciComponentManagerImpl::gComponentManager);
CCI_IMPL_RELEASE_USING_AGGREGATOR(cciNativeModuleLoader,cciComponentManagerImpl::gComponentManager);

cci_result
cciNativeModuleLoader::Init()
{
  return CCI_OK;
}

cciNativeModuleEntry*
cciNativeModuleLoader::FindModuleEntry(const dmCString& aLocation)
{
  cciNativeModuleEntry *entry = dm_null;

  dmHashPtrDict::iterator _found  = mModules.find(aLocation);
  if(_found != mModules.end())
     entry = reinterpret_cast<cciNativeModuleEntry*>((*_found).second);

  return entry;
}

cci_result
cciNativeModuleLoader::ResolveKeyToFile(const char* locationKey,dmCString& aFile)
{
  // Find module in CCI_COMPONENTS_PATH
  aFile = dmPortability::SearchEnv(CCI_COMPONENTS_PATH_ENV,locationKey);
  if(aFile.IsEmpty())
  {
    dmCString _search = dmPortability::GetEnv(CCI_COMPONENTS_PATH_ENV);

    dmLOG_ERROR("CCI: Module %s not found in %s\n",locationKey,_search.get());
    return CCI_ERROR_FAILURE;
  }

  return CCI_OK;
}

cci_result
cciNativeModuleLoader::LoadNativeModule(const dmCString& location, cciIModule* *aResult)
{
  cciNativeModuleEntry *entry = FindModuleEntry(location);
  if(entry)
  {
    CCI_ADDREF( *aResult = entry->mModule );
    return CCI_OK;
  }

  dmLOG("CCI: Loading module %-48s",location.get());
  dm_hmodule library = __dm_dlopen( location.get() );

  cci_result rv;

  if(library)
  {
    // Look for "_CCI_Module" symbol
    cciModuleTag *tag = (struct cciModuleTag*)__dm_dlsym( library,"_CCI_Module" );

    rv = LoadNativeModuleTag(tag,location,library,aResult);
    if(CCI_FAILED(rv))
    {
      dmLOG_ERROR("Failed to load module %s",location.get());
      __dm_dlclose( library ); // Close module
    }
  }
  else
  {
    __dm_log_dlerror();     // Log native loader error
    rv = CCI_ERROR_FAILURE;
  }

  return rv;
}

cci_result cciNativeModuleLoader::LoadNativeModuleTag( cciModuleTag *tag, const dmCString& location,
                                                       dm_hmodule library, cciIModule **aResult )
{
  cci_result rv;

  // Magic ?
  if(tag && tag->ccih_Magic == dm_make_id('C','C','I','M')
         && tag->ccih_TagVersion <= CCI_HOOK_VERSION)
  {
    // Allocate a hook structure
    cciNativeModuleEntry* entry = new cciNativeModuleEntry(location);
    if(!entry)
       return CCI_ERROR_OUT_OF_MEMORY;

    // Initialize 'hook' struct
    entry->mHook.cc_Tag     = tag;
    entry->mHook.cc_Library = library;
    entry->mHook.cc_Hooks   = &gPrivateHooks;

    // Call 'init(hook)'
    rv = tag->ccih_Init(&entry->mHook,location.get(),getter_AddRefs(entry->mModule));
    if(CCI_SUCCEEDED(rv))
    {
      mModules[location] = entry;

      CCI_IF_ADDREF( *aResult = entry->mModule );

      rv = CCI_OK;
    }
    else
    {
      delete entry;
      rv = CCI_ERROR_FAILURE;
    }
  }
  else
  {
    // OOps ! we tried to load something not a CCI module
    rv = CCI_ERROR_INVALID_ARG;
  }

  return rv;
}

void cciNativeModuleLoader::UnloadModuleEntry( cciNativeModuleEntry* entry )
{
  dmLOG("CCI: UnLoading module %s",entry->mLocation.get());

  cciModuleHook *hook = &entry->mHook;

  cciModuleTag  *tag  = hook->cc_Tag;     // Get the  'tag' pointer
  dm_hmodule library  = hook->cc_Library;

  entry->mModule = dm_null; // Release module;

  if(tag->ccih_Close)
     tag->ccih_Close(hook); // Call the 'close()' function

  if(library)
     __dm_dlclose(library); // UnLoad
}

void cciNativeModuleLoader::UnloadModules()
{
  dmHashPtrDict::iterator it  = mModules.begin();
  dmHashPtrDict::iterator end = mModules.end();

  cciNativeModuleEntry* entry;
  while(it!=end)
  {
    entry = reinterpret_cast<cciNativeModuleEntry*>((*it).second);
    UnloadModuleEntry(entry);

    delete entry;

    ++it;
  }

  mModules.clear();
}

//=====================
// cciIModuleLoader
//=====================

CCI_IMETHODIMP
cciNativeModuleLoader::LoadModule(const char* locationKey, cciIModule* *aResult)
{
  dmCString location;

  cci_result rv = ResolveKeyToFile(locationKey,location);
  if(CCI_SUCCEEDED(rv))
     rv = LoadNativeModule(location,aResult);

  return rv;
}

