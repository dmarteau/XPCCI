/* ::: BEGIN LICENSE BLOCK:::
 * Copyright (c) 2005 David Marteau
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

#include "dmUtilitiesBase.h"
#include "dmThread.h"

#ifndef DM_CONFIG_SINGLE_THREADED

#if defined(DM_CONFIG_USE_PTHREAD)
  #include "../daim_ports/linux/_dmThread.cpp"
#elif defined(DM_CONFIG_TARGET_WIN32)
  #include "../daim_ports/win32/_dmThread.cpp"
#endif // DM_CONFIG_TARGET

#endif // DM_CONFIG_SINGLE_THREADED
