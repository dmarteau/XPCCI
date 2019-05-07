#ifndef dmUtilitiesBase_h
#define dmUtilitiesBase_h

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
// File         : dmUtilitiesBase.h
// Date         : 4/2008
// Author       : David Marteau
//--------------------------------------------------------

#ifdef DM_UTILITIES_BUILD
  #ifndef DM_UTILITIES
  #define DM_UTILITIES
  #endif
#endif

#ifndef dm_config_h
#include "dm_config.h"
#endif

#ifndef dmTypes_h
#include "dmTypes.h"
#endif

#ifdef DM_UTILITIES

  #ifdef DM_CONFIG_MSVC_BUILD   // Microsoft Visual C++ specific
    #pragma warning(disable: 4275)  // deriving exported class from non-exported
    #pragma warning(disable: 4251)  // using non-exported as public in exported
    #pragma warning(disable: 4284)  // produce false warning !
    #pragma warning(disable: 4786)  // debug symbol truncation !
  #endif // Microsoft Visual C++ specific

  #ifdef DM_UTILITIES_BUILD

    #define __dmUtilities        __DLL_EXPORT_TAG
    #define __dmUtilitiesForward __DLL_EXPORT_FORWARD_TAG

  #else  // DM_UTILITIES_BUILD

    #define __dmUtilities        __DLL_IMPORT_TAG
    #define __dmUtilitiesForward __DLL_IMPORT_FORWARD_TAG

  #endif // DM_UTILITIES_BUILD

  class __dmUtilities dmUtilities
  {
    public:
      static bool IsInitialized;
      static void Initialize( const char** argv, int argc );
      static void Finalize();
  };

#endif // !DM_UTILITIES

#endif // dmUtilitiesBase_h
