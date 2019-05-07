#ifndef dmMutex_h
#define dmMutex_h

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
// File         : dmMutex.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#ifndef DM_CONFIG_SINGLE_THREADED

#if defined(DM_CONFIG_USE_PTHREAD)
  #include "plateform/linux/_dmMutex.h"
#elif defined(DM_CONFIG_TARGET_WIN32)
  #include "plateform/win/_dmMutex.h"
#endif

// Mutex for multithreadeded application

class __dmUtilities dmMutex
{
  public:
    dmMutex();
   ~dmMutex();

    bool TryLock  ();  // Try to lock whithout blocking. return true if locked
    void Unlock   ();  // Unlock !
    void Lock     ();  // Wait until lock becomes available then lock

  protected:
    dmDECLARE_MUTEX()
};


class dmMutexGuard
{
 public:
  dmMutex& _Mutex;
 ~dmMutexGuard()   { _Mutex.Unlock(); }
  dmMutexGuard(dmMutex& _aMutex) : _Mutex(_aMutex) { _Mutex.Lock(); }
};

#define dmCRITICAL_SECTION( _Mutex ) dmMutexGuard __MtxGuard__(_Mutex);//

#else  // DM_CONFIG_SINGLE_THREADED

class dmMutex
{
  public:
    dmMutex() {}
   ~dmMutex() {}

    void Unlock()   {}
    void Lock()     {}
    bool TryLock()  { return true; }
    bool IsLocked() { return false;}

};

#define dmCRITICAL_SECTION( _Mutex )

#endif // DM_CONFIG_SINGLE_THREADED

#endif // dmMutex_h
