#ifndef cciSupportsImpl_h
#define cciSupportsImpl_h

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
 * The Initial Developer of the Original Code is Netscape Communications Corp.
 * Portions created by the Initial Developer are Copyright (C) 2001
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 * David Marteau (CCI port)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
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

#ifndef cciSupportsUtils_h
#include "cciSupportsUtils.h"
#endif

///////////////////////////////////////////////////////////////////////////////

class dmAutoRefCnt {

 public:
    dmAutoRefCnt() : mValue(0) {}
    dmAutoRefCnt(cci_refcnt aValue) : mValue(aValue) {}

    // only support prefix increment/decrement
    cci_refcnt operator++() { return ++mValue; }
    cci_refcnt operator--() { return --mValue; }

    cci_refcnt operator=(cci_refcnt aValue) { return (mValue = aValue); }
    operator cci_refcnt() const { return mValue; }
    cci_refcnt get()        const { return mValue; }
 private:
    // do not define these to enforce the faster prefix notation
    cci_refcnt operator++(int);
    cci_refcnt operator--(int);
    cci_refcnt mValue;
};

/**
 * Declare the reference count variable and the implementations of the
 * AddRef and QueryInterface methods.
 */

#define CCI_DECL_ISUPPORTS                                                   \
public:                                                                       \
  CCI_IMETHOD QueryInterface(DM_REFIID aIID,                                  \
                            void** aInstancePtr) override;                    \
  CCI_IMETHOD_(cci_refcnt) AddRef(void) override;                             \
  CCI_IMETHOD_(cci_refcnt) Release(void) override;                            \
                                                                              \
  static void  CCI_STDCALL DestructorCallback( void* obj );                   \
protected:                                                                    \
  dmAutoRefCnt mRefCnt;                                                       \
public:                                                                       \
  void* operator new   ( size_t n );                                          \
  void  operator delete( void* p  );                                          \

///////////////////////////////////////////////////////////////////////////////

// The Revoke callback enable the object collector to force deletion
// of an object which could have been locked by reference cycle
// The object is then marked as "revoked".
// Such a "revoked" object will then return error when calling CCI_VerifyObject

#define CCI_IMPL_ALLOCATOR(_class)                                        \
CCI_IMETHODIMP_(void) _class::DestructorCallback(void* obj)               \
{                                                                         \
  _class* ptr = reinterpret_cast<_class*>(obj);                           \
  ptr->~_class();                                                         \
}                                                                         \
void* _class::operator new  ( size_t n  )                                 \
{                                                                         \
  return CCI_NewObject(n,&_class::DestructorCallback);                    \
}                                                                         \
void _class::operator delete ( void* p  )                                 \
{                                                                         \
  CCI_RevokeObject(p,true);                                            \
}                                                                         \


/**
 * Use this macro to implement the AddRef method for a given <i>_class</i>
 * @param _class The name of the class implementing the method
 */
#define CCI_IMPL_ADDREF(_class)                                              \
CCI_IMETHODIMP_(cci_refcnt) _class::AddRef(void)                              \
{                                                                             \
  CCI_PRECONDITION(dm_int32(mRefCnt) >= 0, "illegal refcnt");                 \
  ++mRefCnt;                                                                  \
  return mRefCnt;                                                             \
}

/**
 * Use this macro to implement the AddRef method for a given <i>_class</i>
 * implemented as a wholly owned aggregated object intended to implement
 * interface(s) for its owner
 * @param _class The name of the class implementing the method
 * @param _aggregator the owning/containing object
 */
#define CCI_IMPL_ADDREF_USING_AGGREGATOR(_class, _aggregator)                \
CCI_IMETHODIMP_(cci_refcnt) _class::AddRef(void)                              \
{                                                                             \
  CCI_PRECONDITION(_aggregator, "null aggregator");                           \
  return (_aggregator)->AddRef();                                             \
}

/**
 * Use this macro to implement the Release method for a given
 * <i>_class</i>.
 * @param _class The name of the class implementing the method
 * @param _destroy A statement that is executed when the object's
 *   refcount drops to zero.
 *
 * For example,
 *
 *   CCI_IMPL_RELEASE_WITH_DESTROY(Foo, Destroy(this))
 *
 * will cause
 *
 *   Destroy(this);
 *
 * to be invoked when the object's refcount drops to zero. This
 * allows for arbitrary teardown activity to occur (e.g., deallocation
 * of object allocated with placement new).
 */
#define CCI_IMPL_RELEASE_WITH_DESTROY(_class, _destroy)                       \
CCI_IMETHODIMP_(cci_refcnt) _class::Release(void)                             \
{                                                                             \
  if(CCI_FAILED(CCI_VerifyObject(this))) {                                    \
    CCI_WARNING("Releasing an invalid object");                               \
    return 0;                                                                 \
  }                                                                           \
  CCI_PRECONDITION(0 != mRefCnt, "dup release");                              \
  --mRefCnt;                                                                  \
  if (mRefCnt == 0) {                                                         \
    mRefCnt = 1; /* stabilize */                                              \
    _destroy;                                                                 \
    return 0;                                                                 \
  }                                                                           \
  return mRefCnt;                                                             \
}

/**
 * Use this macro to implement the Release method for a given <i>_class</i>
 * @param _class The name of the class implementing the method
 *
 * A note on the 'stabilization' of the refcnt to one. At that point,
 * the object's refcount will have gone to zero. The object's
 * destructor may trigger code that attempts to QueryInterface() and
 * Release() 'this' again. Doing so will temporarily increment and
 * decrement the refcount. (Only a logic error would make one try to
 * keep a permanent hold on 'this'.)  To prevent re-entering the
 * destructor, we make sure that no balanced refcounting can return
 * the refcount to |0|.
 */
#define CCI_IMPL_RELEASE(_class) \
  CCI_IMPL_RELEASE_WITH_DESTROY(_class, CCI_DELETEOBJECT(this))

/**
 * Use this macro to implement the Release method for a given <i>_class</i>
 * implemented as a wholly owned aggregated object intended to implement
 * interface(s) for its owner
 * @param _class The name of the class implementing the method
 * @param _aggregator the owning/containing object
 */
#define CCI_IMPL_RELEASE_USING_AGGREGATOR(_class, _aggregator)                \
CCI_IMETHODIMP_(cci_refcnt) _class::Release(void)                             \
{                                                                             \
  CCI_PRECONDITION(_aggregator, "null aggregator");                           \
  return (_aggregator)->Release();                                            \
}


///////////////////////////////////////////////////////////////////////////////

/**
 * There are two ways of implementing QueryInterface, and we use both:
 *
 * Table-driven QueryInterface uses a static table of IID->offset mappings
 * and a shared helper function. Using it tends to reduce codesize and improve
 * runtime performance (due to processor cache hits).
 *
 * Macro-driven QueryInterface generates a QueryInterface function directly
 * using common macros. This is necessary if special QueryInterface features
 * are being used (such as tearoffs and conditional interfaces).
 *
 * These methods can be combined into a table-driven function call followed
 * by custom code for tearoffs and conditionals.
 */

struct cciQITableEntry
{
  const dmIID *iid;     // null indicates end of the QITableEntry array
  dm_offset32  offset;
};

cci_result CCI_FASTCALL
CCI_TableDrivenQI(void* aThis, const cciQITableEntry* entries,
                  DM_REFIID aIID, void **aInstancePtr);

/**
 * Implement table-driven queryinterface
 */

#define CCI_INTERFACE_TABLE_HEAD(_class)                                     \
CCI_IMETHODIMP _class::QueryInterface(DM_REFIID aIID, void** aInstancePtr)    \
{                                                                             \
  CCI_ASSERTION(aInstancePtr,                                                 \
               "QueryInterface requires a non-NULL destination!");            \
  cci_result rv = CCI_ERROR_FAILURE;

#define CCI_INTERFACE_TABLE_BEGIN                                            \
  static const cciQITableEntry table[] = {                                    \

#define CCI_INTERFACE_TABLE_ENTRY(_class, _interface)                        \
  { &_interface::COMTypeInfo<int>::kIID,                                      \
	dm_offset32(reinterpret_cast<char*>(                                        \
                        static_cast<_interface*>((_class*) 0x1000)) -         \
               reinterpret_cast<char*>((_class*) 0x1000))                     \
  },

#define CCI_INTERFACE_TABLE_ENTRY_AMBIGUOUS(_class, _interface, _implClass)  \
  { &_interface::COMTypeInfo<int>::kIID,                                      \
	  dm_offset32(reinterpret_cast<char*>(                                      \
                        static_cast<_interface*>(                             \
                                       static_cast<_implClass*>(              \
                                                      (_class*) 0x1000))) -   \
               reinterpret_cast<char*>((_class*) 0x1000))                     \
  },

#define CCI_INTERFACE_TABLE_END_WITH_PTR(_ptr)                               \
  { dm_null, 0 } };                                                           \
  rv = CCI_TableDrivenQI(static_cast<void*>(_ptr),                            \
                        table, aIID, aInstancePtr);

#define CCI_INTERFACE_TABLE_END                                              \
  CCI_INTERFACE_TABLE_END_WITH_PTR(this)

#define CCI_INTERFACE_TABLE_TAIL                                             \
  return rv;                                                                  \
}

#define CCI_INTERFACE_TABLE_TAIL_INHERITING(_baseclass)                      \
  if (CCI_SUCCEEDED(rv))                                                      \
    return rv;                                                                \
  return _baseclass::QueryInterface(aIID, aInstancePtr);                      \
}

#define CCI_INTERFACE_TABLE_TAIL_USING_AGGREGATOR(_aggregator)               \
  if (CCI_SUCCEEDED(rv))                                                      \
    return rv;                                                                \
  CCI_ASSERTION(_aggregator, "null aggregator");                              \
  return _aggregator->QueryInterface(aIID, aInstancePtr)                      \
}

/**
 * This implements query interface with two assumptions: First, the
 * class in question implements cciISupports and its own interface and
 * nothing else. Second, the implementation of the class's primary
 * inheritance chain leads to its own interface.
 *
 * @param _class The name of the class implementing the method
 * @param _classiiddef The name of the #define symbol that defines the IID
 * for the class (e.g. CCI_ISUPPORTS_IID)
 */

#define CCI_IMPL_QUERY_HEAD(_class)                                          \
CCI_IMETHODIMP _class::QueryInterface(DM_REFIID aIID, void** aInstancePtr)    \
{                                                                             \
  CCI_ASSERTION(aInstancePtr,                                                 \
               "QueryInterface requires a non-NULL destination!");            \
  cciISupports* foundInterface;

#define CCI_IMPL_QUERY_BODY(_interface)                                      \
  if ( aIID.Equals(CCI_GET_IID(_interface)) )                                 \
    foundInterface = static_cast<_interface*>(this);                          \
  else

#define CCI_IMPL_QUERY_BODY_CONDITIONAL(_interface, condition)               \
  if ( (condition) && aIID.Equals(CCI_GET_IID(_interface)))                   \
    foundInterface = static_cast<_interface*>(this);                          \
  else

#define CCI_IMPL_QUERY_BODY_AMBIGUOUS(_interface, _implClass)                \
  if ( aIID.Equals(CCI_GET_IID(_interface)) )                                 \
    foundInterface = static_cast<_interface*>(                                \
                                    static_cast<_implClass*>(this));          \
  else

#define CCI_IMPL_QUERY_BODY_AGGREGATED(_interface, _aggregate)               \
  if ( aIID.Equals(CCI_GET_IID(_interface)) )                                 \
    foundInterface = static_cast<_interface*>(_aggregate);                    \
  else

#define CCI_IMPL_QUERY_TAIL_GUTS                                             \
  foundInterface = 0;                                                         \
  cci_result status;                                                          \
  if ( !foundInterface )                                                      \
    status = CCI_ERROR_NOINTERFACE;                                           \
  else                                                                        \
    {                                                                         \
      CCI_ADDREF(foundInterface);                                             \
      status = CCI_OK;                                                        \
    }                                                                         \
  *aInstancePtr = foundInterface;                                             \
  return status;                                                              \
}

#define CCI_IMPL_QUERY_TAIL_INHERITING(_baseclass)                           \
    foundInterface = 0;                                                       \
  cci_result status;                                                          \
  if ( !foundInterface )                                                      \
    status = _baseclass::QueryInterface(aIID, (void**)&foundInterface);       \
  else                                                                        \
    {                                                                         \
      CCI_ADDREF(foundInterface);                                             \
      status = CCI_OK;                                                        \
    }                                                                         \
  *aInstancePtr = foundInterface;                                             \
  return status;                                                              \
}

#define CCI_IMPL_QUERY_TAIL_USING_AGGREGATOR(_aggregator)                    \
    foundInterface = 0;                                                       \
  cci_result status;                                                          \
  if ( !foundInterface ) {                                                    \
    CCI_ASSERTION(_aggregator, "null aggregator");                            \
    status = _aggregator->QueryInterface(aIID, (void**)&foundInterface);      \
  } else                                                                      \
    {                                                                         \
      CCI_ADDREF(foundInterface);                                             \
      status = CCI_OK;                                                        \
    }                                                                         \
  *aInstancePtr = foundInterface;                                             \
  return status;                                                              \
}

#define CCI_IMPL_QUERY_TAIL(_supports_interface)                             \
  CCI_IMPL_QUERY_BODY_AMBIGUOUS(cciISupports, _supports_interface)            \
  CCI_IMPL_QUERY_TAIL_GUTS


  /*
    This is the new scheme.  Using this notation now will allow us to switch to
    a table driven mechanism when it's ready.  Note the difference between this
    and the (currently) underlying CCI_IMPL_QUERY_INTERFACE mechanism.  You must
    explicitly mention |cciISupports| when using the interface maps.
  */
#define CCI_INTERFACE_MAP_BEGIN(_implClass)      CCI_IMPL_QUERY_HEAD(_implClass)
#define CCI_INTERFACE_MAP_ENTRY(_interface)      CCI_IMPL_QUERY_BODY(_interface)
#define CCI_INTERFACE_MAP_ENTRY_CONDITIONAL(_interface, condition)             \
  CCI_IMPL_QUERY_BODY_CONDITIONAL(_interface, condition)
#define CCI_INTERFACE_MAP_ENTRY_AGGREGATED(_interface,_aggregate)              \
  CCI_IMPL_QUERY_BODY_AGGREGATED(_interface,_aggregate)

#define CCI_INTERFACE_MAP_END                    CCI_IMPL_QUERY_TAIL_GUTS
#define CCI_INTERFACE_MAP_ENTRY_AMBIGUOUS(_interface, _implClass)              \
  CCI_IMPL_QUERY_BODY_AMBIGUOUS(_interface, _implClass)
#define CCI_INTERFACE_MAP_END_INHERITING(_baseClass)                           \
  CCI_IMPL_QUERY_TAIL_INHERITING(_baseClass)
#define CCI_INTERFACE_MAP_END_AGGREGATED(_aggregator)                          \
  CCI_IMPL_QUERY_TAIL_USING_AGGREGATOR(_aggregator)

#define CCI_INTERFACE_TABLE0(_class)                                          \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(_class, cciISupports)                            \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE1(_class, _i1)                                     \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i1)                                     \
    CCI_INTERFACE_TABLE_ENTRY_AMBIGUOUS(_class, cciISupports, _i1)             \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE2(_class, _i1, _i2)                                \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i1)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i2)                                     \
    CCI_INTERFACE_TABLE_ENTRY_AMBIGUOUS(_class, cciISupports, _i1)             \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE3(_class, _i1, _i2, _i3)                           \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i1)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i2)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i3)                                     \
    CCI_INTERFACE_TABLE_ENTRY_AMBIGUOUS(_class, cciISupports, _i1)             \
  CCI_INTERFACE_TABLE_END


#define CCI_INTERFACE_TABLE4(_class, _i1, _i2, _i3, _i4)                      \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i1)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i2)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i3)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i4)                                     \
    CCI_INTERFACE_TABLE_ENTRY_AMBIGUOUS(_class, cciISupports, _i1)             \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE5(_class, _i1, _i2, _i3, _i4, _i5)                 \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i1)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i2)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i3)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i4)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i5)                                     \
    CCI_INTERFACE_TABLE_ENTRY_AMBIGUOUS(_class, cciISupports, _i1)             \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE6(_class, _i1, _i2, _i3, _i4, _i5, _i6)            \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i1)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i2)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i3)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i4)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i5)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i6)                                     \
    CCI_INTERFACE_TABLE_ENTRY_AMBIGUOUS(_class, cciISupports, _i1)             \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE7(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7)       \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i1)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i2)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i3)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i4)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i5)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i6)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i7)                                     \
    CCI_INTERFACE_TABLE_ENTRY_AMBIGUOUS(_class, cciISupports, _i1)             \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE8(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7, _i8)  \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i1)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i2)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i3)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i4)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i5)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i6)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i7)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i8)                                     \
    CCI_INTERFACE_TABLE_ENTRY_AMBIGUOUS(_class, cciISupports, _i1)             \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE9(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7,       \
                            _i8, _i9)                                          \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i1)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i2)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i3)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i4)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i5)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i6)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i7)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i8)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i9)                                     \
    CCI_INTERFACE_TABLE_ENTRY_AMBIGUOUS(_class, cciISupports, _i1)             \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE10(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7,      \
                             _i8, _i9, _i10)                                   \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i1)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i2)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i3)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i4)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i5)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i6)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i7)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i8)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i9)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i10)                                    \
    CCI_INTERFACE_TABLE_ENTRY_AMBIGUOUS(_class, cciISupports, _i1)             \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE11(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7,      \
                             _i8, _i9, _i10, _i11)                             \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i1)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i2)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i3)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i4)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i5)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i6)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i7)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i8)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i9)                                     \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i10)                                    \
    CCI_INTERFACE_TABLE_ENTRY(_class, _i11)                                    \
    CCI_INTERFACE_TABLE_ENTRY_AMBIGUOUS(_class, cciISupports, _i1)             \
  CCI_INTERFACE_TABLE_END

#define CCI_IMPL_QUERY_INTERFACE0(_class)                                     \
  CCI_INTERFACE_TABLE_HEAD(_class)                                             \
  CCI_INTERFACE_TABLE0(_class)                                                 \
  CCI_INTERFACE_TABLE_TAIL

#define CCI_IMPL_QUERY_INTERFACE1(_class, _i1)                                \
  CCI_INTERFACE_TABLE_HEAD(_class)                                             \
  CCI_INTERFACE_TABLE1(_class, _i1)                                            \
  CCI_INTERFACE_TABLE_TAIL

#define CCI_IMPL_QUERY_INTERFACE2(_class, _i1, _i2)                           \
  CCI_INTERFACE_TABLE_HEAD(_class)                                             \
  CCI_INTERFACE_TABLE2(_class, _i1, _i2)                                       \
  CCI_INTERFACE_TABLE_TAIL

#define CCI_IMPL_QUERY_INTERFACE3(_class, _i1, _i2, _i3)                      \
  CCI_INTERFACE_TABLE_HEAD(_class)                                             \
  CCI_INTERFACE_TABLE3(_class, _i1, _i2, _i3)                                  \
  CCI_INTERFACE_TABLE_TAIL

#define CCI_IMPL_QUERY_INTERFACE4(_class, _i1, _i2, _i3, _i4)                 \
  CCI_INTERFACE_TABLE_HEAD(_class)                                             \
  CCI_INTERFACE_TABLE4(_class, _i1, _i2, _i3, _i4)                             \
  CCI_INTERFACE_TABLE_TAIL

#define CCI_IMPL_QUERY_INTERFACE5(_class, _i1, _i2, _i3, _i4, _i5)            \
  CCI_INTERFACE_TABLE_HEAD(_class)                                             \
  CCI_INTERFACE_TABLE5(_class, _i1, _i2, _i3, _i4, _i5)                        \
  CCI_INTERFACE_TABLE_TAIL

#define CCI_IMPL_QUERY_INTERFACE6(_class, _i1, _i2, _i3, _i4, _i5, _i6)       \
  CCI_INTERFACE_TABLE_HEAD(_class)                                             \
  CCI_INTERFACE_TABLE6(_class, _i1, _i2, _i3, _i4, _i5, _i6)                   \
  CCI_INTERFACE_TABLE_TAIL

#define CCI_IMPL_QUERY_INTERFACE7(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7)  \
  CCI_INTERFACE_TABLE_HEAD(_class)                                             \
  CCI_INTERFACE_TABLE7(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7)              \
  CCI_INTERFACE_TABLE_TAIL

#define CCI_IMPL_QUERY_INTERFACE8(_class, _i1, _i2, _i3, _i4, _i5, _i6,       \
                                 _i7, _i8)                                     \
  CCI_INTERFACE_TABLE_HEAD(_class)                                             \
  CCI_INTERFACE_TABLE8(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7, _i8)         \
  CCI_INTERFACE_TABLE_TAIL

#define CCI_IMPL_QUERY_INTERFACE9(_class, _i1, _i2, _i3, _i4, _i5, _i6,       \
                                 _i7, _i8, _i9)                                \
  CCI_INTERFACE_TABLE_HEAD(_class)                                             \
  CCI_INTERFACE_TABLE9(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7, _i8, _i9)    \
  CCI_INTERFACE_TABLE_TAIL

#define CCI_IMPL_QUERY_INTERFACE10(_class, _i1, _i2, _i3, _i4, _i5, _i6,     \
                                  _i7, _i8, _i9, _i10)                        \
  CCI_INTERFACE_TABLE_HEAD(_class)                                            \
  CCI_INTERFACE_TABLE10(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7, _i8,       \
                       _i9, _i10)                                             \
  CCI_INTERFACE_TABLE_TAIL

#define CCI_IMPL_QUERY_INTERFACE11(_class, _i1, _i2, _i3, _i4, _i5, _i6,     \
                                  _i7, _i8, _i9, _i10, _i11)                  \
  CCI_INTERFACE_TABLE_HEAD(_class)                                            \
  CCI_INTERFACE_TABLE11(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7, _i8,       \
                       _i9, _i10, _i11)                                       \
  CCI_INTERFACE_TABLE_TAIL


/**
 * Declare that you're going to inherit from something that already
 * implements cciISupports, but also implements an additional interface, thus
 * causing an ambiguity. In this case you don't need another mRefCnt, you
 * just need to forward the definitions to the appropriate superclass. E.g.
 *
 * class Bar : public Foo, public nsIBar {  // both provide cciISupports
 * public:
 *   CCI_DECL_ISUPPORTS_INHERITED
 *   ...other nsIBar and Bar methods...
 * };
 */
#define CCI_DECL_ISUPPORTS_INHERITED                                         \
public:                                                                       \
  CCI_IMETHOD QueryInterface(DM_REFIID aIID,                                  \
                            void** aInstancePtr) override;                    \
  CCI_IMETHOD_(cci_refcnt) AddRef(void) override;                             \
  CCI_IMETHOD_(cci_refcnt) Release(void) override;                            \

/**
 * These macros can be used in conjunction with CCI_DECL_ISUPPORTS_INHERITED
 * to implement the cciISupports methods, forwarding the invocations to a
 * superclass that already implements cciISupports.
 *
 * Note that I didn't make these inlined because they're virtual methods.
 */

#define CCI_IMPL_ADDREF_INHERITED(Class, Super)                              \
CCI_IMETHODIMP_(cci_refcnt) Class::AddRef(void)                               \
{                                                                             \
  cci_refcnt r = Super::AddRef();                                             \
  return r;                                                                   \
}                                                                             \

#define CCI_IMPL_RELEASE_INHERITED(Class, Super)                             \
CCI_IMETHODIMP_(cci_refcnt) Class::Release(void)                              \
{                                                                             \
  cci_refcnt r = Super::Release();                                            \
  return r;                                                                   \
}                                                                             \

#define CCI_INTERFACE_TABLE_INHERITED0(Class) /* Nothing to do here */

#define CCI_INTERFACE_TABLE_INHERITED1(Class, i1)                             \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(Class, i1)                                       \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE_INHERITED2(Class, i1, i2)                         \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(Class, i1)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i2)                                       \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE_INHERITED3(Class, i1, i2, i3)                     \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(Class, i1)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i2)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i3)                                       \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE_INHERITED4(Class, i1, i2, i3, i4)                 \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(Class, i1)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i2)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i3)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i4)                                       \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE_INHERITED5(Class, i1, i2, i3, i4, i5)             \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(Class, i1)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i2)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i3)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i4)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i5)                                       \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE_INHERITED6(Class, i1, i2, i3, i4, i5, i6)         \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(Class, i1)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i2)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i3)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i4)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i5)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i6)                                       \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE_INHERITED7(Class, i1, i2, i3, i4, i5, i6, i7)     \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(Class, i1)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i2)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i3)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i4)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i5)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i6)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i7)                                       \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE_INHERITED8(Class, i1, i2, i3, i4, i5, i6, i7, i8) \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(Class, i1)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i2)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i3)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i4)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i5)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i6)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i7)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i8)                                       \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE_INHERITED9(Class, i1, i2, i3, i4, i5, i6, i7,     \
                                      i8, i9)                                  \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(Class, i1)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i2)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i3)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i4)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i5)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i6)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i7)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i8)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i9)                                       \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE_INHERITED10(Class, i1, i2, i3, i4, i5, i6, i7,    \
                                      i8, i9, i10)                             \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(Class, i1)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i2)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i3)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i4)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i5)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i6)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i7)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i8)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i9)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i10)                                      \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE_INHERITED11(Class, i1, i2, i3, i4, i5, i6, i7,    \
                                      i8, i9, i10, i11)                        \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(Class, i1)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i2)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i3)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i4)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i5)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i6)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i7)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i8)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i9)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i10)                                      \
    CCI_INTERFACE_TABLE_ENTRY(Class, i11)                                      \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE_INHERITED12(Class, i1, i2, i3, i4, i5, i6, i7,    \
                                      i8, i9, i10, i11, i12)                   \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(Class, i1)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i2)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i3)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i4)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i5)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i6)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i7)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i8)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i9)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i10)                                      \
    CCI_INTERFACE_TABLE_ENTRY(Class, i11)                                      \
    CCI_INTERFACE_TABLE_ENTRY(Class, i12)                                      \
  CCI_INTERFACE_TABLE_END

#define CCI_INTERFACE_TABLE_INHERITED10(Class, i1, i2, i3, i4, i5, i6, i7,    \
                                       i8, i9, i10)                            \
  CCI_INTERFACE_TABLE_BEGIN                                                    \
    CCI_INTERFACE_TABLE_ENTRY(Class, i1)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i2)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i3)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i4)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i5)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i6)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i7)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i8)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i9)                                       \
    CCI_INTERFACE_TABLE_ENTRY(Class, i10)                                      \
  CCI_INTERFACE_TABLE_END

#define CCI_IMPL_QUERY_INTERFACE_INHERITED0(Class, Super)                     \
  CCI_INTERFACE_TABLE_HEAD(Class)                                              \
  CCI_INTERFACE_TABLE_INHERITED0(Class)                                        \
  CCI_INTERFACE_TABLE_TAIL_INHERITING(Super)

#define CCI_IMPL_QUERY_INTERFACE_INHERITED1(Class, Super, i1)                 \
  CCI_INTERFACE_TABLE_HEAD(Class)                                              \
  CCI_INTERFACE_TABLE_INHERITED1(Class, i1)                                    \
  CCI_INTERFACE_TABLE_TAIL_INHERITING(Super)

#define CCI_IMPL_QUERY_INTERFACE_INHERITED2(Class, Super, i1, i2)             \
  CCI_INTERFACE_TABLE_HEAD(Class)                                              \
  CCI_INTERFACE_TABLE_INHERITED2(Class, i1, i2)                                \
  CCI_INTERFACE_TABLE_TAIL_INHERITING(Super)

#define CCI_IMPL_QUERY_INTERFACE_INHERITED3(Class, Super, i1, i2, i3)         \
  CCI_INTERFACE_TABLE_HEAD(Class)                                              \
  CCI_INTERFACE_TABLE_INHERITED3(Class, i1, i2, i3)                            \
  CCI_INTERFACE_TABLE_TAIL_INHERITING(Super)

#define CCI_IMPL_QUERY_INTERFACE_INHERITED4(Class, Super, i1, i2, i3, i4)     \
  CCI_INTERFACE_TABLE_HEAD(Class)                                              \
  CCI_INTERFACE_TABLE_INHERITED4(Class, i1, i2, i3, i4)                        \
  CCI_INTERFACE_TABLE_TAIL_INHERITING(Super)

#define CCI_IMPL_QUERY_INTERFACE_INHERITED5(Class,Super,i1,i2,i3,i4,i5)       \
  CCI_INTERFACE_TABLE_HEAD(Class)                                              \
  CCI_INTERFACE_TABLE_INHERITED5(Class, i1, i2, i3, i4, i5)                    \
  CCI_INTERFACE_TABLE_TAIL_INHERITING(Super)

#define CCI_IMPL_QUERY_INTERFACE_INHERITED6(Class,Super,i1,i2,i3,i4,i5,i6)    \
  CCI_INTERFACE_TABLE_HEAD(Class)                                              \
  CCI_INTERFACE_TABLE_INHERITED6(Class, i1, i2, i3, i4, i5, i6)                \
  CCI_INTERFACE_TABLE_TAIL_INHERITING(Super)

#define CCI_IMPL_QUERY_INTERFACE_INHERITED7(Class,Super,i1,i2,i3,i4,i5,i6,i7) \
  CCI_INTERFACE_TABLE_HEAD(Class)                                              \
  CCI_INTERFACE_TABLE_INHERITED7(Class, i1, i2, i3, i4, i5, i6, i7)            \
  CCI_INTERFACE_TABLE_TAIL_INHERITING(Super)

#define CCI_IMPL_QUERY_INTERFACE_INHERITED8(Class,Super,i1,i2,i3,i4,i5,i6,    \
                                           i7,i8)                              \
  CCI_INTERFACE_TABLE_HEAD(Class)                                              \
  CCI_INTERFACE_TABLE_INHERITED8(Class, i1, i2, i3, i4, i5, i6, i7, i8)        \
  CCI_INTERFACE_TABLE_TAIL_INHERITING(Super)

#define CCI_IMPL_QUERY_INTERFACE_INHERITED9(Class,Super,i1,i2,i3,i4,i5,i6,    \
                                           i7,i8,i9)                           \
  CCI_INTERFACE_TABLE_HEAD(Class)                                              \
  CCI_INTERFACE_TABLE_INHERITED9(Class, i1, i2, i3, i4, i5, i6, i7, i8, i9)    \
  CCI_INTERFACE_TABLE_TAIL_INHERITING(Super)

/**
 * Convenience macros for implementing all cciISupports methods for
 * a simple class.
 * @param _class The name of the class implementing the method
 * @param _classiiddef The name of the #define symbol that defines the IID
 * for the class (e.g. CCI_ISUPPORTS_IID)
 */

#define CCI_IMPL_ISUPPORTS0(_class)                                           \
  CCI_IMPL_ADDREF(_class)                                                      \
  CCI_IMPL_RELEASE(_class)                                                     \
  CCI_IMPL_ALLOCATOR(_class)                                                   \
  CCI_IMPL_QUERY_INTERFACE0(_class)

#define CCI_IMPL_ISUPPORTS1(_class, _interface)                               \
  CCI_IMPL_ADDREF(_class)                                                      \
  CCI_IMPL_RELEASE(_class)                                                     \
  CCI_IMPL_ALLOCATOR(_class)                                                   \
  CCI_IMPL_QUERY_INTERFACE1(_class, _interface)

#define CCI_IMPL_ISUPPORTS2(_class, _i1, _i2)                                 \
  CCI_IMPL_ADDREF(_class)                                                      \
  CCI_IMPL_RELEASE(_class)                                                     \
  CCI_IMPL_ALLOCATOR(_class)                                                   \
  CCI_IMPL_QUERY_INTERFACE2(_class, _i1, _i2)

#define CCI_IMPL_ISUPPORTS3(_class, _i1, _i2, _i3)                            \
  CCI_IMPL_ADDREF(_class)                                                      \
  CCI_IMPL_RELEASE(_class)                                                     \
  CCI_IMPL_ALLOCATOR(_class)                                                   \
  CCI_IMPL_QUERY_INTERFACE3(_class, _i1, _i2, _i3)

#define CCI_IMPL_ISUPPORTS4(_class, _i1, _i2, _i3, _i4)                       \
  CCI_IMPL_ADDREF(_class)                                                      \
  CCI_IMPL_RELEASE(_class)                                                     \
  CCI_IMPL_ALLOCATOR(_class)                                                   \
  CCI_IMPL_QUERY_INTERFACE4(_class, _i1, _i2, _i3, _i4)

#define CCI_IMPL_ISUPPORTS5(_class, _i1, _i2, _i3, _i4, _i5)                  \
  CCI_IMPL_ADDREF(_class)                                                      \
  CCI_IMPL_RELEASE(_class)                                                     \
  CCI_IMPL_ALLOCATOR(_class)                                                   \
  CCI_IMPL_QUERY_INTERFACE5(_class, _i1, _i2, _i3, _i4, _i5)

#define CCI_IMPL_ISUPPORTS6(_class, _i1, _i2, _i3, _i4, _i5, _i6)             \
  CCI_IMPL_ADDREF(_class)                                                      \
  CCI_IMPL_RELEASE(_class)                                                     \
  CCI_IMPL_ALLOCATOR(_class)                                                   \
  CCI_IMPL_QUERY_INTERFACE6(_class, _i1, _i2, _i3, _i4, _i5, _i6)

#define CCI_IMPL_ISUPPORTS7(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7)        \
  CCI_IMPL_ADDREF(_class)                                                      \
  CCI_IMPL_RELEASE(_class)                                                     \
  CCI_IMPL_ALLOCATOR(_class)                                                   \
  CCI_IMPL_QUERY_INTERFACE7(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7)

#define CCI_IMPL_ISUPPORTS8(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7, _i8)   \
  CCI_IMPL_ADDREF(_class)                                                      \
  CCI_IMPL_RELEASE(_class)                                                     \
  CCI_IMPL_ALLOCATOR(_class)                                                   \
  CCI_IMPL_QUERY_INTERFACE8(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7, _i8)

#define CCI_IMPL_ISUPPORTS9(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7, _i8,   \
                           _i9)                                                \
  CCI_IMPL_ADDREF(_class)                                                      \
  CCI_IMPL_RELEASE(_class)                                                     \
  CCI_IMPL_ALLOCATOR(_class)                                                   \
  CCI_IMPL_QUERY_INTERFACE9(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7, _i8, _i9)

#define CCI_IMPL_ISUPPORTS10(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7, _i8,  \
                            _i9, _i10)                                         \
  CCI_IMPL_ADDREF(_class)                                                      \
  CCI_IMPL_RELEASE(_class)                                                     \
  CCI_IMPL_ALLOCATOR(_class)                                                   \
  CCI_IMPL_QUERY_INTERFACE10(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7, _i8,   \
                            _i9, _i10)

#define CCI_IMPL_ISUPPORTS11(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7, _i8,  \
                            _i9, _i10, _i11)                                   \
  CCI_IMPL_ADDREF(_class)                                                      \
  CCI_IMPL_RELEASE(_class)                                                     \
  CCI_IMPL_ALLOCATOR(_class)                                                   \
  CCI_IMPL_QUERY_INTERFACE11(_class, _i1, _i2, _i3, _i4, _i5, _i6, _i7, _i8,   \
                            _i9, _i10, _i11)

#define CCI_IMPL_ISUPPORTS_INHERITED0(Class, Super)                           \
    CCI_IMPL_QUERY_INTERFACE_INHERITED0(Class, Super)                          \
    CCI_IMPL_ADDREF_INHERITED(Class, Super)                                    \
    CCI_IMPL_RELEASE_INHERITED(Class, Super)                                   \

#define CCI_IMPL_ISUPPORTS_INHERITED1(Class, Super, i1)                       \
    CCI_IMPL_QUERY_INTERFACE_INHERITED1(Class, Super, i1)                      \
    CCI_IMPL_ADDREF_INHERITED(Class, Super)                                    \
    CCI_IMPL_RELEASE_INHERITED(Class, Super)                                   \

#define CCI_IMPL_ISUPPORTS_INHERITED2(Class, Super, i1, i2)                   \
    CCI_IMPL_QUERY_INTERFACE_INHERITED2(Class, Super, i1, i2)                  \
    CCI_IMPL_ADDREF_INHERITED(Class, Super)                                    \
    CCI_IMPL_RELEASE_INHERITED(Class, Super)                                   \

#define CCI_IMPL_ISUPPORTS_INHERITED3(Class, Super, i1, i2, i3)               \
    CCI_IMPL_QUERY_INTERFACE_INHERITED3(Class, Super, i1, i2, i3)              \
    CCI_IMPL_ADDREF_INHERITED(Class, Super)                                    \
    CCI_IMPL_RELEASE_INHERITED(Class, Super)                                   \

#define CCI_IMPL_ISUPPORTS_INHERITED4(Class, Super, i1, i2, i3, i4)           \
    CCI_IMPL_QUERY_INTERFACE_INHERITED4(Class, Super, i1, i2, i3, i4)          \
    CCI_IMPL_ADDREF_INHERITED(Class, Super)                                    \
    CCI_IMPL_RELEASE_INHERITED(Class, Super)                                   \

#define CCI_IMPL_ISUPPORTS_INHERITED5(Class, Super, i1, i2, i3, i4, i5)       \
    CCI_IMPL_QUERY_INTERFACE_INHERITED5(Class, Super, i1, i2, i3, i4, i5)      \
    CCI_IMPL_ADDREF_INHERITED(Class, Super)                                    \
    CCI_IMPL_RELEASE_INHERITED(Class, Super)                                   \

#define CCI_IMPL_ISUPPORTS_INHERITED6(Class, Super, i1, i2, i3, i4, i5, i6)   \
    CCI_IMPL_QUERY_INTERFACE_INHERITED6(Class, Super, i1, i2, i3, i4, i5, i6)  \
    CCI_IMPL_ADDREF_INHERITED(Class, Super)                                    \
    CCI_IMPL_RELEASE_INHERITED(Class, Super)                                   \

#define CCI_IMPL_ISUPPORTS_INHERITED7(Class, Super, i1, i2, i3, i4, i5, i6, i7) \
    CCI_IMPL_QUERY_INTERFACE_INHERITED7(Class, Super, i1, i2, i3, i4, i5, i6, i7) \
    CCI_IMPL_ADDREF_INHERITED(Class, Super)                                    \
    CCI_IMPL_RELEASE_INHERITED(Class, Super)                                   \

// Dummy threadsafe declaration
#define CCI_IMPL_THREADSAFE_ISUPPORTS0  CCI_IMPL_ISUPPORTS0
#define CCI_IMPL_THREADSAFE_ISUPPORTS1  CCI_IMPL_ISUPPORTS1
#define CCI_IMPL_THREADSAFE_ISUPPORTS2  CCI_IMPL_ISUPPORTS2
#define CCI_IMPL_THREADSAFE_ISUPPORTS3  CCI_IMPL_ISUPPORTS3
#define CCI_IMPL_THREADSAFE_ISUPPORTS4  CCI_IMPL_ISUPPORTS4
#define CCI_IMPL_THREADSAFE_ISUPPORTS5  CCI_IMPL_ISUPPORTS5
#define CCI_IMPL_THREADSAFE_ISUPPORTS6  CCI_IMPL_ISUPPORTS6
#define CCI_IMPL_THREADSAFE_ISUPPORTS7  CCI_IMPL_ISUPPORTS7
#define CCI_IMPL_THREADSAFE_ISUPPORTS8  CCI_IMPL_ISUPPORTS8
#define CCI_IMPL_THREADSAFE_ISUPPORTS9  CCI_IMPL_ISUPPORTS9
#define CCI_IMPL_THREADSAFE_ISUPPORTS10 CCI_IMPL_ISUPPORTS10
#define CCI_IMPL_THREADSAFE_ISUPPORTS11 CCI_IMPL_ISUPPORTS11

/*
 * Macro to glue together a QI that starts with an interface table
 * and segues into an interface map (e.g. it uses singleton classinfo
 * or tearoffs).
 */
#define CCI_INTERFACE_TABLE_TO_MAP_SEGUE \
  if (rv == CCI_OK) return rv; \
  cciISupports* foundInterface;

#endif
