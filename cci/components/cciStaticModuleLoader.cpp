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

#include "cciPrivate.h"
#include "cciComponentManager.h"
#include "cciIGenericFactory.h"
#include "cciMemory.h"
#include "cciIModule.h"


#define STATIC_MODULE_ENTRY_BLOCKS 20

/**
 * StaticModule entry
 */
class cciStaticModuleEntry
{
public:
  cciStaticModuleEntry( const cciStaticModuleInfo* info  )
  : mInfo(info)
  {}

  cci_Ptr<cciIModule>        mModule;
  const cciStaticModuleInfo *mInfo;

  DM_DECL_ALLOCATOR_NEW(cciStaticModuleEntry,STATIC_MODULE_ENTRY_BLOCKS)
};

DM_IMPL_ALLOCATOR_NEW(cciStaticModuleEntry,true);

//======================================
// cciStaticModuleLoader implementation
//======================================

// This component is fully owned by the component manager

CCI_IMPL_QUERY_INTERFACE1(cciStaticModuleLoader,cciIModuleLoader);
CCI_IMPL_ADDREF_USING_AGGREGATOR(cciStaticModuleLoader,cciComponentManagerImpl::gComponentManager);
CCI_IMPL_RELEASE_USING_AGGREGATOR(cciStaticModuleLoader,cciComponentManagerImpl::gComponentManager);

cci_result
cciStaticModuleLoader::Init(cciStaticModuleInfo const *aStaticModules,
                            dm_uint32 aStaticModuleCount,
                            bool bRegister)
{
  dmCString name;

  for(dm_uint32 i=0; i<aStaticModuleCount; ++i)
  {
    const cciStaticModuleInfo* staticModule = &aStaticModules[i];

    name = staticModule->name;

#ifdef _DEBUG
    if(mModules.find(name)!=mModules.end()) {
      CCI_ERROR("Static module already exists !");
      return CCI_ERROR_FAILURE;
    }
#endif

    cciStaticModuleEntry *entry = new cciStaticModuleEntry(staticModule);
    if(!entry)
       return CCI_ERROR_OUT_OF_MEMORY;

    mModules[name] = entry;

    /*
     * Register static modules in the order they come in the static list
     */

    if(bRegister)
    {
      cci_Ptr<cciIModule> module;
      cci_result rv = staticModule->getModule(cciComponentManagerImpl::gComponentManager,
                                              dm_null,getter_AddRefs(module));

      if(CCI_SUCCEEDED(rv))
      {
        if(!module) {
          dmLOG("GetModule succeeded without returning a module - %s",staticModule->name);
          return CCI_ERROR_FAILURE;
        }

        rv = module->RegisterSelf(cciComponentManagerImpl::gComponentManager,
                                  staticModule->name,dm_null,staticComponentType);
        if(CCI_FAILED(rv)) {
          dmLOG("RegisterSelf failed for static module %s",staticModule->name);
          return CCI_ERROR_FAILURE;
        }

      } else {
        dmLOG("GetModule failed for static module %s",staticModule->name);
        return rv;
      }
    }
  }

  return CCI_OK;
}

cciStaticModuleEntry*
cciStaticModuleLoader::FindModuleEntry(const dmCString& aLocation)
{
  cciStaticModuleEntry *entry = dm_null;

  dmHashPtrDict::iterator _found  = mModules.find(aLocation);
  if(_found != mModules.end())
     entry = reinterpret_cast<cciStaticModuleEntry*>((*_found).second);

  return entry;
}

cci_result
cciStaticModuleLoader::LoadStaticModule(const dmCString& location,cciIModule **aResult)
{
  cciStaticModuleEntry *entry = FindModuleEntry(location);
  if(entry)
  {
    if(!entry->mModule)
    {
      const cciStaticModuleInfo* info = entry->mInfo;

      cci_result rv = info->getModule(cciComponentManagerImpl::gComponentManager,
                             dm_null,getter_AddRefs(entry->mModule));

      if(CCI_SUCCEEDED(rv))
      {
        if(!entry->mModule) {
          dmLOG("GetModule succeeded without returning a module - %s",info->name);
          return CCI_ERROR_FAILURE;
        }
      } else {
        dmLOG("GetModule failed for static module %s",info->name);
        return rv;
      }
    }

    CCI_ADDREF(*aResult = entry->mModule);
    return CCI_OK;
  }

  return CCI_ERROR_NOT_AVAILABLE;
}

void cciStaticModuleLoader::ReleaseModules()
{
  dmHashPtrDict::iterator it  = mModules.begin();
  dmHashPtrDict::iterator end = mModules.end();

  cciStaticModuleEntry* entry;
  while(it!=end)
  {
    entry = reinterpret_cast<cciStaticModuleEntry*>((*it).second);
    delete entry;
    ++it;
  }

  mModules.clear();
}

//=====================
// cciIModuleLoader
//=====================
CCI_IMETHODIMP
cciStaticModuleLoader::LoadModule(const char* location, cciIModule* *aResult)
{
  return LoadStaticModule(dmCString(location),aResult);
}
