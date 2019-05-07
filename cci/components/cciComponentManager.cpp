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
 * ****** END LICENSE BLOCK *****
 */

#include "cciPrivate.h"
#include "cciComponentManager.h"
#include "cciServiceManagerUtils.h"
#include "cciCategoryManager.h"
#include "cciMemory.h"
#include "cciIModule.h"
#include "cciIFileStreams.h"
#include "dmCrt.h"

const char nativeComponentType[]="application/x-daim-native";
const char staticComponentType[]="application/x-daim-static";

static const char gIDFormat[] =
  "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}";

#define CCI_EMPTY_IID                               \
{                                                    \
    0x00000000,                                      \
    0x0000,                                          \
    0x0000,                                          \
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} \
}

// CCI_DEFINE_CID(kEmptyCID, CCI_EMPTY_IID);

#define UID_STRING_LENGTH 39

static dmCString GetIDString(const dmID& aCID)
{
  return dmCString::FormatString(UID_STRING_LENGTH+1,gIDFormat,
                aCID.m0, (dm_uint32) aCID.m1, (dm_uint32) aCID.m2,
                (dm_uint32) aCID.m3[0], (dm_uint32) aCID.m3[1],
                (dm_uint32) aCID.m3[2], (dm_uint32) aCID.m3[3],
                (dm_uint32) aCID.m3[4], (dm_uint32) aCID.m3[5],
                (dm_uint32) aCID.m3[6], (dm_uint32) aCID.m3[7]);
}

// Set to true from CCI_Shutdown
extern bool gCCIShuttingDown;

////////////////////////////////////////////////////////////////////////////////
// cciFactoryEntry
////////////////////////////////////////////////////////////////////////////////

#define FACTORY_ENTRY_BLOCKS 100

class cciFactoryEntry
{
public:
  cciFactoryEntry(const dmCID      &aClass,
                  const char*      aLocationKey,
                  const char*      aType) :
      mCID(aClass),
      mLocationKey(aLocationKey),
      mType(aType) {}

  cciFactoryEntry(const dmCID    &aClass,
                  cciIFactory    *aFactory) :
      mCID(aClass),
      mFactory(aFactory),
      mType(dm_null) {}

  ~cciFactoryEntry() {}

  dmCID                  mCID;
  dmCString              mLocationKey;
  dmCString              mCIDString;
  cci_Ptr<cciIFactory>   mFactory;
  cci_Ptr<cciISupports>  mServiceObject;
  const char*            mType;

  cci_result GetFactory(cciIFactory* *aResult);
  cci_result CreateInstance(cciISupports *aDelegate,const dmIID &aIID,void **aResult);
  cci_result GetService(const dmIID& aIID, void* *result );

  DM_DECL_ALLOCATOR_NEW(cciFactoryEntry,FACTORY_ENTRY_BLOCKS)
};

DM_IMPL_ALLOCATOR_NEW(cciFactoryEntry,true);

cci_result
cciFactoryEntry::GetFactory( cciIFactory** aFactory )
{
  if(!mFactory)
  {
    cci_result rv;
    cci_Ptr<cciIModule> module;

    cciIModuleLoader* loader =
        cciComponentManagerImpl::gComponentManager->GetLoader(mType);

    if (!loader)
         return CCI_ERROR_FAILURE;

    rv = loader->LoadModule(mLocationKey.get(),getter_AddRefs(module));

    if(CCI_FAILED(rv))
       return rv;

    if(!module) {
       CCI_ERROR("Module returned success but no module!");
       return CCI_ERROR_FAILURE;
    }

    rv = module->GetClassObject(cciComponentManagerImpl::gComponentManager,
                                mCID,CCI_GET_IID(cciIFactory),
                                getter_AddRefs(mFactory));

    if(CCI_FAILED(rv))
       return rv;

    CCI_ASSERTION(mFactory,"Loader says it succeeded; got null factory!");

    if (!mFactory)
        return CCI_ERROR_UNEXPECTED;
  }

  *aFactory = mFactory.get();
  CCI_ADDREF(*aFactory);

  return CCI_OK;
}

cci_result
cciFactoryEntry::CreateInstance(cciISupports *aDelegate,
                                const dmIID &aIID,
                                void **aResult)
{
  cciIFactory *factory = dm_null;
  cci_result rv = GetFactory(&factory);

  if(CCI_SUCCEEDED(rv))
  {
    rv = factory->CreateInstance(aDelegate, aIID, aResult);
    if(CCI_SUCCEEDED(rv) && !*aResult) {
       CCI_ERROR("Factory did not return an object but returned success!");
       rv = CCI_ERROR_SERVICE_NOT_AVAILABLE;
    }
    CCI_RELEASE(factory);
  }
  else
  {
    // Translate error values
    rv = CCI_ERROR_FACTORY_NOT_REGISTERED;
  }

  return rv;
}

cci_result
cciFactoryEntry::GetService(const dmIID& aIID, void* *result)
{
  if(mServiceObject) {
     return mServiceObject->QueryInterface(aIID, result);
  }

  // We need to create the object
  cci_Ptr<cciISupports> service;
  cci_result rv = CreateInstance(dm_null,aIID,getter_AddRefs(service));

  if(CCI_SUCCEEDED(rv))
  {
    mServiceObject = service;
    *result = service.get();
    CCI_ADDREF(static_cast<cciISupports*>(*result));
  }

  return rv;
}

////////////////////////////////////////////////////////////////////////////////
// cciComponentManagerImpl
////////////////////////////////////////////////////////////////////////////////

cciComponentManagerImpl* cciComponentManagerImpl::gComponentManager = dm_null;

cciComponentManagerImpl::cciComponentManagerImpl()
: mShuttingDown(CCI_SHUTDOWN_NEVERHAPPENED)
{
}

cciComponentManagerImpl::~cciComponentManagerImpl()
{
  #ifdef _DEBUG
   fprintf(stderr,"destroying Component Manager\n");
  #endif
  if (mShuttingDown != CCI_SHUTDOWN_COMPLETE)
      Shutdown();
}

cci_result cciComponentManagerImpl::Init(cciStaticModuleInfo const *aStaticModules,
                                         dm_uint32 aStaticModuleCount,
                                         bool registerStaticModules)
{
  CCI_ABORT_IF_FALSE(gComponentManager != dm_null,"Ooops !!! where is gComponentManager !!!");

  CCI_PRECONDITION(mShuttingDown != CCI_SHUTDOWN_INPROGRESS,"Init called while in shutdown !");
  if(mShuttingDown == CCI_SHUTDOWN_INPROGRESS)
     return CCI_ERROR_FAILURE;

  mShuttingDown = CCI_SHUTDOWN_NEVERHAPPENED;

  cci_result rv;

  // Init static modules
  rv = mStaticModuleLoader.Init(aStaticModules,aStaticModuleCount,
                                registerStaticModules);
  if(CCI_FAILED(rv))
     return rv;

  // Init native module loader
  return mNativeModuleLoader.Init();
}

cci_result cciComponentManagerImpl::Shutdown(void)
{
  CCI_PRECONDITION(mShuttingDown == CCI_SHUTDOWN_NEVERHAPPENED,"Shutdown on unitialized manager !");
  if(mShuttingDown != CCI_SHUTDOWN_NEVERHAPPENED)
     return CCI_ERROR_FAILURE;

  mShuttingDown = CCI_SHUTDOWN_INPROGRESS;

  // Clear contractid hash table
  mContractIds.clear();

  // Clear factory entries
  dmHashPtrDict::iterator it  = mFactories.begin();
  dmHashPtrDict::iterator end = mFactories.end();
  while(it!=end)
  {
    // Factory destructor (if any) will be called here
    cciFactoryEntry* fe = reinterpret_cast<cciFactoryEntry*>((*it++).second);
    if(fe)
       delete fe;
  }

  mFactories.clear();

  // Unload libraries
  // Native module will release there resources here
  mNativeModuleLoader.UnloadModules();
  mStaticModuleLoader.ReleaseModules();

  mShuttingDown = CCI_SHUTDOWN_COMPLETE;
  return CCI_OK;
}

////////////////////////////////////////////////////////////////////////////////
// cciISupports
////////////////////////////////////////////////////////////////////////////////

CCI_IMPL_THREADSAFE_ISUPPORTS3(cciComponentManagerImpl,
                               cciIComponentManager,
                               cciIServiceManager,
                               cciIComponentRegistrar);

////////////////////////////////////////////////////////////////////////////////
// Hash Functions
////////////////////////////////////////////////////////////////////////////////
cci_result
cciComponentManagerImpl::HashContractID(const dmCString& aContractID,
                                        cciFactoryEntry *fe)
{
  if(aContractID.IsEmpty())
     return CCI_ERROR_INVALID_ARG;

  mContractIds[aContractID] = fe;
  return CCI_OK;
}


cci_result
cciComponentManagerImpl::AddFactoryEntry( cciFactoryEntry* fe )
{
  fe->mCIDString = GetIDString(fe->mCID);
  mFactories[fe->mCIDString] = fe;

  return CCI_OK;
}

cci_result
cciComponentManagerImpl::RemoveFactoryEntry( cciFactoryEntry* fe )
{
  mFactories.erase(fe->mCIDString);
  return CCI_OK;
}

cciFactoryEntry*
cciComponentManagerImpl::GetFactoryEntry(const dmCString& aContractID)
{
  cciFactoryEntry *fe = dm_null;

  dmHashPtrDict::iterator it = mContractIds.find(aContractID);
  if(it!=mContractIds.end())
     fe = reinterpret_cast<cciFactoryEntry*>((*it).second);

  return fe;
}

cciFactoryEntry*
cciComponentManagerImpl::GetFactoryEntry(const dmCID &aClass)
{
  cciFactoryEntry *fe = dm_null;

  dmCString hashable = GetIDString(aClass);

  dmHashPtrDict::iterator it = mFactories.find(hashable);
  if(it!=mFactories.end())
     fe = reinterpret_cast<cciFactoryEntry*>((*it).second);

  return fe;
}

//=============================
// cciIComponentManager
//=============================

/**
 * GetClassObject()
 *
 * Given a classID, this finds the singleton ClassObject that implements the CID.
 * Returns an interface of type aIID off the singleton classobject.
 */
CCI_IMETHODIMP
cciComponentManagerImpl::GetClassObject(const dmCID &aClass, const dmIID &aIID,
                                        void **aResult)
{
  dmCRITICAL_SECTION(mMutex);

  CCI_PRECONDITION(aResult != dm_null,dm_null);

  cci_result rv;
  cci_Ptr<cciIFactory> factory;

  cciFactoryEntry *entry = GetFactoryEntry(aClass);
  if(!entry)
    return CCI_ERROR_FACTORY_NOT_REGISTERED;

  rv = entry->GetFactory(getter_AddRefs(factory));
  if(CCI_FAILED(rv))
    return rv;

  rv = factory->QueryInterface(aIID,aResult);

  return rv;
}


CCI_IMETHODIMP
cciComponentManagerImpl::GetClassObjectByContractID(const char *contractID,
                                                    const dmIID &aIID,
                                                    void **aResult)
{
  dmCRITICAL_SECTION(mMutex);

  CCI_PRECONDITION(aResult != dm_null,dm_null);

  cci_result rv;
  cci_Ptr<cciIFactory> factory;

  cciFactoryEntry *entry = GetFactoryEntry(dmCString(contractID));
  if(!entry)
   return CCI_ERROR_FACTORY_NOT_REGISTERED;

  rv = entry->GetFactory(getter_AddRefs(factory));
  if(CCI_FAILED(rv))
   return rv;

  rv = factory->QueryInterface(aIID, aResult);

  return rv;
}

/**
 * CreateInstance()
 *
 * Create an instance of an object that implements an interface and belongs
 * to the implementation aClass using the factory. The factory is immediately
 * released and not held onto for any longer.
 */
CCI_IMETHODIMP
cciComponentManagerImpl::CreateInstance(const dmCID &aClass,
                                        cciISupports *aDelegate,
                                        const dmIID &aIID,
                                        void **aResult)
{
  // test this first, since there's no point in creating a component during
  // shutdown -- whether it's available or not would depend on the order it
  // occurs in the list
  if(gCCIShuttingDown)
  {
     // When processing shutdown, don't process new GetService() requests
     return CCI_ERROR_UNEXPECTED;
  }

  CCI_ENSURE_TRUE(aResult != dm_null,CCI_ERROR_NULL_POINTER);
  *aResult = dm_null;

  cciFactoryEntry *entry = dm_null;
  {
    dmREAD_LOCK(mLock);
    entry = GetFactoryEntry(aClass);
  } // exit lock

  if(!entry)
     return CCI_ERROR_FACTORY_NOT_REGISTERED;

  return entry->CreateInstance(aDelegate,aIID,aResult);
}

/**
 * CreateInstanceByContractID()
 *
 * A variant of CreateInstance() that creates an instance of the object that
 * implements the interface aIID and whose implementation has a contractID aContractID.
 *
 * This is only a convenience routine that turns around can calls the
 * CreateInstance() with classid and iid.
 */
CCI_IMETHODIMP
cciComponentManagerImpl::CreateInstanceByContractID(const char *aContractID,
                                                    cciISupports *aDelegate,
                                                    const dmIID &aIID,
                                                    void **aResult)
{
  // test this first, since there's no point in creating a component during
  // shutdown -- whether it's available or not would depend on the order it
  // occurs in the list
  if(gCCIShuttingDown)
  {
    // When processing shutdown, don't process new GetService() requests
    return CCI_ERROR_UNEXPECTED;
  }

  CCI_ENSURE_TRUE(aResult != dm_null,CCI_ERROR_NULL_POINTER);
  *aResult = dm_null;

  cciFactoryEntry *entry = dm_null;
  {
    dmREAD_LOCK(mLock);
    entry = GetFactoryEntry(dmCString(aContractID));
  } // exit lock

  if(!entry)
      return CCI_ERROR_FACTORY_NOT_REGISTERED;

  return entry->CreateInstance(aDelegate,aIID,aResult);
}

//=============================
// cciIServiceManager
//=============================

cci_result
cciComponentManagerImpl::FreeServices()
{
  CCI_ASSERTION(gCCIShuttingDown, "Must be shutting down in order to free all services");

  if(!gCCIShuttingDown)
    return CCI_ERROR_FAILURE;

  // Clear
  dmHashPtrDict::iterator it   = mFactories.begin();
  dmHashPtrDict::iterator last = mFactories.end();

  cciFactoryEntry *fe;
  while(it!=last) {
    fe = reinterpret_cast<cciFactoryEntry*>((*it).second);
    fe->mServiceObject = dm_null;
    ++it;
  }

  return CCI_OK;
}

CCI_IMETHODIMP
cciComponentManagerImpl::GetService(const dmCID& aClass,
                                    const dmIID& aIID,
                                    void* *result)
{
  dmCRITICAL_SECTION(mMutex);

  // test this first, since there's no point in returning a service during
  // shutdown -- whether it's available or not would depend on the order it
  // occurs in the list
  if(gCCIShuttingDown)
  {
    // When processing shutdown, don't process new GetService() requests
    return CCI_ERROR_UNEXPECTED;
  }

  cciFactoryEntry *entry = GetFactoryEntry(aClass);
  if(!entry)
    return CCI_ERROR_FACTORY_NOT_REGISTERED;

  return entry->GetService(aIID,result);
}

CCI_IMETHODIMP
cciComponentManagerImpl::GetServiceByContractID(const char* aContractID,
                                               const dmIID& aIID,
                                               void* *result)
{
  dmCRITICAL_SECTION(mMutex);

  // test this first, since there's no point in returning a service during
  // shutdown -- whether it's available or not would depend on the order it
  // occurs in the list
  if(gCCIShuttingDown)
  {
    // When processing shutdown, don't process new GetService() requests
    return CCI_ERROR_UNEXPECTED;
  }

  cciFactoryEntry *entry = GetFactoryEntry(dmCString(aContractID));
  if(!entry)
    return CCI_ERROR_FACTORY_NOT_REGISTERED;

  return entry->GetService(aIID,result);
}

//==============================
// cciIComponentRegistrar
//==============================

cciIModuleLoader*
cciComponentManagerImpl::GetLoader(const char* aType)
{
  if(aType == nativeComponentType)
    return &mNativeModuleLoader;

  if(aType == staticComponentType)
    return &mStaticModuleLoader;

  return dm_null;
}

/**
 * RegisterFactory()
 *
 * Register a factory to be responsible for creation of implementation of
 * classID aClass. Plus creates as association of aClassName and aContractID
 * to the classID. If replace is PR_TRUE, we replace any existing registrations
 * with this one.
 *
 * Once registration is complete, we add the class to the factories cache
 * that we maintain. The factories cache is the ONLY place where these
 * registrations are ever kept.
 *
 */
CCI_IMETHODIMP
cciComponentManagerImpl::RegisterFactory(const dmCID &aClass,
                                         const char *aClassName,
                                         const char *aContractID,
                                         cciIFactory *aFactory)
{
  CCI_ENSURE_ARG_POINTER(aFactory);

  dmWRITE_LOCK(mLock);

  cciFactoryEntry *entry = GetFactoryEntry(aClass);
  if(!entry)
  {
    entry = new cciFactoryEntry(aClass,aFactory);
    if(!entry)
        return CCI_ERROR_OUT_OF_MEMORY;

    AddFactoryEntry(entry);
  }
  else // Replace the existing factory
  {
    entry->mFactory = aFactory;
  }

  if(aContractID)
  {
    cci_result rv = HashContractID(dmCString(aContractID),entry);
    if(CCI_FAILED(rv))
       return rv; // uuuh !
  }

  #ifdef _DEBUG
  dmLOG("\t\tFactory register succeeded contractid=%s.",
         aContractID ? aContractID : "<none>");
  #endif

  return CCI_OK;
}

CCI_IMETHODIMP
cciComponentManagerImpl::UnregisterFactory(const dmCID &aClass,
                                          cciIFactory *aFactory)
{
  dmWRITE_LOCK(mLock);

  cciFactoryEntry* entry = GetFactoryEntry(aClass);
  if(!entry)
     return CCI_ERROR_FACTORY_NOT_REGISTERED;

  std::vector<dmCString> contractIds;

  // first delete all contract id entries that are registered with this cid.
  dmHashPtrDict::iterator it = mContractIds.begin();
  while(it != mContractIds.end()) {
    if((*it).second == reinterpret_cast<void*>(entry))
       contractIds.push_back((*it).first);
    ++it;
  }

  for(size_t i=0;i<contractIds.size();++i)
      mContractIds.erase(contractIds[i]);

  RemoveFactoryEntry(entry);
  delete entry;

  return CCI_OK;
}


CCI_IMETHODIMP
cciComponentManagerImpl::AutoRegister(const char *aSpec)
{
  cci_Ptr<cciIModule> module;

  dmCString location;
  cci_result rv = mNativeModuleLoader.ResolveKeyToFile(aSpec,location);
  if(CCI_SUCCEEDED(rv))
    rv = mNativeModuleLoader.LoadNativeModule(location,getter_AddRefs(module));

  if (CCI_SUCCEEDED(rv))
  {
    if(!module) {
        CCI_ERROR("Module loader succeeded without returning a module");
        return CCI_ERROR_FAILURE;
    }

    return module->RegisterSelf(this,location.get(),aSpec,nativeComponentType);
  }

  return CCI_ERROR_FAILURE;
}

CCI_IMETHODIMP
cciComponentManagerImpl::AutoUnregister(const char *aSpec)
{
  CCI_ENSURE_ARG_POINTER(aSpec);

  cci_Ptr<cciIModule> module;

  dmCString location;
  cci_result rv = mNativeModuleLoader.ResolveKeyToFile(aSpec,location);
  if(CCI_SUCCEEDED(rv))
    rv = mNativeModuleLoader.LoadNativeModule(location,getter_AddRefs(module));

  if (CCI_SUCCEEDED(rv))
  {
    if(!module) {
        CCI_ERROR("Module loader succeeded without returning a module");
        return CCI_ERROR_FAILURE;
    }

    return module->UnregisterSelf(this,location.get(),aSpec);
  }

  return CCI_ERROR_FAILURE;
}


CCI_IMETHODIMP
cciComponentManagerImpl::RegisterFactoryLocation(const dmCID & aClass,
                                                 const char *aClassName,
                                                 const char *aContractID,
                                                 const char *aLocation,
                                                 const char *loaderStr,
                                                 const char *aType)
{
  dmWRITE_LOCK(mLock);

  cciFactoryEntry *entry = GetFactoryEntry(aClass);

  // Normalize
  // const char *contractID = (aContractID && *aContractID) ? aContractID : dm_null;

  if(!entry)
  {
    // Use the loaderStr as the registry location key
    // If no loader string is provided, use the full location.
    // loaderstring will be resolved by the loader to get the
    // original file path
    if(!loaderStr)
        loaderStr = aLocation;

    entry = new cciFactoryEntry(aClass,loaderStr,aType);
    if(!entry)
       return CCI_ERROR_OUT_OF_MEMORY;

    AddFactoryEntry(entry);
  }

  // Update the ContractID Map
  if(aContractID)
  {
    cci_result rv = HashContractID(dmCString(aContractID),entry);
    if(CCI_FAILED(rv))
      return rv; // uuuh !
  }

  #ifdef _DEBUG
  dmLOG("\t\tFactory register succeeded contractid=%s.",
        aContractID ? aContractID : "<none>");
  #endif

  return CCI_OK;
}

CCI_IMETHODIMP
cciComponentManagerImpl::UnregisterFactoryLocation(const dmCID & aClass,
                                                   const char* aLocationKey)
{
  return UnregisterFactory(aClass,dm_null);
}

CCI_IMETHODIMP
cciComponentManagerImpl::IsCIDRegistered(const dmCID & aClass, bool *_retval)
{
  CCI_ENSURE_ARG_POINTER(_retval);

  *_retval = (dm_null != GetFactoryEntry(aClass));
  return CCI_OK;
}

CCI_IMETHODIMP
cciComponentManagerImpl::IsContractIDRegistered(const char *aContractId,bool *_retval)
{
  *_retval = (GetFactoryEntry(dmCString(aContractId))!=dm_null ? true : false);
  return CCI_OK;
}

CCI_IMETHODIMP
cciComponentManagerImpl::EnumerateCIDs(cciISimpleEnumerator **aEnumerator)
{
  return CCI_ERROR_NOT_IMPLEMENTED;
}

CCI_IMETHODIMP
cciComponentManagerImpl::EnumerateContractIDs(cciISimpleEnumerator **aEnumerator)
{
  return CCI_ERROR_NOT_IMPLEMENTED;
}

CCI_IMETHODIMP
cciComponentManagerImpl::ContractIDToCID(const char *aContractID,dmCID * *_retval)
{
  CCI_PRECONDITION(_retval != dm_null,"null ptr");

  *_retval = (dmCID*) cciMemory::Alloc(sizeof(dmCID));
  if(!*_retval)
     return CCI_ERROR_OUT_OF_MEMORY;

  CCI_ENSURE_ARG_POINTER(aContractID);

  cciFactoryEntry *fe = GetFactoryEntry(dmCString(aContractID));
  if(fe)
  {
    *_retval = reinterpret_cast<dmCID*>
                 (cciMemory::Clone(&fe->mCID,sizeof(dmCID)));
    return CCI_OK;
  }

  return CCI_ERROR_FACTORY_NOT_REGISTERED;
}

#define PERSISTENT_REGISTRY_VERSION_MINOR 1
#define PERSISTENT_REGISTRY_VERSION_MAJOR 0

static
bool ReadSectionHeader(cciManifestLineReader& reader, const char *token)
{
  while (1)
  {
    if (*reader.LinePtr() == '[')
    {
      char* p = reader.LinePtr() + (reader.LineLength() - 1);
      if (*p != ']')
          break;
      *p = 0;

      char* values[1];
      int lengths[1];

      if (1 != reader.ParseLine(values, lengths, 1))
          break;

      // ignore the leading '['
      if (0 != _dm_strcmp(values[0]+1, token))
          break;

      return true;
    }

    if (!reader.NextLine())
        break;
  }
  return false;
}


/* void readPersistentRegistry (in string registryLocation); */
CCI_IMETHODIMP
cciComponentManagerImpl::ReadPersistentRegistry(const char * registryLocation)
{
  CCI_ENSURE_ARG_POINTER(registryLocation);

  if(!dmPortability::AccessFile(registryLocation,dmPortability::eEXISTS|dmPortability::eREAD)) {
    CCI_WARNING("Persistent registry not found !");
    return CCI_ERROR_FILE_NOT_FOUND;
  }

  cciManifestLineReader reader;

  // Get the file size
  dm_size fileSize = dmPortability::GetFileSize(registryLocation);
  if(fileSize==0) {
     CCI_WARNING("Persistent registry is empty !");
     return CCI_OK; // Not really an error
  }

  dm_byte* registry = new (dm_arena) dm_byte[fileSize+1];
  if(!registry)
     return CCI_ERROR_OUT_OF_MEMORY;

  cci_Ptr<cciIFileInputStream> iStream;

  dm_uint32 byteread = 0;

  char* values[6];
  int lengths[6];

  cci_result rv = CCI_NewFileInputStream(getter_AddRefs(iStream),registryLocation);
  if(CCI_FAILED(rv))
     goto out;

  rv = iStream->Read(registry,fileSize,&byteread);
  if(CCI_FAILED(rv))
     goto out;

  registry[fileSize] = '\0';

  reader.Init(reinterpret_cast<char*>(registry),fileSize);

  //////////////////////////////////
  // [HEADER]
  if(!ReadSectionHeader(reader, "HEADER"))
     goto out;

  if(!reader.NextLine())
     goto out;

  // VersionLiteral,major,minor
  if(3 != reader.ParseLine(values,lengths,3))
      goto out;

  // VersionLiteral
  if(_dm_strncmp(values[0],"Version",lengths[0])!=0)
      goto out;

  // major
  if (PERSISTENT_REGISTRY_VERSION_MAJOR != atoi(values[1]))
      goto out;

  // minor
  if (PERSISTENT_REGISTRY_VERSION_MINOR != atoi(values[2]))
      goto out;


  //////////////////////////////////
  // [CONTRACTIDS]
  if(!ReadSectionHeader(reader, "CONTRACTIDS"))
     goto out;

  while (1)
  {
    if (!reader.NextLine())
       break;

    //contractID,cid,locationKey
    if (3 != reader.ParseLine(values,lengths,3))
       break;

    dmCID aClass;
    if(!aClass.Parse(values[1]))
       continue;

    rv = RegisterFactoryLocation(aClass,dm_null,values[0],values[2],dm_null,nativeComponentType);
    if(CCI_FAILED(rv))
      goto out;
  }

  //////////////////////////////////////////
  // [CATEGORIES]
  if(!ReadSectionHeader(reader, "CATEGORIES"))
     goto out;

  {
    // Get the category manager
    cci_Ptr<cciCategoryManager>
      categoryManager = do_GetService(CCI_CATEGORYMANAGER_CONTRACTID,&rv);

    if(categoryManager)
    {
      while (1)
      {
        if (!reader.NextLine())
          break;

        //type,name,value
       if (3 != reader.ParseLine(values, lengths, 3))
       break;

       categoryManager->AddCategoryEntry(values[0],values[1],values[2],
                                         true,true);
      }
    }
  }

out:
  if(registry)
    ::operator delete [] (registry,dm_arena);

  return rv;
}

/* void writePersistentRegistry (in string registryLocation); */
CCI_IMETHODIMP
cciComponentManagerImpl::WritePersistentRegistry(const char * registryLocation)
{
  CCI_ENSURE_ARG_POINTER(registryLocation);

  cci_Ptr<cciIFileOutputStream> oStream;

  cci_result rv = CCI_NewFileOutputStream(getter_AddRefs(oStream),registryLocation);
  if(CCI_FAILED(rv))
     return rv;

  FILE* fd = oStream->GetANSIFileDesc();
  if(!fd) {
    rv = CCI_ERROR_UNEXPECTED;
    goto out;
  }

  if(fprintf(fd,"Generated File. Do not edit.\n") ==  -1) {
      rv = CCI_ERROR_UNEXPECTED;
      goto out;
  }

  if(fprintf(fd, "\n[HEADER]\nVersion,%d,%d\n",
                 PERSISTENT_REGISTRY_VERSION_MAJOR,
                 PERSISTENT_REGISTRY_VERSION_MINOR) ==  -1) {
      rv = CCI_ERROR_UNEXPECTED;
      goto out;
  }

  if(fprintf(fd, "\n[CONTRACTIDS]\n") ==  -1) {
      rv = CCI_ERROR_UNEXPECTED;
      goto out;
  }

  {
    cciFactoryEntry *fe = dm_null;

    dmHashPtrDict::iterator it  = mContractIds.begin();
    dmHashPtrDict::iterator end = mContractIds.end();
    for(;it != end;++it)
    {
      fe = reinterpret_cast<cciFactoryEntry*>((*it).second);
      if(fprintf(fd,"%s,%s,%s\n", (*it).first.get(), fe->mCIDString.get(),
                 fe->mLocationKey.get()) ==  -1)
      {
         rv = CCI_ERROR_UNEXPECTED;
         goto out;
      }
    }
  }

  if(fprintf(fd, "\n[CATEGORIES]\n") ==  -1) {
      rv = CCI_ERROR_UNEXPECTED;
      goto out;
  }

  {
    // Get the category manager
    cci_Ptr<cciCategoryManager>
      categoryManager = do_GetService(CCI_CATEGORYMANAGER_CONTRACTID,&rv);

    if(categoryManager)
       rv = categoryManager->WriteCategoryManagerToRegistry(fd);
  }

out:
  return rv;
}



/**
 * CCI api functions
 */

extern "C" {

__daim_export cci_result
CCI_GetComponentManager(cciIComponentManager* *result)
{
  *result = static_cast<cciIComponentManager*>
                       (cciComponentManagerImpl::gComponentManager);
  CCI_IF_ADDREF(*result);
  return CCI_OK;
}

__daim_export cci_result
CCI_GetServiceManager(cciIServiceManager* *result)
{
  *result = static_cast<cciIServiceManager*>
                       (cciComponentManagerImpl::gComponentManager);
  CCI_IF_ADDREF(*result);
  return CCI_OK;
}

__daim_export cci_result
CCI_GetComponentRegistrar(cciIComponentRegistrar* *result)
{
  *result = static_cast<cciIComponentRegistrar*>
                       (cciComponentManagerImpl::gComponentManager);
  CCI_IF_ADDREF(*result);
  return CCI_OK;
}

}
