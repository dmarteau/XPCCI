#ifndef _dmDebug_h
#define _dmDebug_h

/* :::BEGIN LICENSE BLOCK:::
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
 *  :::END LICENSE BLOCK::: */

//--------------------------------------------------------
// File         : dmDebug.h
// Date         : 4/2008
// Author       : David Marteau


#if defined(_DEBUG) || defined(DEBUG)

  #if defined(_WIN32)

    #define __dm_DebugBreak()  __debugbreak()

  #elif defined(DM_CONFIG_TARGET_MACOSX)
   /* Put this Mac OS X test above the GNUC/x86 test because the
    * GNUC/x86 test is also true on Intel Mac OS X and we want the PPC/x86
    * impls to be the same.
    */
    #include <signal.h>
    #define __dm_DebugBreak() raise(SIGTRAP);

  #elif defined(__GNUC__) && (defined(__i386__) || defined(__i386) || defined(__x86_64__))

    #define __dm_DebugBreak() asm("int $3");

  #else

    #error dont know how to break on this platform

  #endif

#else

  #define __dm_DebugBreak()

#endif

#define __XXXBreakPoint__ __dm_DebugBreak()

#endif // _dmDebug_h
