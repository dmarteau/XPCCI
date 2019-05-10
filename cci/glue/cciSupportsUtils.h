#ifndef cciSupportsUtils_h
#define cciSupportsUtils_h

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
 *   Pierre Phaneuf <pp@ludusdesign.com>
 *   Scott Collins <scc@ScottCollins.net>
 *   Dan Mosedale <dmose@mozilla.org>
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

#ifndef cciDebug_h
#include "cciDebug.h"
#endif

/**
 * Macro for deleting an object that implements cciISupports.
 * Convenient for passing as other macro argument.
 * @param _ptr The object to delete.
 */
#define CCI_DELETEOBJECT(_ptr)   \
  DM_BEGIN_MACRO                 \
   delete (_ptr);                \
  DM_END_MACRO

/**
 * Macro for adding a reference to an interface.
 * @param _ptr The interface pointer.
 */
#define CCI_ADDREF(_ptr) \
  (_ptr)->AddRef()

/**
 * Macro for adding a reference to this. This macro should be used
 * because CCI_ADDREF (when tracing) may require an ambiguous cast
 * from the pointers primary type to cciISupports. This macro sidesteps
 * that entire problem.
 */
#define CCI_ADDREF_THIS() \
  AddRef()


extern "C++" {
// ...because some one is accidentally including this file inside
// an |extern "C"|


// Making this a |inline| |template| allows |expr| to be evaluated only once,
// yet still denies you the ability to |AddRef()| an |nsCOMPtr|.
template <class T>
inline cci_refcnt cci_if_addref( T expr )
{
   return expr ? expr->AddRef() : 0;
}

} /* extern "C++" */

/**
 * Macro for adding a reference to an interface that checks for NULL.
 * @param _expr The interface pointer.
 */
#define CCI_IF_ADDREF(_expr) cci_if_addref(_expr)

/*
 * Given these declarations, it explicitly OK and efficient to end a `getter' with:
 *
 *    CCI_IF_ADDREF(*result = mThing);
 *
 * even if |mThing| is an |nsCOMPtr|.  If |mThing| is an |nsCOMPtr|, however, it is still
 * _illegal_ to say |CCI_IF_ADDREF(mThing)|.
 */

/**
 * Macro for releasing a reference to an interface.
 * @param _ptr The interface pointer.
 */
#define CCI_RELEASE(_ptr)                                                    \
  DM_BEGIN_MACRO                                                              \
   (_ptr)->Release();                                                         \
   (_ptr) = 0;                                                                \
  DM_END_MACRO

/**
 * Macro for releasing a reference to an interface.
 * @param _ptr The interface pointer.
 */
#define CCI_RELEASE_THIS() \
    Release()

/**
 * Macro for releasing a reference to an interface, except that this
 * macro preserves the return value from the underlying Release call.
 * The interface pointer argument will only be NULLed if the reference count
 * goes to zero.
 *
 * @param _ptr The interface pointer.
 */
#define CCI_RELEASE2(_ptr,_rv)                                               \
  DM_BEGIN_MACRO                                                              \
    _rv = (_ptr)->Release();                                                  \
    if (0 == (_rv)) (_ptr) = 0;                                               \
  DM_END_MACRO

/**
 * Macro for releasing a reference to an interface that checks for NULL;
 * @param _ptr The interface pointer.
 */
#define CCI_IF_RELEASE(_ptr)                                                 \
  DM_BEGIN_MACRO                                                              \
    if(_ptr) {                                                                \
      (_ptr)->Release();                                                      \
      (_ptr) = 0;                                                             \
    }                                                                         \
  DM_END_MACRO

/*
 * Often you have to cast an implementation pointer, e.g., |this|, to an
 * |cciISupports*|, but because you have multiple inheritance, a simple cast
 * is ambiguous.  One could simply say, e.g., (given a base |nsIBase|),
 * |static_cast<nsIBase*>(this)|; but that disguises the fact that what
 * you are really doing is disambiguating the |cciISupports|.  You could make
 * that more obvious with a double cast, e.g., |static_cast<cciISupports*>
                                                           (* static_cast<nsIBase*>(this))|, but that is bulky and harder to read...
 *
 * The following macro is clean, short, and obvious.  In the example above,
 * you would use it like this: |CCI_ISUPPORTS_CAST(nsIBase*, this)|.
 */

#define CCI_ISUPPORTS_CAST(__unambiguousBase, __expr) \
  static_cast<cciISupports*>(static_cast<__unambiguousBase>(__expr))

// Prevent name collision with XPCOM
CCI_NAMESPACE_BEGIN

// a type-safe shortcut for calling the |QueryInterface()| member function
template <class T, class DestinationType>
inline cci_result CallQueryInterface( T* aSource, DestinationType** aDestination )
{
  CCI_PRECONDITION(aSource, "null parameter");
  CCI_PRECONDITION(aDestination, "null parameter");

  return aSource->QueryInterface(CCI_GET_TEMPLATE_IID(DestinationType),
                                 reinterpret_cast<void**>(aDestination));
}

CCI_NAMESPACE_END

#endif // cciSupportsUtils_h
