#ifndef cciIGenericFactory_h
#define cciIGenericFactory_h

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
 *   Benjamin Smedberg <benjamin@smedbergs.us>
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

#ifndef cciSupportsImpl_h
#include "cciSupportsImpl.h"
#endif

#include "cciIFactory.h"
#include "cciIModule.h"

class cciIComponentManager;
class cciIClassInfo;

// {3bc97f01-ccdf-11d2-bab8-b548654461fc}
#define CCI_GENERICFACTORY_CID                                               \
  { 0x3bc97f01, 0xccdf, 0x11d2,                                               \
    { 0xba, 0xb8, 0xb5, 0x48, 0x65, 0x44, 0x61, 0xfc } }

// {3bc97f00-ccdf-11d2-bab8-b548654461fc}
#define CCI_IGENERICFACTORY_IID                                              \
  { 0x3bc97f00, 0xccdf, 0x11d2,                                               \
    { 0xba, 0xb8, 0xb5, 0x48, 0x65, 0x44, 0x61, 0xfc } }

#define CCI_GENERICFACTORY_CONTRACTID "@daim.org/generic-factory;1"
#define CCI_GENERICFACTORY_CLASSNAME  "Generic Factory"

struct cciModuleComponentInfo; // forward declaration

/**
 * Provides a Generic nsIFactory implementation that can be used by
 * DLLs with very simple factory needs.
 */
class cciIGenericFactory : public cciIFactory {
public:
    CCI_DECLARE_STATIC_IID_ACCESSOR(CCI_IGENERICFACTORY_IID);

    CCI_IMETHOD SetComponentInfo(const cciModuleComponentInfo *info)   = 0;
    CCI_IMETHOD GetComponentInfo(const cciModuleComponentInfo **infop) = 0;
};

CCI_DEFINE_STATIC_IID_ACCESSOR(cciIGenericFactory, CCI_IGENERICFACTORY_IID);

/** Component Callbacks **/

 /**
  * CCIConstructorProcPtr
  *
  * This function will be used by the generic factory to create an
  * instance of the given CID.
  *
  * @param aOuter    : Pointer to a component that wishes to be aggregated
  *                    in the resulting instance. This will be nsnull if no
  *                    aggregation is requested.
  * @param iid       : The IID of the interface being requested in
  *                    the component which is being currently created.
  * @param result    : [out] Pointer to the newly created instance, if successful.
  *
  * @return CCI_OK                     Component successfully created and the interface
  *                                    being requested was successfully returned in result.
  *         CCI_NOINTERFACE            Interface not accessible.
  *         CCI_ERROR_NO_AGGREGATION   if an 'outer' object is supplied, but the
  *                                    component is not aggregatable.
  *         CCI_ERROR*                 Method failure.
  **/
typedef CCI_CALLBACK(CCIConstructorProcPtr)(cciISupports *aOuter,
                                            DM_REFIID aIID,
                                            void **aResult);

/**
 * CCIRegisterSelfProcPtr
 *
 * One time registration call back.  Allows you to perform registration
 * specific activity like adding yourself to a category.
 *
 * @param aCompMgr    : The global component manager
 * @param aFile       : Component File. This file must have an associated
 *                      loader and export the required symbols which this
 *                      loader defines.
 * @param aLoaderStr  : Opaque loader specific string.  This value is
 *                      passed into the cciIModule's registerSelf
 *                      callback and must be fowarded unmodified when
 *                      registering factories via their location.
 * @param aType       : Component Type of CID aClass.  This value is
 *                      passed into the cciIModule's registerSelf
 *                      callback and must be fowarded unmodified when
 *                      registering factories via their location.
 * @param aInfo       : Pointer to array of cciModuleComponentInfo
 *
 * @param aInfo
 * @return CCI_OK        Registration was successful.
 *         CCI_ERROR*    Method failure.
 **/
typedef CCI_CALLBACK(CCIRegisterSelfProcPtr)(cciIComponentManager *aCompMgr,
                                             const char    *aPath,
                                             const char    *aLoaderStr,
                                             const char    *aType,
                                             const cciModuleComponentInfo *aInfo);

/**
 * CCIUnregisterSelfProcPtr
 *
 * One time unregistration call back.  Allows you to perform unregistration
 * specific activity like removing yourself from a category.
 *
 * @param aCompMgr    : The global component manager
 * @param aFile       : Component File. This file must have an associated
 *                      loader and export the required symbols which this
 *                      loader defines.
 * @param aLoaderStr  : Opaque loader specific string.  This value is
 *                      passed into the cciIModule's registerSelf
 *                      callback and must be fowarded unmodified when
 *                      registering factories via their location
 * @param aInfo       : Pointer to array of cciModuleComponentInfo
 *
 * @param aInfo
 * @return CCI_OK        Registration was successful.
 *         CCI_ERROR*    Method failure.

 **/
typedef CCI_CALLBACK(CCIUnregisterSelfProcPtr)(cciIComponentManager *aCompMgr,
		                                       const char *aPath,
                                               const char *aLoaderStr,
                                               const cciModuleComponentInfo *aInfo);

/**
 * CCIFactoryDestructorProcPtr
 *
 * This function will be called when the factory is being destroyed.
 *
 **/
typedef CCI_CALLBACK(CCIFactoryDestructorProcPtr)(void);


/**
 * CCIGetInterfacesProcPtr
 *
 * This function is used to implement class info.
 *
 * Get an ordered list of the interface ids that instances of the class
 * promise to implement. Note that cciISupports is an implicit member
 * of any such list and need not be included.
 *
 * Should set *count = 0 and *array = null and return CCI_OK if getting the
 * list is not supported.
 *
 * @see cciIClassInfo.idl
 **/
typedef CCI_CALLBACK(CCIGetInterfacesProcPtr)(dm_uint32 *countp,
                                              dmIID* **array);

/**
 * CCIGetLanguageHelperProcPtr
 *
 * This function is used to implement class info.
 *
 * Get a language mapping specific helper object that may assist in using
 * objects of this class in a specific lanaguage. For instance, if asked
 * for the helper for nsIProgrammingLanguage::JAVASCRIPT this might return
 * an object that can be QI'd into the nsIXPCScriptable interface to assist
 * XPConnect in supplying JavaScript specific behavior to callers of the
 * instance object.
 *
 * @see: cciIClassInfo.idl, nsIProgrammingLanguage.idl
 *
 * Should return null if no helper available for given language.
 **/
typedef CCI_CALLBACK(CCIGetLanguageHelperProcPtr)(dm_uint32 language,
                                                  cciISupports **helper);

/**
 * cciModuleComponentInfo
 *
 * Use this type to define a list of module component info to pass to
 * CCI_NewGenericModule.
 *
 * @param mDescription           : Class Name of given object
 * @param mCID                   : CID of given object
 * @param mContractID            : Contract ID of given object
 * @param mConstructor           : Constructor of given object
 * @param mRegisterSelfProc      : (optional) Registration Callback
 * @param mUnregisterSelfProc    : (optional) Unregistration Callback
 * @param mFactoryDestructor     : (optional) Destruction Callback
 * @param mGetInterfacesProc     : (optional) Interfaces Callback
 * @param mGetLanguageHelperProc : (optional) Language Helper Callback
 * @param mClassInfoGlobal       : (optional) Global Class Info of given object
 * @param mFlags                 : (optional) Class Info Flags @see cciIClassInfo
 *
 * E.g.:
 *     static cciModuleComponentInfo components[] = { ... };
 *
 * See xpcom/sample/nsSampleModule.cpp for more info.
 */
struct cciModuleComponentInfo {
    const char*                                 mDescription;
    dmCID                                       mCID;
    const char*                                 mContractID;
    CCIConstructorProcPtr                       mConstructor;
    CCIRegisterSelfProcPtr                      mRegisterSelfProc;
    CCIUnregisterSelfProcPtr                    mUnregisterSelfProc;
    CCIFactoryDestructorProcPtr                 mFactoryDestructor;
    CCIGetInterfacesProcPtr                     mGetInterfacesProc;
    CCIGetLanguageHelperProcPtr                 mGetLanguageHelperProc;
    cciIClassInfo **                            mClassInfoGlobal;
    dm_uint32                                   mFlags;
};

/**
 * CCI_RegisterFactoryComponents
 *
 * Use this method to register components from module that are not loaded directly
 * By the CCI framework.
 * Because this is a direct factory registration
 * Components registered that way do not depend from a specific location.
 * @param components an array of components to be registered
 * @param count the number of components in the array
 *
 * @return CCI_OK        Construction successful.
 *         CCI_ERROR*    Method failure which will result in module not being
 *                       loaded.
 **/

CCI_COM_GLUE cci_result
CCI_RegisterFactoryComponents( const cciModuleComponentInfo *components, dm_uint32 count );


/** Module Callbacks **/


/**
 * cciModuleConstructorProc
 *
 * This function is called when the module is first being constructed.
 * @param self module which is being constructed.
 *
 * @return CCI_OK        Construction successful.
 *         CCI_ERROR*    Method failure which will result in module not being
 *                       loaded.
 **/
typedef cci_result (*cciModuleConstructorProc) (cciIModule *self);


/**
 * cciModuleDestructorProc
 *
 * This function is called when the module is being destroyed.
 * @param self module which is being destroyed.
 *
 **/
typedef void (*cciModuleDestructorProc) (cciIModule *self);

/**
 * cciIModule
 *
 * Use this structure to define meta-information about the module
 * itself, including the name, its components, and an optional
 * module-level initialization or shutdown routine.
 *
 * @param mVersion     : Module Info Version
 * @param mModuleName  : Module Name
 * @param mComponents  : Array of Components
 * @param mCount       : Count of mComponents
 * @param mCtor        : Module user defined constructor
 * @param mDtor        : Module user defined destructor
 *
 **/

struct cciModuleInfo {
    dm_uint32                     mVersion;
    const char*                   mModuleName;
    const cciModuleComponentInfo *mComponents;
    dm_uint32                     mCount;
    cciModuleConstructorProc      mCtor;
    cciModuleDestructorProc       mDtor;
};

/**
 * Rev this if you change the cciIModule, and are worried about
 * binary compatibility to deal with older rev's at the same time.)
 */

#define CCI_MODULEINFO_VERSION 1000 // 1.0

extern "C" {
__daim_export cci_result
__daim_call( CCI_NewGenericModule )(cciModuleInfo*,cciIModule* *result);

__daim_export cci_result
__daim_call( CCI_NewGenericFactory )(cciIGenericFactory* *result, const cciModuleComponentInfo *info);
}

#if defined(CCI_TRANSLATE_MODULE_ENTRY_POINT)
#define CCIGETMODULE_ENTRY_POINT(_name)  cci_result _name##_CCIGetModule
#else
#define CCIGETMODULE_ENTRY_POINT(_name)  extern "C" cci_result CCIGetModule
#endif

/**
 * Ease of use Macros which define CCIGetModule for your component.
 * See xpcom/sample/nsSampleModule.cpp for more info.
 *
 **/

#define CCI_IMPL_GETMODULE(_name, _components)                               \
    CCI_IMPL_GETMODULE_WITH_CTOR_DTOR(_name, _components, dm_null, dm_null)

#define CCI_IMPL_GETMODULE_WITH_CTOR(_name, _components, _ctor)              \
    CCI_IMPL_GETMODULE_WITH_CTOR_DTOR(_name, _components, _ctor, dm_null)

#define CCI_IMPL_GETMODULE_WITH_DTOR(_name, _components, _dtor)              \
    CCI_IMPL_GETMODULE_WITH_CTOR_DTOR(_name, _components, dm_null, _dtor)


#define CCI_IMPL_GETMODULE_WITH_CTOR_DTOR(_name, _components, _ctor, _dtor)   \
static cciModuleInfo kModuleInfo = {                                          \
    CCI_MODULEINFO_VERSION,                                                   \
    (#_name),                                                                 \
    (_components),                                                            \
    (sizeof(_components) / sizeof(_components[0])),                           \
    (_ctor),                                                                  \
    (_dtor)                                                                   \
};                                                                            \
CCIGETMODULE_ENTRY_POINT(_name)                                               \
(cciIComponentManager *servMgr,                                               \
            const char*  location,                                            \
            cciIModule**    result)                                           \
{                                                                             \
   return CCI_NewGenericModule(&kModuleInfo, result);                         \
}


/**
 * Ease of use Macros which define direct factory registration
 * for your modules that are not under the control of the CCI framework.
 *
 * Use case are for module loaded from another framework that embed the
 * CCI/DAIM library. This enables creating components in the CCI framework
 * that exposes method that delegate to the embedding framework
 **/

#define CCI_REGISTER_FACTORY_COMPONENTS( _components ) \
  CCI_RegisterFactoryComponents( _components, (sizeof(_components) / sizeof(_components[0])) )


////////////////////////////////////////////////////////////////////////////////

#define CCI_GENERIC_FACTORY_CONSTRUCTOR(_InstanceClass)                       \
static CCI_IMETHODIMP                                                         \
_InstanceClass##Constructor(cciISupports *aOuter, DM_REFIID aIID,             \
                            void **aResult)                                   \
{                                                                             \
    cci_result rv;                                                            \
                                                                              \
    _InstanceClass * inst;                                                    \
                                                                              \
    *aResult = NULL;                                                          \
    if (NULL != aOuter) {                                                     \
        rv = CCI_ERROR_NO_AGGREGATION;                                        \
        return rv;                                                            \
    }                                                                         \
                                                                              \
    inst = new _InstanceClass();                                              \
    if (NULL == inst) {                                                       \
        rv = CCI_ERROR_OUT_OF_MEMORY;                                         \
        return rv;                                                            \
    }                                                                         \
    CCI_ADDREF(inst);                                                         \
    rv = inst->QueryInterface(aIID, aResult);                                 \
    CCI_RELEASE(inst);                                                        \
                                                                              \
    return rv;                                                                \
}                                                                             \


#define CCI_GENERIC_FACTORY_CONSTRUCTOR_INIT(_InstanceClass, _InitMethod)     \
static CCI_IMETHODIMP                                                         \
_InstanceClass##Constructor(cciISupports *aOuter, DM_REFIID aIID,             \
                            void **aResult)                                   \
{                                                                             \
    cci_result rv;                                                            \
                                                                              \
    _InstanceClass * inst;                                                    \
                                                                              \
    *aResult = NULL;                                                          \
    if (NULL != aOuter) {                                                     \
        rv = CCI_ERROR_NO_AGGREGATION;                                        \
        return rv;                                                            \
    }                                                                         \
                                                                              \
    inst = new _InstanceClass();                                              \
    if (NULL == inst) {                                                       \
        rv = CCI_ERROR_OUT_OF_MEMORY;                                         \
        return rv;                                                            \
    }                                                                         \
    CCI_ADDREF(inst);                                                         \
    rv = inst->_InitMethod();                                                 \
    if(CCI_SUCCEEDED(rv)) {                                                   \
        rv = inst->QueryInterface(aIID, aResult);                             \
    }                                                                         \
    CCI_RELEASE(inst);                                                        \
                                                                              \
    return rv;                                                                \
}                                                                             \

// 'Constructor' that uses an existing getter function that gets a singleton.
// NOTE: assumes that getter does an AddRef - so additional AddRef is not done.
#define CCI_GENERIC_FACTORY_SINGLETON_CONSTRUCTOR(_InstanceClass, _GetterProc) \
static CCI_IMETHODIMP                                                          \
_InstanceClass##Constructor(cciISupports *aOuter, DM_REFIID aIID,             \
                            void **aResult)                                   \
{                                                                             \
    cci_result rv;                                                            \
                                                                              \
    _InstanceClass * inst;                                                    \
                                                                              \
    *aResult = NULL;                                                          \
    if (NULL != aOuter) {                                                     \
        rv = CCI_ERROR_NO_AGGREGATION;                                        \
        return rv;                                                            \
    }                                                                         \
                                                                              \
    inst = _GetterProc();                                                     \
    if (NULL == inst) {                                                       \
        rv = CCI_ERROR_OUT_OF_MEMORY;                                         \
        return rv;                                                            \
    }                                                                         \
    /* CCI_ADDREF(inst); */                                                   \
    rv = inst->QueryInterface(aIID, aResult);                                 \
    CCI_RELEASE(inst);                                                        \
                                                                              \
    return rv;                                                                \
}                                                                             \

#endif // cciIGenericFactory_h
