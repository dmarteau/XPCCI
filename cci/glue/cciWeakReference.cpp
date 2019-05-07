/* 
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
 *   Scott Collins <scc@netscape.com>
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

// cciWeakReference.cpp

#include "cciWeakReference.h"
#include "cciCOMPtr.h"

class cciWeakReference final : public cciIWeakReference
  {
    public:
    // cciISupports...
      CCI_DECL_ISUPPORTS

    // cciIWeakReference...
      CCI_DECL_IWEAKREFERENCE

    private:
      friend class cciSupportsWeakReference;

      cciWeakReference( cciSupportsWeakReference* referent )
          : mReferent(referent)
          // ...I can only be constructed by an |cciSupportsWeakReference|
        {
          // nothing else to do here
        }

      ~cciWeakReference()
           // ...I will only be destroyed by calling |delete| myself.
        {
          if ( mReferent )
            mReferent->NoticeProxyDestruction();
        }

      void
      NoticeReferentDestruction()
          // ...called (only) by an |cciSupportsWeakReference| from _its_ dtor.
        {
          mReferent = 0;
        }

      cciSupportsWeakReference*  mReferent;
  };

cci_result
cciQueryReferent::operator()( const dmIID& aIID, void** answer ) const
  {
    cci_result status;
    if ( mWeakPtr )
      {
        if ( CCI_FAILED(status = mWeakPtr->QueryReferent(aIID, answer)) )
          *answer = 0;
      }
    else
      status = CCI_ERROR_NULL_POINTER;

    if ( mErrorPtr )
      *mErrorPtr = status;
    return status;
  }

CCI_COM_GLUE cciIWeakReference*  // or else |already_AddRefed<cciIWeakReference>|
CCI_GetWeakReference( cciISupports* aInstancePtr, cci_result* aErrorPtr )
  {
    cci_result status;

    cciIWeakReference* result = dm_null;

    if ( aInstancePtr )
      {
        cci_Ptr<cciISupportsWeakReference> factoryPtr = do_QueryInterface(aInstancePtr, &status);
        CCI_ASSERTION(factoryPtr, "Oops!  You're asking for a weak reference to an object that doesn't support that.");
        if ( factoryPtr )
          {
            status = factoryPtr->GetWeakReference(&result);
          }
        // else, |status| has already been set by |do_QueryInterface|
      }
    else
      status = CCI_ERROR_NULL_POINTER;

    if ( aErrorPtr )
      *aErrorPtr = status;
    return result;
  }

CCI_COM_GLUE cci_result
cciSupportsWeakReference::GetWeakReference( cciIWeakReference** aInstancePtr )
  {
    if ( !aInstancePtr )
      return CCI_ERROR_NULL_POINTER;

    if ( !mProxy )
      mProxy = new cciWeakReference(this);
    *aInstancePtr = mProxy;

    cci_result status;
    if ( !*aInstancePtr )
      status = CCI_ERROR_OUT_OF_MEMORY;
    else
      {
        CCI_ADDREF(*aInstancePtr);
        status = CCI_OK;
      }

    return status;
  }

CCI_IMPL_ISUPPORTS1(cciWeakReference, cciIWeakReference)

CCI_IMETHODIMP
cciWeakReference::QueryReferent( const dmIID& aIID, void** aInstancePtr )
  {
    return mReferent ? mReferent->QueryInterface(aIID, aInstancePtr) : CCI_ERROR_NULL_POINTER;
  }

void
cciSupportsWeakReference::ClearWeakReferences()
  {
    if ( mProxy )
      {
        mProxy->NoticeReferentDestruction();
        mProxy = 0;
      }
  }

