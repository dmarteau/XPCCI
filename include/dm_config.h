#ifndef dm_config_h
#define dm_config_h

/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2012 David Marteau
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

//--------------------------------------------------------
// File         : dm_config.h
// Date         : 07/2004
// Author       : David Marteau
//--------------------------------------------------------


// NOTE: change theses options only if you plan to
// rebuild libraries and modules.

// NOTE: all modules not built as 'pure' modules
// (i.e, using calls from any of the support library)
// should be recompiled as well.

//--------------------------------------------
// Define DM_ENABLE_TRACE_MEM
// if you want to include code for memory tracing
//--------------------------------------------

#define DM_ENABLE_TRACE_MEM

//--------------------------------------------
// Define DM_CONFIG_SINGLE_THREADED
// if you want a single threaded system.
//--------------------------------------------

// #define DM_CONFIG_SINGLE_THREADED

//--------------------------------------------
// Define DM_CONFIG_USE_PTHREAD if you plan to
// use POSIX threads
// (useless if DM_CONFIG_SINGLE_THREADED is defined )
//
// NOTE : this will be automatically defined
//        for linux target
//        (see DM_CONFIG_TARGET_LINUX)
//--------------------------------------------

// #define DM_CONFIG_USE_PTHREAD

//--------------------------------------------
// undef DM_CONFIG_NO_STL_COMPLEX if you want to use
// the STL Implementation of <complex>
//--------------------------------------------

#define DM_CONFIG_NO_STL_COMPLEX

//--------------------------------------------
// By default assume that sizeof(int) is 4 bytes
// on this pltaform
// This value is set only if DM_BYTES_PER_INT is
// not defined
//--------------------------------------------

#define DM_ASSUME_BYTES_PER_INT 4

//--------------------------------------------
// By default assume that we ar running on Intel
// arch
//--------------------------------------------

#define DM_IS_LITTLE_ENDIAN

//--------------------------------------------
// Supported (or not yet) target system
// Should be defined in makefile specific for that
// target
//--------------------------------------------

// Windows

// NOTE: automatically defined if _WIN32 is in
//       compiler global scope
//
// see dm_ports/dmTarget.h for autoconfig

//#define DM_CONFIG_TARGET_WIN32

// Linux

//#define DM_CONFIG_TARGET_LINUX

// Cygwin

//#define DM_CONFIG_TARGET_CYGWIN

// Include specific target configurations
#include "dmTarget.h"

// Include specific compiler stuff
#include "dmCompilers.h"

// This is used to resolve some configuration entry
// specific to target OS

#if defined(DM_CONFIG_TARGET_WIN32)

  #define DM_PLATFORM_IDSTR() _TXT("win")

  #define DM_DLL_PREFIX
  #define DM_DLL_SUFFIX ".dll"

  // By default assume that we have win32 libs available
  #define DM_CONFIG_WIN32_WIN32

#elif defined(DM_CONFIG_TARGET_MACOSX)

  #define DM_PLATFORM_IDSTR() _TXT("macosx")

  #define DM_DLL_PREFIX "lib"
  #define DM_DLL_SUFFIX ".dylib"

#elif defined(DM_CONFIG_TARGET_LINUX)

  #define DM_PLATFORM_IDSTR() _TXT("linux")

  #define DM_DLL_PREFIX "lib"
  #define DM_DLL_SUFFIX ".so"

#endif //


#endif // dm_config.h
