#ifndef _dmTarget_h
#define _dmTarget_h

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
// File         : _dmTarget.h
// Date         : 10/2004
// Author       : David Marteau
// Description  : Specific header for win32 system targets
//--------------------------------------------------------

#ifdef DM_UTILITIES_BUILD
  //#define DM_TARGET_WIN_NT4  
    #define DM_TARGET_WIN_2000 
  //#define DM_TARGET_WIN_XP   
#else
  #define DM_CONFIG_IGNORE_WINVER
#endif

#ifndef DM_CONFIG_IGNORE_WINVER

  #if !defined(WINVER) 

    #if defined(DM_TARGET_WIN_XP) 
  
      #define _WIN32_WINNT 0x0501
      #define WINVER       0x0501

    #elif defined(DM_TARGET_WIN_2000)

      #define _WIN32_WINNT 0x0500
      #define WINVER       0x0500

    #elif defined(DM_TARGET_WIN_NT4) 

      #define _WIN32_WINNT 0x0400
      #define WINVER       0x0400

    #else // Will cause an error

     #error "WINDOWS TARGET NOT SUPPORTED !"
    
    #endif // DM_TARGET_XXX

  #elif !defined(DM_CONFIG_IGNORE_WINVER)
    // If building dm_utilities, this file must be included
    // before any windows specific headers
    #error "WINVER ALREADY DEFINED !"

  #endif // WINVER

#endif // DM_CONFIG_IGNORE_WINVER 

#endif // _dmTarget_h
