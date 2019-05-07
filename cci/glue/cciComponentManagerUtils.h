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

#ifndef cciComponentManagerUtils_h
#define cciComponentManagerUtils_h

#ifndef cciCOMPtr_h
#include "cciCOMPtr.h"
#endif

#ifndef cciIFactory_h
#include "cciIFactory.h"
#endif

#ifndef cciIComponentManager_h
#include "cciIComponentManager.h"
#endif


CCI_NAMESPACE_BEGIN

CCI_COM_GLUE cci_result
CallCreateInstance
  (const dmCID &aClass, cciISupports *aDelegate, const dmIID &aIID,
   void **aResult);

CCI_COM_GLUE cci_result
CallCreateInstance
  (const char *aContractID, cciISupports *aDelegate, const dmIID &aIID,
   void **aResult);

CCI_COM_GLUE cci_result
CallGetClassObject
  (const dmCID &aClass, const dmIID &aIID, void **aResult);

CCI_COM_GLUE cci_result
CallGetClassObject
  (const char *aContractID, const dmIID &aIID, void **aResult);

CCI_NAMESPACE_END

class CCI_COM_GLUE cciCreateInstanceByCID : public cci_Ptr_helper
{
public:
    cciCreateInstanceByCID( const dmCID& aCID, cciISupports* aOuter, cci_result* aErrorPtr )
        : mCID(aCID),
          mOuter(aOuter),
          mErrorPtr(aErrorPtr)
    {
        // nothing else to do here
    }

    virtual cci_result CCI_FASTCALL operator()( const dmIID&, void** ) const;

private:
    const dmCID&    mCID;
    cciISupports*    mOuter;
    cci_result*       mErrorPtr;
};

class CCI_COM_GLUE cciCreateInstanceByContractID : public cci_Ptr_helper
{
public:
    cciCreateInstanceByContractID( const char* aContractID, cciISupports* aOuter, cci_result* aErrorPtr )
        : mContractID(aContractID),
          mOuter(aOuter),
          mErrorPtr(aErrorPtr)
    {
        // nothing else to do here
    }

    virtual cci_result CCI_FASTCALL operator()( const dmIID&, void** ) const;

private:
    const char*   mContractID;
    cciISupports*  mOuter;
    cci_result*     mErrorPtr;
};

class CCI_COM_GLUE cciCreateInstanceFromFactory : public cci_Ptr_helper
{
public:
    cciCreateInstanceFromFactory( cciIFactory* aFactory, cciISupports* aOuter, cci_result* aErrorPtr )
        : mFactory(aFactory),
          mOuter(aOuter),
          mErrorPtr(aErrorPtr)
    {
        // nothing else to do here
    }

    virtual cci_result CCI_FASTCALL operator()( const dmIID&, void** ) const;

private:
    cciIFactory*   mFactory;
    cciISupports*  mOuter;
    cci_result*    mErrorPtr;
};

CCI_NAMESPACE_BEGIN

inline
const cciCreateInstanceByCID
do_CreateInstance( const dmCID& aCID, cci_result* error = 0 )
{
    return cciCreateInstanceByCID(aCID, 0, error);
}

inline
const cciCreateInstanceByCID
do_CreateInstance( const dmCID& aCID, cciISupports* aOuter, cci_result* error = 0 )
{
    return cciCreateInstanceByCID(aCID, aOuter, error);
}

inline
const cciCreateInstanceByContractID
do_CreateInstance( const char* aContractID, cci_result* error = 0 )
{
    return cciCreateInstanceByContractID(aContractID, 0, error);
}

inline
const cciCreateInstanceByContractID
do_CreateInstance( const char* aContractID, cciISupports* aOuter, cci_result* error = 0 )
{
    return cciCreateInstanceByContractID(aContractID, aOuter, error);
}

inline
const cciCreateInstanceFromFactory
do_CreateInstance( cciIFactory* aFactory, cci_result* error = 0 )
{
    return cciCreateInstanceFromFactory(aFactory, 0, error);
}

inline
const cciCreateInstanceFromFactory
do_CreateInstance( cciIFactory* aFactory, cciISupports* aOuter, cci_result* error = 0 )
{
    return cciCreateInstanceFromFactory(aFactory, aOuter, error);
}


CCI_NAMESPACE_END

class CCI_COM_GLUE cciGetClassObjectByCID : public cci_Ptr_helper
{
public:
    cciGetClassObjectByCID( const dmCID& aCID, cci_result* aErrorPtr )
        : mCID(aCID),
          mErrorPtr(aErrorPtr)
    {
        // nothing else to do here
    }

    virtual cci_result CCI_FASTCALL operator()( const dmIID&, void** ) const;

private:
    const dmCID&  mCID;
    cci_result*   mErrorPtr;
};

class CCI_COM_GLUE cciGetClassObjectByContractID : public cci_Ptr_helper
{
public:
    cciGetClassObjectByContractID( const char* aContractID, cci_result* aErrorPtr )
        : mContractID(aContractID),
          mErrorPtr(aErrorPtr)
    {
        // nothing else to do here
    }

    virtual cci_result CCI_FASTCALL operator()( const dmIID&, void** ) const;

private:
    const char*   mContractID;
    cci_result*   mErrorPtr;
};

CCI_NAMESPACE_BEGIN

/**
 * do_GetClassObject can be used to improve performance of callers
 * that call |CreateInstance| many times.  They can cache the factory
 * and call do_CreateInstance or CallCreateInstance with the cached
 * factory rather than having the component manager retrieve it every
 * time.
 */
inline const cciGetClassObjectByCID
do_GetClassObject( const dmCID& aCID, cci_result* error = 0 )
{
    return cciGetClassObjectByCID(aCID, error);
}

inline const cciGetClassObjectByContractID
do_GetClassObject( const char* aContractID, cci_result* error = 0 )
{
    return cciGetClassObjectByContractID(aContractID, error);
}

// type-safe shortcuts for calling |CreateInstance|
template <class DestinationType>
inline
cci_result
CallCreateInstance( const dmCID &aClass,
                    cciISupports *aDelegate,
                    DestinationType** aDestination )
{
    CCI_PRECONDITION(aDestination, "null parameter");

    return CallCreateInstance(aClass, aDelegate,
                              CCI_GET_TEMPLATE_IID(DestinationType),
                              reinterpret_cast<void**>(aDestination));
}

template <class DestinationType>
inline
cci_result
CallCreateInstance( const dmCID &aClass,
                    DestinationType** aDestination )
{
    CCI_PRECONDITION(aDestination, "null parameter");

    return CallCreateInstance(aClass, dm_null,
                              CCI_GET_TEMPLATE_IID(DestinationType),
                              reinterpret_cast<void**>(aDestination));
}

template <class DestinationType>
inline
cci_result
CallCreateInstance( const char *aContractID,
                    cciISupports *aDelegate,
                    DestinationType** aDestination )
{
    CCI_PRECONDITION(aContractID, "null parameter");
    CCI_PRECONDITION(aDestination, "null parameter");

    return CallCreateInstance(aContractID,
                              aDelegate,
                              CCI_GET_TEMPLATE_IID(DestinationType),
                              reinterpret_cast<void**>(aDestination));
}

template <class DestinationType>
inline
cci_result
CallCreateInstance( const char *aContractID,
                    DestinationType** aDestination )
{
    CCI_PRECONDITION(aContractID, "null parameter");
    CCI_PRECONDITION(aDestination, "null parameter");

    return CallCreateInstance(aContractID, dm_null,
                              CCI_GET_TEMPLATE_IID(DestinationType),
                              reinterpret_cast<void**>(aDestination));
}

template <class DestinationType>
inline
cci_result
CallCreateInstance( cciIFactory *aFactory,
                    cciISupports *aDelegate,
                    DestinationType** aDestination )
{
    CCI_PRECONDITION(aFactory, "null parameter");
    CCI_PRECONDITION(aDestination, "null parameter");

    return aFactory->CreateInstance(aDelegate,
                                    CCI_GET_TEMPLATE_IID(DestinationType),
                                    reinterpret_cast<void**>(aDestination));
}

template <class DestinationType>
inline
cci_result
CallCreateInstance( cciIFactory *aFactory,
                    DestinationType** aDestination )
{
    CCI_PRECONDITION(aFactory, "null parameter");
    CCI_PRECONDITION(aDestination, "null parameter");

    return aFactory->CreateInstance(dm_null,
                                    CCI_GET_TEMPLATE_IID(DestinationType),
                                    reinterpret_cast<void**>(aDestination));
}

template <class DestinationType>
inline
cci_result
CallGetClassObject( const dmCID &aClass,
                    DestinationType** aDestination )
{
    CCI_PRECONDITION(aDestination, "null parameter");

    return CallGetClassObject(aClass,
        CCI_GET_TEMPLATE_IID(DestinationType), reinterpret_cast<void**>(aDestination));
}

template <class DestinationType>
inline
cci_result
CallGetClassObject( const char* aContractID,
                    DestinationType** aDestination )
{
    CCI_PRECONDITION(aDestination, "null parameter");

    return CallGetClassObject(aContractID,
        CCI_GET_TEMPLATE_IID(DestinationType), reinterpret_cast<void**>(aDestination));
}

CCI_NAMESPACE_END

#endif // cciComponentManagerUtils_h
