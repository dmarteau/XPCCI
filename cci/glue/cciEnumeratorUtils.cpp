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
 *
 * Code moved from nsEmptyEnumerator.cpp:
 *   L. David Baron <dbaron@dbaron.org>
 *   Pierre Phaneuf <pp@ludusdesign.com>
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


#include "cciString.h"
#include "cciEnumeratorUtils.h"

#include "cciISimpleEnumerator.h"
#include "cciIStringEnumerator.h"

#include "cciCOMPtr.h"

class EmptyEnumeratorImpl final: public cciISimpleEnumerator,
                                 public cciIUTF8StringEnumerator,
                                 public cciIStringEnumerator
{
public:
    // cciISupports interface
    CCI_DECL_ISUPPORTS_INHERITED  // not really inherited, but no mRefCnt

    // cciISimpleEnumerator
    CCI_DECL_ISIMPLEENUMERATOR
    CCI_DECL_IUTF8STRINGENUMERATOR
    // can't use CCI_DECL_NSISTRINGENUMERATOR because they share the
    // HasMore() signature
    CCI_IMETHOD GetNext(dmAString& aResult) override;

    static EmptyEnumeratorImpl* GetInstance() {
        return const_cast<EmptyEnumeratorImpl*>(&kInstance);
    }

private:
    static const EmptyEnumeratorImpl kInstance;
    
    EmptyEnumeratorImpl() {}
};

// cciISupports interface
CCI_IMETHODIMP_(cci_refcnt) EmptyEnumeratorImpl::AddRef(void)
{
    return 2;
}

CCI_IMETHODIMP_(cci_refcnt) EmptyEnumeratorImpl::Release(void)
{
    return 1;
}

CCI_IMPL_QUERY_INTERFACE1(EmptyEnumeratorImpl, cciISimpleEnumerator)

// cciISimpleEnumerator interface
CCI_IMETHODIMP_(bool) EmptyEnumeratorImpl::HasMoreElements()
{
  return false;
}

CCI_IMETHODIMP_(bool)  EmptyEnumeratorImpl::HasMore()
{
  return false;
}

CCI_IMETHODIMP EmptyEnumeratorImpl::GetNext(cciISupports** aResult)
{
    return CCI_ERROR_UNEXPECTED;
}

CCI_IMETHODIMP EmptyEnumeratorImpl::GetNext(dmACString& aResult)
{
    return CCI_ERROR_UNEXPECTED;
}

CCI_IMETHODIMP EmptyEnumeratorImpl::GetNext(dmAString& aResult)
{
    return CCI_ERROR_UNEXPECTED;
}

const EmptyEnumeratorImpl EmptyEnumeratorImpl::kInstance;

cci_result
CCI_NewEmptyEnumerator(cciISimpleEnumerator** aResult)
{
    *aResult = EmptyEnumeratorImpl::GetInstance();
    return CCI_OK;
}

////////////////////////////////////////////////////////////////////////////////

class cciSingletonEnumerator final: public cciISimpleEnumerator
{
public:
    CCI_DECL_ISUPPORTS

    // cciISimpleEnumerator methods
    CCI_IMETHOD_(bool) HasMoreElements() override;
    CCI_IMETHOD GetNext(cciISupports** aResult) override;

    cciSingletonEnumerator(cciISupports* aValue);

private:
    ~cciSingletonEnumerator();

protected:
    cciISupports* mValue;
    bool mConsumed;
};

cciSingletonEnumerator::cciSingletonEnumerator(cciISupports* aValue)
    : mValue(aValue)
{
    CCI_IF_ADDREF(mValue);
    mConsumed = (mValue ? false : true);
}

cciSingletonEnumerator::~cciSingletonEnumerator()
{
    CCI_IF_RELEASE(mValue);
}

CCI_IMPL_ISUPPORTS1(cciSingletonEnumerator, cciISimpleEnumerator)

CCI_IMETHODIMP_(bool)
cciSingletonEnumerator::HasMoreElements()
{
  return !mConsumed;
}


CCI_IMETHODIMP
cciSingletonEnumerator::GetNext(cciISupports** aResult)
{
    CCI_PRECONDITION(aResult != 0, "null ptr");
    if (! aResult)
        return CCI_ERROR_NULL_POINTER;

    if (mConsumed)
        return CCI_ERROR_UNEXPECTED;

    mConsumed = true;

    *aResult = mValue;
    CCI_ADDREF(*aResult);
    return CCI_OK;
}

cci_result
CCI_NewSingletonEnumerator(cciISimpleEnumerator* *result,
                          cciISupports* singleton)
{
    cciSingletonEnumerator* enumer = new cciSingletonEnumerator(singleton);
    if (enumer == dm_null)
        return CCI_ERROR_OUT_OF_MEMORY;
    *result = enumer;
    CCI_ADDREF(*result);
    return CCI_OK;
}

////////////////////////////////////////////////////////////////////////////////

class cciUnionEnumerator final: public cciISimpleEnumerator
{
public:
    CCI_DECL_ISUPPORTS

    // cciISimpleEnumerator methods
    CCI_IMETHOD_(bool) HasMoreElements() override;
    CCI_IMETHOD GetNext(cciISupports** aResult) override;

    cciUnionEnumerator(cciISimpleEnumerator* firstEnumerator,
                      cciISimpleEnumerator* secondEnumerator);

private:
    ~cciUnionEnumerator();

protected:
    cci_Ptr<cciISimpleEnumerator> mFirstEnumerator, mSecondEnumerator;
    bool mConsumed;
    bool mAtSecond;
};

cciUnionEnumerator::cciUnionEnumerator(cciISimpleEnumerator* firstEnumerator,
                                     cciISimpleEnumerator* secondEnumerator)
    : mFirstEnumerator(firstEnumerator),
      mSecondEnumerator(secondEnumerator),
      mConsumed(false), mAtSecond(false)
{
}

cciUnionEnumerator::~cciUnionEnumerator()
{
}

CCI_IMPL_ISUPPORTS1(cciUnionEnumerator, cciISimpleEnumerator)

CCI_IMETHODIMP_(bool)
cciUnionEnumerator::HasMoreElements()
{
    if (mConsumed)
        return false;

    if (! mAtSecond)
    {
      if(mFirstEnumerator->HasMoreElements())
         return true;

      mAtSecond = true;
    }

    if (mSecondEnumerator->HasMoreElements())
        return true;

    mConsumed = true;
    return false;
}

CCI_IMETHODIMP
cciUnionEnumerator::GetNext(cciISupports** aResult)
{
    CCI_PRECONDITION(aResult != 0, "null ptr");
    if (! aResult)
        return CCI_ERROR_NULL_POINTER;

    if (mConsumed)
        return CCI_ERROR_UNEXPECTED;

    if (! mAtSecond)
        return mFirstEnumerator->GetNext(aResult);

    return mSecondEnumerator->GetNext(aResult);
}

cci_result
CCI_NewUnionEnumerator(cciISimpleEnumerator* *result,
                      cciISimpleEnumerator* firstEnumerator,
                      cciISimpleEnumerator* secondEnumerator)
{
    *result = dm_null;
    if (! firstEnumerator) {
        *result = secondEnumerator;
    } else if (! secondEnumerator) {
        *result = firstEnumerator;
    } else {
        cciUnionEnumerator* enumer = new cciUnionEnumerator(firstEnumerator, secondEnumerator);
        if (enumer == dm_null)
            return CCI_ERROR_OUT_OF_MEMORY;
        *result = enumer;
    }
    CCI_ADDREF(*result);
    return CCI_OK;
}
