#ifndef cciInterfaceRequestorUtils_h
#define cciInterfaceRequestorUtils_h

/*
 *
 * ***** BEGIN LICENSE BLOCK *****
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
 * The Original Code is the Mozilla browser.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications, Inc.
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
 * ***** END LICENSE BLOCK ***** */

#ifndef cciCOMPtr_h
#include "cciCOMPtr.h"
#endif


CCI_NAMESPACE_BEGIN

// a type-safe shortcut for calling the |GetInterface()| member function
// T must inherit from nsIInterfaceRequestor, but the cast may be ambiguous.
template <class T, class DestinationType>
inline
cci_result
CallGetInterface( T* aSource, DestinationType** aDestination )
{
    CCI_PRECONDITION(aSource, "null parameter");
    CCI_PRECONDITION(aDestination, "null parameter");

    return aSource->GetInterface(CCI_GET_TEMPLATE_IID(DestinationType),
                                 reinterpret_cast<void**>(aDestination));
}

class CCI_COM_GLUE cciGetInterface : public cci_Ptr_helper
  {
    public:
      cciGetInterface( cciISupports* aSource, cci_result* error )
          : mSource(aSource),
            mErrorPtr(error)
        {
          // nothing else to do here
        }

      virtual cci_result CCI_FASTCALL operator()( const dmIID&, void** ) const;

    private:
      cciISupports*          mSource;
      cci_result*            mErrorPtr;
  };

inline
const cciGetInterface
do_GetInterface( cciISupports* aSource, cci_result* error = 0 )
{
  return cciGetInterface(aSource, error);
}

CCI_NAMESPACE_END

#endif // cciInterfaceRequestorUtils_h

