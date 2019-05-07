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
 * David Marteau (CCI Implementation)
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

#ifndef cciComponentManager_h
#define cciComponentManager_h

#ifndef cciCOM_h
#include "cciCOM.h"
#endif

#ifndef cciCOMPtr_h
#include "cciCOMPtr.h"
#endif

#ifndef cciIFactory_h
#include "cciIFactory.h"
#endif

#include "cciIServiceManager.h"
#include "cciIComponentManager.h"
#include "cciIComponentRegistrar.h"
#include "cciIInterfaceRequestor.h"
#include "cciNativeModuleLoader.h"
#include "cciStaticModuleLoader.h"

#include "cciManifestReader.h"

#define CCI_COMPONENTMANAGER_CID                    \
{ /* 91775d60-d5dc-11d2-92fb-00e09805570f */         \
    0x91775d60,                                      \
    0xd5dc,                                          \
    0x11d2,                                          \
    {0x92, 0xfb, 0x00, 0xe0, 0x98, 0x05, 0x57, 0x0f} \
}

extern const char staticComponentType[];

////////////////////////////////////////////////////////////////////////////////
/**
 * Class: cciFactoryEntry()
 *
 * There are two types of FactoryEntries.
 *
 * 1. {CID, dll} mapping.
 *    Factory is a consequence of the dll. These can be either session
 *    specific or persistent based on whether we write this
 *    to the registry or not.
 *
 * 2. {CID, factory} mapping
 *    These are strictly session specific and in memory only.
 */

////////////////////////////////////////////////////////////////////////////////

// Forward declarations
class cciFactoryEntry;
struct cciStaticModuleInfo;

class cciComponentManagerImpl final
    : public cciIComponentManager,
      public cciIServiceManager,
      public cciIComponentRegistrar
{
  public:
    CCI_DECL_ISUPPORTS
    CCI_DECL_ICOMPONENTMANAGER
    CCI_DECL_ICOMPONENTREGISTRAR
    CCI_DECL_ISERVICEMANAGER

  public:

    cciComponentManagerImpl();

    static cciComponentManagerImpl*   gComponentManager;

    cciIModuleLoader* GetLoader(const char* aType);

    cci_result Init(cciStaticModuleInfo const *aStaticModules,
                    dm_uint32 aStaticModuleCount,
                    bool registerStaticModules);

    cci_result Shutdown(void);
    cci_result FreeServices(void);

  protected:
    cciNativeModuleLoader  mNativeModuleLoader;
    cciStaticModuleLoader  mStaticModuleLoader;

    // Shutdown
    #define CCI_SHUTDOWN_NEVERHAPPENED 0
    #define CCI_SHUTDOWN_INPROGRESS 1
    #define CCI_SHUTDOWN_COMPLETE 2
    dm_uint32 mShuttingDown;

    dmRWLock mLock;
    dmMutex  mMutex;

    dmHashPtrDict mContractIds;
    dmHashPtrDict mFactories;

    cci_result HashContractID(const dmCString& aContractID,cciFactoryEntry*);

    cci_result AddFactoryEntry(cciFactoryEntry*);
    cci_result RemoveFactoryEntry(cciFactoryEntry*);

    cciFactoryEntry* GetFactoryEntry(const dmCString& aContractID);
    cciFactoryEntry* GetFactoryEntry(const dmCID &aClass);

  private:
    ~cciComponentManagerImpl();
};
////////////////////////////////////////////////////////////////////////////////


#endif // cciComponentManager_h__

