#ifndef dm_utilities_h
#define dm_utilities_h

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
// File         : dm_utilities.h
// Date         : 03/2008
// Author       : David Marteau
//--------------------------------------------------------

#include "dmUtilitiesBase.h"
#include "dmVersion.h"

#include "dmMemory.h"
#include "dmNew.h"

#include "dmThread.h"
#include "dmMutex.h"
#include "dmRWLocks.h"

#include "dmString.h"
#include "dmDate.h"
#include "dmTime.h"
#include "dmEnvironment.h"
#include "dmPortability.h"
#include "dmExceptions.h"
#include "dmErrors.h"
#include "dmDebug.h"

// Common header files
#include "dmShared.h"
#include "dmSetOf.h"

#ifdef dmUseUtilitiesExtra
#include "dmFastMemory.h"
#include "dmList.h"
#include "dmVector.h"
#include "dmConstruct.h"
#include "dmHashMap.h"
#include "dmHashString.h"
#endif // dmUseUtilitiesExtra

#endif // dm_utilities_h
