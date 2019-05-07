#ifndef cciDebug_h
#define cciDebug_h
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
 * David Marteau (CCI Implementation)
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

#define CCI_DEBUG_ABORT     DM_DEBUG_ABORT
#define CCI_DEBUG_BREAK     DM_DEBUG_BREAK
#define CCI_DEBUG_WARNING   DM_DEBUG_WARNING
#define CCI_DEBUG_ASSERTION DM_DEBUG_ASSERTION

extern "C" {

__daim_export void __daim_call( CCI_DebugBreak )(dm_uint32 aSeverity,
                                const char *aStr,
                                const char *aExpr,
                                const char *aFile, dm_uint32 aLine);
}

#ifdef DEBUG

/**
 * Abort the execution of the program if the expression evaluates to
 * false.
 *
 * There is no status value returned from the macro.
 *
 * Note that the non-debug version of this macro does <b>not</b>
 * evaluate the expression argument. Hence side effect statements
 * as arguments to the macro will yield improper execution in a
 * non-debug build. For example:
 *
 *      CCI_ABORT_IF_FALSE(0 == foo++, "yikes foo should be zero");
 *
 * Note also that the non-debug version of this macro does <b>not</b>
 * evaluate the message argument.
 */
#define CCI_ABORT_IF_FALSE(_expr, _msg)                        \
  DM_BEGIN_MACRO                                                \
    if (!(_expr)) {                                             \
      CCI_DebugBreak(CCI_DEBUG_ABORT, _msg, #_expr, __FILE__, __LINE__); \
    }                                                           \
  DM_END_MACRO

/**
 * Warn if a given condition is false.
 *
 * Program execution continues past the usage of this macro.
 *
 * Note also that the non-debug version of this macro does <b>not</b>
 * evaluate the message argument.
 */
#define CCI_WARN_IF_FALSE(_expr,_msg)                          \
  DM_BEGIN_MACRO                                              \
    if (!(_expr)) {                                           \
      CCI_DebugBreak(CCI_DEBUG_WARNING, _msg, #_expr, __FILE__, __LINE__); \
    }                                                         \
  DM_END_MACRO

/**
 * Test a precondition for truth. If the expression is not true then
 * trigger a program failure.
 */
#define CCI_PRECONDITION(expr, str)                            \
  DM_BEGIN_MACRO                                              \
    if (!(expr)) {                                            \
      CCI_DebugBreak(CCI_DEBUG_ASSERTION, str, #expr, __FILE__, __LINE__); \
    }                                                         \
  DM_END_MACRO

/**
 * Test an assertion for truth. If the expression is not true then
 * trigger a program failure.
 */
#define CCI_ASSERTION(expr, str)                               \
  DM_BEGIN_MACRO                                              \
    if (!(expr)) {                                            \
      CCI_DebugBreak(CCI_DEBUG_ASSERTION, str, #expr, __FILE__, __LINE__); \
    }                                                         \
  DM_END_MACRO

/**
 * Test a post-condition for truth. If the expression is not true then
 * trigger a program failure.
 */
#define CCI_POSTCONDITION(expr, str)                           \
  DM_BEGIN_MACRO                                              \
    if (!(expr)) {                                            \
      CCI_DebugBreak(CCI_DEBUG_ASSERTION, str, #expr, __FILE__, __LINE__); \
    }                                                         \
  DM_END_MACRO

/**
 * This macros triggers a program failure if executed. It indicates that
 * an attempt was made to execute some unimplemented functionality.
 */
#define CCI_NOTYETIMPLEMENTED(str)                             \
  CCI_DebugBreak(CCI_DEBUG_ASSERTION, str, "NotYetImplemented", __FILE__, __LINE__)

/**
 * This macros triggers a program failure if executed. It indicates that
 * an attempt was made to execute some unimplemented functionality.
 */
#define CCI_NOTREACHED(str)                                    \
  CCI_DebugBreak(CCI_DEBUG_ASSERTION, str, "Not Reached", __FILE__, __LINE__)

/**
 * Log an error message.
 */
#define CCI_ERROR(str)                                         \
  CCI_DebugBreak(CCI_DEBUG_ASSERTION, str, "Error", __FILE__, __LINE__)

/**
 * Log a warning message.
 */
#define CCI_WARNING(str)                                       \
  CCI_DebugBreak(CCI_DEBUG_WARNING, str, dm_null, __FILE__, __LINE__)

/**
 * Trigger an abort
 */
#define CCI_ABORT()                                            \
  CCI_DebugBreak(CCI_DEBUG_ABORT, dm_null, dm_null, __FILE__, __LINE__)

/**
 * Cause a break
 */
#define CCI_BREAK()                                            \
  CCI_DebugBreak(CCI_DEBUG_BREAK, dm_null, dm_null, __FILE__, __LINE__)

#else /* CCI_DEBUG */

/**
 * The non-debug version of these macros do not evaluate the
 * expression or the message arguments to the macro.
 */
#define CCI_ABORT_IF_FALSE(_expr, _msg) DM_BEGIN_MACRO /* nothing */ DM_END_MACRO
#define CCI_WARN_IF_FALSE(_expr, _msg)  DM_BEGIN_MACRO /* nothing */ DM_END_MACRO
#define CCI_PRECONDITION(expr, str)     DM_BEGIN_MACRO /* nothing */ DM_END_MACRO
#define CCI_ASSERTION(expr, str)        DM_BEGIN_MACRO /* nothing */ DM_END_MACRO
#define CCI_POSTCONDITION(expr, str)    DM_BEGIN_MACRO /* nothing */ DM_END_MACRO
#define CCI_NOTYETIMPLEMENTED(str)      DM_BEGIN_MACRO /* nothing */ DM_END_MACRO
#define CCI_NOTREACHED(str)             DM_BEGIN_MACRO /* nothing */ DM_END_MACRO
#define CCI_ERROR(str)                  DM_BEGIN_MACRO /* nothing */ DM_END_MACRO
#define CCI_WARNING(str)                DM_BEGIN_MACRO /* nothing */ DM_END_MACRO
#define CCI_ABORT()                     DM_BEGIN_MACRO /* nothing */ DM_END_MACRO
#define CCI_BREAK()                     DM_BEGIN_MACRO /* nothing */ DM_END_MACRO

#endif /* ! CCI_DEBUG */

/* Macros for checking the trueness of an expression passed in within an
 * interface implementation.  These need to be compiled regardless of the */
/* CCI_DEBUG flag
******************************************************************************/

#define CCI_ENSURE_TRUE(x, ret)                               \
  DM_BEGIN_MACRO                                               \
    if (DM_UNLIKELY(!(x))) {                                   \
       CCI_WARNING("CCI_ENSURE_TRUE(" #x ") failed");          \
       return ret;                                             \
    }                                                          \
  DM_END_MACRO

#define CCI_ENSURE_FALSE(x, ret)                              \
  CCI_ENSURE_TRUE(!(x), ret)

/******************************************************************************
** Macros for checking results
******************************************************************************/

#define CCI_ENSURE_SUCCESS_BODY(res, ret)                                \
    CCI_WARNING("CCI_ENSURE_SUCCESS(" #res ", " #ret ") failed");

#define CCI_ENSURE_SUCCESS(res, ret)                                     \
  DM_BEGIN_MACRO                                                          \
    cci_result __rv = res; /* Don't evaluate |res| more than once */      \
    if (CCI_FAILED(__rv)) {                                               \
      CCI_ENSURE_SUCCESS_BODY(res, ret)                                   \
      return ret;                                                         \
    }                                                                     \
  DM_END_MACRO

/******************************************************************************
** Macros for checking state and arguments upon entering interface boundaries
******************************************************************************/

#define CCI_ENSURE_ARG(arg)                                    \
  CCI_ENSURE_TRUE(arg, CCI_ERROR_INVALID_ARG)

#define CCI_ENSURE_ARG_POINTER(arg)                            \
  CCI_ENSURE_TRUE(arg, CCI_ERROR_INVALID_POINTER)

#define CCI_ENSURE_ARG_MIN(arg, min)                           \
  CCI_ENSURE_TRUE((arg) >= min, CCI_ERROR_INVALID_ARG)

#define CCI_ENSURE_ARG_MAX(arg, max)                           \
  CCI_ENSURE_TRUE((arg) <= max, CCI_ERROR_INVALID_ARG)

#define CCI_ENSURE_ARG_RANGE(arg, min, max)                    \
  CCI_ENSURE_TRUE(((arg) >= min) && ((arg) <= max), CCI_ERROR_INVALID_ARG)

#define CCI_ENSURE_STATE(state)                                \
  CCI_ENSURE_TRUE(state, CCI_ERROR_UNEXPECTED)

#define CCI_ENSURE_NO_AGGREGATION(outer)                       \
  CCI_ENSURE_FALSE(outer, CCI_ERROR_NO_AGGREGATION)

#define CCI_ENSURE_PROPER_AGGREGATION(outer, iid)              \
  CCI_ENSURE_FALSE(outer && !iid.Equals(CCI_GET_IID(cciISupports)), CCI_ERROR_INVALID_ARG)

#endif // cciDebug_h
