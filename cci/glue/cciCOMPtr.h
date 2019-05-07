#ifndef cciCOMPtr_h
#define cciCOMPtr_h


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
 *   Scott Collins <scc@mozilla.org> (original author)
 *   L. David Baron <dbaron@dbaron.org>
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

#ifndef cciISupportsBase_h
#include "cciISupportsBase.h"
#endif

#ifndef cciStubs_h
#include "cciStubs.h"
#endif

#ifndef cciSupportsImpl_h
#include "cciSupportsImpl.h"
#endif

/**
 * This is a CCI  implementation of the Mozilla XPCOM pointer
 *
 * It should be used each time you need a pointer an cciISupports object
 * It has been adapted to check the validity of the owned object before
 * any call of the object method
 *
 * It is also in charge of releasing the object when going out of scope
 *
 * See the User Manual at:
 *   http://www.mozilla.org/projects/xpcom/cci_Ptr.html
 *
 */

/*
  WARNING:
    This file defines several macros for internal use only.  These macros begin with the
    prefix |NSCAP_|.  Do not use these macros in your own code.  They are for internal use
    only for cross-platform compatibility, and are subject to change without notice.
*/


#ifdef _MSC_VER
  #define CCI_CAP_FEATURE_INLINE_STARTASSIGNMENT
    // under VC++, we win by inlining StartAssignment

    // Also under VC++, at the highest warning level, we are overwhelmed  with warnings
    //  about (unused) inline functions being removed.  This is to be expected with
    //  templates, so we disable the warning.
  #pragma warning( disable: 4514 )
#endif

#ifdef __GNUC__ 
  // Our use of cci_Ptr_base::mRawPtr violates the C++ standard's aliasing
  // rules. Mark it with the may_alias attribute so that gcc 3.3 and higher
  // don't reorder instructions based on aliasing assumptions for
  // this variable.  Fortunately, gcc versions < 3.3 do not do any
  // optimizations that break cci_Ptr.

  #define DM_MAY_ALIAS_PTR(t)    t*  __attribute__((__may_alias__))
#else
  #define DM_MAY_ALIAS_PTR(t)    t*
#endif

  /*
    The following two macros (|CCICAP_ADDREF| and |CCICAP_RELEASE| )
      allow external clients the ability to add logging or other interesting debug facilities.
      In fact, if you want |cci_Ptr| to participate in the standard logging facility, you
      provide (e.g., in "nsTraceRefcnt.h") suitable definitions

        #define CCICAP_ADDREF(this, ptr)         CCI_ADDREF(ptr)
        #define CCICAP_RELEASE(this, ptr)        CCI_RELEASE(ptr)
  */

#ifndef CCICAP_ADDREF
  #define CCICAP_ADDREF(this, ptr)     (ptr)->AddRef()
#endif

#ifndef CCICAP_RELEASE
  #define CCICAP_RELEASE(this, ptr)    \
  DM_BEGIN_MACRO                       \
   if(ptr)                             \
     (ptr)->Release();                 \
  DM_END_MACRO
#endif


CCI_NAMESPACE_BEGIN

template <class T>
struct already_AddRefed
    /*
      ...cooperates with |cci_Ptr| to allow you to assign in a pointer _without_
      |AddRef|ing it.  You might want to use this as a return type from a function
      that produces an already |AddRef|ed pointer as a result.

      See also |getter_AddRefs()|, |dont_AddRef()|, and |class cciGetterAddRefs|.

      This type should be a nested class inside |cci_Ptr<T>|.

      Yes, |already_AddRefed| could have been implemented as an |cci_Ptr_helper| to
      avoid adding specialized machinery to |cci_Ptr| ... but this is the simplest
      case, and perhaps worth the savings in time and space that its specific
      implementation affords over the more general solution offered by
      |cci_Ptr_helper|.
    */


  {
    already_AddRefed( T* aRawPtr )
        : mRawPtr(aRawPtr)
      {
        // nothing else to do here
      }

    T* get() const { return mRawPtr; }

    T* mRawPtr;
  };

template <class T>
inline const already_AddRefed<T>
getter_AddRefs( T* aRawPtr )
{
  return already_AddRefed<T>(aRawPtr);
}

template <class T>
inline const already_AddRefed<T>
getter_AddRefs( const already_AddRefed<T> aAlreadyAddRefedPtr )
{
  return aAlreadyAddRefedPtr;
}

template <class T>
inline const already_AddRefed<T>
dont_AddRef( T* aRawPtr )
{
  return already_AddRefed<T>(aRawPtr);
}

template <class T>
inline const already_AddRefed<T>&&
dont_AddRef( const already_AddRefed<T> aAlreadyAddRefedPtr )
{
  return aAlreadyAddRefedPtr;
}

CCI_NAMESPACE_END

class cci_Ptr_helper
    /*
      An |cci_Ptr_helper| transforms commonly called getters into typesafe forms
      that are more convenient to call, and more efficient to use with |cci_Ptr|s.
      Good candidates for helpers are |QueryInterface()|, |CreateInstance()|, etc.

      Here are the rules for a helper:
        - it implements |operator()| to produce an interface pointer
        - (except for its name) |operator()| is a valid [XP]COM `getter'
        - the interface pointer that it returns is already |AddRef()|ed (as from any good getter)
        - it matches the type requested with the supplied |dmIID| argument
        - its constructor provides an optional |cci_result*| that |operator()| can fill
          in with an error when it is executed

      See |class cciGetInterface| for an example.
    */
  {
    public:
      virtual cci_result CCI_FASTCALL operator()( const dmIID&, void** ) const = 0;
  };

/*
  |cciQueryInterface| could have been implemented as an |cci_Ptr_helper| to
  avoid adding specialized machinery in |cci_Ptr|, But |do_QueryInterface|
  is called often enough that the codesize savings are big enough to
  warrant the specialcasing.
*/

class cciQueryInterface
  {
    public:
      explicit
      cciQueryInterface( cciISupports* aRawPtr )
          : mRawPtr(aRawPtr)
        {
          // nothing else to do here
        }

      cci_result CCI_FASTCALL operator()( const dmIID& aIID, void** ) const;

    private:
      cciISupports*  mRawPtr;
  };

class cciQueryInterfaceWithError
  {
    public:
      cciQueryInterfaceWithError( cciISupports* aRawPtr, cci_result* error )
          : mRawPtr(aRawPtr),
            mErrorPtr(error)
        {
          // nothing else to do here
        }

      cci_result CCI_FASTCALL operator()( const dmIID& aIID, void** ) const;

    private:
      cciISupports*  mRawPtr;
      cci_result*     mErrorPtr;
  };

CCI_NAMESPACE_BEGIN

inline
cciQueryInterface
do_QueryInterface( cciISupports* aRawPtr )
  {
    return cciQueryInterface(aRawPtr);
  }

inline
cciQueryInterfaceWithError
do_QueryInterface( cciISupports* aRawPtr, cci_result* error )
  {
    return cciQueryInterfaceWithError(aRawPtr, error);
  }

template <class T>
inline
void
do_QueryInterface( already_AddRefed<T>& )
  {
    // This signature exists solely to _stop_ you from doing the bad thing.
    //  Saying |do_QueryInterface()| on a pointer that is not otherwise owned by
    //  someone else is an automatic leak.  See <http://bugzilla.mozilla.org/show_bug.cgi?id=8221>.
  }

template <class T>
inline
void
do_QueryInterface( already_AddRefed<T>&, cci_result* )
  {
    // This signature exists solely to _stop_ you from doing the bad thing.
    //  Saying |do_QueryInterface()| on a pointer that is not otherwise owned by
    //  someone else is an automatic leak.  See <http://bugzilla.mozilla.org/show_bug.cgi?id=8221>.
  }

CCI_NAMESPACE_END
////////////////////////////////////////////////////////////////////////////
// Using servicemanager with COMPtrs
class cciGetServiceByCID
{
 public:
    explicit cciGetServiceByCID(const dmCID& aCID)
        : mCID(aCID)
        {
            // nothing else to do
        }

    cci_result CCI_FASTCALL operator()( const dmIID&, void** ) const;

 private:
    const dmCID&                mCID;
};

class cciGetServiceByCIDWithError
{
 public:
    cciGetServiceByCIDWithError( const dmCID& aCID, cci_result* aErrorPtr )
        : mCID(aCID),
          mErrorPtr(aErrorPtr)
        {
            // nothing else to do
        }

    cci_result CCI_FASTCALL operator()( const dmIID&, void** ) const;

 private:
    const dmCID&                mCID;
    cci_result*                 mErrorPtr;
};

class cciGetServiceByContractID
{
 public:
    explicit cciGetServiceByContractID(const char* aContractID)
        : mContractID(aContractID)
        {
            // nothing else to do
        }

    cci_result CCI_FASTCALL operator()( const dmIID&, void** ) const;

 private:
    const char*                 mContractID;
};

class cciGetServiceByContractIDWithError
{
 public:
    cciGetServiceByContractIDWithError(const char* aContractID, cci_result* aErrorPtr)
        : mContractID(aContractID),
          mErrorPtr(aErrorPtr)
        {
            // nothing else to do
        }

    cci_result CCI_FASTCALL operator()( const dmIID&, void** ) const;

 private:
    const char*                 mContractID;
    cci_result*                   mErrorPtr;
};

class cci_Ptr_base
    /*
      ...factors implementation for all template versions of |cci_Ptr|.
    */
  {
    public:
      cci_Ptr_base( cciISupports* rawPtr = 0 )
          : mRawPtr(rawPtr)
        {
          // nothing else to do here
        }

      CCI_CONSTRUCTOR_FASTCALL ~cci_Ptr_base();

      void CCI_FASTCALL   assign_with_AddRef( cciISupports* );
      void CCI_FASTCALL   assign_from_qi( const cciQueryInterface, const dmIID& );
      void CCI_FASTCALL   assign_from_qi_with_error( const cciQueryInterfaceWithError&, const dmIID& );
      void CCI_FASTCALL   assign_from_gs_cid( const cciGetServiceByCID, const dmIID& );
      void CCI_FASTCALL   assign_from_gs_cid_with_error( const cciGetServiceByCIDWithError&, const dmIID& );
      void CCI_FASTCALL   assign_from_gs_contractid( const cciGetServiceByContractID, const dmIID& );
      void CCI_FASTCALL   assign_from_gs_contractid_with_error( const cciGetServiceByContractIDWithError&, const dmIID& );
      void CCI_FASTCALL   assign_from_helper( const cci_Ptr_helper&, const dmIID& );
      void** CCI_FASTCALL begin_assignment();

    protected:
      DM_MAY_ALIAS_PTR(cciISupports) mRawPtr;

      void
      assign_assuming_AddRef( cciISupports* newPtr )
        {
            /*
              |AddRef()|ing the new value (before entering this function) before
              |Release()|ing the old lets us safely ignore the self-assignment case.
              We must, however, be careful only to |Release()| _after_ doing the
              assignment, in case the |Release()| leads to our _own_ destruction,
              which would, in turn, cause an incorrect second |Release()| of our old
              pointer.  Thank <waterson@netscape.com> for discovering this.
            */
          cciISupports* oldPtr = mRawPtr;
          mRawPtr = newPtr;
          if ( oldPtr )
            CCICAP_RELEASE(this, oldPtr);
        }
  };

// template <class T> class cciGetterAddRefs;

template <class T>
class cci_Ptr : private cci_Ptr_base
  {
    public:
      typedef T element_type;

    #define CCI_CAP_CTOR_BASE(x) cci_Ptr_base(x)

        // Constructors

      cci_Ptr()
        : CCI_CAP_CTOR_BASE(0)
          // default constructor
        {
        }

      cci_Ptr( const cci_Ptr<T>& aSmartPtr )
            : CCI_CAP_CTOR_BASE(aSmartPtr.mRawPtr)
          // copy-constructor
        {
          if ( mRawPtr )
            CCICAP_ADDREF(this, mRawPtr);
        }

      cci_Ptr( cci_Ptr<T>&& aSmartPtr )
            : CCI_CAP_CTOR_BASE(aSmartPtr.mRawPtr)
          // move operator
        {
            aSmartPtr.mRawPtr = nullptr;
        }

      cci_Ptr( T* aRawPtr )
            : CCI_CAP_CTOR_BASE(aRawPtr)
          // construct from a raw pointer (of the right type)
        {
          if ( mRawPtr )
            CCICAP_ADDREF(this, mRawPtr);
        }

      cci_Ptr( const CCI_NS_(already_AddRefed)<T>& aSmartPtr )
            : CCI_CAP_CTOR_BASE(aSmartPtr.mRawPtr)
          // construct from |dont_AddRef(expr)|
        {
        }

      cci_Ptr( const CCI_NS_(already_AddRefed)<T>&& aSmartPtr )
            : CCI_CAP_CTOR_BASE(aSmartPtr.mRawPtr)
          // construct from |dont_AddRef(expr)|
        {
        }

      cci_Ptr( const cciQueryInterface qi )
            : CCI_CAP_CTOR_BASE(0)
          // construct from |do_QueryInterface(expr)|
        {
          assign_from_qi(qi, CCI_GET_TEMPLATE_IID(T));
        }

      cci_Ptr( const cciQueryInterfaceWithError& qi )
            : CCI_CAP_CTOR_BASE(0)
          // construct from |do_QueryInterface(expr, &rv)|
        {
          assign_from_qi_with_error(qi, CCI_GET_TEMPLATE_IID(T));
        }

      cci_Ptr( const cciGetServiceByCID gs )
            : CCI_CAP_CTOR_BASE(0)
          // construct from |do_GetService(cid_expr)|
        {
          assign_from_gs_cid(gs, CCI_GET_TEMPLATE_IID(T));
        }

      cci_Ptr( const cciGetServiceByCIDWithError& gs )
            : CCI_CAP_CTOR_BASE(0)
          // construct from |do_GetService(cid_expr, &rv)|
        {
          assign_from_gs_cid_with_error(gs, CCI_GET_TEMPLATE_IID(T));
        }

      cci_Ptr( const cciGetServiceByContractID gs )
            : CCI_CAP_CTOR_BASE(0)
          // construct from |do_GetService(contractid_expr)|
        {
          assign_from_gs_contractid(gs, CCI_GET_TEMPLATE_IID(T));
        }

      cci_Ptr( const cciGetServiceByContractIDWithError& gs )
            : CCI_CAP_CTOR_BASE(0)
          // construct from |do_GetService(contractid_expr, &rv)|
        {
          assign_from_gs_contractid_with_error(gs, CCI_GET_TEMPLATE_IID(T));
        }

      cci_Ptr( const cci_Ptr_helper& helper )
            : CCI_CAP_CTOR_BASE(0)
          // ...and finally, anything else we might need to construct from
          //  can exploit the |cci_Ptr_helper| facility
        {
          assign_from_helper(helper, CCI_GET_TEMPLATE_IID(T));
        }


        // Assignment operators

      cci_Ptr<T>& operator=( const cci_Ptr<T>& rhs )
          // copy assignment operator
        {
          assign_with_AddRef(rhs.mRawPtr);
          return *this;
        }

      cci_Ptr<T>& operator=( T* rhs )
          // assign from a raw pointer (of the right type)
        {
          assign_with_AddRef(rhs);
          return *this;
        }

      cci_Ptr<T>&  operator=( const CCI_NS_(already_AddRefed)<T>& rhs )
          // assign from |dont_AddRef(expr)|
        {
          assign_assuming_AddRef(rhs.mRawPtr);
          return *this;
        }

      cci_Ptr<T>& operator=( const cciQueryInterface rhs )
          // assign from |do_QueryInterface(expr)|
        {
          assign_from_qi(rhs, CCI_GET_TEMPLATE_IID(T));
          return *this;
        }

      cci_Ptr<T>& operator=( const cciQueryInterfaceWithError& rhs )
          // assign from |do_QueryInterface(expr, &rv)|
        {
          assign_from_qi_with_error(rhs, CCI_GET_TEMPLATE_IID(T));
          return *this;
        }

      cci_Ptr<T>&  operator=( const cciGetServiceByCID rhs )
          // assign from |do_GetService(cid_expr)|
        {
          assign_from_gs_cid(rhs, CCI_GET_TEMPLATE_IID(T));
          return *this;
        }

      cci_Ptr<T>& operator=( const cciGetServiceByCIDWithError& rhs )
          // assign from |do_GetService(cid_expr, &rv)|
        {
          assign_from_gs_cid_with_error(rhs, CCI_GET_TEMPLATE_IID(T));
          return *this;
        }

      cci_Ptr<T>&  operator=( const cciGetServiceByContractID rhs )
          // assign from |do_GetService(contractid_expr)|
        {
          assign_from_gs_contractid(rhs, CCI_GET_TEMPLATE_IID(T));
          return *this;
        }

      cci_Ptr<T>& operator=( const cciGetServiceByContractIDWithError& rhs )
          // assign from |do_GetService(contractid_expr, &rv)|
        {
          assign_from_gs_contractid_with_error(rhs, CCI_GET_TEMPLATE_IID(T));
          return *this;
        }

      cci_Ptr<T>& operator=( const cci_Ptr_helper& rhs )
          // ...and finally, anything else we might need to assign from
          //  can exploit the |cci_Ptr_helper| facility.
        {
          assign_from_helper(rhs, CCI_GET_TEMPLATE_IID(T));
          return *this;
        }

      void swap( cci_Ptr<T>& rhs )
          // ...exchange ownership with |rhs|; can save a pair of refcount operations
        {
          cciISupports* temp = rhs.mRawPtr;
          rhs.mRawPtr = mRawPtr;
          mRawPtr = temp;
        }

      void swap( T*& rhs )
          // ...exchange ownership with |rhs|; can save a pair of refcount operations
        {
          cciISupports* temp = rhs;
          rhs = reinterpret_cast<T*>(mRawPtr);
          mRawPtr = temp;
        }


        // Other pointer operators

      CCI_NS_(already_AddRefed)<T>
      forget()
          // return the value of mRawPtr and null out mRawPtr. Useful for
          // already_AddRefed return values.
        {
          T* temp = 0;
          swap(temp);
          return temp;
        }

      void
      forget( T** rhs )
          // Set the target of rhs to the value of mRawPtr and null out mRawPtr.
          // Useful to avoid unnecessary AddRef/Release pairs with "out"
          // parameters.
        {
          CCI_ASSERTION(rhs, "Null pointer passed to forget!");
          *rhs = 0;
          swap(*rhs);
        }

      // Prefer the implicit conversion provided automatically by
      // |operator T*() const|. Use |get()| to resolve ambiguity or to get a
      // castable pointer.
      T* get() const { return reinterpret_cast<T*>(mRawPtr); }

      // Makes an nsCOMPtr act like its underlying raw pointer type whenever it is
      // used in a context where a raw pointer is expected. It is this operator
      // that makes an nsCOMPtr substitutable for a raw pointer.
      //
      // Prefer the implicit use of this operator to calling |get()|, except where
      // necessary to resolve ambiguity.
      operator T*() const & { return get(); }

      // Don't allow implicit conversion of temporary nsCOMPtr to raw pointer,
      // because the refcount might be one and the pointer will immediately become
      // invalid.
      operator T*() const && = delete;

      // Needed to avoid the deleted operator above
      explicit operator bool() const { return !!mRawPtr; }

      T* operator->() const
      {
        CCI_PRECONDITION(mRawPtr != 0, "You can't dereference a NULL cci_Ptr with operator->().");
        return get();
      }

      // This is not intended to be used by clients.  See |address_of|
      // below.
      cci_Ptr<T>* get_address() { return this; }
      const cci_Ptr<T>*  get_address() const { return this; }

    public:
      T& operator*() const 
      {
          CCI_PRECONDITION(mRawPtr != 0, "You can't dereference a NULL cci_Ptr with operator*().");
          return *get();
      }

      T**
      StartAssignment()
        {
#ifndef CCI_CAP_FEATURE_INLINE_STARTASSIGNMENT
          return reinterpret_cast<T**>(begin_assignment());
#else
          assign_assuming_AddRef(0);
          return reinterpret_cast<T**>(&mRawPtr);
#endif
        }
  };



  /*
    Specializing |cci_Ptr| for |cciISupports| allows us to use |cci_Ptr<cciISupports>| the
    same way people use |cciISupports*| and |void*|, i.e., as a `catch-all' pointer pointing
    to any valid [XP]COM interface.  Otherwise, an |cci_Ptr<cciISupports>| would only be able
    to point to the single [XP]COM-correct |cciISupports| instance within an object; extra
    querying ensues.  Clients need to be able to pass around arbitrary interface pointers,
    without hassles, through intermediary code that doesn't know the exact type.
  */

template<>
class cci_Ptr<cciISupports> : private cci_Ptr_base
  {
    public:
      typedef cciISupports element_type;

        // Constructors

      cci_Ptr()
            : cci_Ptr_base(0)
          // default constructor
        {
        }

      cci_Ptr( const cci_Ptr<cciISupports>& aSmartPtr )
            : cci_Ptr_base(aSmartPtr.mRawPtr)
          // copy constructor
        {
          if ( mRawPtr )
            CCICAP_ADDREF(this, mRawPtr);
        }

      cci_Ptr( cciISupports* aRawPtr )
            : cci_Ptr_base(aRawPtr)
          // construct from a raw pointer (of the right type)
        {
          if ( mRawPtr )
            CCICAP_ADDREF(this, mRawPtr);
        }

      cci_Ptr( const CCI_NS_(already_AddRefed)<cciISupports>& aSmartPtr )
            : cci_Ptr_base(aSmartPtr.mRawPtr)
          // construct from |dont_AddRef(expr)|
        {
        }

      cci_Ptr( const CCI_NS_(already_AddRefed)<cciISupports>&& aSmartPtr )
            : cci_Ptr_base(aSmartPtr.mRawPtr)
          // construct from |dont_AddRef(expr)|
        {
        }

      cci_Ptr( const cciQueryInterface qi )
            : cci_Ptr_base(0)
          // assign from |do_QueryInterface(expr)|
        {
          assign_from_qi(qi, CCI_GET_IID(cciISupports));
        }

      cci_Ptr( const cciQueryInterfaceWithError& qi )
            : cci_Ptr_base(0)
          // assign from |do_QueryInterface(expr, &rv)|
        {
          assign_from_qi_with_error(qi, CCI_GET_IID(cciISupports));
        }

      cci_Ptr( const cciGetServiceByCID gs )
            : cci_Ptr_base(0)
          // assign from |do_GetService(cid_expr)|
        {
          assign_from_gs_cid(gs, CCI_GET_IID(cciISupports));
        }

      cci_Ptr( const cciGetServiceByCIDWithError& gs )
            : cci_Ptr_base(0)
          // assign from |do_GetService(cid_expr, &rv)|
        {
          assign_from_gs_cid_with_error(gs, CCI_GET_IID(cciISupports));
        }

      cci_Ptr( const cciGetServiceByContractID gs )
            : cci_Ptr_base(0)
          // assign from |do_GetService(contractid_expr)|
        {
          assign_from_gs_contractid(gs, CCI_GET_IID(cciISupports));
        }

      cci_Ptr( const cciGetServiceByContractIDWithError& gs )
            : cci_Ptr_base(0)
          // assign from |do_GetService(contractid_expr, &rv)|
        {
          assign_from_gs_contractid_with_error(gs, CCI_GET_IID(cciISupports));
        }

      cci_Ptr( const cci_Ptr_helper& helper )
            : cci_Ptr_base(0)
          // ...and finally, anything else we might need to construct from
          //  can exploit the |cci_Ptr_helper| facility
        {
          assign_from_helper(helper, CCI_GET_IID(cciISupports));
        }


        // Assignment operators

      cci_Ptr<cciISupports>& operator=( const cci_Ptr<cciISupports>& rhs )
          // copy assignment operator
        {
          assign_with_AddRef(rhs.mRawPtr);
          return *this;
        }

      cci_Ptr<cciISupports>& operator=( cciISupports* rhs )
          // assign from a raw pointer (of the right type)
        {
          assign_with_AddRef(rhs);
          return *this;
        }

      cci_Ptr<cciISupports>& operator=( const CCI_NS_(already_AddRefed)<cciISupports>& rhs )
          // assign from |dont_AddRef(expr)|
        {
          assign_assuming_AddRef(rhs.mRawPtr);
          return *this;
        }

      cci_Ptr<cciISupports>& operator=( const cciQueryInterface rhs )
          // assign from |do_QueryInterface(expr)|
        {
          assign_from_qi(rhs, CCI_GET_IID(cciISupports));
          return *this;
        }

      cci_Ptr<cciISupports>& operator=( const cciQueryInterfaceWithError& rhs )
          // assign from |do_QueryInterface(expr, &rv)|
        {
          assign_from_qi_with_error(rhs, CCI_GET_IID(cciISupports));
          return *this;
        }

      cci_Ptr<cciISupports>& operator=( const cciGetServiceByCID rhs )
          // assign from |do_GetService(cid_expr)|
        {
          assign_from_gs_cid(rhs, CCI_GET_IID(cciISupports));
          return *this;
        }

      cci_Ptr<cciISupports>& operator=( const cciGetServiceByCIDWithError& rhs )
          // assign from |do_GetService(cid_expr, &rv)|
        {
          assign_from_gs_cid_with_error(rhs, CCI_GET_IID(cciISupports));
          return *this;
        }

      cci_Ptr<cciISupports>& operator=( const cciGetServiceByContractID rhs )
          // assign from |do_GetService(contractid_expr)|
        {
          assign_from_gs_contractid(rhs, CCI_GET_IID(cciISupports));
          return *this;
        }

      cci_Ptr<cciISupports>& operator=( const cciGetServiceByContractIDWithError& rhs )
          // assign from |do_GetService(contractid_expr, &rv)|
        {
          assign_from_gs_contractid_with_error(rhs, CCI_GET_IID(cciISupports));
          return *this;
        }

      cci_Ptr<cciISupports>& operator=( const cci_Ptr_helper& rhs )
          // ...and finally, anything else we might need to assign from
          //  can exploit the |cci_Ptr_helper| facility.
        {
          assign_from_helper(rhs, CCI_GET_IID(cciISupports));
          return *this;
        }

      void swap( cci_Ptr<cciISupports>& rhs )
          // ...exchange ownership with |rhs|; can save a pair of refcount operations
        {
          cciISupports* temp = rhs.mRawPtr;
          rhs.mRawPtr = mRawPtr;
          mRawPtr = temp;
        }

      void swap( cciISupports*& rhs )
          // ...exchange ownership with |rhs|; can save a pair of refcount operations
        {
          cciISupports* temp = rhs;
          rhs = mRawPtr;
          mRawPtr = temp;
        }

      void forget( cciISupports** rhs )
          // Set the target of rhs to the value of mRawPtr and null out mRawPtr.
          // Useful to avoid unnecessary AddRef/Release pairs with "out"
          // parameters.
        {
          CCI_ASSERTION(rhs, "Null pointer passed to forget!");
          *rhs = 0;
          swap(*rhs);
        }

      // Other pointer operators

      // Prefer the implicit conversion provided automatically by
      // |operator nsISupports*() const|. Use |get()| to resolve ambiguity or to
      // get a castable pointer.
      cciISupports* get() const { return reinterpret_cast<cciISupports*>(mRawPtr); }

      // Makes an nsCOMPtr act like its underlying raw pointer type whenever it is
      // used in a context where a raw pointer is expected. It is this operator
      // that makes an nsCOMPtr substitutable for a raw pointer.
      //
      // Prefer the implicit use of this operator to calling |get()|, except where
      // necessary to resolve ambiguity/
      operator cciISupports* () const { return get(); }

      cciISupports* operator->() const
      {
        CCI_PRECONDITION(mRawPtr != 0, "You can't dereference a NULL cci_Ptr with operator->().");
        return get();
      }
  
      // These are not intended to be used by clients. See |address_of| below.
      cci_Ptr<cciISupports>* get_address() { return this; }
      const cci_Ptr<cciISupports>* get_address() const { return this; }


    public:

      cciISupports& operator*() const
      {
        CCI_PRECONDITION(mRawPtr != 0, "You can't dereference a NULL cci_Ptr with operator*().");
        return *get();
      }

      cciISupports** StartAssignment()
        {
#ifndef CCI_CAP_FEATURE_INLINE_STARTASSIGNMENT
          return reinterpret_cast<cciISupports**>(begin_assignment());
#else
          assign_assuming_AddRef(0);
          return reinterpret_cast<cciISupports**>(&mRawPtr);
#endif
        }

  };

// Prevent name collision with XPCOM
CCI_NAMESPACE_BEGIN

#ifdef DM_CANT_RESOLVE_CPP_CONST_AMBIGUITY

// This is the broken version for IRIX, which can't handle the version below.

template <class T>
inline cci_Ptr<T>* address_of( const cci_Ptr<T>& aPtr )
{
  return aPtr.get_address();
}

#else // DM_CANT_RESOLVE_CPP_CONST_AMBIGUITY

template <class T>
inline cci_Ptr<T>* address_of( cci_Ptr<T>& aPtr )
{
  return aPtr.get_address();
}

template <class T>
inline const cci_Ptr<T>* address_of( const cci_Ptr<T>& aPtr )
{
  return aPtr.get_address();
}

#endif // DM_CANT_RESOLVE_CPP_CONST_AMBIGUITY

CCI_NAMESPACE_END

template <class T>
class cciGetterAddRefs
    /*
      ...

      This class is designed to be used for anonymous temporary objects in the
      argument list of calls that return COM interface pointers, e.g.,

        cci_Ptr<IFoo> fooP;
        ...->QueryInterface(iid, getter_AddRefs(fooP))

      DO NOT USE THIS TYPE DIRECTLY IN YOUR CODE.  Use |getter_AddRefs()| instead.

      When initialized with a |cci_Ptr|, as in the example above, it returns
      a |void**|, a |T**|, or an |cciISupports**| as needed, that the outer call (|QueryInterface| in this
      case) can fill in.

      This type should be a nested class inside |cci_Ptr<T>|.
    */
  {
    public:
      explicit
      cciGetterAddRefs( cci_Ptr<T>& aSmartPtr )
          : mTargetSmartPtr(aSmartPtr)
        {
          // nothing else to do
        }

      operator void**()
        {
          return reinterpret_cast<void**>(mTargetSmartPtr.StartAssignment());
        }

      operator cciISupports**()
        {
          return reinterpret_cast<cciISupports**>(mTargetSmartPtr.StartAssignment());
        }

      operator T**()
        {
          return mTargetSmartPtr.StartAssignment();
        }

      T*& operator*()
        {
          return *(mTargetSmartPtr.StartAssignment());
        }

    private:
      cci_Ptr<T>& mTargetSmartPtr;
  };


template<>
class cciGetterAddRefs<cciISupports>
  {
    public:
      explicit
      cciGetterAddRefs( cci_Ptr<cciISupports>& aSmartPtr )
          : mTargetSmartPtr(aSmartPtr)
        {
          // nothing else to do
        }

      operator void**()
        {
          return reinterpret_cast<void**>(mTargetSmartPtr.StartAssignment());
        }

      operator cciISupports**()
        {
          return mTargetSmartPtr.StartAssignment();
        }

      cciISupports*&
      operator*()
        {
          return *(mTargetSmartPtr.StartAssignment());
        }

    private:
      cci_Ptr<cciISupports>& mTargetSmartPtr;
  };

// Prevent name collision with XPCOM
CCI_NAMESPACE_BEGIN

template <class T>
inline cciGetterAddRefs<T>
getter_AddRefs( cci_Ptr<T>& aSmartPtr )
    /*
      Used around a |cci_Ptr| when
      ...makes the class |cciGetterAddRefs<T>| invisible.
    */
  {
    return cciGetterAddRefs<T>(aSmartPtr);
  }

CCI_NAMESPACE_END

  // Comparing two |cci_Ptr|s

template <class T, class U>
inline bool operator==( const cci_Ptr<T>& lhs, const cci_Ptr<U>& rhs )
{
  return static_cast<const T*>(lhs.get()) == static_cast<const U*>(rhs.get());
}


template <class T, class U>
inline bool operator!=( const cci_Ptr<T>& lhs, const cci_Ptr<U>& rhs )
{
  return static_cast<const T*>(lhs.get()) != static_cast<const U*>(rhs.get());
}


  // Comparing an |cci_Ptr| to a raw pointer

template <class T, class U>
inline bool operator==( const cci_Ptr<T>& lhs, const U* rhs )
{
  return static_cast<const T*>(lhs.get()) == rhs;
}

template <class T, class U>
inline bool operator==( const U* lhs, const cci_Ptr<T>& rhs )
{
   return lhs == static_cast<const T*>(rhs.get());
}

template <class T, class U>
inline bool operator!=( const cci_Ptr<T>& lhs, const U* rhs )
{
  return static_cast<const T*>(lhs.get()) != rhs;
}

template <class T, class U>
inline bool operator!=( const U* lhs, const cci_Ptr<T>& rhs )
{
  return lhs != static_cast<const T*>(rhs.get());
}

  // To avoid ambiguities caused by the presence of builtin |operator==|s
  // creating a situation where one of the |operator==| defined above
  // has a better conversion for one argument and the builtin has a
  // better conversion for the other argument, define additional
  // |operator==| without the |const| on the raw pointer.
  // See bug 65664 for details.

#ifndef DM_DONT_PROVIDE_NONCONST_OPEQ
template <class T, class U>
inline bool operator==( const cci_Ptr<T>& lhs, U* rhs )
{
  return static_cast<const T*>(lhs.get()) == const_cast<const U*>(rhs);
}

template <class T, class U>
inline bool operator==( U* lhs, const cci_Ptr<T>& rhs )
{
  return const_cast<const U*>(lhs) == static_cast<const T*>(rhs.get());
}

template <class T, class U>
inline bool operator!=( const cci_Ptr<T>& lhs, U* rhs )
{
  return static_cast<const T*>(lhs.get()) != const_cast<const U*>(rhs);
}

template <class T, class U>
inline bool operator!=( U* lhs, const cci_Ptr<T>& rhs )
{
  return const_cast<const U*>(lhs) != static_cast<const T*>(rhs.get());
}
#endif



  // Comparing an |cci_Ptr| to |0|

class CCICAP_Zero;

template <class T>
inline bool operator==( const cci_Ptr<T>& lhs, CCICAP_Zero* rhs )
// specifically to allow |smartPtr == 0|
{
  return static_cast<const void*>(lhs.get()) == reinterpret_cast<const void*>(rhs);
}

template <class T>
inline bool operator==( CCICAP_Zero* lhs, const cci_Ptr<T>& rhs )
// specifically to allow |0 == smartPtr|
{
  return reinterpret_cast<const void*>(lhs) == static_cast<const void*>(rhs.get());
}

template <class T>
inline bool operator!=( const cci_Ptr<T>& lhs, CCICAP_Zero* rhs )
// specifically to allow |smartPtr != 0|
{
  return static_cast<const void*>(lhs.get()) != reinterpret_cast<const void*>(rhs);
}

template <class T>
inline bool operator!=( CCICAP_Zero* lhs, const cci_Ptr<T>& rhs )
// specifically to allow |0 != smartPtr|
{
  return reinterpret_cast<const void*>(lhs) != static_cast<const void*>(rhs.get());
}


#ifdef DM_HAVE_CPP_TROUBLE_COMPARING_TO_ZERO

  // We need to explicitly define comparison operators for `int'
  // because the compiler is lame.

template <class T>
inline bool operator==( const cci_Ptr<T>& lhs, int rhs )
// specifically to allow |smartPtr == 0|
{
  return static_cast<const void*>(lhs.get()) == reinterpret_cast<const void*>(rhs);
}

template <class T>
inline bool operator==( int lhs, const cci_Ptr<T>& rhs )
// specifically to allow |0 == smartPtr|
{
  return reinterpret_cast<const void*>(lhs) == static_cast<const void*>(rhs.get());
}

#endif // !defined(DM_HAVE_CPP_TROUBLE_COMPARING_TO_ZERO)

  // Comparing any two [XP]COM objects for identity

// Prevent name collision with XPCOM
CCI_NAMESPACE_BEGIN

inline bool SameCOMIdentity( cciISupports* lhs, cciISupports* rhs )
{
  return cci_Ptr<cciISupports>( do_QueryInterface(lhs) ) == cci_Ptr<cciISupports>( do_QueryInterface(rhs) );
}


template <class SourceType, class DestinationType>
inline cci_result CallQueryInterface( cci_Ptr<SourceType>& aSourcePtr, DestinationType** aDestPtr )
{
  return CallQueryInterface(aSourcePtr.get(), aDestPtr);
}

CCI_NAMESPACE_END

#endif // cciCOMPtr_h
