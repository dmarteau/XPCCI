#ifndef cciCore_h
#define cciCore_h
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
 *   David Marteau (CCI port)
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

#ifndef dm_prolog_h
#include "dm_prolog.h"
#endif

/**
 * Marker for GLUE stub functions
 */
#define CCI_COM_GLUE

/**
 * Prevent virtual table creation
 */
#ifdef CCI_NO_VTABLE
#undef CCI_NO_VTABLE
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1100
#define CCI_NO_VTABLE __declspec(novtable)
#else
#define CCI_NO_VTABLE
#endif


#define CCI_SCRIPTABLE
#define CCI_OUTPARAM
#define CCI_INOUTPARAM

/*
 * CCI_DEFCALL undoes the effect of a global regparm/stdcall setting
 * Pass all arguments from the stack
 */
#if defined(__i386__) && defined(__GNUC__) && (__GNUC__ >= 3)
#define CCI_DEFCALL __attribute__ ((regparm (0), cdecl))
#else
#define CCI_DEFCALL
#endif


/**
 * Mark a function as using a potentially non-standard function calling
 * convention.  This can be used on functions that are called very
 * frequently, to reduce the overhead of the function call.  It is still worth
 * using the macro for C++ functions which take no parameters since it allows
 * passing |this| in a register.
 *
 *  - Do not use this on any scriptable interface method since xptcall won't be
 *    aware of the different calling convention.
 *  - This must appear on the declaration, not the definition.
 *  - Adding this to a public function _will_ break binary compatibility.
 *  - This may be used on virtual functions but you must ensure it is applied
 *    to all implementations - the compiler will _not_ warn but it will crash.
 *  - This has no effect for inline functions or functions which take a
 *    variable number of arguments.
 *  - __fastcall on windows should not be applied to class
 *    constructors/destructors - use the CCI_CONSTRUCTOR_FASTCALL macro for
 *    constructors/destructors.
 *
 * Examples: int CCI_FASTCALL func1(char *foo);
 *           CCI_HIDDEN_(int) CCI_FASTCALL func2(char *foo);
 */


#if defined(__i386__) && defined(__GNUC__) && (__GNUC__ >= 3)

#define CCI_FASTCALL __attribute__ ((regparm (3), stdcall))
#define CCI_CONSTRUCTOR_FASTCALL __attribute__ ((regparm (3), stdcall))

#elif defined(DM_CONFIG_MSVC_BUILD)

#define CCI_FASTCALL __fastcall
#define CCI_CONSTRUCTOR_FASTCALL

#else

#define CCI_FASTCALL
#define CCI_CONSTRUCTOR_FASTCALL

#endif

/**
 * Map import export tag
 */
#define CCI_HIDDEN __DLL_PRIVATE_TAG
#define CCI_EXPORT __DLL_EXPORT_TAG
#define CCI_IMPORT __DLL_IMPORT_TAG

#if defined(DM_CONFIG_TARGET_WIN32)

#define CCI_IMETHOD_(type) virtual type __stdcall
#define CCI_IMETHODIMP_(type) type __stdcall
#define CCI_METHOD_(type) type __stdcall
#define CCI_CALLBACK_(_type, _name) _type (__stdcall * _name)
#define CCI_STDCALL __stdcall

#else

#define CCI_IMETHOD_(type) virtual CCI_HIDDEN type CCI_DEFCALL
#define CCI_IMETHODIMP_(type) type
#define CCI_METHOD_(type) type
#define CCI_CALLBACK_(_type, _name) _type (* _name)
#define CCI_STDCALL

#endif

/**
 * Macro for creating typedefs for pointer-to-member types which are
 * declared with stdcall.  It is important to use this for any type which is
 * declared as stdcall (i.e. CCI_IMETHOD).  For example, instead of writing:
 *
 *  typedef cci_result (cci_Foo::*someType)(cci_Supports* arg);
 *
 *  you should write:
 *
 *  typedef
 *  CCI_STDCALL_FUNCPROTO(nsresult, someType, nsIFoo, typeFunc, (cci_Supports*));
 *
 *  where cci_Foo::typeFunc is any method declared as
 *  CCI_IMETHOD typeFunc(cci_Supports*);
 *
 *  XXX this can be simplified to always use the non-typeof implementation
 *  when http://gcc.gnu.org/bugzilla/show_bug.cgi?id=11893 is fixed.
 */

#ifdef __GNUC__
#define CCI_STDCALL_FUNCPROTO(ret, name, class, func, args) \
  typeof(&class::func) name
#else
#define CCI_STDCALL_FUNCPROTO(ret, name, class, func, args) \
  ret (CCI_STDCALL class::*name) args
#endif

/**
 * Generic CCI result data type
 */
typedef dm_uint32 cci_result;

/**
 * Reference count values
 */
typedef dm_uint32 cci_refcnt;

/**
 *  CCI Error definitions
 */
#include "cciErrors.h"

/**
 * Generic API modifiers which return the standard CCI cci_result type
 */
#define CCI_IMETHOD          CCI_IMETHOD_(cci_result)
#define CCI_IMETHODIMP       CCI_IMETHODIMP_(cci_result)
#define CCI_METHOD           CCI_METHOD_(cci_result)
#define CCI_CALLBACK(_name)  CCI_CALLBACK_(cci_result, _name)

#ifndef NO_CCI_NAMESPACE
  #define CCI_NAMESPACE_BEGIN namespace cci { //
  #define CCI_NAMESPACE_END   }
  #define CCI_USE_NS   using namespace cci;
  #define CCI_NS_( n )  cci::n
#else
  #define CCI_NAMESPACE_BEGIN //
  #define CCI_NAMESPACE_END   //
  #define CCI_USE_NS
  #define CCI_NS_( n )
#endif

#ifndef CCI_NAMESPACE
  CCI_NAMESPACE_BEGIN
  CCI_NAMESPACE_END
  CCI_USE_NS
#endif

// The following should is designed to  detect thread safety in
// debug mode
// XXX Implement me
#define CCI_DECL_OWNINGTHREAD            /* nothing */
#define CCI_ASSERT_OWNINGTHREAD(_class)  ((void)0)
#define CCI_ASSERT_OWNINGTHREAD_AND_NOT_CCTHREAD(_class)  ((void)0)

// For debug purpose
// XXX Implement me
#define CCI_LOG_ADDREF
  
#endif // cciCore_h
