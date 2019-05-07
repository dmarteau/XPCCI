#ifndef cciGenericFactory_h
#define cciGenericFactory_h

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
 * ***** END LICENSE BLOCK ***** */

#include "cciCOMPtr.h"
#include "cciIClassInfo.h"
#include "cciIGenericFactory.h"

/**
 * Generic factory implementation
 */
class cciGenericFactory final : public cciIGenericFactory, public cciIClassInfo {
public:
    CCI_DEFINE_STATIC_CID_ACCESSOR(CCI_GENERICFACTORY_CID)

    cciGenericFactory(const cciModuleComponentInfo *info = NULL);
    
    CCI_DECL_ISUPPORTS
    CCI_DECL_ICLASSINFO
    
    /* cciIGenericFactory methods */
    CCI_IMETHOD SetComponentInfo(const cciModuleComponentInfo *info) override; 
    CCI_IMETHOD GetComponentInfo(const cciModuleComponentInfo **infop) override;

    CCI_IMETHOD CreateInstance(cciISupports *aOuter, DM_REFIID aIID, void **aResult) override;

    CCI_IMETHOD LockFactory(bool aLock) override;

    static CCI_METHOD Create(cciISupports* outer, const dmIID& aIID, void* *aInstancePtr);
    
private:
    ~cciGenericFactory();

    const cciModuleComponentInfo *mInfo;
};

////////////////////////////////////////////////////////////////////////////////

#include "cciIModule.h"

class cciGenericModule final : public cciIModule
{
public:
    cciGenericModule(const char* moduleName, 
                    dm_uint32 componentCount,
                    const cciModuleComponentInfo* components,
                    cciModuleConstructorProc ctor,
                    cciModuleDestructorProc dtor);

private:
    ~cciGenericModule();

public:
    CCI_DECL_ISUPPORTS
    CCI_DECL_IMODULE
    
    struct FactoryNode
    {
        FactoryNode(cciIGenericFactory* fact, FactoryNode* next) 
        { 
            mFactory = fact; 
            mNext    = next;
        }
        ~FactoryNode(){}

        cci_Ptr<cciIGenericFactory> mFactory;
        FactoryNode* mNext;
    };

protected:
    cci_result Initialize(cciIComponentManager* compMgr);

    void Shutdown();
    cci_result AddFactoryNode(cciIGenericFactory* fact);

    bool                       mInitialized;
    const char*                   mModuleName;
    dm_uint32                     mComponentCount;
    const cciModuleComponentInfo* mComponents;
    FactoryNode*                  mFactoriesNotToBeRegistered;
    cciModuleConstructorProc      mCtor;
    cciModuleDestructorProc       mDtor;
};

#endif // cciGenericFactory_h

