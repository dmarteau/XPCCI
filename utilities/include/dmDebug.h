#ifndef dmDebug_h
#define dmDebug_h

/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2005 David Marteau
 *
 * This file is part of the DAIM Image Processing Library (DAIM library)
 *
 * The DAIM library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License (LGPL)
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * The DAIM library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the DAIM library; see the file LICENSE.  If
 * not, write to the Free Software Foundation, Inc., 59 Temple Place -
 * Suite 330, Boston, MA 02111-1307, USA.
 *
 *  ::: END LICENSE BLOCK::: */

#include "dmDebugBreak.h"

#ifdef dmDEBUG_CATCH_ENABLED
#define dmDEBUG_TRY   try {
#define dmDEBUG_CATCH } catch(...) { throw dmABNORMAL_EXCEPTION; }
#else
#define dmDEBUG_TRY
#define dmDEBUG_CATCH
#endif

__dmUtilities
void dmDebugBreak(dm_uint32 aSeverity,
                  const char *aStr,
                  const char *aExpr,
                  const char *aFile, dm_uint32 aLine);

#define DM_ABORT_IF_FALSE(_expr, _msg)                          \
  DM_BEGIN_MACRO                                                \
    if (!(_expr)) {                                             \
      dmDebugBreak(DM_DEBUG_ABORT, _msg, #_expr, __FILE__, __LINE__); \
    }                                                           \
  DM_END_MACRO

#define DM_ASSERTION(_expr, _msg)                          \
  DM_BEGIN_MACRO                                                \
    if (!(_expr)) {                                             \
      dmDebugBreak(DM_DEBUG_ASSERTION, _msg, #_expr, __FILE__, __LINE__); \
    }                                                           \
  DM_END_MACRO

#define DM_PRECONDITION  DM_ASSERTION
#define DM_POSTCONDITION DM_ASSERTION


#endif // dmDebug_h
