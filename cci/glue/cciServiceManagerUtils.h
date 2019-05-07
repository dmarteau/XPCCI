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
 * The Original Code is XPCOM.
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

#ifndef cciServiceManagerUtils_h
#define cciServiceManagerUtils_h

#ifndef cciCOMPtr_h
#include "cciCOMPtr.h"
#endif

#ifndef cciIServiceManager_h
#include "cciIServiceManager.h"
#endif

CCI_NAMESPACE_BEGIN

inline
const cciGetServiceByCID
do_GetService(const dmCID& aCID)
{
    return cciGetServiceByCID(aCID);
}

inline
const cciGetServiceByCIDWithError
do_GetService(const dmCID& aCID, cci_result* error)
{
    return cciGetServiceByCIDWithError(aCID, error);
}

inline
const cciGetServiceByContractID
do_GetService(const char* aContractID)
{
    return cciGetServiceByContractID(aContractID);
}

inline
const cciGetServiceByContractIDWithError
do_GetService( const char* aContractID, cci_result* error)
{
    return cciGetServiceByContractIDWithError(aContractID, error);
}

class cciGetServiceFromCategory : public cci_Ptr_helper
{
 public:
    cciGetServiceFromCategory(const char* aCategory, const char* aEntry,
                             cci_result* aErrorPtr)
        : mCategory(aCategory),
        mEntry(aEntry),
        mErrorPtr(aErrorPtr)
        {
            // nothing else to do
        }

    virtual cci_result CCI_FASTCALL operator()( const dmIID&, void** ) const;
 protected:
    const char*                 mCategory;
    const char*                 mEntry;
    cci_result*                   mErrorPtr;
};

inline
const cciGetServiceFromCategory
do_GetServiceFromCategory( const char* category, const char* entry,
                           cci_result* error = 0)
{
    return cciGetServiceFromCategory(category, entry, error);
}


CCI_COM_GLUE cci_result
CallGetService(const dmCID &aClass, const dmIID &aIID, void **aResult);

CCI_COM_GLUE cci_result
CallGetService(const char *aContractID, const dmIID &aIID, void **aResult);

// type-safe shortcuts for calling |GetService|
template <class DestinationType>
inline cci_result
CallGetService( const dmCID &aClass, DestinationType** aDestination)
{
    CCI_PRECONDITION(aDestination, "null parameter");

    return CallGetService(aClass,
                          CCI_GET_TEMPLATE_IID(DestinationType),
                          reinterpret_cast<void**>(aDestination));
}

template <class DestinationType>
inline cci_result
CallGetService( const char *aContractID, DestinationType** aDestination)
{
    CCI_PRECONDITION(aContractID, "null parameter");
    CCI_PRECONDITION(aDestination, "null parameter");

    return CallGetService(aContractID,
                          CCI_GET_TEMPLATE_IID(DestinationType),
                          reinterpret_cast<void**>(aDestination));
}

CCI_NAMESPACE_END


#endif // cciServiceManagerUtils_h
