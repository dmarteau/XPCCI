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
 *  David Marteau (CCI COM implementation)
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

#include "cciComponentManagerUtils.h"
#include "cciServiceManagerUtils.h"

CCI_NAMESPACE_BEGIN

cci_result
CallGetService(const dmCID &aCID, const dmIID &aIID, void **aResult)
{
    cci_Ptr<cciIServiceManager> servMgr;
    cci_result status = CCI_GetServiceManager(getter_AddRefs(servMgr));
    if (servMgr)
        status = servMgr->GetService(aCID, aIID, aResult);
    return status;
}

cci_result
CallGetService(const char *aContractID, const dmIID &aIID, void **aResult)
{
    cci_Ptr<cciIServiceManager> servMgr;
    cci_result status = CCI_GetServiceManager(getter_AddRefs(servMgr));
    if (servMgr)
        status = servMgr->GetServiceByContractID(aContractID, aIID, aResult);
    return status;
}


cci_result
CallCreateInstance(const dmCID &aCID, cciISupports *aDelegate,
                   const dmIID &aIID, void **aResult)
{
    cci_Ptr<cciIComponentManager> compMgr;
    cci_result status = CCI_GetComponentManager(getter_AddRefs(compMgr));
    if (compMgr)
        status = compMgr->CreateInstance(aCID, aDelegate, aIID, aResult);
    return status;
}

cci_result
CallCreateInstance(const char *aContractID, cciISupports *aDelegate,
                   const dmIID &aIID, void **aResult)
{
    cci_Ptr<cciIComponentManager> compMgr;
    cci_result status = CCI_GetComponentManager(getter_AddRefs(compMgr));
    if (compMgr)
        status = compMgr->CreateInstanceByContractID(aContractID, aDelegate,
                                                     aIID, aResult);
    return status;
}

cci_result
CallGetClassObject(const dmCID &aCID, const dmIID &aIID, void **aResult)
{
    cci_Ptr<cciIComponentManager> compMgr;
    cci_result status = CCI_GetComponentManager(getter_AddRefs(compMgr));
    if (compMgr)
        status = compMgr->GetClassObject(aCID, aIID, aResult);
    return status;
}

cci_result
CallGetClassObject(const char *aContractID, const dmIID &aIID, void **aResult)
{
    cci_Ptr<cciIComponentManager> compMgr;
    cci_result status = CCI_GetComponentManager(getter_AddRefs(compMgr));
    if (compMgr)
        status = compMgr->GetClassObjectByContractID(aContractID, aIID,
                                                     aResult);
    return status;
}

CCI_NAMESPACE_END

cci_result
cciCreateInstanceByCID::operator()( const dmIID& aIID, void** aInstancePtr ) const
{
    cci_result status = CCI_NS_(CallCreateInstance)(mCID, mOuter, aIID, aInstancePtr);
    if ( CCI_FAILED(status) )
        *aInstancePtr = 0;
    if ( mErrorPtr )
        *mErrorPtr = status;
    return status;
}

cci_result
cciCreateInstanceByContractID::operator()( const dmIID& aIID, void** aInstancePtr ) const
{
    cci_result status = CCI_NS_(CallCreateInstance)(mContractID, mOuter, aIID, aInstancePtr);
    if (CCI_FAILED(status))
        *aInstancePtr = 0;
    if ( mErrorPtr )
        *mErrorPtr = status;
    return status;
}

cci_result
cciCreateInstanceFromFactory::operator()( const dmIID& aIID, void** aInstancePtr ) const
{
    cci_result status = mFactory->CreateInstance(mOuter, aIID, aInstancePtr);
    if ( CCI_FAILED(status) )
        *aInstancePtr = 0;
    if ( mErrorPtr )
        *mErrorPtr = status;
    return status;
}


cci_result
cciGetClassObjectByCID::operator()( const dmIID& aIID, void** aInstancePtr ) const
{
    cci_result status = CCI_NS_(CallGetClassObject)(mCID, aIID, aInstancePtr);
    if ( CCI_FAILED(status) )
        *aInstancePtr = 0;
    if ( mErrorPtr )
        *mErrorPtr = status;
    return status;
}

cci_result
cciGetClassObjectByContractID::operator()( const dmIID& aIID, void** aInstancePtr ) const
{
    cci_result status = CCI_NS_(CallGetClassObject)(mContractID, aIID, aInstancePtr);
    if ( CCI_FAILED(status) )
        *aInstancePtr = 0;
    if ( mErrorPtr )
        *mErrorPtr = status;
    return status;
}


cci_result
cciGetServiceByCID::operator()( const dmIID& aIID, void** aInstancePtr ) const
{
    cci_result status = CCI_NS_(CallGetService)(mCID, aIID, aInstancePtr);
    if ( CCI_FAILED(status) )
        *aInstancePtr = 0;

    return status;
}

cci_result
cciGetServiceByCIDWithError::operator()( const dmIID& aIID, void** aInstancePtr ) const
{
    cci_result status = CCI_NS_(CallGetService)(mCID, aIID, aInstancePtr);
    if ( CCI_FAILED(status) )
        *aInstancePtr = 0;

    if ( mErrorPtr )
        *mErrorPtr = status;
    return status;
}

cci_result
cciGetServiceByContractID::operator()( const dmIID& aIID, void** aInstancePtr ) const
{
    cci_result status = CCI_NS_(CallGetService)(mContractID, aIID, aInstancePtr);
    if ( CCI_FAILED(status) )
        *aInstancePtr = 0;

    return status;
}

cci_result
cciGetServiceByContractIDWithError::operator()( const dmIID& aIID, void** aInstancePtr ) const
{
    cci_result status = CCI_NS_(CallGetService)(mContractID, aIID, aInstancePtr);
    if ( CCI_FAILED(status) )
        *aInstancePtr = 0;

    if ( mErrorPtr )
        *mErrorPtr = status;
    return status;
}
