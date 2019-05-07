#ifndef dmTarget_h
#define dmTarget_h

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
// File         : dmTarget.h
// Date         : 10/2004
// Author       : David Marteau
//--------------------------------------------------------

//============================================
// Check system version from predefined macros
//============================================

// WIN32
#if defined(_WIN32) || defined(WIN32)

  #ifndef DM_CONFIG_TARGET_WIN32
  #define DM_CONFIG_TARGET_WIN32
  #endif

  #ifndef DM_CONFIG_TARGET_WIN
  #define DM_CONFIG_TARGET_WIN
  #endif

#endif

// CYGWIN
#if defined(__CYGWIN__) || defined(__CYGWIN32__) || defined(__MINGW32__)

 #ifndef DM_CONFIG_TARGET_CYGWIN
 #define DM_CONFIG_TARGET_CYGWIN
 #endif

 #ifndef DM_CONFIG_TARGET_WIN
 #define DM_CONFIG_TARGET_WIN
 #endif

#endif

#if defined(__APPLE__) && defined(__GNUC__)
 #define DM_CONFIG_TARGET_MACOSX
 #define DM_CONFIG_TARGET_BSD
#endif

// LINUX
#if defined(__linux__)

 #ifndef DM_CONFIG_TARGET_LINUX
 #define DM_CONFIG_TARGET_LINUX
 #endif

#endif

#if defined(DM_CONFIG_TARGET_LINUX) || defined(DM_CONFIG_TARGET_BSD)

 #ifndef DM_CONFIG_TARGET_UNIX
 #define DM_CONFIG_TARGET_UNIX
 #endif

#endif



#endif // dmTarget_h
