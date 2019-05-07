#ifndef dmRWLocks_h
#define dmRWLocks_h

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
  #include "plateform/linux/_dmRWLocks.h"
#elif defined(DM_CONFIG_TARGET_WIN32)
  #include "plateform/win/_dmRWLocks.h"
#endif

// Read/Write locks for multithreadeded application

class __dmUtilities dmRWLock
{
  public:
    dmRWLock();
   ~dmRWLock();

    bool TryRdLock ();  // Try Locks for reading
    void RdLock    ();  // Wait until rdlock becomes available then lock

    bool TryWrLock ();  // Try to lock whithout blocking. return true if locked
    void WrLock    ();  // Wait until read lock becomes available then lock

    void Unlock    ();  // Unlock !

  protected:
    dmDECLARE_RWLOCK()
};


class dmReadLock
{
 public:
  dmRWLock& _RWLock;
 ~dmReadLock()   { _RWLock.Unlock(); }
  dmReadLock(dmRWLock& _aLock) : _RWLock(_aLock) { _RWLock.RdLock(); }
};

class dmWriteLock
{
 public:
  dmRWLock& _RWLock;
 ~dmWriteLock()   { _RWLock.Unlock(); }
  dmWriteLock(dmRWLock& _aLock) : _RWLock(_aLock) { _RWLock.WrLock(); }
};

#define dmREAD_LOCK( _RWLock )  dmReadLock  __RLock__(_RWLock);
#define dmWRITE_LOCK( _RWLock ) dmWriteLock __WLock__(_RWLock);

#else  // DM_CONFIG_SINGLE_THREADED

class dmRWLock
{
  public:
    dmRWLock() {}
   ~dmRWLock() {}

    bool TryRdLock () { return true; }
    void RdLock    () {}

    bool TryWrLock () { return true; }
    void WrLock    () {}

    void Unlock    () {}
};

#define dmREAD_LOCK( _RWLock )
#define dmWRITE_LOCK( _RWLock )

#endif // DM_CONFIG_SINGLE_THREADED

#endif // dmRWLocks_h
