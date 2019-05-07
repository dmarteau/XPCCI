#ifndef dmCompilers_h
#define dmCompilers_h

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

//--------------------------------------------------------
// File         : dmCompilers.h
// Date         : 01/2005
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

//============================================
// Check compiler version from predefined macros
//============================================

// MS Visual C++ compiler 6.0 or higher
#if defined(_MSC_VER)
  #define DM_CONFIG_MSVC_BUILD _MSC_VER
#elif defined(MSC_VER)
  #define DM_CONFIG_MSVC_BUILD MSC_VER
#endif

#ifdef DM_CONFIG_MSVC_BUILD

  // Get the proper Visual C++ compiler version

  #if   (DM_CONFIG_MSVC_BUILD >= 1400)
    #define DM_CONFIG_MSVC8_BUILD DM_CONFIG_MSVC_BUILD
  #elif (DM_CONFIG_MSVC_BUILD >= 1300)
    #define DM_CONFIG_MSVC7_BUILD DM_CONFIG_MSVC_BUILD
  #elif (DM_CONFIG_MSVC_BUILD >= 1200)
    #define DM_CONFIG_MSVC6_BUILD DM_CONFIG_MSVC_BUILD
  #elif
    #error Unsupported VC++ version !
  #endif


  #ifdef DM_CONFIG_MSVC8_BUILD
    #define DM_CPP_HAVE_USING_DECLARATION
  #elif defined(DM_CONFIG_MSVC6_BUILD)
    // Visual C++ does not accept partial specialization
    #define DM_NO_PARTIAL_SPECIALIZATION
    // But it's ok to specialize template members out of namespace
    #define DM_TEMPLATES_MEMBERS_SPECIALIZATION
  #endif

  #if (DM_CONFIG_MSVC_BUILD < 1310)
    #ifndef DM_DONT_PROVIDE_NONCONST_OPEQ
    #define DM_DONT_PROVIDE_NONCONST_OPEQ
    #endif
  #endif

  // Has a way to represent 64 bits quantities
  #define DM_HAVE_LONG_LONG 1

#endif


// GNU C++ compiler
#if defined(__GNUC__)
  #define DM_CONFIG_GCC_BUILD __GNUC__
#endif

#if defined(DM_CONFIG_GCC_BUILD)

  // Has a way to represent 64 bits quantities
  #define DM_HAVE_LONG_LONG 1

#endif

//============================================
// Specific plateform stuff
//============================================

#if defined(DM_CONFIG_TARGET_WIN32)
  #include "_dmWinVer.h"

  #if defined(__MSVCRT__)
    #define DM_MSVCRT_BUILD
  #elif defined(DM_CONFIG_MSVC_BUILD)
    #ifndef DM_MSVCRT_BUILD
    #define DM_MSVCRT_BUILD
    #endif
  #endif

#endif

// POSIX threads used on linux/cygwin
#if defined(DM_CONFIG_TARGET_UNIX) // || defined(DM_CONFIG_TARGET_CYGWIN)
   #ifndef DM_CONFIG_USE_PTHREAD
   #define DM_CONFIG_USE_PTHREAD
   #endif
#endif

// POSIX compliant headers file
#if defined(DM_CONFIG_TARGET_UNIX) || defined(DM_CONFIG_TARGET_CYGWIN)

   #ifndef DM_CONFIG_POSIX_OK
   #define DM_CONFIG_POSIX_OK
   #endif

#endif

#if defined(DM_CONFIG_TARGET_CYGWIN) && !defined(__CYGWIN__)
  // Ensure that __CYGWIN__ is defined
  #define __CYGWIN__
#endif

#if defined(DM_CONFIG_TARGET_MACOSX)
  #define DM_NO_GNU_EXTENSION 1
#endif

//============================================
// DLL TAGS
//============================================
#if defined(DM_CONFIG_TARGET_WIN32)

   #if defined(DM_CONFIG_MSVC_BUILD) || defined(DM_CONFIG_TARGET_CYGWIN)
     #define __DLL_EXPORT_TAG   __declspec(dllexport)
     #define __DLL_IMPORT_TAG   __declspec(dllimport)
     #define __DLL_PRIVATE_TAG

     #define __DLL_EXPORT_FORWARD_TAG __DLL_EXPORT_TAG
     #define __DLL_IMPORT_FORWARD_TAG __DLL_IMPORT_TAG
   #endif

#elif defined(DM_CONFIG_TARGET_UNIX)

     #define __DLL_EXPORT_TAG   __attribute__ ((visibility("default")))
     #define __DLL_IMPORT_TAG   __attribute__ ((visibility("default")))
     #define __DLL_PRIVATE_TAG  __attribute__ ((visibility("hidden")))

     #define __DLL_EXPORT_FORWARD_TAG
     #define __DLL_IMPORT_FORWARD_TAG

#else

  // Define defaults tags
  #define __DLL_EXPORT_TAG
  #define __DLL_IMPORT_TAG
  #define __DLL_PRIVATE_TAG
  #define __DLL_EXPORT_FORWARD_TAG
  #define __DLL_IMPORT_FORWARD_TAG

#endif // __DLL_EXPORT_TAG


#if defined(DM_CONFIG_TARGET_WIN32)

#define DM_CALLBACK_(_type, _name) _type (__stdcall * _name)
#define DM_STDCALL __stdcall

#else

#define DM_CALLBACK_(_type, _name) _type (* _name)
#define DM_STDCALL

#endif


#endif // dmCompilers_h
