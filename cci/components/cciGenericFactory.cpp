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
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Pierre Phaneuf <pp@ludusdesign.com>
 *   David Marteau (CCI implementation)
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
 * ***** END LICENSE BLOCK ***** */

#include "cciGenericFactory.h"
#include "cciComponentManagerUtils.h"
#include "cciServiceManagerUtils.h"
#include "cciIComponentRegistrar.h"
#include "cciMemory.h"

#include "cciPrivate.h"

// Required on Linux for strlen/strcpy
#include <string.h>

cciGenericFactory::cciGenericFactory(const cciModuleComponentInfo *info)
    : mInfo(info)
{
  if(mInfo && mInfo->mClassInfoGlobal)
    *mInfo->mClassInfoGlobal = static_cast<cciIClassInfo *>(this);
}

cciGenericFactory::~cciGenericFactory()
{
    if (mInfo)
    {
      if(mInfo->mFactoryDestructor)
         mInfo->mFactoryDestructor();
      if(mInfo->mClassInfoGlobal)
        *mInfo->mClassInfoGlobal = 0;
    }
}

CCI_IMPL_THREADSAFE_ISUPPORTS3(cciGenericFactory,
                                  cciIGenericFactory,
                                  cciIFactory,
                                  cciIClassInfo);

//===================
// cciIGenericFactory
//===================

// cciIGenericFactory: component-info accessors
CCI_IMETHODIMP cciGenericFactory::SetComponentInfo(const cciModuleComponentInfo *info)
{
    if(mInfo && mInfo->mClassInfoGlobal)
      *mInfo->mClassInfoGlobal = 0;

    mInfo = info;

    if(mInfo && mInfo->mClassInfoGlobal)
      *mInfo->mClassInfoGlobal = static_cast<cciIClassInfo *>(this);

    return CCI_OK;
}

CCI_IMETHODIMP cciGenericFactory::GetComponentInfo(const cciModuleComponentInfo **infop)
{
    *infop = mInfo;
    return CCI_OK;
}


//===================
// cciIFactory
//===================

CCI_IMETHODIMP cciGenericFactory::CreateInstance(cciISupports *aOuter,
                                                 DM_REFIID aIID, void **aResult)
{
    if(mInfo->mConstructor) {
       return mInfo->mConstructor(aOuter, aIID, aResult);
    }

    return CCI_ERROR_FACTORY_NOT_REGISTERED;
}

CCI_IMETHODIMP cciGenericFactory::LockFactory(bool aLock)
{
    return CCI_OK;
}


//===================
// cciIClassInfo
//===================


CCI_IMETHODIMP cciGenericFactory::GetInterfaces(dm_uint32 *countp,dmIID* **array)
{
  if(!mInfo->mGetInterfacesProc) {
      *countp = 0;
      *array = dm_null;
      return CCI_OK;
  }
  return mInfo->mGetInterfacesProc(countp, array);
}

CCI_IMETHODIMP cciGenericFactory::GetHelperForLanguage(dm_uint32 language,
                                                     cciISupports **helper)
{
    if (mInfo->mGetLanguageHelperProc)
        return mInfo->mGetLanguageHelperProc(language, helper);
    *helper = dm_null;
    return CCI_OK;
}

CCI_IMETHODIMP cciGenericFactory::GetContractID(char **aContractID)
{
    if(mInfo->mContractID)
    {
        *aContractID = (char *)cciMemory::Alloc(strlen(mInfo->mContractID) + 1);
        if(!*aContractID)
           return CCI_ERROR_OUT_OF_MEMORY;
        strcpy(*aContractID, mInfo->mContractID);
    } else {
      *aContractID = dm_null;
    }
    return CCI_OK;
}

CCI_IMETHODIMP cciGenericFactory::GetClassDescription(char **aClassDescription)
{
  if(mInfo->mDescription)
  {
     *aClassDescription = (char *)cciMemory::Alloc(strlen(mInfo->mDescription) + 1);
     if(!*aClassDescription)
       return CCI_ERROR_OUT_OF_MEMORY;
       strcpy(*aClassDescription, mInfo->mDescription);
    } else {
       *aClassDescription = dm_null;
    }
    return CCI_OK;
}

CCI_IMETHODIMP cciGenericFactory::GetClassID(dmCID * *aClassID)
{
    *aClassID =  reinterpret_cast<dmCID*>
                        (cciMemory::Clone(&mInfo->mCID, sizeof mInfo->mCID));
    if(!*aClassID)
        return CCI_ERROR_OUT_OF_MEMORY;
    return CCI_OK;
}

CCI_IMETHODIMP cciGenericFactory::GetClassIDNoAlloc(dmCID *aClassID)
{
    *aClassID = mInfo->mCID;
    return CCI_OK;
}

CCI_IMETHODIMP cciGenericFactory::GetImplementationLanguage(dm_uint32 *langp)
{
  *langp = 0; //cciIProgrammingLanguage::CPLUSPLUS;
  return CCI_OK;
}

CCI_IMETHODIMP cciGenericFactory::GetFlags(dm_uint32 *flagsp)
{
  *flagsp = mInfo->mFlags;
  return CCI_OK;
}

//=====================
// Static method
//=====================

CCI_METHOD cciGenericFactory::Create(cciISupports* outer, const dmIID& aIID, void* *aInstancePtr)
{
  // sorry, aggregation not spoken here.
  cci_result rv = CCI_ERROR_NO_AGGREGATION;
  if(outer == dm_null)
  {
    cciGenericFactory* factory = new cciGenericFactory;
    if(factory)
    {
      rv = factory->QueryInterface(aIID, aInstancePtr);
      if(CCI_FAILED(rv))
         delete factory;
     } else {
    	 rv = CCI_ERROR_OUT_OF_MEMORY;
     }
   }
   return rv;
}

extern "C"
__daim_export cci_result
CCI_NewGenericFactory(cciIGenericFactory* *result, const cciModuleComponentInfo *info)
{
    cci_result rv;
    cciIGenericFactory* fact;
    rv = cciGenericFactory::Create(dm_null,CCI_GET_IID(cciIGenericFactory),(void**)&fact);
    if(CCI_FAILED(rv)) return rv;
    rv = fact->SetComponentInfo(info);
    if(CCI_FAILED(rv))
       CCI_RELEASE(fact);
    else
       *result = fact;

    return rv;
}


////////////////////////////////////////////////////////////////////////////////

cciGenericModule::cciGenericModule(const char* moduleName, dm_uint32 componentCount,
                                 const cciModuleComponentInfo* components,
                                 cciModuleConstructorProc ctor,
                                 cciModuleDestructorProc dtor)
    : mInitialized(false),
      mModuleName(moduleName),
      mComponentCount(componentCount),
      mComponents(components),
      mFactoriesNotToBeRegistered(dm_null),
      mCtor(ctor),
      mDtor(dtor)
{
}

cciGenericModule::~cciGenericModule()
{
    Shutdown();
}

CCI_IMPL_THREADSAFE_ISUPPORTS1(cciGenericModule, cciIModule);

cci_result
cciGenericModule::AddFactoryNode(cciIGenericFactory* fact)
{
    if (!fact)
        return CCI_ERROR_FAILURE;

    FactoryNode *node = new (dm_arena) FactoryNode(fact, mFactoriesNotToBeRegistered);
    if (!node)
        return CCI_ERROR_OUT_OF_MEMORY;

    mFactoriesNotToBeRegistered = node;
    return CCI_OK;
}


// Perform our one-time intialization for this module
cci_result
cciGenericModule::Initialize(cciIComponentManager *compMgr)
{
    cci_result rv;

    if(mInitialized) {
       return CCI_OK;
    }

    if(mCtor) {
       rv = mCtor(this);
       if(CCI_FAILED(rv))
          return rv;
    }

    cci_Ptr<cciIComponentRegistrar> registrar = do_QueryInterface(compMgr,&rv);
    if (CCI_FAILED(rv))
        return rv;

    // Eagerly populate factory/class object hash for entries
    // without constructors. If we didn't, the class object would
    // never get created. Also create the factory, which doubles
    // as the class object, if the EAGER_CLASSINFO flag was given.
    // This allows objects to be created (within their modules)
    // via operator new rather than CreateInstance, yet still be
    // QI'able to cciIClassInfo.
    const cciModuleComponentInfo* desc = mComponents;
    for (dm_uint32 i = 0; i < mComponentCount; i++)
    {
        if (!desc->mConstructor || (desc->mFlags & cciIClassInfo::EAGER_CLASSINFO))
        {
            cci_Ptr<cciIGenericFactory> fact;
            cci_result rv = CCI_NewGenericFactory(getter_AddRefs(fact), desc);
            if(CCI_FAILED(rv))
               return rv;

            // if we don't have a mConstructor, then we should not populate
            // the component manager.
            if (!desc->mConstructor) {
                rv = AddFactoryNode(fact);
            } else {
                rv = registrar->RegisterFactory(desc->mCID,
                                                desc->mDescription,
                                                desc->mContractID,
                                                fact);
            }

            if(CCI_FAILED(rv))
              return rv;
        }
        desc++;
    }

    mInitialized = true;
    return CCI_OK;
}

// Shutdown this module, releasing all of the module resources
void
cciGenericModule::Shutdown()
{
    // Free cached factories that were not registered.
    FactoryNode* node;
    while (mFactoriesNotToBeRegistered)
    {
        node = mFactoriesNotToBeRegistered->mNext;
        
        mFactoriesNotToBeRegistered->~FactoryNode();
        ::operator delete [] (mFactoriesNotToBeRegistered,dm_arena);
        
        mFactoriesNotToBeRegistered = node;
    }

    if (mInitialized) {
        mInitialized = false;

        if (mDtor)
            mDtor(this);
    }
}

//=============================
// cciIModule implementation
//=============================

// Create a factory object for creating instances of aClass.
CCI_IMETHODIMP
cciGenericModule::GetClassObject(cciIComponentManager *aCompMgr,
                                const dmCID& aClass,
                                const dmIID& aIID,
                                void** r_classObj)
{
    cci_result rv;

    // Defensive programming: Initialize *r_classObj in case of error below
    if (!r_classObj) {
        return CCI_ERROR_INVALID_POINTER;
    }
    *r_classObj = NULL;

    // Do one-time-only initialization if necessary
    if (!mInitialized) {
        rv = Initialize(aCompMgr);
        if (CCI_FAILED(rv)) {
            // Initialization failed! yikes!
            return rv;
        }
    }

    // Choose the appropriate factory, based on the desired instance
    // class type (aClass).
    const cciModuleComponentInfo* desc = mComponents;
    for (dm_uint32 i = 0; i < mComponentCount; i++)
    {
        if (desc->mCID.Equals(aClass))
        {
           cci_Ptr<cciIGenericFactory> fact;
           rv = CCI_NewGenericFactory(getter_AddRefs(fact), desc);

           if(CCI_FAILED(rv))
              return rv;

           return fact->QueryInterface(aIID, r_classObj);
        }
        desc++;
    }
    // not found in descriptions
    return CCI_ERROR_FACTORY_NOT_REGISTERED;
}

CCI_IMETHODIMP
cciGenericModule::RegisterSelf(cciIComponentManager *aCompMgr,
                               const char* aPath,
                               const char* registryLocation,
                               const char* componentType)
{
    cci_result rv = CCI_OK;

#ifdef DEBUG
    dmLOG("*** Registering components in: %s", mModuleName);
#endif

    const cciModuleComponentInfo* cp = mComponents;
    for (dm_uint32 i = 0; i < mComponentCount; i++)
    {
        // Register the component only if it has a constructor
        if (cp->mConstructor)
        {
            cci_Ptr<cciIComponentRegistrar> registrar = do_QueryInterface(aCompMgr, &rv);
            if (registrar)
                rv = registrar->RegisterFactoryLocation(cp->mCID,
                                                        cp->mDescription,
                                                        cp->mContractID,
                                                        aPath,
                                                        registryLocation,
                                                        componentType);
            if (CCI_FAILED(rv)) {
#ifdef DEBUG
                dmLOG("cciGenericModule %s: unable to register %s component => %x\n",
                       mModuleName?mModuleName:"(null)", cp->mDescription?cp->mDescription:"(null)", rv);
#endif
                break;
            }
        }
        // Call the registration hook of the component, if any
        if (cp->mRegisterSelfProc)
        {
            rv = cp->mRegisterSelfProc(aCompMgr, aPath, registryLocation,
                                       componentType, cp);
            if (CCI_FAILED(rv)) {
#ifdef DEBUG
                dmLOG("cciGenericModule %s: Register hook for %s component returned error => %x\n",
                       mModuleName?mModuleName:"(null)", cp->mDescription?cp->mDescription:"(null)", rv);
#endif
                break;
            }
        }
        cp++;
    }

    return rv;
}

CCI_IMETHODIMP
cciGenericModule::UnregisterSelf(cciIComponentManager* aCompMgr,
                            const char* aPath,
                            const char* registryLocation)
{
#ifdef DEBUG
    dmLOG("*** Unregistering components in: %s\n", mModuleName);
#endif
    const cciModuleComponentInfo* cp = mComponents;
    for (dm_uint32 i = 0; i < mComponentCount; i++)
    {
        // Call the unregistration hook of the component, if any
        if (cp->mUnregisterSelfProc)
        {
            cp->mUnregisterSelfProc(aCompMgr, aPath, registryLocation, cp);
        }

        // Unregister the component
        cci_result rv;
        cci_Ptr<cciIComponentRegistrar> registrar = do_QueryInterface(aCompMgr, &rv);
        if (registrar)
             rv = registrar->UnregisterFactoryLocation(cp->mCID, aPath);
        if (CCI_FAILED(rv)) {
#ifdef DEBUG
            dmLOG("cciGenericModule %s: unable to unregister %s component => %x\n",
                   mModuleName, cp->mDescription, rv);
#endif
        }
        cp++;
    }

    return CCI_OK;
}

CCI_IMETHODIMP
cciGenericModule::CanUnload(cciIComponentManager *aCompMgr, bool *okToUnload)
{
    if(!okToUnload) {
        return CCI_ERROR_INVALID_POINTER;
    }

    *okToUnload = false;
    return CCI_ERROR_FAILURE;
}


/**
 * CCI API functions
 */

extern "C"
__daim_export cci_result
CCI_NewGenericModule(cciModuleInfo* info,cciIModule* *result)
{
  cci_result rv = CCI_OK;

  // Create and initialize the module instance
  cciGenericModule *m =
    new cciGenericModule(info->mModuleName, info->mCount, info->mComponents,
                         info->mCtor, info->mDtor);
  if(!m)
     return CCI_ERROR_OUT_OF_MEMORY;

  // Increase refcnt and store away cciIModule interface to m in result
  CCI_ADDREF(*result = m);
  return rv;
}

////////////////////////////////////////////////////////////////////////////////
